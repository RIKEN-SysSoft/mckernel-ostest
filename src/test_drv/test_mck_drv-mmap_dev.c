/* test_mck_drv-mmap_dev.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/mm.h>
#include "test_mck_drv.h"

#define TEST_MCK_TS_NAME "mmap_dev"

/* vma ops */
static void test_mck_mmap_dev_vma_open(struct vm_area_struct *vma)
{
	mck_printk("call vm_open function.\n");
}

static void test_mck_mmap_dev_vma_close(struct vm_area_struct *vma)
{
	mck_printk("call vm_close function.\n");
}

static struct vm_operations_struct test_mck_mmap_dev_vm_ops = {
	.open = test_mck_mmap_dev_vma_open,
	.close = test_mck_mmap_dev_vma_close,
};

/* file ops */
static int test_mck_mmap_dev_open(struct inode *inode, struct file *file)
{
	mck_printk("call open function.\n");
	return 0;
}

static int test_mck_mmap_dev_release(struct inode *inode, struct file *file)
{
	mck_printk("call release function.\n");
	return 0;
}

static int test_mck_mmap_dev_mmap(struct file *file, struct vm_area_struct *vma)
{
	int res;
	mck_printk("call mmap function.(vm_start=%lx, vm_end=%lx, vm_pgoff==%lx)\n",
		   vma->vm_start, vma->vm_end, vma->vm_pgoff);
	res = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
			      vma->vm_end - vma->vm_start,
			      vma->vm_page_prot);
	if (res) {
		goto out;
	}
	vma->vm_ops = &test_mck_mmap_dev_vm_ops;
	if (vma->vm_ops->open) {
		vma->vm_ops->open(vma);
	}
out:
	mck_printk("return mmap function(%d).\n", res);
	return 0;
}

//static int test_mck_mmap_dev_ioctl(struct inode *inode, struct file *file,
//		   unsigned int iocmd, unsigned long ioarg)
static long test_mck_mmap_dev_ioctl(struct file *file, unsigned int iocmd, unsigned long ioarg)
{
	mck_printk("call ioctl function(cmd:%u arg:%lu).\n",
		   iocmd, ioarg);
	return 0;
}

struct file_operations test_mck_mmap_dev_fops = {
	.owner   = THIS_MODULE,
	.release =  test_mck_mmap_dev_release,
	.mmap    =  test_mck_mmap_dev_mmap,
	//.ioctl   =  test_mck_mmap_dev_ioctl,
	.unlocked_ioctl = test_mck_mmap_dev_ioctl,
	.open    =  test_mck_mmap_dev_open,
};

int test_mck_mmap_dev_init(struct test_mck_dev* dev)
{
	int ret = 0;
	return ret;
}

void test_mck_mmap_dev_fin(struct test_mck_dev* dev)
{
}
