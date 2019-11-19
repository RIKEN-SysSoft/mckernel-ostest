rm -rf /tmp/ostest
mkdir -p /tmp/ostest
temp=/tmp/ostest/tempfile
dd if=/dev/zero of=${temp} bs=1M count=10
