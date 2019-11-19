sleep 3
echo "send SIGKILL for mcexec."
kill -9 `${pidofcomm}`

rm -rf /tmp/ostest

