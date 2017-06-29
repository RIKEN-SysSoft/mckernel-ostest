#!/bin/sh
# run_test_x86.sh COPYRIGHT FUJITSU LIMITED 2016
this_dir="$(cd $(dirname $0); pwd)"

usage() {
  echo "$0 [-N] [-H] -n | -e | -b"
  echo "  options:"
  echo "    -N  NG/HANG item including run. (default off)"
  echo "    -H  run on host. (default off)"
  echo "    -n  normal test."
  echo "    -e  use execve test."
  echo "    -b  normal and use execve test."
  echo "    -h  show usage."
}

normal=
execve=
option=
while getopts NHnebh OPT
do
  case $OPT in
    N)
      option="$option -N"
      ;;
    H)
      option="$option -H"
      trap ":" USR1
      ;;
    n)
      normal="yes"
      ;;
    e)
      execve="yes"
      ;;
    b)
      normal="yes"
      execve="yes"
      ;;
    h)
      usage
      exit 0
      ;;
    *)
      usage
      exit 0
      ;;
  esac
done

if [ "$normal" != "yes" ] && [ "$execve" != "yes" ]; then
  usage
  exit 0
fi

if [ "$normal" = "yes" ] ; then
  sh ${this_dir}/run_testset_x86.sh $option
fi

if [ "$execve" = "yes" ] ; then
  sh ${this_dir}/run_testset_x86.sh -e $option
fi
