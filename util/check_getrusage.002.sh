. ${AUTOTEST_HOME}/ostest/util/check_getrusage.common.sh

if [ "${runHOST}" != "yes" ]; then
	rc=0


	# check parent usage
	getrusage_section $WORKDIR/output/ostest-getrusage.002.output parent child

	# ru_utime should be near-zero, near-zero
	getrusage_time ru_utime /tmp/output.txt 1
	if [ "$ret" -gt "1000" ]; then
		rc=1
	fi

	# ru_stime should be near-zero, near-four-second
	getrusage_time ru_stime /tmp/output.txt 1
	if [ "$ret" -lt "`expr 4000000 / 10 \* 9`" ]; then
		rc=1
	fi

	# ru_maxrss should be zero, >16384
	getrusage_maxrss ru_maxrss /tmp/output.txt 1
	if [ "$ret" -lt "`expr 16384 / 10 \* 9`" ]; then
		rc=1
	fi

	# check child usage
	getrusage_section $WORKDIR/output/ostest-getrusage.002.output child parent

	# ru_utime should be zero, near-two-second
	getrusage_time ru_utime /tmp/output.txt 1
	if [ "$ret" -lt "`expr 2000000 / 10 \* 9`" ]; then
		rc=1
	fi

	# ru_stime should be zero, near-two-second
	getrusage_time ru_stime /tmp/output.txt 1
	if [ "$ret" -lt "`expr 2000000 / 10 \* 9`" ]; then
		rc=1
	fi

	# ru_maxrss should be X, X+16384
	getrusage_maxrss ru_maxrss /tmp/output.txt 1
	if [ "$ret" -lt "`expr 16384 / 10 \* 9`" ]; then
		rc=1
	fi
fi
echo $rc > $WORKDIR/result.log
