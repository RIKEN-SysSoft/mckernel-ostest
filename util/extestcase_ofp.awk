#!/usr/bin/awk -f

function append_testscript(filename) {
    command = "cat " filename;
    while ((command | getline var) > 0) {
	print var >> testscript;
    }
    close(command);
}


BEGIN { 
    "pwd -P" | getline cwd;
    testcasedir = sprintf("%s/%s", cwd, "testcase_ofp"); 
    outputdir = sprintf("%s/%s", cwd, "output_ofp");
    workdir = sprintf("%s/%s", cwd, "work_ofp");
    testlistfile = sprintf("%s/testlist_ofp", cwd);
    system("rm -f " testlistfile);
    system("rm -rf " testcasedir);
    system("rm -rf " outputdir);
    system("rm -rf " workdir);
    system("mkdir -p " testcasedir);
    system("mkdir -p " outputdir);
    system("mkdir -p " workdir);
    testname = "#";
    flagSkip = 0;
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

!/^##/ && flagSkip != 1 {
    testscript = sprintf("%s/%s.%03d", testcasedir, testname, count);
    outputfile = sprintf("%s/%s.%03d", outputdir, testname, count);
    system("mkdir -p " workdir "/" testname);
    print "#!/bin/sh"  > testscript;
    append_testscript("before_run_testcase_ofp.sh");
    printf("\necho \"## %s ##\"\n\n", testname) >> testscript;
    printf("testcase=%s.%03d\n", testname, count) >> testscript;
    printf("testno=%d\n", count) >> testscript;
    printf("cd " workdir "/" testname) >> testscript;
    if (existScript) {
	append_testscript("before_" testname ".sh");
    }
    if ((testname == "siginfo" || testname == "force_exit") && count == 1) {
	printf("%s > %s &\n", $0, outputfile)  >> testscript;
    } else {
	printf("%s > %s\n", $0, outputfile)  >> testscript;
    }
    if (existScript) {
	append_testscript("after_" testname ".sh");
    }
    append_testscript("after_run_testcase_ofp.sh");
    system("chmod +x " testscript);

    print testscript >> testlistfile;
    count++;
}
