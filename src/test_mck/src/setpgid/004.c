/* 004.c COPYRIGHT FUJITSU LIMITED 2015 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>

static void child_execve(struct setpgid_args* args)
{       
        char **argv;
        extern char **environ;
        int i = 0;

        argv = malloc(args->argc * (sizeof(char **) + 2));
        argv[0] = args->file_path;
        for (i = 0; i < args->argc; i++) {
                argv[i + 1] = *(args->argv + i);
        }
        argv[i + 1] = NULL;

        execve(args->file_path, argv, environ);
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
        struct setpgid_args* args;
        args = setpgid_parse_args(tc_argc, tc_argv);
        return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
        struct setpgid_args* args;
        pid_t child_pid;

        args = tc_arg;
        tp_assert(args != NULL, "invalid arguement.");
        tp_assert(args->file_path != NULL, "invalid file path.");

        child_pid = fork();
        switch (child_pid) {
        case -1:
                /* error */
                printf("ERROR: fork()=%d, errno=%d\n", child_pid, errno);
                break;
        case 0:
                /* child process */
                child_execve(args);
                tp_assert(0, "Why return from execve() ???");
                break;
        default:
		{
			int result;
			int _errno;
			/* parent process */
			while (1) {
				int wait_id;
				int wait_status;

				/* set tgid */
				errno = 0;
				result = setpgid(child_pid, child_pid);
				if (result != 0) {
					_errno = errno;
					break;
				}
				/* check child process. */
				wait_id = waitpid(child_pid, &wait_status, WNOHANG);
				if (0 < wait_id) {
					if (WIFEXITED(wait_status) || WIFSIGNALED(wait_status)) {
						tp_assert(NULL, "child process exited.");
					}
				}
			}
			printf("setpgid result:%d, errno:%d (expect error is \"EACCES\"=%d)\n", result, _errno, EACCES);
			tp_assert(_errno == EACCES, "invalid errno.");
			tp_assert(result == -1, "Why setpgid didn't failed!?");
			waitpid(child_pid, NULL, 0);
		}
                break;
        }
        return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, 0)
