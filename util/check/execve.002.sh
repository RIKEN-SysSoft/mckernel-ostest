if [ "${linux_run}" != "yes" ]; then
	rc=0

	if ! grep 'TEST_SUITE: env' $recordfile > /dev/null; then
	    rc=1
	fi

	if ! grep 'TEST_NUMBER: 0' $recordfile > /dev/null; then
	    rc=1
	fi

	echo $rc > $WORKDIR/result.log
fi
