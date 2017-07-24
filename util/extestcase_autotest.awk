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
    autotest_home = "/work/mcktest";
    testcasedir = sprintf("%s/data/script", autotest_home); 
    testlistfile = sprintf("%s/data/ostest-testlist", autotest_home);
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
	testname == "force_exit") {
	existScript = 1;
    } else {
	existScript = 0;
    }
    initialized = 1;
}

!/^##/ {
    testscript = sprintf("%s/ostest-%s.%03d", testcasedir, testname, count);
    outputfile = sprintf("$WORKDIR/ostest-%s.%03d.output", testname, count);
    outputfile_host = sprintf("$DATADIR/linux/ostest-%s.%03d.output", testname, count);
    workdir2 = sprintf("$WORKDIR/ostest-%s.%03d", testname, count);
    workdir2_host = sprintf("$DATADIR/linux/ostest-%s.%03d", testname, count);
    print "#!/bin/sh"  > testscript;
    printf("export AUTOTEST_HOME=%s\n", autotest_home) >> testscript;
    printf(". $AUTOTEST_HOME/bin/config.sh\n") >> testscript;
    printf("cd $AUTOTEST_HOME/ostest/util\n\n") >> testscript;

    append_testscript("before_run_testcase.sh");
    printf("\necho \"## %s ##\"\n\n", testname) >> testscript;
    printf("testcase=%s.%03d\n", testname, count) >> testscript;
    printf("testno=%d\n", count) >> testscript;
    printf("if [ \"${runHOST}\" != \"yes\" ]; then\n") >> testscript;
    printf("	outputfile=%s\n", outputfile) >> testscript;
    printf("	workdir=%s\n", workdir2) >> testscript;
    printf("else\n") >> testscript;
    printf("	outputfile=%s\n", outputfile_host) >> testscript;
    printf("	workdir=%s\n", workdir2_host) >> testscript;
    printf("fi\n") >> testscript;
    printf("mkdir -p $workdir\n") >> testscript;
    printf("cd $workdir\n") >> testscript;
    if (existScript) {
	append_testscript("before_" testname ".sh");
    }
    if ((testname == "siginfo" && count == 1) || testname == "force_exit") {
	printf("%s > $outputfile &\n", $0)  >> testscript;
    } else {
	printf("%s > $outputfile\n", $0)  >> testscript;
    }

    if (existScript) {
	append_testscript("after_" testname ".sh");
    }

    printf("if [ \"${runHOST}\" != \"yes\" ]; then\n") >> testscript;
    printf("	core_linux=`ls %s | wc -l`\n", workdir2_host) >> testscript;
    printf("	core_mck=`ls %s | wc -l`\n", workdir2) >> testscript;
    printf("	nl_linux=`wc -l %s | cut -d ' ' -f 1`\n", outputfile_host) >> testscript;
    printf("	nl_mck=`wc -l %s | cut -d ' ' -f 1`\n", outputfile) >> testscript;
    printf("	result_linux=`awk -F ':' '$1=="RESULT" {print $2}' %s`\n", outputfile_host) >> testscript;
    printf("	result_mck=`awk -F ':' '$1=="RESULT" {print $2}' %s`\n", outputfile) >> testscript;
    printf("	if [ $core_linux -eq $core_mck ] && [ $nl_linux -eq $nl_mck ] && [ $result_linux -eq $result_mck ]; then\n") >> testscript;
    printf("		rc=0\n") >> testscript;
    printf("	else\n") >> testscript;
    printf("		rc=1\n") >> testscript;
    printf("	fi\n") >> testscript;
    printf("fi\n") >> testscript;

    append_testscript("after_run_testcase.sh");
    system("chmod +x " testscript);

    printf("$DATADIR/script/ostest-%s.%03d\n", testname, count) >> testlistfile;
    count++;
}
