/* main.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __MAIN_H__
#define __MAIN_H__

#define TASK_UNMAPPED_BASE 0xFFFFFF7FC0000000UL

#define _4MB  (1024UL * 1024UL * 4UL)
#define _256MB  (1024UL * 1024UL * 256UL)
#define _1GB  (1024UL * 1024UL * 1024UL)

#define ALIGN_UP(a, b)  ( ((a)+(b)-1) & ~((b)-1) )

#endif /*__MAIN_H__*/
