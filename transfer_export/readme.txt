说明：
1. 目前只实现了单个客户端进程，每传输一个文件需要一个进程，传输完毕进程退出。
多文件传输计划实现一个管理程序，根据文件列表，同时启动多个客户端传输。

2. 目前已经试验90M以下的单个文件成功传输，900MB单个文件传输后检验失败，有待查验。

3. 执行方法(!!!! 非root用户执行 !!!!)
	1). 解压源码。
	2). 在源码根目录下执行"make clean && make && make install"。
	3). 进入bin目录下，执行"./prepare.sh"，申请共享内存和信号量。
	4). 传输文件执行"./client $(TEST_FILE)", TEST_FILE为需要传输的文件。可多次执行此操作。
	5). (!!!! 脚本会将用户的所有共享内存和信号都删除，若有其他环境，请单独删除 !!!!)
	结束所有环境请执行"./clear.sh"(与执行prepare.sh同一个用户)，停止后台进程，删除共享内存和信号量。
	
源码结构：
bin/	可执行文件目录
		conf/		中转服务器共享内存配置文件目录
		data/		中转服务器FIFO文件目录
config.mak	编译配置
include/	头文件目录
		share_mem/	中转服务器共享内存头文件
Makefile	总Makefile
src/	源码文件
		client/		客户端代码
		read_proc/	中转服务器内，读取共享内存并发送给后台服务器的进程
		share_mem/	共享内存管道，管理代码
		shm_app/		申请共享内存和信号量，初始化程序
		storage_server/		存储后台服务器
		transfer/		中转服务器网络收发相关代码
		