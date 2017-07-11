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
#mck_max_cpus=`cat /proc/cpuinfo | grep -c "processor"`
#mck_max_cpus=`expr $mck_max_cpus - 1`
num_cpus=`numactl -H | awk '$3=="cpus:"{ncpu += NF - 3} END{print ncpu}'`
num_cpus_p1=`expr $num_cpus + 1`
num_cpus_m1=`expr $num_cpus - 1`
num_cpus_m2=`expr $num_cpus - 2`
num_cpus_m3=`expr $num_cpus - 3`
num_cpus_m4=`expr $num_cpus - 4`
num_cpus_m5=`expr $num_cpus - 5`
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
#      mck_max_cpus=`expr $mck_max_cpus + 1`
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
#      mck_max_cpus=`expr $mck_max_cpus + 1`
      trap ":" USR1
      HANG=""
      NG=""
      incNH="yes"
      app_dir='${app_dir}'
      app_prefix=$app_dir
      num_cpus='${num_cpus}'
      num_cpus_p1='${num_cpus_p1}'
      num_cpus_m1='${num_cpus_m1}'
      num_cpus_m2='${num_cpus_m2}'
      num_cpus_m3='${num_cpus_m3}'
      num_cpus_m4='${num_cpus_m4}'
      num_cpus_m5='${num_cpus_m5}'
      num_other_procs='${num_other_procs}'
      rlimit_nproc='${rlimit_nproc}'
      mck_max_mem_size='${mck_max_mem_size}'
      mck_max_mem_size_95p='${mck_max_memsize_95p}'
      mck_max_mem_size_95p='${mck_max_memsize_110p}'

      # test file
      this_dir='${this_dir}'
      temp=$this_dir/tempfile
      link=/tmp/templink
      mmapfile_name=$this_dir/mmapfile
      ostype_name=$this_dir/ostype
      org_pid_max=/proc/sys/kernel/pid_max
      pid_max_name=$this_dir/pid_max

      ;;
    h)
      usage
      exit 0
      ;;
  esac
done
shift `expr $OPTIND - 1`

# get mck ap num
#mck_ap_num=`expr $mck_max_cpus - 1`
#mck_ap_num_even=$mck_ap_num

#if [ `expr $mck_ap_num_even % 2` -ne 0 ]; then
#  mck_ap_num_even=`expr $mck_ap_num_even - 1`
#fi

	#### initialize ####
	addusr=0
	id $test_user_name > /dev/null 2>&1
	if [ "$?" -eq 0 ]; then
		uid=`id -u $test_user_name`
		gid=`id -g $test_user_name`
	else
	        useradd $test_user_name
		if [ "$?" -eq 0 ]; then
			uid=`id -u $test_user_name`
			gid=`id -g $test_user_name`
			addusr=1
		else
			uid=1000
			gid=1050
		fi
	fi
	echo "use uid:$uid gid:$gid"
	echo "use uid:$uid gid:$gid"

	echo a > $mmapfile_name
	dd if=/dev/zero of=${temp} bs=1M count=10
	ln -s ${temp} ${link}

	echo "Linux" > $ostype_name
	cat $org_pid_max > $pid_max_name

	#### console output setting ####
#	orig_printk_setting=`cat /proc/sys/kernel/printk`
#	echo "set 4 4 1 7 => /proc/sys/kernel/printk"
#	echo "4 4 1 7" > /proc/sys/kernel/printk

	#### host output corefile-name setting ####
#	orig_core_pattern=`cat /proc/sys/kernel/core_pattern`
#	echo "set core.host.%p => /proc/sys/kernel/core_pattern"
#	echo "core.host.%p" > /proc/sys/kernel/core_pattern

if [ $do_initialize = "yes" ]; then
	if [ "${runHOST}" != "yes" ]; then
	        num_other_procs=0
		rlimit_nproc=`expr $num_other_procs + $num_cpus`
	else
	        num_other_procs=`ps ux | wc -l`
		num_other_procs=`expr $num_other_procs - 3`
		rlimit_nproc=`expr $num_other_procs + $num_cpus + 1`
	fi

	if [ "${runHOST}" != "yes" ]; then
		#### boot McKernel ####
   	        if [ $do_initialize = "yes" ]; then
#		echo "boot McKernel, processor id 0 core is HOST assigned, other core assigned McKernel."
#		sh $mcreboot -c 1-${mck_max_cpus} -m ${boot_mem}
#		sleep 1
	        fi

		#### get McKernel memory size ####
		echo "get McKernel memory size."
		mck_max_mem_size=`"$ihkosctl" 0 query mem | cut -d '@' -f 1`
	else
		${DRYRUN} echo "calc test use memory size."
		mck_max_mem_size=`expr $total_mem \* 1024 \* 1024`
	fi

	mck_max_mem_size_95p=`expr $mck_max_mem_size / 20`
	mck_max_mem_size_110p=`expr $mck_max_mem_size_95p \* 22`
	mck_max_mem_size_95p=`expr $mck_max_mem_size_95p \* 19`
	${DRYRUN} echo "mck_max_mem_size:$mck_max_mem_size"
fi

if [ $do_initialize = "yes" ]; then
	#### insmod test driver ####
#	echo "insmod test_drv"
#	sh "$insmod_test_drv_sh"
fi
