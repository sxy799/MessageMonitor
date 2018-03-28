#!/bin/bash


source ./PiHealth.conf

NowTime=`date +"%Y-%m-%d %H:%M:%S"`

hostname=`hostname`

Version=`cat /etc/issue`

Info=`uptime`

eval $(df -T -x tmpfs -m -x devtmpfs | tail -n +2 | awk \
    '{printf("paramount["NR"]=%d;paraleft["NR"]=%d;pararate["NR"]=%d;\
    paratype["NR"]=%s\n", $3,$5,$6,$7);DiskSum+=$3;LeftSum+=$5;}\
    END{printf("paracount=%d;DiskSum=%d;LeftSum=%d\n", NR,DiskSum,\
    LeftSum)}')

DiskPerc=$[ (100-$LeftSum*100/$DiskSum) ]


Mem=(`free -m | head -n 2 | tail -n 1 | tr -s " " |  awk '{printf("%d %d %d", $2, $3, $7)}'`)

MemTotalSize=${Mem[0]}

Memused=${Mem[1]}

MemPerc=$[ ($MemTotalSize*100/$Memused) ]

CpuTemp=`cat /sys/class/thermal/thermal_zone0/temp`
CpuTemp=`echo "scale=2;$CpuTemp/1000" | bc`


CpuWarnLevel="normal"
if [[ `echo $CpuTemp '>=' 80 | bc -l` = 1 ]];then
    CpuWarnLevel="warning"
elif [[ `echo $CpuTemp '>=' 70 | bc -l` = 1 ]];then
    CpuWarnLevel="note"
fi

DiskWarnLevel="normal"
if [[ `echo $DiskPerc '>=' 90 | bc -l` = 1 ]];then
    DiskWarnLevel="warning"
elif [[ `echo $DiskPerc '>=' 80 | bc -l` = 1 ]];then
    DiskWarnLevel="note"
fi

MemWarnLevel="normal"
if [[ `echo $MemPerc '>=' 80 | bc -l` = 1 ]];then
    MemWarnLevel="warning"
elif [[ `echo $MemPerc '>=' 70 | bc -l` = 1 ]];then
    MemWarnLevel="note"
fi

echo $NowTime $hostname $Version $Info $DiskSum $DiskPerc% $MemTotalSize $MemPerc% $CpuTemp $DiskWarnLevel $MemWarnLevel $CpuWarnLevel #>> $Syslog
