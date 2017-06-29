/* test_mck_drv.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include "test_mck_drv.h"

#define TEST_MCK_TS_NAME ""

MODULE_AUTHOR("Fujitsu");
MODULE_DESCRIPTION("McKernel Test Driver.");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("0.1");

/* major/minor */
static int test_mck_major   = 0;
static int test_mck_minor   = 0;
static char test_mck_majmin[test_mck_nr_devs * 8 + 1];

/* module param */
static char* ident;
module_param(ident, charp, S_IRUGO);
MODULE_PARM_DESC(ident, "test driver identifier.");
const char* test_mck_ident(void)
{
	if (ident == NULL) {
		return "";
	}
	return ident;
}

/* lookup initialize function */
#define TEST_MCK_DRV_TEST_SUITE(n) extern int test_mck_## n ##_init(struct test_mck_dev*);
#include "test_suite.list"
#undef TEST_MCK_DRV_TEST_SUITE

/* lookup finalize function */
#define TEST_MCK_DRV_TEST_SUITE(n) extern void test_mck_## n ##_fin(struct test_mck_dev*);
#include "test_suite.list"
#undef TEST_MCK_DRV_TEST_SUITE

/* lookup file_operations */
#define TEST_MCK_DRV_TEST_SUITE(n) extern struct file_operations test_mck_## n ##_fops;
#include "test_suite.list"
#undef TEST_MCK_DRV_TEST_SUITE

/* device list */
#define TEST_MCK_DRV_TEST_SUITE(n) {		\
		.ts_name = #n,			\
		.fops = &test_mck_## n ##_fops,	\
		.init = &test_mck_## n ##_init,	\
		.fin = &test_mck_## n ##_fin,	\
		},
static struct test_mck_dev test_mck_devlist[test_mck_nr_devs] = {
#include "test_suite.list"
};
#undef TEST_MCK_DRV_TEST_SUITE
const struct test_mck_dev* test_mck_lookup_devlist(void)
{
	return test_mck_devlist;
}

/* test suite name list */
#define TEST_MCK_DRV_TEST_SUITE(n) #n " "
static const char* const test_mck_tp_names = 
#include "test_suite.list"
;
#undef TEST_MCK_DRV_TEST_SUITE

/* file operations  */
static ssize_t test_mck_procfs_string_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos, const char* string)
{
	ssize_t ret = 0;
	const size_t len = strlen(string);
	size_t count;
	const char* from;
	
	if (*ppos > len) {
		goto out;
	}
	
	from = string + *ppos;
	count = nbytes;
	if (*ppos + count > len) {
		count = len - *ppos;
	}
	
	if (copy_to_user(buf, from, count)) {
		ret = -EFAULT;
		goto out;
	}
	*ppos += count;
	ret = count;
out:
	return ret;
}

static int test_mck_procfs_devno_open(struct inode *inode, struct file *filep)
{
	filep->private_data = test_mck_majmin;
	return 0;
}

static ssize_t test_mck_procfs_devno_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	const char* test_mck_majmin = file->private_data;
	return test_mck_procfs_string_read(file, buf, nbytes, ppos, test_mck_majmin);
}

static int test_mck_procfs_tp_names_open(struct inode *inode, struct file *filep)
{
	filep->private_data = (void*)test_mck_tp_names;
	return 0;
}

static ssize_t test_mck_procfs_tp_names_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	const char* test_mck_tp_names = file->private_data;
	return test_mck_procfs_string_read(file, buf, nbytes, ppos, test_mck_tp_names);
}

#define TEST_MCK_PROCFS_DIRNAME "test_mck"
static struct proc_dir_entry* test_mck_procdir;
static struct test_mck_procfs_entry {
       const char *fname; /*proc file name*/
       struct file_operations ops;
       struct proc_dir_entry *pent;
} proc_ents[] = {
	{
		.fname = "devno",
		.ops = {
			.owner = THIS_MODULE,
			.open = test_mck_procfs_devno_open,
			.read = test_mck_procfs_devno_read,
		},
	},
	{
		.fname = "tp_names",
		.ops = {
			.owner = THIS_MODULE,
			.open = test_mck_procfs_tp_names_open,
			.read = test_mck_procfs_tp_names_read,
		},
	},
};

static void test_mck_cleanup_procfs(void)
{
	int i;
	for (i = 0; i < sizeof(proc_ents)/sizeof(proc_ents[0]); i++) {
		struct test_mck_procfs_entry* p = &proc_ents[i];
		if (p->pent != NULL) {
			remove_proc_entry(p->fname, test_mck_procdir);
			p->pent = NULL;
		}
	}
	remove_proc_entry(TEST_MCK_PROCFS_DIRNAME, NULL);
	test_mck_procdir = NULL;
}

static int test_mck_init_procfs(void)
{
	int i;

	test_mck_procdir = proc_mkdir(TEST_MCK_PROCFS_DIRNAME, NULL);
	if (test_mck_procdir == NULL) {
		return -ENOMEM;
	}
	for (i = 0; i < sizeof(proc_ents)/sizeof(proc_ents[0]); i++) {
		struct test_mck_procfs_entry* p = &proc_ents[i];
		p->pent = proc_create_data(p->fname, S_IRUGO, test_mck_procdir,
					   &p->ops, NULL);
		if (p->pent == NULL) {
			return -ENOMEM;
		}
	}
	return 0;
}

static void test_mck_cleanup(void)
{
	int i;
	dev_t devno;

	/* procfs */
	test_mck_cleanup_procfs();

	/* cdev */
	for (i = 0; i < test_mck_nr_devs; i++) {
		struct test_mck_dev* dev = &test_mck_devlist[i];
		dev->fin(dev);
		cdev_del(&dev->cdev);
		device_destroy(dev->class, dev->devno);
		class_destroy(dev->class);
	}
	devno = MKDEV(test_mck_major, test_mck_minor);

	/* devno */
	unregister_chrdev_region(devno, test_mck_nr_devs);
}

static int test_mck_setup(void)
{
        int res;
        dev_t devno;
	int i;
	char* majmin;

	/* devno */
	res = alloc_chrdev_region(&devno, test_mck_minor,
				  test_mck_nr_devs,
				  TEST_MCK_DEVNAME);
        if (res < 0) {
		mck_printk("error get major %d\n", res);
		goto fail;
        }
	test_mck_major = MAJOR(devno);

	/* cdev */
	majmin = test_mck_majmin;
	for (i = 0; i < test_mck_nr_devs; i++) {
		int print;
		struct test_mck_dev* dev = &test_mck_devlist[i];
		dev->class = class_create(THIS_MODULE, dev->ts_name);
		if (IS_ERR(dev->class)) {
			dev->class = NULL;
			mck_printk("error create class(%s).",
				   dev->ts_name);
			goto fail;
		}

		cdev_init(&dev->cdev, dev->fops);
		dev->devno = MKDEV(test_mck_major, test_mck_minor + i);
		dev->cdev.owner = THIS_MODULE;
		res = cdev_add(&dev->cdev, dev->devno, 1);
		if (res) {
			mck_printk("error %d adding mck test driver(%s).",
				   res, dev->ts_name);
			goto fail;
		}

		device_create(dev->class, NULL, dev->devno, NULL, dev->ts_name);

		print = snprintf(majmin,
				 sizeof(test_mck_majmin) - (majmin - test_mck_majmin) - 1,
				 "%3d:%-3d ", test_mck_major, i);
		if (0 <= print) {
			majmin += print;
		}

		res = dev->init(dev);
		if (res) {
			goto fail;
		}
	}

	/* procfs */
	res = test_mck_init_procfs();
	if (res) {
		mck_printk("error %d create procfs.", res);
		goto fail;
	}
	return 0;
fail:
	test_mck_cleanup();
        return res;
}

static int test_mck_init(void)
{
	return test_mck_setup();
}

static void test_mck_exit(void)
{
	test_mck_cleanup();
}

module_init(test_mck_init);
module_exit(test_mck_exit);
