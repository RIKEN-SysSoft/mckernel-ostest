#!/usr/bin/awk -f

BEGIN { 
    exclude = 0;
}

/^##/ {
    testname = $2;
    count = 0;
    if (testname == "siginfo" ||
	testname == "mem_stack_limits" ||
	testname == "mmap_populate"  ||
	testname == "mmap_file" ||
	testname == "sched_getaffinity" ||
	testname == "mmap_locked" ||
	testname == "madvise" ||
	testname == "wait4" ||
	testname == "lv07" ||
	testname == "lv09" ||
	testname == "lv11" ||
	testname == "lv15" ||
	testname == "readlinkat" ||
	testname == "force_exit") {
	existScript = 1;
    } else {
	existScript = 0;
    }
    initialized = 1;
    if (testname == "mmap_dev" ||
	testname == "getegid" ||
	testname == "geteuid" ||
	testname == "getgid" ||
	testname == "getresgid" ||
	testname == "getresuid" ||
	testname == "getuid" ||
	testname == "setfsgid" ||
	testname == "setfsuid" ||
	testname == "setgid" ||
	testname == "setregid" ||
	testname == "setresgid" ||
	testname == "setresuid" ||
	testname == "setreuid" ||
	testname == "setuid" ||
	testname == "force_exit") {
	flagSkip = 1;
    } else {
	flagSkip = 0;
    }
}

!/^##/ && flagSkip == 1 {
    exclude++
}

END {
    print exclude
}
