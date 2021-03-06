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
		FD_SET(fileno(fp), &rset);  //fileno???change "FILE" to "int"
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
			FD_SET(fileno(fp), &rset);  //fileno???change "FILE" to "int"
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

//?????????????????????????????????
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

/***************************************************************************************
 * Description   : misc functions of chapter 8
 ***************************************************************************************/
void dg_ehco(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];

	while (1) {
		len = clilen;
		n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

		Sendto(sockfd, mesg, n, 0, pcliaddr, len);
	}
}

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int n;
	char sendline[MAXLINE];
	char recvline[MAXLINE+1];
	socklen_t len;
	struct sockaddr *preply_addr;

	preply_addr = Malloc(servlen);
	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		len = servlen;
		n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
		if (len != servlen || memcmp(pservaddr, preply_addr, len) != 0) {
			printf("reply from %s (ignored)\n", Sock_ntop(preply_addr, len));
			continue;
		}
		recvline[n] = 0;    //null terminate
		Fputs(recvline, stdout);
	}
}

void dg_cli_connect(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int n;
	char sendline[MAXLINE];
	char recvline[MAXLINE+1];

	Connect(sockfd, (SA*)pservaddr, servlen);

	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		Write(sockfd, sendline, strlen(sendline));

		n = Read(sockfd, recvline, MAXLINE);
		recvline[n] = 0;    //null terminate
		Fputs(recvline, stdout);
	}
}

#define NDG      2000    //datagrams to send
#define DGLEN    1400    //length of echo datagrams
void dg_cli_loop1(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int i;
	char sendline[DGLEN];

	for (i=0; i<NDG; i++) {
		Sendto(sockfd, sendline, DGLEN, 0, pservaddr, servlen);
	}
}

static int count;

static void recvfrom_int(int signo)
{
	printf("\nreceived %d datagrams\n", count);
	return;
}

void dg_echo_loop1(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	socklen_t len;
	char mesg[MAXLINE] = {0};

	Signal(SIGINT, recvfrom_int);

	while (1) {
		len = clilen;
		Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
		count++;
	}
}

void dg_echo_loop2(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	int n = (220 * 1024);
	socklen_t len;
	char mesg[MAXLINE] = {0};

	Signal(SIGINT, recvfrom_int);

	Setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));

	while (1) {
		len = clilen;
		Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
		count++;
	}
}

/***************************************************************************************
 * Description   : udpserv01
 ***************************************************************************************/
int udp_serv01()
{
	int sockfd;
	struct sockaddr_in servaddr = {0};
	struct sockaddr_in cliaddr = {0};

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(sockfd, (SA*)&servaddr, sizeof(servaddr));

	dg_ehco(sockfd, (SA*)&cliaddr, sizeof(cliaddr));
	return 0;
}

/***************************************************************************************
 * Description   : udpcli01
 ***************************************************************************************/
int udp_cli01(char *ip_addr)
{
	int sockfd;
	struct sockaddr_in servaddr = {0};

	if (ip_addr == NULL || ip_addr[0] == '\0') {
		err_quit("input ip_addr error");
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, ip_addr, &servaddr.sin_addr);

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	dg_cli(stdin, sockfd, (SA*)&servaddr, sizeof(servaddr));
	return 0;
}

/***************************************************************************************
 * Description   : udpcli09
 ***************************************************************************************/
int udp_cli09(char *ip_addr)
{
	int sockfd;
	socklen_t len;
	struct sockaddr_in cliaddr = {0};
	struct sockaddr_in servaddr = {0};

	if (ip_addr == NULL || ip_addr[0] == '\0') {
		err_quit("input ip_addr error");
	}

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, ip_addr, &servaddr.sin_addr);

	Connect(sockfd, (SA*)&servaddr, sizeof(servaddr));

	len = sizeof(cliaddr);
	Getsockname(sockfd, (SA*)&cliaddr, &len);
	printf("local address %s\n", Sock_ntop((SA*)&cliaddr, len));

	return 0;
}

/***************************************************************************************
 * Description   : udpservselect01
 ***************************************************************************************/
int udp_serv_select01()
{
	int listenfd, connfd, udpfd, nready, maxfdp1;
	char mesg[MAXLINE];
	pid_t childpid;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr = {0};
	struct sockaddr_in servaddr = {0};

	/* create listening TCP socket */
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	/* create UDP socket */
	udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(udpfd, (SA *) &servaddr, sizeof(servaddr));

	Signal(SIGCHLD, sig_chld_wait_pid);    /* must call waitpid() */

	FD_ZERO(&rset);
	maxfdp1 = max(listenfd, udpfd) + 1;
	while (1) {
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
		if ( (nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
			if (errno == EINTR) {
				continue;    /* back to while() */
			} else {
				err_sys("select error");
			}
		}

		if (FD_ISSET(listenfd, &rset)) {
			len = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &len);
	
			if ( (childpid = Fork()) == 0) {	/* child process */
				Close(listenfd);	/* close listening socket */
				str_echo(connfd);	/* process the request */
				exit(0);
			}
			Close(connfd);			/* parent closes connected socket */
		}

		if (FD_ISSET(udpfd, &rset)) {
			len = sizeof(cliaddr);
			n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *) &cliaddr, &len);
			Sendto(udpfd, mesg, n, 0, (SA *) &cliaddr, len);
		}
	}
}

#ifdef HAVE_NETINET_SCTP_H
/***************************************************************************************
 * Description   : sctp misc function
 ***************************************************************************************/
#define SCTP_MAXLINE    800

void sctpstr_cli(FILE *fp, int sock_fd, struct sockaddr *to, socklen_t tolen)
{
	struct sockaddr_in peeraddr;
	struct sctp_sndrcvinfo sri;
	socklen_t len;
	char sendline[MAXLINE], recvline[MAXLINE];
	int out_sz, rd_sz;
	int msg_flags;

	bzero(&sri, sizeof(sri));
	while (fgets(sendline, sizeof(sendline), fp) != NULL) {
		if (sendline[0] != '[') {
			printf("Error, line must be of the form '[streamnum]text'\n");
			continue;
		}

		sri.sinfo_stream = strtol(&sendline[1], NULL, 0);
		out_sz = strlen(sendline);
		Sctp_sendmsg(sock_fd, sendline, out_sz, to, tolen, 0, 0, sri.sinfo_stream, 0, 0);

		len = sizeof(peeraddr);
		rd_sz = Sctp_recvmsg(sock_fd, recvline, sizeof(recvline), (SA*)&peeraddr, &len, &sri, &msg_flags);
		printf("From str:%d (assoc:0x%x):", sri.sinfo_stream, sri.sinfo_ssn, (u_int)sri.sinfo_assoc_id);
		printf("%.*s", rd_sz, recvline);
	}
}

void sctpstr_cli_echoall(FILE *fp, int sock_fd, struct sockaddr *to, socklen_t tolen)
{
	struct sockaddr_in peeraddr;
	struct sctp_sndrcvinfo sri;
	char sendline[SCTP_MAXLINE], recvline[SCTP_MAXLINE];
	socklen_t len;
	int rd_sz,i,strsz;
	int msg_flags;

	bzero(sendline,sizeof(sendline));
	bzero(&sri,sizeof(sri));
	while (fgets(sendline, SCTP_MAXLINE - 9, fp) != NULL) {
		strsz = strlen(sendline);
		if(sendline[strsz-1] == '\n') {
			sendline[strsz-1] = '\0';
			strsz--;
		}
/* include modified_echo */
		for(i=0;i<SERV_MAX_SCTP_STRM;i++) {
			snprintf(sendline + strsz, sizeof(sendline) - strsz,
				".msg.%d 1", i);
			Sctp_sendmsg(sock_fd, sendline, sizeof(sendline), 
				     to, tolen, 
				     0, 0,
				     i,
				     0, 0);
			snprintf(sendline + strsz, sizeof(sendline) - strsz,
				".msg.%d 2", i);
			Sctp_sendmsg(sock_fd, sendline, sizeof(sendline), 
				     to, tolen, 
				     0, 0,
				     i,
				     0, 0);
		}
		for(i=0;i<SERV_MAX_SCTP_STRM*2;i++) {
			len = sizeof(peeraddr);
/* end modified_echo */
			rd_sz = Sctp_recvmsg(sock_fd, recvline, sizeof(recvline),
				     (SA *)&peeraddr, &len,
				     &sri,&msg_flags);
			printf("From str:%d seq:%d (assoc:0x%x):",
				sri.sinfo_stream,sri.sinfo_ssn,
				(u_int)sri.sinfo_assoc_id);
			printf("%.*s\n",rd_sz,recvline);
		}
	}
}

/***************************************************************************************
 * Description   : sctpserv01
 ***************************************************************************************/
int sctp_serv01(int increment)
{
	int sock_fd;
	int msg_flags;
	int stream_increment = 1;
	char readbuf[BUFFSIZE];
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	socklen_t len;
	size_t rd_sz;

	if (increment) {
		stream_increment = increment;
	}
	sock_fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.port = htons(SERV_PORT);

	Bind(sock_fd, (SA*)&servaddr, sizeof(servaddr));

	bzero(&evnts, sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	Setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts));

	Listen(sock_fd, LISTENQ);
	while (1) {
		len = sizeof(struct sockaddr_in);
		rd_sz = Sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf), (SA*)&cliaddr, &len, &sri, &msg_flags);

		if (stream_increment) {
			sri.sinfo_stream++;
			if (sri.sinfo_stream >= sctp_get_no_strms(sock_fd, (SA*)&cliaddr, len)) {
				sri.sinfo_stream = 0;
			}
		}
		Sctp_sendmsg(sock_fd, readbuf, rd_sz, (SA*)&cliaddr, len, sri.sinfo_ppid, sri.sinfo_flags, sri.sinfo_stream, 0, 0);
	}
}

/***************************************************************************************
 * Description   : sctpserv01
 ***************************************************************************************/
int sctp_client01(char *ip_addr, int echo_2_all)
{
	int sock_fd;
	int echo_to_all = 0;
	struct sockaddr_in servaddr;
	struct sctp_event_subscribe evnts;

	if (ip_addr == NULL || ip_addr[0] == '\0') {
		err_quit("Missing host argument");
	}
	if (echo_2_all) {
		printf("Echoing messages to all streams\n");
		echo_to_all = echo_2_all;
	}
	//set servaddr
	sock_fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, ip_addr, &servaddr.sin_addr);
	//set evnts
	bzero(&evnts, sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	Setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts));

	if (echo_to_all == 0) {
		sctpstr_cli(stdin, sock_fd, (SA*)&servaddr, sizeof(servaddr));
	} else {
		sctpstr_cli_echoall(stdin, sock_fd, (SA*)&servaddr, sizeof(servaddr));
	}

	Close(sock_fd);
	return 0;
}
#endif /*HAVE_NETINET_SCTP_H */

/***************************************************************************************
 * Description   : hostent - call gethostbyname
 ***************************************************************************************/
int hostent(char *host)
{
	char **pptr;
	char str[INET_ADDRSTRLEN];
	struct hostent *hptr;

	if (host && host[0] != '\0') {
		if ((hptr = gethostbyname(host)) == NULL) {
			err_msg("gethostbyname error for host: %s: %s", host, hstrerror(h_errno));
			return -1;
		}
		printf("official hostname: %s\n", hptr->h_name);

		for (pptr=hptr->h_aliases; *pptr != NULL; pptr++) {
			printf("\t");
		}

		switch (hptr->h_addrtype) {
			case AF_INET:
				pptr = hptr->h_addr_list;
				for (; *pptr != NULL; pptr++) {
					printf("\taddress: %s\n", Inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
				}
				break;
			default:
				err_ret("unknown address type");
				break;
		}
	}
	return 0;
}

/***************************************************************************************
 * Description   : daytimetcpcli1 - call gethostbyname and getservbyname
 ***************************************************************************************/
int daytimetcpcli1(char *hostname, char *servname)
{
	int sockfd, n;
	char recvline[MAXLINE + 1] = {0};
	struct sockaddr_in servaddr;
	struct in_addr **pptr;
	struct in_addr *inetaddrp[2];
	struct in_addr inetaddr;
	struct hostent *hp;
	struct servent *sp;


	if ( (hp = gethostbyname(hostname)) == NULL) {
		if (inet_aton(hostname, &inetaddr) == 0) {
			err_quit("hostname error for %s: %s", hostname, hstrerror(h_errno));
		} else {
			inetaddrp[0] = &inetaddr;
			inetaddrp[1] = NULL;
			pptr = inetaddrp;
		}
	} else {
		pptr = (struct in_addr **)hp->h_addr_list;
	}

	if ( (sp = getservbyname(servname, "tcp")) == NULL) {
		err_quit("getservbyname error for %s", servname);
	}

	for ( ; *pptr != NULL; pptr++) {
		sockfd = Socket(AF_INET, SOCK_STREAM, 0);

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = sp->s_port;
		memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
		printf("trying %s\n", Sock_ntop((SA*)&servaddr, sizeof(servaddr)));

		if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) == 0) {
			break;
		}
		err_ret("connect error");
		close(sockfd);
	}

	if (*pptr == NULL) {
		err_quit("unable to connect");
	}

	while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;
		Fputs(recvline, stdout);
	}
	return 0;
}

/***************************************************************************************
 * Description   : daytimetcpcli - call tcp_connect
 ***************************************************************************************/
int daytimetcpcli(char *hostname, char *servname)
{
	int				sockfd, n;
	char			recvline[MAXLINE + 1];
	socklen_t		len;
	struct sockaddr_storage	ss;

	sockfd = Tcp_connect(hostname, servname);

	len = sizeof(ss);
	Getpeername(sockfd, (SA *)&ss, &len);
	printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));

	while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
	return 0;
}

/***************************************************************************************
 * Description   : daytimetcpsrv1 - call tcp_listen
 ***************************************************************************************/
int daytimetcpsrv1(char *servname)
{
	int				listenfd, connfd;
	socklen_t		len;
	char			buff[MAXLINE];
	time_t			ticks;
	struct sockaddr_storage	cliaddr;

	listenfd = Tcp_listen(NULL, servname, NULL);

	for ( ; ; ) {
		len = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *)&cliaddr, &len);
		printf("connection from %s\n", Sock_ntop((SA *)&cliaddr, len));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}

/***************************************************************************************
 * Description   : daytimetcpsrv2 - call tcp_listen
 ***************************************************************************************/
int daytimetcpsrv2(char *hostname, char *servname)
{
	int				listenfd, connfd;
	socklen_t		len, addrlen;
	char			buff[MAXLINE];
	time_t			ticks;
	struct sockaddr_storage	cliaddr;

	listenfd = Tcp_listen(hostname, servname, &addrlen);

	for ( ; ; ) {
		len = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *)&cliaddr, &len);
		printf("connection from %s\n", Sock_ntop((SA *)&cliaddr, len));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}

/***************************************************************************************
 * Description   : daytimeudpcli1
 ***************************************************************************************/
int daytimeudpcli1(char *hostname, char *servname)
{
	int				sockfd, n;
	char			recvline[MAXLINE + 1];
	socklen_t		salen;
	struct sockaddr *sa;

	sockfd = Udp_client(hostname, servname, &sa, &salen);

	printf("sending to %s\n", Sock_ntop_host(sa, salen));
	Sendto(sockfd, " ", 1, 0, sa, salen);    //send 1-byte data

	n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
	recvline[n] = '\0';	/* null terminate */
	Fputs(recvline, stdout);

	return 0;
}

/***************************************************************************************
 * Description   : daytimeudpsrv2
 ***************************************************************************************/
int daytimeudpsrv2(char *hostname, char *servname)
{
	int sockfd;
	ssize_t n;
	char buff[MAXLINE];
	time_t ticks;
	socklen_t len;
	struct sockaddr_storage cliaddr;

	sockfd = Udp_server(servname, hostname, NULL);

	for ( ; ; ) {
		len = sizeof(cliaddr);
		n = Recvfrom(sockfd, buff, MAXLINE, 0, (SA*)&cliaddr, &len);
		printf("%ld datagrams from %s\n", n, Sock_ntop((SA *)&cliaddr, len));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		Sendto(sockfd, buff, strlen(buff), 0, (SA*)&cliaddr, len);
	}
}


