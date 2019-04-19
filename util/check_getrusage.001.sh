. ${AUTOTEST_HOME}/ostest/util/check_getrusage.common.sh

if [ "${runHOST}" != "yes" ]; then
	rc=0
	output=${WORKDIR}/output/ostest-getrusage.001.output

	# ru_utime should be zero, zero, near-two-second
	getrusage_time ru_utime $output 1
	if [ "$ret" != "0" ]; then
		rc=1
	fi

	getrusage_time ru_utime $output 2
	if [ "$ret" -lt "`expr 2000000 / 10 \* 9`" ]; then
		rc=1
	fi

	# ru_stime should be zero, zero, near-two-second
	getrusage_time ru_stime $output 1
	if [ "$ret" != "0" ]; then
		rc=1
	fi

	getrusage_time ru_stime $output 2
	if [ "$ret" -lt "`expr 2000000 / 10 \* 9`" ]; then
		rc=1
	fi

	# ru_maxrss should be zero, zero, near-16384
	getrusage_maxrss ru_maxrss $output 1
	if [ "$ret" != "0" ]; then
		rc=1
	fi

	getrusage_maxrss ru_maxrss $output 2
	if [ "$ret" -lt "`expr 16384 / 10 \* 9`" ]; then
		rc=1
	fi
fi
echo $rc > $WORKDIR/result.log
