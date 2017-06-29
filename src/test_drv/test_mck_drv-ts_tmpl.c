/* test_mck_drv-ts_tmpl.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include <linux/fs.h>
#include "test_mck_drv.h"

#define TEST_MCK_TS_NAME "ts_tmpl"

/* file ops */
static int test_mck_ts_tmpl_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int test_mck_ts_tmpl_release(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations test_mck_ts_tmpl_fops = {
	.owner   = THIS_MODULE,
	.release =  test_mck_ts_tmpl_release,
	.open    =  test_mck_ts_tmpl_open,
};
