#!/bin/sh

if [ $# -ne 2 ]; then
    echo $0 '<testoutputdir> <testworkdir>'
    exit -1
fi


for file in `ls $1`
do
    core=`ls $2/$file | wc -l`
    nl=`wc -l $1/$file | cut -d ' ' -f 1`
    result=`awk -F ':' '$1=="RESULT" {print $2}' $1/$file`
    echo $file $core $nl $result
done
