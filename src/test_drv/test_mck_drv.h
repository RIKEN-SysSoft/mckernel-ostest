/* test_mck_drv.h COPYRIGHT FUJITSU LIMITED 2015-2016 */

#ifndef __MCK_TEST_DRV_H__
#define __MCK_TEST_DRV_H__

#include <linux/cdev.h>
#include "test_mck_public.h"
#define TEST_MCK_DEVNAME "test_mck"

/* logger */
const char* test_mck_ident(void);
#define mck_printk(fmt, ...) \
  printk("%s::%s(%s):" fmt, TEST_MCK_DEVNAME, TEST_MCK_TS_NAME, test_mck_ident(), ##__VA_ARGS__)

/* device identifier */
#define TEST_MCK_DRV_TEST_SUITE(name) test_mck ## name,
enum test_mck_nr_devs {
#include "test_suite.list"
	test_mck_nr_devs,
};
#undef TEST_MCK_DRV_TEST_SUITE

/* device */
struct test_mck_dev {
	const char* const ts_name; /* test suite name */
	const struct file_operations* fops;
	int (*init)(struct test_mck_dev *dev);
	void (*fin)(struct test_mck_dev *dev);
	void* private_data;
        dev_t devno;
	struct cdev cdev;
	struct class *class;
};
extern const struct test_mck_dev* test_mck_lookup_devlist(void);

#endif /*__MCK_TEST_DRV_H__*/

