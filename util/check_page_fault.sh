if [ "${runHOST}" != "yes" ]; then
	sleep 3
	$MCKINSTALL/sbin/ihkosctl 0 kmsg > /tmp/autotest-kmsg.txt
	fgrep page_fault_handler /tmp/autotest-kmsg.txt > /dev/null
	if [ $? -eq 0 ]; then
		rc=0
	else
		rc=1
	fi
fi
echo $rc > $WORKDIR/result.log
