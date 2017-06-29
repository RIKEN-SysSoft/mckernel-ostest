/* test_mck_drv-mmap_dev2.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include "test_mck_drv.h"

#define TEST_MCK_TS_NAME "mmap_dev2"

const unsigned long test_mck_mmap_dev2_buf_size = PAGE_SIZE * 3;

/* vma ops */
static void test_mck_mmap_dev2_vma_open(struct vm_area_struct *vma)
{
	mck_printk("call vm_open function.\n");
}

static void test_mck_mmap_dev2_vma_close(struct vm_area_struct *vma)
{
	mck_printk("call vm_close function.\n");
}

static int test_mck_mmap_dev2_vma_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct test_mck_dev *dev;
	unsigned long buf;
	mck_printk("call vm_fault function.\n");

	dev = vma->vm_private_data;
	buf = (unsigned long)dev->private_data;
	if (!buf) {
		mck_printk("buffer is NULL.\n");
		return -ENOMEM;
	}

	if (test_mck_mmap_dev2_buf_size <= (vmf->pgoff << PAGE_SHIFT)) {
		mck_printk("invalid page offset.\n");
		return -EFAULT;
	}
	vmf->page = vmalloc_to_page((void*)(buf + (vmf->pgoff << PAGE_SHIFT)));
	get_page(vmf->page);

	return 0;
}

static struct vm_operations_struct test_mck_mmap_dev2_vm_ops = {
	.open = test_mck_mmap_dev2_vma_open,
	.close = test_mck_mmap_dev2_vma_close,
	.fault = test_mck_mmap_dev2_vma_fault,
};

/* file ops */
static int test_mck_mmap_dev2_open(struct inode *inode, struct file *file)
{
	struct test_mck_dev *dev;
	mck_printk("call open function.\n");

	dev = container_of(inode->i_cdev, struct test_mck_dev, cdev);
	file->private_data = dev;
	//mck_printk("file->private_data(%p).\n", file->private_data);
	return 0;
}

static int test_mck_mmap_dev2_release(struct inode *inode, struct file *file)
{
	mck_printk("call release function.\n");
	return 0;
}

static int test_mck_mmap_dev2_mmap(struct file *file, struct vm_area_struct *vma)
{
	int res = 0;
	mck_printk("call mmap function.(vm_start=%lx, vm_end=%lx, vm_pgoff==%lx)\n",
		   vma->vm_start, vma->vm_end, vma->vm_pgoff);

	vma->vm_private_data = file->private_data;
	//mck_printk("vma->vm_private_data(%p).\n", vma->vm_private_data);
	vma->vm_ops = &test_mck_mmap_dev2_vm_ops;
	if (vma->vm_ops->open) {
		vma->vm_ops->open(vma);
	}

	mck_printk("return mmap function(%d).\n", res);
	return res;
}

static void test_mck_mmap_dev2_buffer_reset(struct test_mck_dev* dev)
{
	int val;
	unsigned long start, off, len;

	val = 1;
	start = (unsigned long)dev->private_data;
	for (off = 0; off < test_mck_mmap_dev2_buf_size; off += PAGE_SIZE) {
		if (off + PAGE_SIZE < test_mck_mmap_dev2_buf_size) {
			len = PAGE_SIZE;
		} else {
			len = test_mck_mmap_dev2_buf_size - off;
		}
		memset((void*)(start + off), val, len);
		val++;
	}
}

static long test_mck_mmap_dev2_ioctl(struct file *file, unsigned int iocmd, unsigned long ioarg)
{
	int res = 0;
	mck_printk("call ioctl function(cmd:%u arg:%lu).\n",
		   iocmd, ioarg);

	switch (iocmd) {
	case TEST_MCK_MMAP_DEV2_BUF_RESET:
		test_mck_mmap_dev2_buffer_reset((struct test_mck_dev*)file->private_data);
		break;
	}
	return res;
}

struct file_operations test_mck_mmap_dev2_fops = {
	.owner   = THIS_MODULE,
	.release =  test_mck_mmap_dev2_release,
	.mmap    =  test_mck_mmap_dev2_mmap,
	.unlocked_ioctl = test_mck_mmap_dev2_ioctl,
	.open    =  test_mck_mmap_dev2_open,
};

int test_mck_mmap_dev2_init(struct test_mck_dev* dev)
{
	int ret = 0;

	dev->private_data = vmalloc_user(test_mck_mmap_dev2_buf_size);
	mck_printk("dev(%p) buffer(%p).\n", dev, dev->private_data);

	if (!dev->private_data) {
		ret = -ENOMEM;
		goto out;
	}
	test_mck_mmap_dev2_buffer_reset(dev);
out:
	return ret;
}

void test_mck_mmap_dev2_fin(struct test_mck_dev* dev)
{
	if (dev->private_data) {
		vfree(dev->private_data);
		dev->private_data = NULL;
	}
}
