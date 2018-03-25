# MessageMonitor

## 项目描述：
采用C/S结构，Master自动获取Client的信息，可以并发监控多台Client;Client端收集CPU、内存、磁盘、资源消耗、用户及进程等系统信息并将信息按照健康程度分级，实时发送到Master端，达到预警值向Master报警、
在Master与Client运行的过程中，将重要的运行信息写入日志文件


## 功能
* 从配置文件读取所有参数，如端口及Client的IP
* Master并发10个进程采集信息，将10个进程与所有的Client做简单的映射
* Master与Client,设立长短连接，分别传递控制信息，数据信息
* Client 实现守护进程
* Client 按照指定的时间间隔获取信息，减少读写磁盘的次数
* Client 将信息成功发送后，本地清空

## 在完善的问题
1. 数据文件的加密
2. Client端对恶意进程的监控
