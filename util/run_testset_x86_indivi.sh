#!/bin/sh
# run_testset_x86.sh COPYRIGHT FUJITSU LIMITED 2016
this_dir="$(cd $(dirname $0); pwd)"

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
  echo "$0 [-N] [-H] [-e] [-h]"
  echo "  options:"
  echo "    -N  NG/HANG item including run. (default off)"
  echo "    -H  run on host. (default off)"
  echo "    -e  use execve test. (default off)"
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
while getopts NHeh OPT
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

if [ `expr $mck_ap_num_even % 2` != 0 ]; then
  mck_ap_num_even=`expr $mck_ap_num_even - 1`
fi

# run regression
#while :
#do
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

	echo a > $mmapfile_name
	dd if=/dev/zero of=${temp} bs=1M count=10
	ln -s ${temp} ${link}

	echo "Linux" > $ostype_name
	cat $org_pid_max > $pid_max_name

	#### console output setting ####
	orig_printk_setting=`cat /proc/sys/kernel/printk`
	echo "set 4 4 1 7 => /proc/sys/kernel/printk"
	echo "4 4 1 7" > /proc/sys/kernel/printk

	#### host output corefile-name setting ####
	orig_core_pattern=`cat /proc/sys/kernel/core_pattern`
	echo "set core.host.%p => /proc/sys/kernel/core_pattern"
	echo "core.host.%p" > /proc/sys/kernel/core_pattern

	if [ "${runHOST}" != "yes" ]; then
		#### boot McKernel ####
#MT		echo "boot McKernel, processor id 0 core is HOST assigned, other core assigned McKernel."
#MT		sh $mcreboot -c 1-${mck_max_cpus} -m ${boot_mem}
		sleep 1

		#### get McKernel memory size ####
		echo "get McKernel memory size."
		mck_max_mem_size=`"$ihkosctl" 0 query mem | head -c-3`
	else
		echo "calc test use memory size."
		mck_max_mem_size=`expr $mem_size_def \* 1024 \* 1024`
	fi

	mck_max_mem_size_95p=`expr $mck_max_mem_size / 20`
	mck_max_mem_size_110p=`expr $mck_max_mem_size_95p \* 22`
	mck_max_mem_size_95p=`expr $mck_max_mem_size_95p \* 19`
	echo "mck_max_mem_size:$mck_max_mem_size"

	#### insmod test driver ####
	echo "insmod test_drv"
#MT	sh "$insmod_test_drv_sh"

	#### other than test_mck tp case ####
	echo "## hello_world ##"
#SKIP	${mcexec} $execve_comm "$app_prefix/exit_group"
#SKIP	${mcexec} $execve_comm "$app_prefix/hello_world"
#SKIP	${mcexec} $execve_comm "$app_prefix/glibc_hello_world.static"
#MT	${mcexec} $execve_comm "$app_prefix/glibc_hello_world"

	echo "## lv07 ##"
#MT	${mcexec} $execve_comm "$app_prefix/lv07-st" $execve_arg_end $ostype_name
#MT	${mcexec} $execve_comm "$app_prefix/lv07-pth" $execve_arg_end $ostype_name

	count=1
	while [ $count -le $mck_max_cpus ]
	do
#MT		${mcexec} $execve_comm "$app_prefix/lv07-pth" $execve_arg_end $ostype_name $count
		count=`expr $count + 1`
	done
	count=0

	echo "## lv09 ##"
#MT	${mcexec} $execve_comm "$app_prefix/lv09-pgf" $execve_arg_end w $temp aaabbbcccdddeeefffggghhh\\n
#MT	${mcexec} $execve_comm "$app_prefix/lv09-pgf" $execve_arg_end r $temp

	echo "## lv11 ##"
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w rp   $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w rwp  $temp
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w rep  $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w rwep $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w wp   $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w wep  $temp
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w ep   $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r rp   $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r rwp  $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r rep  $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r rwep $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r wp   $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r wep  $temp
#MT	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r ep   $temp
#MT
#MT	echo "## lv12 ##"
#MT	${mcexec} $execve_comm "$app_prefix/lv12-kill"
#MT	${mcexec} $execve_comm "$app_prefix/lv12-kill-single"
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/lv12-segv"
#MT
#MT	echo "## lv14 ##"
#MT	${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 0
#MT	${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 1
#MT	${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 2
#MT
#MT	if [ $mck_max_mem_size -ge 1181116006 ]; then
#MT		${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 3
#MT		${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 4
#MT		${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 5
#MT	else
#MT		echo "## lv14 03-05 SKIP ##"
#MT	fi

	echo "## lv15 ##"
	count=0
	while [ $count -le 10 ]
	do
#MT		${mcexec} $execve_comm "$app_prefix/lv15-kill"
		count=`expr $count + 1`
	done
	count=0
	while [ $count -le 10 ]
	do
#MT		${mcexec} $execve_comm "$app_prefix/lv15-manon"
		count=`expr $count + 1`
	done
	count=0
	while [ $count -le 10 ]
	do	
#MT		${mcexec} $execve_comm "$app_prefix/lv15-mfile" $execve_arg_end $ostype_name
		count=`expr $count + 1`
	done

#	echo "## socket ##"
#MT ${HANG}	${mcexec} $execve_comm "$app_prefix/single_node"
#MANUAL	${mcexec} $execve_comm "$app_prefix/2node_recv"
#MANUAL	${mcexec} $execve_comm "$app_prefix/2node_send" $execve_arg_end $ipaddress

	#### test_mck case ####
	echo "## siginfo ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s siginfo -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s siginfo -n 1 &
#MT	sleep 3
#MT	siginfo_send_signal `${pidofcomm}`
#MT	sleep 1
#MT
#MT	echo "## wait4 ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s wait4 -n 0 
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s wait4 -n 1 -- -f $pid_max_name
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s wait4 -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s wait4 -n 3
#MT
#MT#	echo "## env ##"
#MT#	env_opt="-e AAA -e USER= -e a -e b -e ARCH=x86 -e ARCH=postk"
#MT#SKIP	${mcexec} $env_opt $execve_comm "$app_prefix/test_mck" $execve_arg_end -s env -n 0
#MT
#MT	echo "## rt_sigsuspend ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigsuspend -n 0
#MT
#MT	echo "## cpu_thread_limits ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 0 -- -t $mck_ap_num
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 1 -- -t $mck_ap_num
#MT
#MT	echo "## gettid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s gettid -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s gettid -n 1
#MT
#MT	echo "## mprotect ##"
#MT	for tp_num in `seq 0 7`
#MT	do	
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mprotect -n $tp_num
#MT	done
#MT
	echo "## mem_stack_limits ##"
#MT	initial_ulimit_orig=`ulimit -s`
#MT	echo "ulimit -s 10MiB (10240 KiB)"
#MT	ulimit -s 10240
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s 9961472
#MT
#MT	if [ $mck_max_mem_size -ge 2244120412 ]; then
#MT		echo "ulimit -s 2GiB (2097152 KiB)"
#MT		ulimit -s 2097152
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s 2040109466
#MT	else
#MT		echo "## mem_stack_limits 2GiB SKIP ##"
#MT	fi
#MT
	echo "ulimit -s unlimited"
	ulimit -s unlimited
#	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s $mck_max_mem_size_95p
#	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s $mck_max_mem_size
#	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s $mck_max_mem_size_110p

#MT	echo "ulimit -s [initial: (${initial_ulimit_orig})]"
#MT	ulimit -s ${initial_ulimit_orig}
#MT
#MT	echo "## munlock ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s munlock -n 0
#MT
#MT	echo "## rt_sigaction ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 3
#MT${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 4
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 5
#MT
#MT	echo "## fork ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fork -n 0
#MT
#MT	echo "## pause ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s pause -n 0
#MT
#MT	echo "## sigaltstack ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sigaltstack -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sigaltstack -n 1
#MT
#MT	echo "## ptrace ##"
#MT	for tp_num in `seq 0 20`
#MT	do
#MT#3, 4, 6, 7, 8, 10, 11, 12, 16, 17 SKIP
#MT		if [ $tp_num -eq  3 ] || [ $tp_num -eq  4 ] || [ $tp_num -eq  6 ] || [ $tp_num -eq  7 ] ||
#MT		   [ $tp_num -eq  8 ] || [ $tp_num -eq 10 ] || [ $tp_num -eq 11 ] || [ $tp_num -eq 12 ] ||
#MT		   [ $tp_num -eq 16 ] || [ $tp_num -eq 17 ]; then
#MT			continue
#MT		fi
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s ptrace -n $tp_num
#MT	done
#MT
#MT	echo "## mmap_dev ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_dev -n 0 -- -d /dev/test_mck/mmap_dev -s 8192
#MT${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_dev -n 1 -- -d /dev/test_mck/mmap_dev2 -s 8192
#MT${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_dev -n 2 -- -d /dev/test_mck/mmap_dev2 -s 8192
#MT
#MT	echo "## tgkill ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tgkill -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tgkill -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tgkill -n 2
#MT
#MT	echo "## rt_sigpending ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigpending -n 0
#MT
#MT	echo "## rt_sigqueueinfo ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigqueueinfo -n 0
#MT
#MT	echo "## rt_sigprocmask ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigprocmask -n 0
#MT
#MT	echo "## mmap_populate ##"
#MT	echo a > $mmapfile_name
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_populate -n 0 -- -f $mmapfile_name
#MT
#MT#	echo "## mem_large_page ##"
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 0
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 1
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 2
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 3
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 4
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 5
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 6
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 7
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 8
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 9
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 10
#MT
#MT	echo "## tls ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tls -n 0
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tls -n 1 -- -t $mck_ap_num
#MT
#MT	echo "## mmap_file ##"
#MT	rm -f $mmapfile_name
#MT	for tp_num in `seq 0 48`
#MT	do
#MT#1, 3, 5, 7, 12, 13, 14, 15, 33, 35, 37, 39, 44, 45, 46, 47 HANG
#MT		if [ "${incNH}" != "yes" ]; then
#MT			if [ $tp_num -eq  1 ] || [ $tp_num -eq  3 ] || [ $tp_num -eq  5 ] || [ $tp_num -eq  7 ] || \
#MT			   [ $tp_num -eq 12 ] || [ $tp_num -eq 13 ] || [ $tp_num -eq 14 ] || [ $tp_num -eq 15 ] || \
#MT			   [ $tp_num -eq 33 ] || [ $tp_num -eq 35 ] || [ $tp_num -eq 37 ] || [ $tp_num -eq 39 ] || \
#MT			   [ $tp_num -eq 44 ] || [ $tp_num -eq 45 ] || [ $tp_num -eq 46 ] || [ $tp_num -eq 46 ] || \
#MT			   [ $tp_num -eq 47 ]; then
#MT				continue
#MT			fi
#MT		fi
#MT		echo a > $mmapfile_name
#MT		cat $mmapfile_name
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_file -n $tp_num -- -f $mmapfile_name
#MT		cat $mmapfile_name
#MT	done
#MT
#MT	echo "## execve ##"
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 0 -- -f "$app_dir/execve_app"
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 1 -- -f "$app_dir/execve_app"
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 1 -- -f "$app_dir/test_mck"
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 1 -- -f "$app_dir/test_mck" -- -s env -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 2
#MT
#MT	echo "## madvise ##"
#MT	for tp_num in `seq 0 15`
#MT	do	
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s madvise -n $tp_num -- -f $mmapfile_name
#MT	done
#MT
#MT	echo "## cpu_proc_limits ##"
#MT	count=1
#MT	while [ $count -lt $mck_max_cpus ]
#MT	do
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_proc_limits -n 0 -- -p $count
#MT		count=`expr $count + 1`
#MT	done
#MT	count=0
#MT
#MT	echo "## nfo ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nfo -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nfo -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nfo -n 2
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nfo -n 3
#MT
#MT	echo "## getrlimit ##"
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 1
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 3
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 4
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 5
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 6
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 7
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 8
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 9
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 10
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 11
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 12
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 13
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 14
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 15
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 16
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 17
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 18
#MT
#MT	echo "## rt_sigtimedwait ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigtimedwait -n 0
#MT
#MT	echo "## mlock ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mlock -n 0
#MT
#MT	echo "## mmap_locked ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_locked -n 0 -- -f $mmapfile_name
#MT
#MT	echo "## remap_file_pages ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s remap_file_pages -n 0 -- -s $((1024*16))
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s remap_file_pages -n 0 -- -s $((256*1024*1024))
#MT
#MT	echo "## mem_limits ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -s $((1024*1024)) -c 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -s $mck_max_mem_size_95p -c 1
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -s $mck_max_mem_size -c 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -S mmap -c 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f brk -s $((1024*1024)) -c 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f brk -s $mck_max_mem_size_95p -c 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f brk -s $mck_max_mem_size -c 1
#MT
#MT	if [ $mck_max_mem_size -ge 2244120412 ]; then
#MT		echo "## large_bss ##"
#MT		${mcexec} $execve_comm "$app_prefix/large_bss"
#MT	else
#MT		echo "## large_bss SKIP ##"
#MT	fi
#MT
#MT	echo "## system ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s system -n 0
#MT
#MT	echo "## vfork ##"
#MT#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s vfork -n 0 -- -f "$app_dir/execve_app"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s vfork -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s vfork -n 2 -- -f "$app_dir/execve_app"
#MT
#MT#	echo "## coredump ##"
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s coredump -n 0
#MT#	mv core core.$$
#MT#	echo "generate corefile: core.$$"
#MT#	readelf -a core.$$
#MT#	file core.$$
#MT#	gdb -x $app_dir/autorun.inf $app_dir/test_mck core.$$
#MT
#MT	echo "## popen ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s popen -n 0
#MT
#MT	echo "## procfs ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 1
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 2
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 3
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 4
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 5
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 6
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 7
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 8
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 9
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 10
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 11
#MT
#MT	echo "## fork_execve ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fork_execve -n 0 -- -f "$app_dir/execve_app"
#MT
#MT	echo "## shellscript ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_shell.sh"
#MT
#MT	echo "## mremap_mmap_anon ##"
#MT#1 NG
#MT#14, 15, 16 HANG
#MT	for tp_num in `seq 0 16`
#MT	do
#MT		if [ "${incNH}" != "yes" ]; then
#MT			if [ $tp_num -eq 1 ] || [ $tp_num -eq 14 ] || [ $tp_num -eq 15 ] || [ $tp_num -eq 16 ]; then
#MT				continue
#MT			fi
#MT		fi
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
#MT	done
#MT#101 NG
#MT#103, 105, 114, 115, 116 HANG
#MT	for tp_num in `seq 100 113`
#MT	do
#MT		if [ "${incNH}" != "yes" ]; then
#MT			if [ $tp_num -eq 101 ] || [ $tp_num -eq 103 ] || [ $tp_num -eq 105 ] || [ $tp_num -eq 114 ] || \
#MT			   [ $tp_num -eq 115 ] || [ $tp_num -eq 116 ]; then
#MT				continue
#MT			fi
#MT		fi
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
#MT	done
#MT#201 NG
#MT#207, 214, 215, 216 HANG
#MT	for tp_num in `seq 200 216`
#MT	do
#MT		if [ "${incNH}" != "yes" ]; then
#MT			if [ $tp_num -eq 201 ] || [ $tp_num -eq 207 ] || [ $tp_num -eq 214 ] || \
#MT			   [ $tp_num -eq 215 ] || [ $tp_num -eq 216 ]; then
#MT				continue
#MT			fi
#MT		fi
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
#MT	done
#MT#301 NG
#MT#303, 305, 314, 315, 316 HANG
#MT	for tp_num in `seq 300 316`
#MT	do
#MT		if [ "${incNH}" != "yes" ]; then
#MT			if [ $tp_num -eq 301 ] || [ $tp_num -eq 303 ] || [ $tp_num -eq 305 ] || [ $tp_num -eq 314 ] || \
#MT			   [ $tp_num -eq 315 ] || [ $tp_num -eq 316 ]; then
#MT				continue
#MT			fi
#MT		fi
#MT		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
#MT	done
#MT
#MT	echo "## get_cpu_id ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s get_cpu_id -n 0
#MT
#MT	echo "## setpgid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 3
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 4 -- -f "$app_dir/execve_app"
#MT
#MT	echo "## kill ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s kill -n 0 -- -p $mck_ap_num
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s kill -n 1 -- -p $mck_ap_num
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s kill -n 2 -- -p $mck_ap_num
#MT
#MT	echo "## sched_setaffinity ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 0 -- -p $mck_max_cpus
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 1 -- -p $mck_max_cpus
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 2 -- -p $mck_max_cpus
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 3 -- -p $mck_max_cpus
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 4 -- -p $mck_max_cpus
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 5
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 6 -- -p $mck_max_cpus
#MT${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 7 -- -p $mck_max_cpus
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 8 -- -p $mck_max_cpus
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 9 -- -p $mck_max_cpus
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 10 -- -p $mck_max_cpus
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 11 -- -p $mck_max_cpus
#MT
#MT	getaff_cpus=`expr $mck_max_cpus + 5`
#MT	echo "## sched_getaffinity ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 0 -- -p $mck_max_cpus
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 2 -- -p $mck_max_cpus
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 3 -- -p $mck_max_cpus
#MT${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 4 -- -p $mck_max_cpus
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 5 -- -p $mck_max_cpus
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 6 -- -p $getaff_cpus -f "$app_dir/show_affinity" -- -p $getaff_cpus
#MT
#MT${HANG}	echo "## pthread_setaffinity ##"
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s pthread_setaffinity -n 0 -- -p $mck_max_cpus
#MT
#MT${HANG}	echo "## pthread_getaffinity ##"
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s pthread_getaffinity -n 0 -- -p $mck_max_cpus
#MT
#MT	echo "## enosys ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s enosys -n 0
#MT
#MT	echo "## getcpu ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getcpu -n 0
#MT
#MT	echo "## getegid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getegid -n 0 -- -e $gid
#MT
#MT	echo "## geteuid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s geteuid -n 0 -- -e $uid
#MT
#MT	echo "## getgid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getgid -n 0 -- -g $gid
#MT
#MT	echo "## getppid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getppid -n 0
#MT
#MT	echo "## getresgid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getresgid -n 0 -- -r $gid -e $gid -s $gid
#MT
#MT	echo "## getresuid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getresuid -n 0 -- -r $uid -e $uid -s $uid
#MT
#MT	echo "## getuid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getuid -n 0 -- -u $uid
#MT
#MT	echo "## ipc ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s ipc -n 0
#MT
#MT	echo "## mincore ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mincore -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mincore -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mincore -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mincore -n 3
#MT
#MT	echo "## mlockall ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mlockall -n 0
#MT
#MT	echo "## msync ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s msync -n 0
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s msync -n 1
#MT
#MT	echo "## munlockall ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s munlockall -n 0
#MT
#MT	echo "## page_fault_forwording ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s page_fault_forwording -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s page_fault_forwording -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s page_fault_forwording -n 2
#MT
#MT#	echo "## process_vm_readv ##"
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s process_vm_readv -n 0
#MT
#MT#	echo "## process_vm_writev ##"
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s process_vm_writev -n 0
#MT
#MT	echo "## sched_get_priority_max ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_max -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_max -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_max -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_max -n 3
#MT
#MT	echo "## sched_get_priority_min ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_min -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_min -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_min -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_min -n 3
#MT
#MT	echo "## sched_getparam ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getparam -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getparam -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getparam -n 2
#MT
#MT	echo "## sched_getscheduler ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getscheduler -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getscheduler -n 1
#MT
#MT	echo "## sched_rr_get_interval ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 3
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 4
#MT
#MT	echo "## sched_setparam ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setparam -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setparam -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setparam -n 2
#MT
#MT	echo "## sched_setscheduler ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 3
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 4
#MT
#MT${NG}	echo "## setfsgid ##"
#MT${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setfsgid -n 0 -- -f $gid
#MT
#MT	echo "## setfsuid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setfsuid -n 0 -- -f $uid
#MT
#MT	echo "## setgid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setgid -n 0 -- -g $gid
#MT
#MT	echo "## setregid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setregid -n 0 -- -r $gid -e $gid
#MT
#MT	echo "## setresgid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setresgid -n 0 -- -r $gid -e $gid -s $gid
#MT
#MT	echo "## setresuid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setresuid -n 0 -- -r $uid -e $uid -s $uid
#MT
#MT	echo "## setreuid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setreuid -n 0 -- -r $uid -e $uid
#MT
#MT	echo "## setrlimit ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 3
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 4
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 5
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 6
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 7
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 8
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 9
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 10
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 11
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 12
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 13
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 14
#MT${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 15
#MT
#MT	echo "## setuid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setuid -n 0 -- -u $uid
#MT
#MT	echo "## waitid ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s waitid -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s waitid -n 1 -- -p $mck_ap_num_even
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s waitid -n 2 -- -p $mck_ap_num
#MT
#MT	echo "## signalfd4 ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s signalfd4 -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s signalfd4 -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s signalfd4 -n 2
#MT
#MT	echo "## gettimeofday ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s gettimeofday -n 0
#MT
#MT${NG}	echo "## sched_yield ##"
#MT${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_yield -n 0
#MT
#MT	echo "## set_tid_address ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s set_tid_address -n 0
#MT
#MT	echo "## getrusage ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 1
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 3
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 4
#MT
#MT	echo "## tkill ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tkill -n 0
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tkill -n 1
#MT
#MT	echo "## times ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s times -n 0
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s times -n 1
#MT
#MT	echo "## nanosleep ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nanosleep -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nanosleep -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nanosleep -n 2
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nanosleep -n 3
#MT
#MT	echo "## getitimer ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getitimer -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getitimer -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getitimer -n 2
#MT
#MT	echo "## setitimer ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setitimer -n 0
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setitimer -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setitimer -n 2
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setitimer -n 3
#MT
#MT	echo "## clock_gettime ##"
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_gettime -n 0
#MT${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_gettime -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_gettime -n 2
#MT
#MT	echo "## clock_getres ##"
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_getres -n 0
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_getres -n 1
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_getres -n 2
#MT
#MT	echo "## readlinkat ##"
#MT	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s readlinkat -n 0 -- -f ${temp} -l ${link}
#MT
#MT#	echo "## fpregs ##"
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 0
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 1
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 2
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 3
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 4
#MT#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 5 -- -p $mck_max_cpus
#MT
	echo "## force_exit ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s force_exit -n 0 -- -f $mmapfile_name -d /dev/test_mck/mmap_dev &
	sleep 3
	echo "send SIGKILL for mcexec."
	kill -9 `${pidofcomm}`
	echo "rmmod test_drv"
	sh "$rmmod_test_drv_sh"

	if [ "${runHOST}" != "yes" ]; then
		echo "shutdown_mck..."
#MT		sh "$mcstop"
		sleep 1
	fi

	#### finalize ####
	#### host output corefile-name setting restore ####
	echo "restore $orig_core_pattern => /proc/sys/kernel/core_pattern"
	echo $orig_core_pattern > /proc/sys/kernel/core_pattern

	#### console output setting restore ####
	echo "restore $orig_printk_setting => /proc/sys/kernel/printk"
	echo $orig_printk_setting > /proc/sys/kernel/printk

	rm $ostype_name
	rm $pid_max_name
	rm $link
	rm $temp
	rm $mmapfile_name
	if [ "$addusr" -eq 1 ]; then
		userdel $test_user_name --remove
	fi

#	if [ -e ${sh_base}/continue_end ]; then
#		echo "find continue_end file."
#		break
#	fi
#done

