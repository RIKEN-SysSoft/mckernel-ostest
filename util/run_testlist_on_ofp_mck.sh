#!/bin/sh

export PJM_MCK_AVAILABLE=1

while read line
do
    naccept=`pjstat --limit | fgrep gg10 | awk '{print $2}' | cut -d '/' -f 1`
    while [ $naccept -gt 20 ]
    do
	sleep 10
	naccept=`pjstat --limit | fgrep gg10 | awk '{print $2}' | cut -d '/' -f 1`
    done
    (cd testcase_ofp; pjsub $line)
done < testlist_ofp
