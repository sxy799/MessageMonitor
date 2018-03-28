#!/bin/bash


source ./PiHealth.conf

time=`date +"%Y-%m-%d %H:%M:%S"` # echo "$time" # >> userlog 
mes=`awk -F: -v sum=0 '{if($3 > 1000 && $3 != 65534) {sum++;printf("User["sum"]=%s\n", $1)}} END {print sum}' /etc/passwd` 
 # >> $Userlog 
mes=`last | head -n -2 | cut -d " " -f 1 | sort | uniq -c | head -n 3  | tail -n +3 | awk '{printf("User["NR"]=%s, TTY=%s, FROM%s\n", $1, $2, $3)}' `
echo "$time  $mes" 
# >> $Userlog

