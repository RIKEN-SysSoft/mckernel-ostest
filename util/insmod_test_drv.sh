#!/bin/sh
# insmod_test_drv_x86.sh COPYRIGHT FUJITSU LIMITED 2016
E=

this_dir="$(cd $(dirname $0); pwd)"

app_dir=${this_dir}/../bin
test_mck_ko=${this_dir}/../bin/test_mck.ko
test_mck_proc_devno=/proc/test_mck/devno
test_mck_proc_tp_names=/proc/test_mck/tp_names
test_mck_device_dir=/dev/test_mck

usage()
{
  echo "$0 [-i ident] [-h]"
  echo "  options:"
  echo "    -i  identifier for the syslog."
  echo "    -h  show usage."
}

ident=
while getopts i:h OPT
do
    case $OPT in
        i)  ident=$OPTARG
            ;;
        h)  usage; exit 0
            ;;
        \?) usage; exit 1
            ;;
    esac
done

#
# check path
#
if [ ! -d "$app_dir" ]; then
  echo "test app directory not found.($app_dir)"
  exit 1
fi

#
# insmod
#
insmod "$test_mck_ko" ident="$ident"
echo "insmod $test_mck_ko"

#
# create device files
#
devnos=`cat "$test_mck_proc_devno"`
devno_ents=`echo $devnos | awk '{print NF}'`

names=`cat "$test_mck_proc_tp_names"`
names_ents=`echo $names | awk '{print NF}'`

$E mkdir -p "$test_mck_device_dir"
for i in `seq 1 $devno_ents`
do
	maj=`echo $devnos | cut -d' ' -f$i | sed 's/:[0-9]*//g'`
	min=`echo $devnos | cut -d' ' -f$i | sed 's/[0-9]*://g'`
	if [ $i -gt  $names_ents ]; then
		devname="test_mck$min"
	else
		devname=`echo $names | cut -d' ' -f$i`
	fi
	path="$test_mck_device_dir/$devname"
	$E mknod "$path" c $maj $min
	echo "create charcter device $path($maj:$min)"
done
