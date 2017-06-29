#ifndef __TEST_MCK_IOCTL_H__
#define __TEST_MCK_PUBLIC_H__

#include <linux/ioctl.h>
#define TEST_MCK_IOC_MAGIC 'M'
#define TEST_MCK_MMAP_DEV2_BUF_RESET _IO(TEST_MCK_IOC_MAGIC, 0)

#endif /*__TEST_MCK_PUBLIC_H__*/
