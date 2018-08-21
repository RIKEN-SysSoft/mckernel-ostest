initial_ulimit_orig=`ulimit -s`
echo "ulimit -s 10MiB (10240 KiB)"
ulimit -s 8192

if [ $testno -ge 1 ]; then
    if [ $mck_max_mem_size -ge 2244120412 ]; then
	echo "ulimit -s 2GiB (2097152 KiB)"
	ulimit -s 2097152
    else
	echo "## mem_stack_limits 2GiB SKIP ##"
    fi
fi

if [ $testno -ge 2 ]; then
    echo "ulimit -s unlimited"
    ulimit -s unlimited
fi

if [ "X$runHOST" = Xyes ]; then
    swapoff -a
fi
