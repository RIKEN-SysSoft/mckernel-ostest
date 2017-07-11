#!/bin/sh
# run_testset_x86.sh COPYRIGHT FUJITSU LIMITED 2016
this_dir="$(cd $(dirname $0); pwd)"

# memsize
total_mem=`free -m | grep Mem: | awk '{print $2}'`
#mem_size_def=$(( $total_mem - ($total_mem * (100 - 45) / 100)))
mem_size_def=4096
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
      mcexec='echo ${mcexec} '
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
      mck_max_mem_size_110p='${mck_max_memsize_110p}'
      mck_ap_num='${mck_ap_num}'
      mck_ap_num_even='${mck_ap_num_even}'

      # test file
      this_dir='${this_dir}'
      temp='${temp}'
      link='${link}'
      mmapfile_name='${mmapfile_name}'
      ostype_name='${ostype_name}'
      org_pid_max='${org_pid_max}'
      pid_max_name='${pid_max_name}'

      ;;
    h)
      usage
      exit 0
      ;;
  esac
done
shift `expr $OPTIND - 1`

if [ $do_initialize = "yes" ]; then
    # get mck ap num
    mck_ap_num=$num_cpus_m1
    mck_ap_num_even=$mck_ap_num

    if [ `expr $mck_ap_num_even % 2` -ne 0 ]; then
	mck_ap_num_even=`expr $mck_ap_num_even - 1`
    fi
fi

# run regression
#while :
#do
if [ $do_initialize = "yes" ]; then
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
	orig_printk_setting=`cat /proc/sys/kernel/printk`
	echo "set 4 4 1 7 => /proc/sys/kernel/printk"
	echo "4 4 1 7" > /proc/sys/kernel/printk

	#### host output corefile-name setting ####
	orig_core_pattern=`cat /proc/sys/kernel/core_pattern`
	echo "set core.host.%p => /proc/sys/kernel/core_pattern"
	echo "core.host.%p" > /proc/sys/kernel/core_pattern
fi

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
		echo "boot McKernel, processor id 0 core is HOST assigned, other core assigned McKernel."
		sh $mcreboot -c 1-${mck_max_cpus} -m ${boot_mem}
		sleep 1
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
	echo "insmod test_drv"
	sh "$insmod_test_drv_sh"
fi

	#### other than test_mck tp case ####
	echo "## hello_world ##"
#SKIP	${mcexec} $execve_comm "$app_prefix/exit_group"
#SKIP	${mcexec} $execve_comm "$app_prefix/hello_world"
#SKIP	${mcexec} $execve_comm "$app_prefix/glibc_hello_world.static"
	${mcexec} $execve_comm "$app_prefix/glibc_hello_world"

	echo "## lv07 ##"
	${mcexec} $execve_comm "$app_prefix/lv07-st" $execve_arg_end $ostype_name
#	${mcexec} $execve_comm "$app_prefix/lv07-pth" $execve_arg_end $ostype_name

#	count=1
#	while [ $count -le $mck_max_cpus ]
#	do
#		${mcexec} $execve_comm "$app_prefix/lv07-pth" $execve_arg_end $ostype_name $count
#		count=`expr $count + 1`
#	done
#	count=0

	echo "## lv09 ##"
	${mcexec} $execve_comm "$app_prefix/lv09-pgf" $execve_arg_end w $temp aaabbbcccdddeeefffggghhh\\n
	${mcexec} $execve_comm "$app_prefix/lv09-pgf" $execve_arg_end r $temp

	echo "## lv11 ##"
${HANG}	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w rp   $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w rwp  $temp
${HANG}	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w rep  $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w rwep $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w wp   $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w wep  $temp
${HANG}	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end w ep   $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r rp   $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r rwp  $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r rep  $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r rwep $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r wp   $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r wep  $temp
	${mcexec} $execve_comm "$app_prefix/lv11" $execve_arg_end r ep   $temp

	echo "## lv12 ##"
	${mcexec} $execve_comm "$app_prefix/lv12-kill"
	${mcexec} $execve_comm "$app_prefix/lv12-kill-single"
${HANG}	${mcexec} $execve_comm "$app_prefix/lv12-segv"

	echo "## lv14 ##"
	${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 0
	${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 1
	${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 2

#	if [ $mck_max_mem_size -ge 1181116006 ]; then
		${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 3
		${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 4
		${mcexec} $execve_comm "$app_prefix/lv14" $execve_arg_end 5
#	else
#		echo "## lv14 03-05 SKIP ##"
#	fi

	echo "## lv15 ##"
	count=0
	while [ $count -le 10 ]
	do
		${mcexec} $execve_comm "$app_prefix/lv15-kill"
		count=`expr $count + 1`
	done
	count=0
	while [ $count -le 10 ]
	do
		${mcexec} $execve_comm "$app_prefix/lv15-manon"
		count=`expr $count + 1`
	done
	count=0
	while [ $count -le 10 ]
	do	
		${mcexec} $execve_comm "$app_prefix/lv15-mfile" $execve_arg_end $ostype_name
		count=`expr $count + 1`
	done

	echo "## socket ##"
${HANG}	${mcexec} $execve_comm "$app_prefix/single_node"
#MANUAL	${mcexec} $execve_comm "$app_prefix/2node_recv"
#MANUAL	${mcexec} $execve_comm "$app_prefix/2node_send" $execve_arg_end $ipaddress

	#### test_mck case ####
	echo "## siginfo ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s siginfo -n 0
${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s siginfo -n 1 &
${DRYRUN} ${NG}	sleep 3
${DRYRUN} ${NG}	siginfo_send_signal `${pidofcomm}`
${DRYRUN} ${NG}	sleep 1

	echo "## wait4 ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s wait4 -n 0 
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s wait4 -n 1 -- -f $pid_max_name
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s wait4 -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s wait4 -n 3

#	echo "## env ##"
#	env_opt="-e AAA -e USER= -e a -e b -e ARCH=x86 -e ARCH=postk"
#SKIP	${mcexec} $env_opt $execve_comm "$app_prefix/test_mck" $execve_arg_end -s env -n 0

	echo "## rt_sigsuspend ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigsuspend -n 0

	echo "## cpu_thread_limits ##"
#	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 0 -- -t $mck_ap_num
#	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 1 -- -t $mck_ap_num
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 0 -- -t $num_cpus_m2 -c $rlimit_nproc
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 0 -- -t $num_cpus_m1 -c $rlimit_nproc
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 0 -- -t $num_cpus -c $rlimit_nproc
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 1 -- -t $num_cpus_m2 -c $rlimit_nproc
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 1 -- -t $num_cpus_m1 -c $rlimit_nproc
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_thread_limits -n 1 -- -t $num_cpus -c $rlimit_nproc

	echo "## gettid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s gettid -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s gettid -n 1

	echo "## mprotect ##"
	for tp_num in `seq 0 7`
	do	
		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mprotect -n $tp_num
	done

	echo "## mem_stack_limits ##"
	${DRYRUN} initial_ulimit_orig=`ulimit -s`
	${DRYRUN} echo "ulimit -s 10MiB (10240 KiB)"
	${DRYRUN} ulimit -s 10240
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s 9961472

#	if [ $mck_max_mem_size -ge 2244120412 ]; then
		${DRYRUN} echo "ulimit -s 2GiB (2097152 KiB)"
		${DRYRUN} ulimit -s 2097152
		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s 2040109466
#	else
#		echo "## mem_stack_limits 2GiB SKIP ##"
#	fi

	${DRYRUN} echo "ulimit -s unlimited"
	${DRYRUN} ulimit -s unlimited
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s $mck_max_mem_size_95p
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_stack_limits -n 0 -- -s $mck_max_mem_size_110p

	${DRYRUN} echo "ulimit -s [initial: (${initial_ulimit_orig})]"
	${DRYRUN} ulimit -s ${initial_ulimit_orig}

	echo "## munlock ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s munlock -n 0

	echo "## rt_sigaction ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 3
${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 4
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigaction -n 5

	echo "## fork ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fork -n 0

	echo "## pause ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s pause -n 0

	echo "## sigaltstack ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sigaltstack -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sigaltstack -n 1

	echo "## ptrace ##"
	for tp_num in `seq 0 20`
	do
#3, 4, 6, 7, 8, 10, 11, 12, 16, 17 SKIP
		if [ $tp_num -eq  3 ] || [ $tp_num -eq  4 ] || [ $tp_num -eq  6 ] || [ $tp_num -eq  7 ] ||
		   [ $tp_num -eq  8 ] || [ $tp_num -eq 10 ] || [ $tp_num -eq 11 ] || [ $tp_num -eq 12 ] ||
		   [ $tp_num -eq 16 ] || [ $tp_num -eq 17 ]; then
			continue
		fi
		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s ptrace -n $tp_num
	done

	echo "## mmap_dev ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_dev -n 0 -- -d /dev/test_mck/mmap_dev -s 8192
${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_dev -n 1 -- -d /dev/test_mck/mmap_dev2 -s 8192
${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_dev -n 2 -- -d /dev/test_mck/mmap_dev2 -s 8192

	echo "## tgkill ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tgkill -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tgkill -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tgkill -n 2

	echo "## rt_sigpending ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigpending -n 0

	echo "## rt_sigqueueinfo ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigqueueinfo -n 0

	echo "## rt_sigprocmask ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigprocmask -n 0

	echo "## mmap_populate ##"
if [ x${DRYRUN} != "x:" ]; then
	echo a > $mmapfile_name
fi
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_populate -n 0 -- -f $mmapfile_name

#	echo "## mem_large_page ##"
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 0
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 1
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 2
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 3
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 4
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 5
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 6
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 7
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 8
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 9
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_large_page -n 10

	echo "## tls ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tls -n 0
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tls -n 1 -- -t $num_cpus_m1

	echo "## mmap_file ##"
	rm -f $mmapfile_name
	for tp_num in `seq 0 48`
	do
#1, 3, 5, 7, 12, 13, 14, 15, 33, 35, 37, 39, 44, 45, 46, 47 HANG
		if [ "${incNH}" != "yes" ]; then
			if [ $tp_num -eq  1 ] || [ $tp_num -eq  3 ] || [ $tp_num -eq  5 ] || [ $tp_num -eq  7 ] || \
			   [ $tp_num -eq 12 ] || [ $tp_num -eq 13 ] || [ $tp_num -eq 14 ] || [ $tp_num -eq 15 ] || \
			   [ $tp_num -eq 33 ] || [ $tp_num -eq 35 ] || [ $tp_num -eq 37 ] || [ $tp_num -eq 39 ] || \
			   [ $tp_num -eq 44 ] || [ $tp_num -eq 45 ] || [ $tp_num -eq 46 ] || [ $tp_num -eq 46 ] || \
			   [ $tp_num -eq 47 ]; then
				continue
			fi
		fi
if [ x${DRYRUN} != "x:" ]; then
	echo a > $mmapfile_name
fi
#		cat $mmapfile_name
		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_file -n $tp_num -- -f $mmapfile_name
#		cat $mmapfile_name
	done

	echo "## execve ##"
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 0 -- -f "$app_dir/execve_app"
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 1 -- -f "$app_dir/execve_app"
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 1 -- -f "$app_dir/test_mck"
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 1 -- -f "$app_dir/test_mck" -- -s env -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s execve -n 2

	echo "## madvise ##"
	for tp_num in `seq 0 15`
	do	
		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s madvise -n $tp_num -- -f $mmapfile_name
	done

	echo "## cpu_proc_limits ##"
#	count=1
#	while [ $count -lt $mck_max_cpus ]
#	do
#		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_proc_limits -n 0 -- -p $count
#		count=`expr $count + 1`
#	done
#	count=0

	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_proc_limits -n 0 -- -p $num_cpus_m2 -c $rlimit_nproc
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_proc_limits -n 0 -- -p $num_cpus_m1 -c $rlimit_nproc
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s cpu_proc_limits -n 0 -- -p $num_cpus -c $rlimit_nproc


	echo "## nfo ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nfo -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nfo -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nfo -n 2
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nfo -n 3

	echo "## getrlimit ##"
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 1
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 3
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 4
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 5
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 6
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 7
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 8
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 9
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 10
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 11
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 12
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 13
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 14
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 15
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 16
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 17
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrlimit -n 18

	echo "## rt_sigtimedwait ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s rt_sigtimedwait -n 0

	echo "## mlock ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mlock -n 0

	echo "## mmap_locked ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mmap_locked -n 0 -- -f $mmapfile_name

	echo "## remap_file_pages ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s remap_file_pages -n 0 -- -s $((1024*16))
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s remap_file_pages -n 0 -- -s $((256*1024*1024))

	echo "## mem_limits ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -s $((1024*1024)) -c 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -s $mck_max_mem_size_95p -c 1
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -s $mck_max_mem_size -c 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f mmap -S mmap -c 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f brk -s $((1024*1024)) -c 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f brk -s $mck_max_mem_size_95p -c 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mem_limits -n 0 -- -f brk -s $mck_max_mem_size -c 1

#	if [ $mck_max_mem_size -ge 2244120412 ]; then
		echo "## large_bss ##"
		${mcexec} $execve_comm "$app_prefix/large_bss"
#	else
#		echo "## large_bss SKIP ##"
#	fi

	echo "## system ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s system -n 0

	echo "## vfork ##"
#REPEAL	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s vfork -n 0 -- -f "$app_dir/execve_app"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s vfork -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s vfork -n 2 -- -f "$app_dir/execve_app"

#	echo "## coredump ##"
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s coredump -n 0
#	mv core core.$$
#	echo "generate corefile: core.$$"
#	readelf -a core.$$
#	file core.$$
#	gdb -x $app_dir/autorun.inf $app_dir/test_mck core.$$

	echo "## popen ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s popen -n 0

	echo "## procfs ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 1
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 2
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 3
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 4
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 5
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 6
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 7
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 8
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 9
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 10
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s procfs -n 11

	echo "## fork_execve ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fork_execve -n 0 -- -f "$app_dir/execve_app"

	echo "## shellscript ##"
	${mcexec} $execve_comm "$app_prefix/test_shell.sh"

	echo "## mremap_mmap_anon ##"
#1 NG
#14, 15, 16 HANG
	for tp_num in `seq 0 16`
	do
		if [ "${incNH}" != "yes" ]; then
			if [ $tp_num -eq 1 ] || [ $tp_num -eq 14 ] || [ $tp_num -eq 15 ] || [ $tp_num -eq 16 ]; then
				continue
			fi
		fi
		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
	done
#101 NG
#103, 105, 114, 115, 116 HANG
	for tp_num in `seq 100 113`
	do
		if [ "${incNH}" != "yes" ]; then
			if [ $tp_num -eq 101 ] || [ $tp_num -eq 103 ] || [ $tp_num -eq 105 ] || [ $tp_num -eq 114 ] || \
			   [ $tp_num -eq 115 ] || [ $tp_num -eq 116 ]; then
				continue
			fi
		fi
		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
	done
#201 NG
#207, 214, 215, 216 HANG
	for tp_num in `seq 200 216`
	do
		if [ "${incNH}" != "yes" ]; then
			if [ $tp_num -eq 201 ] || [ $tp_num -eq 207 ] || [ $tp_num -eq 214 ] || \
			   [ $tp_num -eq 215 ] || [ $tp_num -eq 216 ]; then
				continue
			fi
		fi
		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
	done
#301 NG
#303, 305, 314, 315, 316 HANG
	for tp_num in `seq 300 316`
	do
		if [ "${incNH}" != "yes" ]; then
			if [ $tp_num -eq 301 ] || [ $tp_num -eq 303 ] || [ $tp_num -eq 305 ] || [ $tp_num -eq 314 ] || \
			   [ $tp_num -eq 315 ] || [ $tp_num -eq 316 ]; then
				continue
			fi
		fi
		${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mremap_mmap_anon -n $tp_num
	done

	echo "## get_cpu_id ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s get_cpu_id -n 0

	echo "## setpgid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 3
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setpgid -n 4 -- -f "$app_dir/execve_app"

	echo "## kill ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s kill -n 0 -- -p $num_cpus_m1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s kill -n 1 -- -p $num_cpus_m1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s kill -n 2 -- -p $num_cpus_m1

	echo "## sched_setaffinity ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 0 -- -p $num_cpus
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 1 -- -p $num_cpus
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 2 -- -p $num_cpus
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 3 -- -p $num_cpus
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 4 -- -p $num_cpus
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 5
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 6 -- -p $num_cpus
${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 7 -- -p $num_cpus
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 8 -- -p $num_cpus
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 9 -- -p $num_cpus
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 10 -- -p $num_cpus
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setaffinity -n 11 -- -p $num_cpus

if [ x${DRYRUN} != "x:" ]; then
	getaff_cpus=`expr $num_cpus + 5`
fi
	echo "## sched_getaffinity ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 0 -- -p $num_cpus
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 2 -- -p $num_cpus
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 3 -- -p $num_cpus
${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 4 -- -p $num_cpus
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 5 -- -p $num_cpus
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getaffinity -n 6 -- -p $getaff_cpus -f "$app_dir/show_affinity" -- -p $getaff_cpus

${HANG}	echo "## pthread_setaffinity ##"
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s pthread_setaffinity -n 0 -- -p $num_cpus

${HANG}	echo "## pthread_getaffinity ##"
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s pthread_getaffinity -n 0 -- -p $num_cpus

	echo "## enosys ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s enosys -n 0

	echo "## getcpu ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getcpu -n 0

	echo "## getegid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getegid -n 0 -- -e $gid

	echo "## geteuid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s geteuid -n 0 -- -e $uid

	echo "## getgid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getgid -n 0 -- -g $gid

	echo "## getppid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getppid -n 0

	echo "## getresgid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getresgid -n 0 -- -r $gid -e $gid -s $gid

	echo "## getresuid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getresuid -n 0 -- -r $uid -e $uid -s $uid

	echo "## getuid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getuid -n 0 -- -u $uid

	echo "## ipc ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s ipc -n 0

	echo "## mincore ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mincore -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mincore -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mincore -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mincore -n 3

	echo "## mlockall ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s mlockall -n 0

	echo "## msync ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s msync -n 0
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s msync -n 1

	echo "## munlockall ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s munlockall -n 0

	echo "## page_fault_forwording ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s page_fault_forwording -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s page_fault_forwording -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s page_fault_forwording -n 2

#	echo "## process_vm_readv ##"
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s process_vm_readv -n 0

#	echo "## process_vm_writev ##"
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s process_vm_writev -n 0

	echo "## sched_get_priority_max ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_max -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_max -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_max -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_max -n 3

	echo "## sched_get_priority_min ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_min -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_min -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_min -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_get_priority_min -n 3

	echo "## sched_getparam ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getparam -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getparam -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getparam -n 2

	echo "## sched_getscheduler ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getscheduler -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_getscheduler -n 1

	echo "## sched_rr_get_interval ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 3
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_rr_get_interval -n 4

	echo "## sched_setparam ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setparam -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setparam -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setparam -n 2

	echo "## sched_setscheduler ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 3
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_setscheduler -n 4

${NG}	echo "## setfsgid ##"
${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setfsgid -n 0 -- -f $gid

	echo "## setfsuid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setfsuid -n 0 -- -f $uid

	echo "## setgid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setgid -n 0 -- -g $gid

	echo "## setregid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setregid -n 0 -- -r $gid -e $gid

	echo "## setresgid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setresgid -n 0 -- -r $gid -e $gid -s $gid

	echo "## setresuid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setresuid -n 0 -- -r $uid -e $uid -s $uid

	echo "## setreuid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setreuid -n 0 -- -r $uid -e $uid

	echo "## setrlimit ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 3
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 4
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 5
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 6
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 7
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 8
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 9
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 10
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 11
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 12
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 13
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 14
${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setrlimit -n 15

	echo "## setuid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setuid -n 0 -- -u $uid

	echo "## waitid ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s waitid -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s waitid -n 1 -- -p $mck_ap_num_even
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s waitid -n 2 -- -p $mck_ap_num

	echo "## signalfd4 ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s signalfd4 -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s signalfd4 -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s signalfd4 -n 2

	echo "## gettimeofday ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s gettimeofday -n 0

${NG}	echo "## sched_yield ##"
${NG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s sched_yield -n 0

	echo "## set_tid_address ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s set_tid_address -n 0

	echo "## getrusage ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 1
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 3
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getrusage -n 4

	echo "## tkill ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tkill -n 0
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s tkill -n 1

	echo "## times ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s times -n 0
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s times -n 1

	echo "## nanosleep ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nanosleep -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nanosleep -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nanosleep -n 2
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s nanosleep -n 3

	echo "## getitimer ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getitimer -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getitimer -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s getitimer -n 2

	echo "## setitimer ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setitimer -n 0
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setitimer -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setitimer -n 2
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s setitimer -n 3

	echo "## clock_gettime ##"
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_gettime -n 0
${HANG}	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_gettime -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_gettime -n 2

	echo "## clock_getres ##"
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_getres -n 0
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_getres -n 1
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s clock_getres -n 2

	echo "## readlinkat ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s readlinkat -n 0 -- -f ${temp} -l ${link}

#	echo "## fpregs ##"
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 0
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 1
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 2
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 3
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 4
#SKIP	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s fpregs -n 5 -- -p $mck_max_cpus

	echo "## force_exit ##"
	${mcexec} $execve_comm "$app_prefix/test_mck" $execve_arg_end -s force_exit -n 0 -- -f $mmapfile_name -d /dev/test_mck/mmap_dev &
${DRYRUN}	sleep 3
${DRYRUN}	echo "send SIGKILL for mcexec."
${DRYRUN}	kill -9 `${pidofcomm}`
${DRYRUN}	echo "rmmod test_drv"
${DRYRUN}	sh "$rmmod_test_drv_sh"

	if [ "${runHOST}" != "yes" ]; then
		echo "shutdown_mck..."
		sh "$mcstop"
		sleep 1
	fi

	#### finalize ####

if [ $do_initialize = "yes" ]; then
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
fi

#	if [ -e ${sh_base}/continue_end ]; then
#		echo "find continue_end file."
#		break
#	fi
#done

