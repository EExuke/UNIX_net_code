/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : 2_basic_socket.c
 *     FIRST CREATION DATE    : 2021/11/29
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/11/29
 *     FILE DESCRIPTION       : functions in part 2 basic socket coding
** ************************************************************************** */

#include "unp.h"
#include "sum.h"
#include "2_basic_socket.h"
#include <netinet/tcp.h>
#include <limits.h>

// functions
/***************************************************************************************
 * Description   : get host byte order
 ***************************************************************************************/
int byte_order()
{
	union {
		short s;
		char c[sizeof(short)];
	} un;  //2byte

	un.s = 0x0102;
	printf("%s: ", CPU_VENDOR_OS);
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2) {
			printf("big-endian\n");
		} else if (un.c[0] == 2 && un.c[1] == 1) {
			printf("little-endian\n");
		} else {
			printf("unknown\n");
		}
	} else {
		printf("sizeof(short) = %ld\n", sizeof(short));
	}
	return 0;
}

/***************************************************************************************
 * Description   : serv show client ip and port
 ***************************************************************************************/
int daytime_tcp_srv1()
{
	int listenfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr = {0};
	struct sockaddr_in cliaddr = {0};
	char buff[MAXLINE];
	time_t ticks;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);  //my daytime server port

	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);

	while (1) {
		len = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA*)&cliaddr, &len);
		printf("connection from %s, port %d\n", Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "daytime_tcp_srv: %.24s\r\n", ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}

/***************************************************************************************
 * Description   : tcpserv01
 ***************************************************************************************/
int tcp_serv01()
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in servaddr = {0};
	struct sockaddr_in cliaddr = {0};

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);

	while (1) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);
		if ((childpid = Fork()) == 0) { /* child process */
			Close(listenfd);  //close listen socket
			str_echo(connfd); //process the request
			exit(0);
		}
		Close(connfd);  //parent close connected socket
	}
}

/***************************************************************************************
 * Description   : tcpcli01
 ***************************************************************************************/
int tcp_cli01(char *ip_addr)
{
	int sockfd;
	struct sockaddr_in servaddr = {0};

	if (ip_addr == NULL || ip_addr[0] == '\0') {
		err_quit("input ip_addr is error");
	}

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, ip_addr, &servaddr.sin_addr);

	Connect(sockfd, (SA*)&servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd); //do it all

	return 0;
}

/***************************************************************************************
 * Description   : sigchldwait
 ***************************************************************************************/
void sig_chld_wait(int signo)
{
	pid_t pid;
	int stat;

	pid = wait(&stat);
	printf("child %d terminated\n", pid);

	return;
}

/***************************************************************************************
 * Description   : tcpcli04
 ***************************************************************************************/
int tcp_cli04(char *ip_addr)
{
	int i, sockfd[5];
	struct sockaddr_in servaddr = {0};

	if (ip_addr == NULL || ip_addr[0] == '\0') {
		err_quit("input ip_addr is error");
	}

	for (i=0; i<5; i++) {
		sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);

		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(SERV_PORT);
		Inet_pton(AF_INET, ip_addr, &servaddr.sin_addr);

		Connect(sockfd[i], (SA*)&servaddr, sizeof(servaddr));
	}

	/*str_cli(stdin, sockfd[0]); //do it all*/
	str_cli_select02(stdin, sockfd[0]); //do it all

	return 0;
}

/***************************************************************************************
 * Description   : sigchldwaitpid
 ***************************************************************************************/
void sig_chld_wait_pid(int signo)
{
	pid_t pid;
	int stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		printf("child %d terminated\n", pid);
	}

	return;
}

/***************************************************************************************
 * Description   : tcpserv04
 ***************************************************************************************/
int tcp_serv04()
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in servaddr = {0};
	struct sockaddr_in cliaddr = {0};
	void sig_chld_wait_pid(int);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);
	Signal(SIGCHLD, sig_chld_wait_pid);  //must call waitpid()

	while (1) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				err_sys("accept error");
			}
		}
		if ((childpid = Fork()) == 0) { /* child process */
			Close(listenfd);  //close listen socket
			str_echo(connfd); //process the request
			exit(0);
		}
		Close(connfd);  //parent close connected socket
	}
}

/***************************************************************************************
 * Description   : str_cli11
 ***************************************************************************************/
void str_cli11(FILE *fp, int sockfd)
{
	char sendline[MAXLINE] = {0};
	char recvline[MAXLINE] = {0};

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, 1);
		sleep(1);
		Writen(sockfd, sendline+1, strlen(sendline)-1);

		if (Readline(sockfd, recvline, MAXLINE) == 0) {
			err_quit("str_cli: server terminated prematurely");
		}

		Fputs(recvline, stdout);
	}
}

/***************************************************************************************
 * Description   : str_echo08
 ***************************************************************************************/
void str_echo08(int sockfd)
{
	long arg1, arg2;
	ssize_t n;
	char line[MAXLINE] = {0};

	while (1) {
		if ((n = Readline(sockfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */

		if (sscanf(line, "%ld%ld", &arg1, &arg2) == 2) {
			snprintf(line, sizeof(line), "%ld\n", arg1 + arg2);
		} else {
			snprintf(line, sizeof(line), "input error\n");
		}

		n = strlen(line);
		Writen(sockfd, line, n);
	}
}

/***************************************************************************************
 * Description   : str_cli09
 ***************************************************************************************/
void str_cli09(FILE *fp, int sockfd)
{
	char sendline[MAXLINE];
	struct args args;
	struct result result;

	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		if (sscanf(sendline, "%ld%ld", &args.arg1, &args.arg2) != 2) {
			printf("invalid input: %s", sendline);
			continue;
		}
		Writen(sockfd, &args, sizeof(args));

		if (Readn(sockfd, &result, sizeof(result)) == 0) {
			err_quit("str_cli: server terminated prematurely");
		}
		printf("%ld\n", result.sum);
	}
}

/***************************************************************************************
 * Description   : str_echo09
 ***************************************************************************************/
void str_echo09(int sockfd)
{
	ssize_t n;
	struct args args;
	struct result result;

	while (1) {
		if ( (n = Readn(sockfd, &args, sizeof(args))) == 0) {
			return;		/* connection closed by other end */
		}

		result.sum = args.arg1 + args.arg2;
		Writen(sockfd, &result, sizeof(result));
	}
}

/***************************************************************************************
 * Description   : strcliselect01
 ***************************************************************************************/
void str_cli_select01(FILE *fp, int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	FD_ZERO(&rset);
	while (1) {
		FD_SET(fileno(fp), &rset);  //fileno：change "FILE" to "int"
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) { //socket is readable
			if (Readline(sockfd, recvline, MAXLINE) == 0) {
				err_quit("str_cli: server terminated prematurely");
			}
			Fputs(recvline, stdout);
		}

		if (FD_ISSET(fileno(fp), &rset)) { //input is readable
			if (Fgets(sendline, MAXLINE, fp) == NULL) {
				return;  //all done
			}
			Writen(sockfd, sendline, strlen(sendline));
		}
	}
}

/***************************************************************************************
 * Description   : strcliselect02
 ***************************************************************************************/
void str_cli_select02(FILE *fp, int sockfd)
{
	int n;
	int maxfdp1;
	int stdineof = 0;
	char buf[MAXLINE];
	fd_set rset;

	FD_ZERO(&rset);
	while (1) {
		if (stdineof == 0) {
			FD_SET(fileno(fp), &rset);  //fileno：change "FILE" to "int"
		}
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) { //socket is readable
			if ((n = Read(sockfd, buf, MAXLINE)) == 0) {
				if (stdineof == 1) {
					return;  //normal termination
				} else {
					err_quit("str_cli: server terminated prematurely");
				}
			}
			Write(fileno(fp), buf, n);
		}

		if (FD_ISSET(fileno(fp), &rset)) { //input is readable
			if ((n = Read(fileno(fp), buf, MAXLINE)) == 0) {
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR);  //send FIN
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			Writen(sockfd, buf, n);
		}
	}
}

/***************************************************************************************
 * Description   : tcpservselect01
 ***************************************************************************************/
int tcp_serv_select01()
{
	int i, maxi, maxfd;
	int listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr = {0};
	struct sockaddr_in servaddr = {0};

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);

	maxfd = listenfd;  //initialize
	maxi = -1;         //max index into client[] array
	for (i=0; i<FD_SETSIZE; i++) {
		client[i] = -1;  //-1 indicates available entry
	}
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	while (1) {
		rset = allset;    //structure assignment
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);

			for (i=0; i<FD_SETSIZE; i++) {
				if (client[i] < 0) {
					client[i] = connfd;  //save descriptor
					break;
				}
			}
			if (i == FD_SETSIZE) {
				err_quit("too many clients");
			}

			FD_SET(connfd, &allset);  //add new descriptor to set
			if (connfd > maxfd) {
				maxfd = connfd;    //for Select
			}
			if (i > maxi) {
				maxi = i;    //max index in client[] array
			}
			if (--nready <= 0) {
				continue;    //no more readable descriptors
			}
		}

		for (i=0; i<=maxi; i++) {  //check all clients for data
			if ((sockfd = client[i]) < 0) {
				continue;
			}
			if (FD_ISSET(sockfd, &rset)) {
				if ((n = Read(sockfd, buf, MAXLINE)) == 0) {  //connection closed by client
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else {
					Writen(sockfd, buf, n);
				}
				if (--nready <= 0) {
					break;    //no more readable descriptors
				}
			}
		}
	}
}

/***************************************************************************************
 * Description   : tcpservpoll01
 ***************************************************************************************/
/*"OPEN_MAX" is cancel after linux-2.6.24*/
#define OPEN_MAX    1024

int tcp_serv_poll01()
{
	int i, maxi;
	int listenfd, connfd, sockfd;
	int nready;
	ssize_t n;
	char buf[MAXLINE];
	socklen_t clilen;
	struct pollfd client[OPEN_MAX];
	struct sockaddr_in cliaddr = {0};
	struct sockaddr_in servaddr = {0};

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for (i=1; i<OPEN_MAX; i++) {
		client[i].fd = -1;  //-1 indicates available entry
	}
	maxi = 0;         //max index into client[] array

	while (1) {
		nready = Poll(client, maxi+1, INFTIM);

		if (client[0].revents & POLLRDNORM) {   //new client connection
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);

			for (i=1; i<OPEN_MAX; i++) {
				if (client[i].fd < 0) {
					client[i].fd = connfd;  //save descriptor
					break;
				}
			}
			if (i == OPEN_MAX) {
				err_quit("too many clients");
			}

			client[i].events = POLLRDNORM;
			if (i > maxi) {
				maxi = i;    //max index in client[] array
			}
			if (--nready <= 0) {
				continue;    //no more readable descriptors
			}
		}

		for (i=1; i<=maxi; i++) {  //check all clients for data
			if ((sockfd = client[i].fd) < 0) {
				continue;
			}
			if (client[i].revents & (POLLRDNORM | POLLERR)) {
				if ((n = read(sockfd, buf, MAXLINE)) < 0) {
					if (errno == ECONNRESET) {  //connection reset by client
						Close(sockfd);
						client[i].fd = -1;
					} else {
						err_sys("read error");
					}
				} else if (n == 0) {  //connection reset by client
					Close(sockfd);
					client[i].fd = -1;
				} else {
					Writen(sockfd, buf, n);
				}
				if (--nready <= 0) {
					break;    //no more readable descriptors
				}
			}
		}
	}
}

/***************************************************************************************
 * Description   : checkopts.c
 ***************************************************************************************/
union val {
  int i_val;
  long l_val;
  struct linger linger_val;
  struct timeval timeval_val;
} val;

static char strres[128];

//将标志选项转换为字符串
static char *sock_str_flag(union val *ptr, int len)
{
/* *INDENT-OFF* */
	if (len != sizeof(int)) {
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
	} else {
		snprintf(strres, sizeof(strres), "%s", (ptr->i_val == 0) ? "off" : "on");
	}
	return(strres);
/* *INDENT-ON* */
}

static char *sock_str_int(union val *ptr, int len)
{
	if (len != sizeof(int)) {
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
	} else {
		snprintf(strres, sizeof(strres), "%d", ptr->i_val);
	}
	return(strres);
}

static char *sock_str_linger(union val *ptr, int len)
{
	struct linger	*lptr = &ptr->linger_val;

	if (len != sizeof(struct linger)) {
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct linger)", len);
	} else {
		snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d", lptr->l_onoff, lptr->l_linger);
	}
	return(strres);
}

static char *sock_str_timeval(union val *ptr, int len)
{
	struct timeval	*tvptr = &ptr->timeval_val;

	if (len != sizeof(struct timeval)) {
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct timeval)", len);
	} else {
		snprintf(strres, sizeof(strres), "%ld sec, %ld usec", tvptr->tv_sec, tvptr->tv_usec);
	}
	return(strres);
}

struct sock_opts {
  const char *opt_str;
  int opt_level;
  int opt_name;
  char *(*opt_val_str)(union val *, int);
} sock_opts[] = {
	{ "SO_BROADCAST",		SOL_SOCKET,	SO_BROADCAST,	sock_str_flag },
	{ "SO_DEBUG",			SOL_SOCKET,	SO_DEBUG,		sock_str_flag },
	{ "SO_DONTROUTE",		SOL_SOCKET,	SO_DONTROUTE,	sock_str_flag },
	{ "SO_ERROR",			SOL_SOCKET,	SO_ERROR,		sock_str_int },
	{ "SO_KEEPALIVE",		SOL_SOCKET,	SO_KEEPALIVE,	sock_str_flag },
	{ "SO_LINGER",			SOL_SOCKET,	SO_LINGER,		sock_str_linger },
	{ "SO_OOBINLINE",		SOL_SOCKET,	SO_OOBINLINE,	sock_str_flag },
	{ "SO_RCVBUF",			SOL_SOCKET,	SO_RCVBUF,		sock_str_int },
	{ "SO_SNDBUF",			SOL_SOCKET,	SO_SNDBUF,		sock_str_int },
	{ "SO_RCVLOWAT",		SOL_SOCKET,	SO_RCVLOWAT,	sock_str_int },
	{ "SO_SNDLOWAT",		SOL_SOCKET,	SO_SNDLOWAT,	sock_str_int },
	{ "SO_RCVTIMEO",		SOL_SOCKET,	SO_RCVTIMEO,	sock_str_timeval },
	{ "SO_SNDTIMEO",		SOL_SOCKET,	SO_SNDTIMEO,	sock_str_timeval },
	{ "SO_REUSEADDR",		SOL_SOCKET,	SO_REUSEADDR,	sock_str_flag },
#ifdef	SO_REUSEPORT
	{ "SO_REUSEPORT",		SOL_SOCKET,	SO_REUSEPORT,	sock_str_flag },
#else
	{ "SO_REUSEPORT",		0,			0,				NULL },
#endif
	{ "SO_TYPE",			SOL_SOCKET,	SO_TYPE,		sock_str_int },
#ifdef SO_USELOOPBACK
	{ "SO_USELOOPBACK",		SOL_SOCKET,	SO_USELOOPBACK,	sock_str_flag },
#endif
	{ "IP_TOS",				IPPROTO_IP,	IP_TOS,			sock_str_int },
	{ "IP_TTL",				IPPROTO_IP,	IP_TTL,			sock_str_int },
#ifdef	IPV6_DONTFRAG
	{ "IPV6_DONTFRAG",		IPPROTO_IPV6,IPV6_DONTFRAG,	sock_str_flag },
#else
	{ "IPV6_DONTFRAG",		0,			0,				NULL },
#endif
#ifdef	IPV6_UNICAST_HOPS
	{ "IPV6_UNICAST_HOPS",	IPPROTO_IPV6,IPV6_UNICAST_HOPS,sock_str_int },
#else
	{ "IPV6_UNICAST_HOPS",	0,			0,				NULL },
#endif
#ifdef	IPV6_V6ONLY
	{ "IPV6_V6ONLY",		IPPROTO_IPV6,IPV6_V6ONLY,	sock_str_flag },
#else
	{ "IPV6_V6ONLY",		0,			0,				NULL },
#endif
	{ "TCP_MAXSEG",			IPPROTO_TCP,TCP_MAXSEG,		sock_str_int },
	{ "TCP_NODELAY",		IPPROTO_TCP,TCP_NODELAY,	sock_str_flag },
#ifdef	SCTP_AUTOCLOSE
	{ "SCTP_AUTOCLOSE",		IPPROTO_SCTP,SCTP_AUTOCLOSE,sock_str_int },
#else
	{ "SCTP_AUTOCLOSE",		0,			0,				NULL },
#endif
#ifdef	SCTP_MAXBURST
	{ "SCTP_MAXBURST",		IPPROTO_SCTP,SCTP_MAXBURST,	sock_str_int },
#else
	{ "SCTP_MAXBURST",		0,			0,				NULL },
#endif
#ifdef	SCTP_MAXSEG
	{ "SCTP_MAXSEG",		IPPROTO_SCTP,SCTP_MAXSEG,	sock_str_int },
#else
	{ "SCTP_MAXSEG",		0,			0,				NULL },
#endif
#ifdef	SCTP_NODELAY
	{ "SCTP_NODELAY",		IPPROTO_SCTP,SCTP_NODELAY,	sock_str_flag },
#else
	{ "SCTP_NODELAY",		0,			0,				NULL },
#endif
	{ NULL,					0,			0,				NULL }
};

int checkopts()
{
	int fd;
	socklen_t len;
	struct sock_opts *ptr;

	for (ptr = sock_opts; ptr->opt_str != NULL; ptr++) {
		printf("%s: ", ptr->opt_str);
		if (ptr->opt_val_str == NULL) {
			printf("(undefined)\n");
		} else {
			switch(ptr->opt_level) {
			case SOL_SOCKET:
			case IPPROTO_IP:
			case IPPROTO_TCP:
				fd = Socket(AF_INET, SOCK_STREAM, 0);
				break;
#ifdef	IPV6
			case IPPROTO_IPV6:
				fd = Socket(AF_INET6, SOCK_STREAM, 0);
				break;
#endif
#ifdef	IPPROTO_SCTP
			case IPPROTO_SCTP:
				fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
				break;
#endif
			default:
				err_quit("Can't create fd for level %d\n", ptr->opt_level);
			}

			len = sizeof(val);
			if (getsockopt(fd, ptr->opt_level, ptr->opt_name,
						   &val, &len) == -1) {
				err_ret("getsockopt error");
			} else {
				printf("default = %s\n", (*ptr->opt_val_str)(&val, len));
			}
			close(fd);
		}
	}
	return 0;
}

