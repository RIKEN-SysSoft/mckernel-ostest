
	rc=0
	if [ $core_linux -ne $core_mck ]; then
		echo "Core file counts do not match ($core_linux vs. $core_mck)"
		rc=1
	fi

	if [ $nl_linux -ne $nl_mck ]; then
		echo "Numbers of lines of log do not match ($nl_linux vs. $nl_mck)"
		rc=1
	fi

	if ! echo $result_linux | grep -cE '^[0-9]$' > /dev/null; then
		echo "internal error: Custom check needed. Consider adding check_<test_name>.<test_num>.sh"
		rc=1
	fi

	if [ "$result_linux" != "$result_mck" ]; then
		echo "Exit statuses do not match ($result_linux vs. $result_mck)"
		rc=1
	fi
fi
echo $rc > $WORKDIR/result.log
