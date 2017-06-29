#!/bin/sh
# test_shell.sh COPYRIGHT FUJITSU LIMITED 2015

endshell()
{
  echo "## ls command ##"
  ls
  echo "shell end.(shellfunc called.)"
}

echo "## echo command ##"
echo "Hello World"

echo "## pwd command ##"
pwd
home=`pwd`

echo "## outside command(execve_app) ##"
$TEST_HOME/execve_app

echo "## for, seq, if-elif-else ##"
for temp in `seq 0 5`
do
  if [ $temp -eq 5 ] ; then
    echo "\$temp == 5"
  elif [ $temp -eq 0 ] ; then
    echo "\$temp == 0"
  else
    echo "\$temp != 5 && \$temp != 0"
  fi
done

echo "## echo, cat, rm command ##"
echo "cat, rm command." > $home/shell_temp
cat $home/shell_temp
rm $home/shell_temp

echo "## shellfunction ##"
endshell
