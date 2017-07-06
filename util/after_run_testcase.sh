
	#### finalize ####

	echo "rmmod test_drv"
	sh "$rmmod_test_drv_sh"

	#### host output corefile-name setting restore ####
	echo "restore $orig_core_pattern => /proc/sys/kernel/core_pattern"
	echo $orig_core_pattern > /proc/sys/kernel/core_pattern

	#### console output setting restore ####
	echo "restore $orig_printk_setting => /proc/sys/kernel/printk"
	echo $orig_printk_setting > /proc/sys/kernel/printk


	rm $ostype_name
	rm $pid_max_name
	rm $link
	rm $temp
	rm $mmapfile_name
	if [ "$addusr" -eq 1 ]; then
		userdel $test_user_name --remove
	fi
