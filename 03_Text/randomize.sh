#!/bin/sh
if [ $# = 1 ]; then sleep_time=$1; else sleep_time=0; fi

TMPFILE="`mktemp`"
i=0; j=0;

# while IFS='' read -r -n1 byte; do
# 	ord=$(printf "%b" "${byte:-\000}" | hexdump -X | { read offset hex; echo "$hex"; });
# 	printf "%d %d %b\n" $i $j $ord >> TMPFILE;
# 	if [ $ord = 00 ]; then j=0; i=$((i+1)); else j=$((j+1)); fi
# done

# max_i=$i

# tput clear
# cat TMPFILE | shuf | while read -r X Y sym; do
# 	tput cup $X $Y;
# 	printf "%b\n" "\u${sym}";
# 	sleep $sleep_time;
# done

tput clear

{ while IFS='' read -r -n 1 byte; do
	ord=$(printf "%b" "${byte:-\000}" | hexdump -X | { read offset hex; echo "$hex"; });
	printf "%d %d %b\n" $i $j $ord;
	if [ $ord = 00 ]; then j=0; i=$((i+1)); printf "%d" $i > TMPFILE; else j=$((j+1)); fi
done } | shuf | { while read -r X Y sym; do
	tput cup $X $Y;
	printf "%b\n" "\u${sym}";
	sleep $sleep_time;
done }

tput cup $(cat TMPFILE) 0
test -r "$TMPFILE" && rm "$TMPFILE"
