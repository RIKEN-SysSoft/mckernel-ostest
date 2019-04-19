# calculate deltas
getrusage_time () {
	temp=`grep $1 $2 | cut -d '=' -f 2| perl -ne 'if (/(\d+)s \+ (\d+)us/) { if ($i != 0) { printf("%d\n", ($1 - $ps) * 1000000 + ($2 - $pus)); } $ps = $1; $pus = $2; $i++; }'`
	ret=`echo $temp | cut -d ' ' -f $3`;
}

# calculate deltas
getrusage_maxrss () {
	temp=`grep $1 $2 | cut -d '=' -f 2 | perl -ne 'if ($i != 0) { print $_ - $prev . "\n"; } $prev = $_; $i++;'`
	ret=`echo $temp | cut -d ' ' -f $3`;
}

# getrusage_section sections begins with begin and ends with end
getrusage_section () {
	awk -v begin="$2" -v end="$3" '$0 ~ begin { c = 1 } $0 ~ end { c = 0 } { if (c == 1) { print } }' $1 > /tmp/output.txt
}
