/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h> 

struct setpgid_args {
        char *file_path;
        char **argv;
        int argc;
};      
                
/* setpgid.c */
void* setpgid_parse_args(int argc, char** argv);
 
#endif /*__TEST_SUITE_H__*/
