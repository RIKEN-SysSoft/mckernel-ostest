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
    "dirname " ARGV[0] | getline dir;
    "cd " dir "/../.. && pwd -P" | getline autotest_home;
    testcasedir = sprintf("%s/%s", cwd, "testcase_linux"); 
    outputdir = sprintf("%s/data/linux", autotest_home);
    workdir = sprintf("%s/data/linux", autotest_home);
    testlistfile = sprintf("%s/testlist_linux", cwd);
    system("rm -f " testlistfile);
    system("rm -rf " testcasedir);
    system("rm -rf " outputdir);
    system("rm -rf " workdir);
    system("mkdir -p " testcasedir);
    system("mkdir -p " outputdir);
    system("mkdir -p " workdir);
    testname = "#";
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
	testname == "force_exit" ||
	testname == "mem_limits") {
	existScript = 1;
    } else {
	existScript = 0;
    }
    initialized = 1;
}

!/^##/ {
    testscript = sprintf("%s/%s.%03d", testcasedir, testname, count);
    outputfile = sprintf("%s/ostest-%s.%03d.output", outputdir, testname, count);
    workdir2 = sprintf("%s/ostest-%s.%03d", workdir, testname, count);
    system("mkdir -p " workdir2);
    print "#!/bin/sh"  > testscript;
    append_testscript("before_run_testcase.sh");
    printf("\necho \"## %s ##\"\n\n", testname) >> testscript;
    printf("testcase=%s.%03d\n", testname, count) >> testscript;
    printf("testno=%d\n", count) >> testscript;
    printf("cd " workdir2 "\n") >> testscript;
    if (existScript) {
	append_testscript("before_" testname ".sh");
    }
    if ((testname == "siginfo" && count == 1) || (testname == "force_exit" && count == 0)) {
	printf("%s > %s &\n", $0, outputfile)  >> testscript;
    } else {
	printf("%s > %s\n", $0, outputfile)  >> testscript;
    }
    if (existScript) {
	append_testscript("after_" testname ".sh");
    }
    append_testscript("after_run_testcase.sh");
    system("chmod +x " testscript);

    print testscript >> testlistfile;
    count++;
}
