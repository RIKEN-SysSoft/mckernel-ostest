if [ "X$runHOST" = Xyes ]; then
    swapon -a
fi

echo "ulimit -s [initial: (${initial_ulimit_orig})]"
ulimit -s ${initial_ulimit_orig}

echo "ulimit -c ${ulimit_c_orig}"
ulimit -c ${ulimit_c_orig}

