#!/bin/bash

source ./PiHealth.conf

Nowtime=`date +"%Y-%m-%d %H:%M:%S"`


Mes=(`free -m |head -n 2|tail -n 1 |awk '{printf("%d %d %d ",$2,$3,$5)}'`)

MemSum=${Mes[0]}
MemUse=${Mes[1]}
MemFree=${Mes[2]}
MemUsePrec=`echo "scale=4;($MemUse/$MemSum)*100" | bc`
Num1=0.2
Num2=0.8
MemDymPrec=`echo "scale=4;($Num1 * $MemUse + $Num2* $1)" | bc`

if [[ `echo $MemDymPrec '>=' 80 | bc -l` = 1 ]]; then
  echo "Warning of MEM !!!" >>$Warnlog
fi

echo "$Nowtime $MemSum  $MemFree  ${MemUsePrec}% ${MemDymPrec}% " #>>$Memlog
