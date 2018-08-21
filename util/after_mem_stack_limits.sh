if [ "X$runHOST" = Xyes ]; then
    swapon -a
fi

echo "ulimit -s [initial: (${initial_ulimit_orig})]"
ulimit -s ${initial_ulimit_orig}

