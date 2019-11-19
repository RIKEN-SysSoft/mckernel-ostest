. ${AUTOTEST_HOME}/ostest/util/check/getrusage.common.sh

if [ "${linux_run}" != "yes" ]; then
	rc=0

	# check parent usage
	getrusage_section $recordfile parent child ${recorddir}/rusage.txt

	# ru_utime should be near-zero, near-zero
	getrusage_time ru_utime ${recorddir}/rusage.txt 1
	if [ "$ret" -gt "1000" ]; then
		rc=1
	fi

	# ru_stime should be near-zero, near-four-second
	getrusage_time ru_stime ${recorddir}/rusage.txt 1
	if [ "$ret" -lt "`expr 4000000 / 10 \* 9`" ]; then
		rc=1
	fi

	# ru_maxrss should be zero, >16384
	getrusage_maxrss ru_maxrss ${recorddir}/rusage.txt 1
	if [ "$ret" -lt "`expr 16384 / 10 \* 9`" ]; then
		rc=1
	fi

	# check child usage
	getrusage_section $recordfile child parent ${recorddir}/rusage.txt

	# ru_utime should be zero, near-two-second
	getrusage_time ru_utime ${recorddir}/rusage.txt 1
	if [ "$ret" -lt "`expr 2000000 / 10 \* 9`" ]; then
		rc=1
	fi

	# ru_stime should be zero, near-two-second
	getrusage_time ru_stime ${recorddir}/rusage.txt 1
	if [ "$ret" -lt "`expr 2000000 / 10 \* 9`" ]; then
		rc=1
	fi

	# ru_maxrss should be X, X+16384
	getrusage_maxrss ru_maxrss ${recorddir}/rusage.txt 1
	if [ "$ret" -lt "`expr 16384 / 10 \* 9`" ]; then
		rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
