rm -rf /tmp/ostest
mkdir -p /tmp/ostest
org_pid_max=/proc/sys/kernel/pid_max
pid_max_name=/tmp/ostest/pid_max
cat $org_pid_max > $pid_max_name


