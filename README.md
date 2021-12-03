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
- dg_echo
- UDP回射客户程序 udpcli01
- dg_cli
- dgcliaddr
- dgcliconnect
- dgcliloop1
- dgecholoop1
- dgecholoop2
- udpcli09
- udpservselect01
#### 10 SCTP客户/服务器程序例子
- SCTP流分回射服务器程序 sctpserv01
- SCTP流分回射客户程序 sctpclient01
- sctp_strcli
- sctp_strcliecho
- sctp_strcliecho2
- 服务器程序返回更多流 sctpserv02
- 服务器程序应答同时终止关联 sctpserv03
- 客户程序预先中止关联 sctpclient02
#### 11 名字与地址转换
- 调用gethostbyname并显示返回的信息 hostent
- 使用gethostbyname和getservbyname的时间获取客户程序 daytimetcpcli1
- host_serv
- tcp_connect
- 用tcp_connect重新编写的时间获取客户程序 daytimetcpclin
- tcp_listen
- 用tcp_listen重新编写的时间获取服务器程序 daytimetcpsrv1n
- 使用tcp_listen的协议无关时间获取服务器程序 daytimetcpsrv2
- udp_client
- 使用udp_client的UDP时间获取客户程序 daytimeudpcli1
- udp_connect
- udp_server
- 协议无关的UDP时间获取服务器程序 daytimeudpsrv2

3.高级套接字编程

4.客户/服务器程序设计范式

5.流

---

## volume_2: IPC

---
