# UNIX_net_code
---
# UNIX Networking Code:
- source code: https://gitee.com/lifeinvader/unpv13e.git

## volume_1: API
1.简介和TCP/IP
### 1
- 一个简单的时间获取客户程序：daytimetcpcli
- 协议无关性：daytimetcpcliv6
- 一个简单的时间获取服务器程序 daytimetcpsrv

2.基本套接字编程
### 3
- 确定主机字节序 byteorder
- 仅支持IPv4的inet_pton简化版本 inet_pton_ipv4
- 仅支持IPv4的inet_ntop简化版本 inet_ntoppton_ipv4
- 自己的sock_ntop函数 sock_ntop
- 从一个描述符读n字节 readn
- 往一个描述符写n字节 writen
- 从一个描述符读文本行，一次读1个字节 raw_readline
- readline改进版 readline
### 4
- 显示客户IP地址和端口号的时间服务器程序 daytimetcpsrv1
### 5
- TCP回射服务器程序 tcpserv01
- TCP回射客户程序   tcpcli01
- 调用wait的SIGCHLD信号处理函数 sigchldwait
- 与服务器建立了5个链接的TCP客户程序 tcpcli04
- 调用waitpid函数的sig_chld函数最终版本 sigchldwaitpid
- 处理accept返回EINTR错误的TCP服务器程序最终版本 tcpserv04
- 调用writen两次的str_cli函数 str_cli11
- 对两个数求和的str_echo函数 str_echo08
- 头文件sum.h
- 发送两个二进制整数给服务器的str_cli函数 str_cli09
- 对两个二进制整数求和的str_echo函数 str_echo09
### 6
-

3.高级套接字编程

4.客户/服务器程序设计范式

---

## volume_2: IPC

---
