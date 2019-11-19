if [ $testcase = "siginfo.001" ]; then
    sleep 3
    siginfo_send_signal `${pidofcomm}`
    sleep 1
fi
