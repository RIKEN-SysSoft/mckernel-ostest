#!/bin/sh

if [ "$1" == "" ]; then
    list="testlist_linux"
else
    list=$1
fi

while read line
do
    sudo sh $line -H
done < $list

