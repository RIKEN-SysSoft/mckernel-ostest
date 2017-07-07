this_dir="$(cd $(dirname $0); pwd)/.."

# memsize
total_mem=`free -m | grep Mem: | awk '{print $2}'`
mem_size_def=$(( $total_mem - ($total_mem * (100 - 45) / 100)))
boot_mem="${mem_size_def}M@0"

# path
install_dir=${this_dir}/../install
mcexec=${install_dir}/bin/mcexec
app_dir=${this_dir}/../bin
mcreboot=${install_dir}/sbin/mcreboot.sh
mcstop=${install_dir}/sbin/mcstop+release.sh
ihkosctl=${install_dir}/sbin/ihkosctl
insmod_test_drv_sh=${this_dir}/insmod_test_drv.sh
rmmod_test_drv_sh=${this_dir}/rmmod_test_drv.sh

export TEST_HOME=${app_dir}

# test file
temp=$this_dir/tempfile
link=/tmp/templink
mmapfile_name=$this_dir/mmapfile
ostype_name=$this_dir/ostype
org_pid_max=/proc/sys/kernel/pid_max
pid_max_name=$this_dir/pid_max

# loop counter
count=0

# test user
test_user_name="temp_mck_test"

# for siginfo TP send signal.
signal_name="HUP INT QUIT ILL TRAP ABRT EMT FPE KILL BUS SEGV SYS PIPE ALRM TERM URG STOP TSTP CONT CHLD TTIN TTOU IO XCPU XFSZ VTALRM PROF WINCH USR1 USR2"

siginfo_send_signal() {
  local pid=$1
  for sig in $signal_name ; do
    if [ ${sig} == "KILL" ] ; then
      continue
    elif [ ${sig} == "STOP" ] ; then
      continue
    elif [ ${sig} == "TSTP" ] ; then
      continue
    elif [ ${sig} == "CONT" ] ; then
      continue
    elif [ ${sig} == "CHLD" ] ; then
      continue
    elif [ ${sig} == "TTIN" ] ; then
      continue
    elif [ ${sig} == "TTOU" ] ; then
      continue
    fi
    echo -e "send SIG${sig} to ${pid}"
    kill -${sig} ${pid}
    sleep 1
  done
  kill ${pid}
}

# parse parameter
usage()
{
  echo "$0 [-N] [-H] [-e] [-d] [-h]"
  echo "  options:"
  echo "    -N  NG/HANG item including run. (default off)"
  echo "    -H  run on host. (default off)"
  echo "    -e  use execve test. (default off)"
  echo "    -d  dryrun including NG/HANG items"
  echo "    -h  show usage."
}

# option check
execve_comm=
execve_arg_end=
app_prefix=$app_dir
mck_max_mem_size=
mck_max_cpus=`cat /proc/cpuinfo | grep -c "processor"`
mck_max_cpus=`expr $mck_max_cpus - 1`
HANG=":"
NG=":"
incNH=
runHOST=
pidofcomm="pidof mcexec"
do_initialize="yes"
DRYRUN=
DRYRUNECHO=":"
while getopts NHedh OPT
do
  case $OPT in
    N)
      HANG=""
      NG=""
      incNH="yes"
      ;;
    H)
      mcexec=""
      runHOST="yes"
      pidofcomm="pidof test_mck"
      mck_max_cpus=`expr $mck_max_cpus + 1`
      trap ":" USR1
      ;;
    e)
      execve_comm="${app_prefix}/test_mck -s execve -n 1 -- -f"
      execve_arg_end="--"
      ;;
    d)
      do_initialize="No"
      DRYRUN=":"
      DRYRUNECHO="echo"
      mcexec="echo"
      runHOST="yes"
      pidofcomm="pidof test_mck"
      mck_max_cpus=`expr $mck_max_cpus + 1`
      trap ":" USR1
      HANG=""
      NG=""
      incNH="yes"
      app_dir='${app_dir}'
      app_prefix=$app_dir
      ;;
    h)
      usage
      exit 0
      ;;
  esac
done
shift `expr $OPTIND - 1`

# get mck ap num
mck_ap_num=`expr $mck_max_cpus - 1`
mck_ap_num_even=$mck_ap_num

if [ `expr $mck_ap_num_even % 2` -ne 0 ]; then
  mck_ap_num_even=`expr $mck_ap_num_even - 1`
fi
	echo a > $mmapfile_name
	dd if=/dev/zero of=${temp} bs=1M count=10
	ln -s ${temp} ${link}

	echo "Linux" > $ostype_name
	cat $org_pid_max > $pid_max_name

	if [ "${runHOST}" != "yes" ]; then
#		sh $mcreboot -c 1-${mck_max_cpus} -m ${boot_mem}
		mck_max_mem_size=`"$ihkosctl" 0 query mem | cut -d '@' -f 1`
	else
		mck_max_mem_size=`expr $mem_size_def \* 1024 \* 1024`
	fi

	mck_max_mem_size_95p=`expr $mck_max_mem_size / 20`
	mck_max_mem_size_110p=`expr $mck_max_mem_size_95p \* 22`
	mck_max_mem_size_95p=`expr $mck_max_mem_size_95p \* 19`
	${DRYRUN} echo "mck_max_mem_size:$mck_max_mem_size"

