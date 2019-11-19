. ${AUTOTEST_HOME}/ostest/util/check/getrusage.common.sh

if [ "${linux_run}" != "yes" ]; then
	rc=0

	# ru_utime should be near-zero, near-two-second, near-two-second
	getrusage_time ru_utime $recordfile 1
	if [ "$ret" -lt "`expr 2000000 / 10 \* 9`" ]; then
		rc=1
	fi

	getrusage_time ru_utime $recordfile 2
	if [ "$ret" -gt 1000 ]; then
		rc=1
	fi

	# ru_stime should be near-zero, near-two-second, near-two-second
	getrusage_time ru_stime $recordfile 1
	if [ "$ret" -lt "`expr 2000000 / 10 \* 9`" ]; then
		rc=1
	fi

	getrusage_time ru_stime $recordfile 2
	if [ "$ret" -gt 1000 ]; then
		rc=1
	fi

	# ru_maxrss should be X, X + 16384, X + 16384
	getrusage_maxrss ru_maxrss $recordfile 1
	if [ "$ret" -lt "`expr 16384 / 10 \* 9`" ]; then
		rc=1
	fi

	getrusage_maxrss ru_maxrss $recordfile 2
	if [ "$ret" != "0" ]; then
		rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
