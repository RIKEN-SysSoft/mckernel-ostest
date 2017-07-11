#!/bin/sh

while read line
do
    sudo sh $line -H
done < testlist_ofp

