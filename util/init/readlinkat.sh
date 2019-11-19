rm -rf /tmp/ostest
mkdir -p /tmp/ostest
temp=/tmp/ostest/tempfile
link=/tmp/ostest/templink
dd if=/dev/zero of=${temp} bs=1M count=10
ln -s ${temp} ${link}
