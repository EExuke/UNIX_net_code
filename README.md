# UNIX_net_code
---
# UNIX Networking Code:
- source code: https://gitee.com/lifeinvader/unpv13e.git

## volume_1: API
1.简介和TCP/IP
#### 1 简介
- 一个简单的时间获取客户程序：daytimetcpcli
- 协议无关性：daytimetcpcliv6
- 一个简单的时间获取服务器程序 daytimetcpsrv

2.基本套接字编程
#### 3 套接字编程简介
- 确定主机字节序 byteorder
#### 4 基本TCP套接字编程
- 显示客户IP地址和端口号的时间服务器程序 daytimetcpsrv1
#### 5 TCP客户/服务器程序示例
- TCP回射服务器程序 tcpserv01
- TCP回射客户程序   tcpcli01
- 与服务器建立了5个链接的TCP客户程序 tcpcli04
- 处理accept返回EINTR错误的TCP服务器程序最终版本 tcpserv04
#### 6 select和poll函数
- 使用单进程和select的TCP服务器程序 tcpservselect01
- 使用poll函数的TCP服务器程序 tcpservpoll01
#### 7 套接字选项
- 套接字选项检查程序 checkopts
#### 8 基本UDP套接字编程
- UDP回射服务器程序 udpserv01
- UDP回射客户程序 udpcli01
- 使用connect来确定输出接口的UDP程序 udpcli09
- 使用select处理TCP和UDP的回射服务器程序 udpservselect01
#### 10 SCTP客户/服务器程序例子
- SCTP流分回射服务器程序 sctpserv01
- SCTP流分回射客户程序 sctpclient01
- 服务器程序返回更多流 sctpserv02 (略)
- 服务器程序应答同时终止关联 sctpserv03 (略)
- 客户程序预先中止关联 sctpclient02 (略)
#### 11 名字与地址转换
- 调用gethostbyname并显示返回的信息 hostent
- 使用gethostbyname和getservbyname的时间获取客户程序 daytimetcpcli1
- 用tcp_connect重新编写的时间获取客户程序 daytimetcpclin
- 用tcp_listen重新编写的时间获取服务器程序 daytimetcpsrv1n
- 使用tcp_listen的协议无关时间获取服务器程序 daytimetcpsrv2
- 使用udp_client的UDP时间获取客户程序 daytimeudpcli1
- 协议无关的UDP时间获取服务器程序 daytimeudpsrv2

3.高级套接字编程
#### 12 IPv4与IPv6互操作性
#### 13 守护进程和inetd
#### 14 高级I/O函数
#### 15 Unix域协议
#### 16 非阻塞式I/O
#### 17 ioctl操作
#### 18 路由套接字
#### 19 密钥管理套接字
#### 20 广播
#### 21 多播
#### 22 高级UDP套接字编程
#### 23 高级SCTP套接字编程
#### 24 带外数据
#### 25 信号驱动式I/O
#### 26 线程
#### 27 IP选项
#### 28 原始套接字
#### 29 数据链路访问

4.客户/服务器程序设计范式
#### tcp测试用客户程序
#### tcp迭代服务器程序
#### tcp并发服务器程序
#### tcp预先派生子进程服务程序，accept无上锁保护
#### tcp预先派生子进程服务程序，accept使用文件上锁保护
#### tcp预先派生子进程服务程序，accept使用线程上锁保护
#### tcp预先派生子进程服务程序，传递描述符
#### tcp并发服务器程序，每个客户一个线程
#### tcp预先创建线程服务器程序，每个线程各自accept
#### tcp预先创建线程服务器程序，主线程统一accept

5.流
#### TPI传输提供者接口
---

## volume_2: IPC

---
