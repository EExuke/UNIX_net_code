/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : lib.c
 *     FIRST CREATION DATE    : 2021/11/30
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/11/30
 *     FILE DESCRIPTION       : shared lib code
** ************************************************************************** */

#include "unp.h"
#include "lib.h"

#include	<sys/mman.h>
#ifdef	HAVE_SOCKADDR_DL_STRUCT
#include	<net/if_dl.h>
#endif


/***************************************************************************************
 * Functions: wrapsock.c
 * Socket wrapper functions.
 * These could all go into separate files, so only the ones needed cause
 * the corresponding function to be added to the executable.  If sockets
 * are a library (SVR4) this might make a difference (?), but if sockets
 * are in the kernel (BSD) it doesn't matter.
 *
 * These wrapper functions also use the same prototypes as POSIX.1g,
 * which might differ from many implementations (i.e., POSIX.1g specifies
 * the fourth argument to getsockopt() as "void *", not "char *").
 *
 * If your system's headers are not correct [i.e., the Solaris 2.5
 * <sys/socket.h> omits the "const" from the second argument to both
 * bind() and connect()], you'll get warnings of the form:
 *warning: passing arg 2 of `bind' discards `const' from pointer target type
 *warning: passing arg 2 of `connect' discards `const' from pointer target type
 ***************************************************************************************/
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;

again:
	if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED) {
#else
		if (errno == ECONNABORTED) {
#endif
			goto again;
		} else {
			err_sys("accept error");
		}
	}
	return(n);
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0) {
		err_sys("bind error");
	}
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (connect(fd, sa, salen) < 0) {
		err_sys("connect error");
	}
}

void Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	if (getpeername(fd, sa, salenptr) < 0) {
		err_sys("getpeername error");
	}
}

void Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	if (getsockname(fd, sa, salenptr) < 0) {
		err_sys("getsockname error");
	}
}

void Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
	if (getsockopt(fd, level, optname, optval, optlenptr) < 0) {
		err_sys("getsockopt error");
	}
}

#ifdef    HAVE_INET6_RTH_INIT
int Inet6_rth_space(int type, int segments)
{
	int ret;

	ret = (int)inet6_rth_space(type, segments);
	if (ret < 0) {
		err_quit("inet6_rth_space error");
	}

	return ret;
}

void *Inet6_rth_init(void *rthbuf, socklen_t rthlen, int type, int segments)
{
	void *ret;

	ret = inet6_rth_init(rthbuf, rthlen, type, segments);
	if (ret == NULL) {
		err_quit("inet6_rth_init error");
	}

	return ret;
}

void Inet6_rth_add(void *rthbuf, const struct in6_addr *addr)
{
	if (inet6_rth_add(rthbuf, addr) < 0) {
		err_quit("inet6_rth_add error");
	}
}

void Inet6_rth_reverse(const void *in, void *out)
{
	if (inet6_rth_reverse(in, out) < 0) {
		err_quit("inet6_rth_reverse error");
	}
}

int Inet6_rth_segments(const void *rthbuf)
{
	int ret;

	ret = inet6_rth_segments(rthbuf);
	if (ret < 0) {
		err_quit("inet6_rth_segments error");
	}

	return ret;
}

struct in6_addr *Inet6_rth_getaddr(const void *rthbuf, int idx)
{
	struct in6_addr *ret;

	ret = inet6_rth_getaddr(rthbuf, idx);
	if (ret == NULL) {
		err_quit("inet6_rth_getaddr error");
	}

	return ret;
}
#endif

#ifdef HAVE_KQUEUE
int Kqueue(void)
{
	int ret;

	if ((ret = kqueue()) < 0) {
		err_sys("kqueue error");
	}
	return ret;
}

int Kevent(int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout)
{
	int ret;

	if ((ret = kevent(kq, changelist, nchanges, eventlist, nevents, timeout)) < 0) {
		err_sys("kevent error");
	}
	return ret;
}
#endif

void Listen(int fd, int backlog)
{
	char *ptr;

	/*4can override 2nd argument with environment variable */
	if ( (ptr = getenv("LISTENQ")) != NULL) {
		backlog = atoi(ptr);
	}

	if (listen(fd, backlog) < 0) {
		err_sys("listen error");
	}
}

#ifdef	HAVE_POLL
int Poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
	int n;

	if ( (n = poll(fdarray, nfds, timeout)) < 0) {
		err_sys("poll error");
	}

	return(n);
}
#endif

ssize_t Recv(int fd, void *ptr, size_t nbytes, int flags)
{
	ssize_t n;

	if ( (n = recv(fd, ptr, nbytes, flags)) < 0) {
		err_sys("recv error");
	}
	return(n);
}

ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *sa, socklen_t *salenptr)
{
	ssize_t n;

	if ( (n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0) {
		err_sys("recvfrom error");
	}
	return(n);
}

ssize_t Recvmsg(int fd, struct msghdr *msg, int flags)
{
	ssize_t n;

	if ( (n = recvmsg(fd, msg, flags)) < 0) {
		err_sys("recvmsg error");
	}
	return(n);
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int n;

	if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0) {
		err_sys("select error");
	}
	return n;		/* can return 0 on timeout */
}

void Send(int fd, const void *ptr, size_t nbytes, int flags)
{
	if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes)
		err_sys("send error");
}

void Sendto(int fd, const void *ptr, size_t nbytes, int flags, const struct sockaddr *sa, socklen_t salen)
{
	if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t)nbytes) {
		err_sys("sendto error");
	}
}

void Sendmsg(int fd, const struct msghdr *msg, int flags)
{
	unsigned int i;
	ssize_t nbytes;

	nbytes = 0;	/* must first figure out what return value should be */
	for (i = 0; i < msg->msg_iovlen; i++) {
		nbytes += msg->msg_iov[i].iov_len;
	}

	if (sendmsg(fd, msg, flags) != nbytes) {
		err_sys("sendmsg error");
	}
}

void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (setsockopt(fd, level, optname, optval, optlen) < 0) {
		err_sys("setsockopt error");
	}
}

void Shutdown(int fd, int how)
{
	if (shutdown(fd, how) < 0) {
		err_sys("shutdown error");
	}
}

int Sockatmark(int fd)
{
	int n;

	if ((n = sockatmark(fd)) < 0) {
		err_sys("sockatmark error");
	}
	return n;
}

int Socket(int family, int type, int protocol)
{
	int n;

	if ((n = socket(family, type, protocol)) < 0) {
		err_sys("socket error");
	}
	return n;
}

void Socketpair(int family, int type, int protocol, int *fd)
{
	int n;

	if ((n = socketpair(family, type, protocol, fd)) < 0) {
		err_sys("socketpair error");
	}
}

/***************************************************************************************
 * Functions: wrappthread.c
 * pthreads wrapper functions.
 ***************************************************************************************/
void Pthread_create(pthread_t *tid, const pthread_attr_t *attr, void * (*func)(void *), void *arg)
{
	int		n;

	if ( (n = pthread_create(tid, attr, func, arg)) == 0)
		return;
	errno = n;
	err_sys("pthread_create error");
}

void Pthread_join(pthread_t tid, void **status)
{
	int		n;

	if ( (n = pthread_join(tid, status)) == 0)
		return;
	errno = n;
	err_sys("pthread_join error");
}

void Pthread_detach(pthread_t tid)
{
	int		n;

	if ( (n = pthread_detach(tid)) == 0)
		return;
	errno = n;
	err_sys("pthread_detach error");
}

void Pthread_kill(pthread_t tid, int signo)
{
	int		n;

	if ( (n = pthread_kill(tid, signo)) == 0)
		return;
	errno = n;
	err_sys("pthread_kill error");
}

void Pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
	int		n;

	if ( (n = pthread_mutexattr_init(attr)) == 0)
		return;
	errno = n;
	err_sys("pthread_mutexattr_init error");
}

#ifdef	_POSIX_THREAD_PROCESS_SHARED
void Pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int flag)
{
	int		n;

	if ( (n = pthread_mutexattr_setpshared(attr, flag)) == 0)
		return;
	errno = n;
	err_sys("pthread_mutexattr_setpshared error");
}
#endif

void Pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr)
{
	int		n;

	if ( (n = pthread_mutex_init(mptr, attr)) == 0)
		return;
	errno = n;
	err_sys("pthread_mutex_init error");
}

/* include Pthread_mutex_lock */
void Pthread_mutex_lock(pthread_mutex_t *mptr)
{
	int		n;

	if ( (n = pthread_mutex_lock(mptr)) == 0)
		return;
	errno = n;
	err_sys("pthread_mutex_lock error");
}
/* end Pthread_mutex_lock */

void Pthread_mutex_unlock(pthread_mutex_t *mptr)
{
	int		n;

	if ( (n = pthread_mutex_unlock(mptr)) == 0)
		return;
	errno = n;
	err_sys("pthread_mutex_unlock error");
}

void Pthread_cond_broadcast(pthread_cond_t *cptr)
{
	int		n;

	if ( (n = pthread_cond_broadcast(cptr)) == 0)
		return;
	errno = n;
	err_sys("pthread_cond_broadcast error");
}

void Pthread_cond_signal(pthread_cond_t *cptr)
{
	int		n;

	if ( (n = pthread_cond_signal(cptr)) == 0)
		return;
	errno = n;
	err_sys("pthread_cond_signal error");
}

void Pthread_cond_wait(pthread_cond_t *cptr, pthread_mutex_t *mptr)
{
	int		n;

	if ( (n = pthread_cond_wait(cptr, mptr)) == 0)
		return;
	errno = n;
	err_sys("pthread_cond_wait error");
}

void Pthread_cond_timedwait(pthread_cond_t *cptr, pthread_mutex_t *mptr,
					   const struct timespec *tsptr)
{
	int		n;

	if ( (n = pthread_cond_timedwait(cptr, mptr, tsptr)) == 0)
		return;
	errno = n;
	err_sys("pthread_cond_timedwait error");
}

void Pthread_once(pthread_once_t *ptr, void (*func)(void))
{
	int		n;

	if ( (n = pthread_once(ptr, func)) == 0)
		return;
	errno = n;
	err_sys("pthread_once error");
}

void Pthread_key_create(pthread_key_t *key, void (*func)(void *))
{
	int		n;

	if ( (n = pthread_key_create(key, func)) == 0)
		return;
	errno = n;
	err_sys("pthread_key_create error");
}

void Pthread_setspecific(pthread_key_t key, const void *value)
{
	int		n;

	if ( (n = pthread_setspecific(key, value)) == 0)
		return;
	errno = n;
	err_sys("pthread_setspecific error");
}

/***************************************************************************************
 * Functions: wrapunix.c
 * These could all go into separate files, so only the ones needed cause
 * the corresponding function to be added to the executable.  If sockets
 * are a library (SVR4) this might make a difference (?), but if sockets
 * are in the kernel (BSD) it doesn't matter.
 *
 * These wrapper functions also use the same prototypes as POSIX.1g,
 * which might differ from many implementations (i.e., POSIX.1g specifies
 * the fourth argument to getsockopt() as "void *", not "char *").
 *
 * If your system's headers are not correct [i.e., the Solaris 2.5
 * <sys/socket.h> omits the "const" from the second argument to both
 * bind() and connect()], you'll get warnings of the form:
 *warning: passing arg 2 of `bind' discards `const' from pointer target type
 *warning: passing arg 2 of `connect' discards `const' from pointer target type
 ***************************************************************************************/
void *Calloc(size_t n, size_t size)
{
	void	*ptr;

	if ( (ptr = calloc(n, size)) == NULL)
		err_sys("calloc error");
	return(ptr);
}

void Close(int fd)
{
	if (close(fd) == -1)
		err_sys("close error");
}

void Dup2(int fd1, int fd2)
{
	if (dup2(fd1, fd2) == -1)
		err_sys("dup2 error");
}

int Fcntl(int fd, int cmd, int arg)
{
	int	n;

	if ( (n = fcntl(fd, cmd, arg)) == -1)
		err_sys("fcntl error");
	return(n);
}

void Gettimeofday(struct timeval *tv, void *foo)
{
	if (gettimeofday(tv, foo) == -1)
		err_sys("gettimeofday error");
	return;
}

int Ioctl(int fd, int request, void *arg)
{
	int n;

	if ( (n = ioctl(fd, request, arg)) == -1)
		err_sys("ioctl error");
	return(n);	/* streamio of I_LIST returns value */
}

pid_t Fork(void)
{
	pid_t pid;

	if ((pid = fork()) == -1)
		err_sys("fork error");
	return(pid);
}

void *Malloc(size_t size)
{
	void	*ptr;

	if ( (ptr = malloc(size)) == NULL)
		err_sys("malloc error");
	return(ptr);
}

int Mkstemp(char *template)
{
	int i;

#ifdef HAVE_MKSTEMP
	if ((i = mkstemp(template)) < 0)
		err_quit("mkstemp error");
#else
	if (mktemp(template) == NULL || template[0] == 0)
		err_quit("mktemp error");
	i = Open(template, O_CREAT | O_WRONLY, FILE_MODE);
#endif

	return i;
}

void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	void	*ptr;

	if ( (ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1))
		err_sys("mmap error");
	return(ptr);
}

int Open(const char *pathname, int oflag, mode_t mode)
{
	int		fd;

	if ( (fd = open(pathname, oflag, mode)) == -1)
		err_sys("open error for %s", pathname);
	return(fd);
}

void Pipe(int *fds)
{
	if (pipe(fds) < 0)
		err_sys("pipe error");
}

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t		n;

	if ( (n = read(fd, ptr, nbytes)) == -1)
		err_sys("read error");
	return(n);
}

void Sigaddset(sigset_t *set, int signo)
{
	if (sigaddset(set, signo) == -1)
		err_sys("sigaddset error");
}

void Sigdelset(sigset_t *set, int signo)
{
	if (sigdelset(set, signo) == -1)
		err_sys("sigdelset error");
}

void Sigemptyset(sigset_t *set)
{
	if (sigemptyset(set) == -1)
		err_sys("sigemptyset error");
}

void Sigfillset(sigset_t *set)
{
	if (sigfillset(set) == -1)
		err_sys("sigfillset error");
}

int Sigismember(const sigset_t *set, int signo)
{
	int		n;

	if ( (n = sigismember(set, signo)) == -1)
		err_sys("sigismember error");
	return(n);
}

void Sigpending(sigset_t *set)
{
	if (sigpending(set) == -1)
		err_sys("sigpending error");
}

void Sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	if (sigprocmask(how, set, oset) == -1)
		err_sys("sigprocmask error");
}

char *Strdup(const char *str)
{
	char	*ptr;

	if ( (ptr = strdup(str)) == NULL)
		err_sys("strdup error");
	return(ptr);
}

long Sysconf(int name)
{
	long	val;

	errno = 0;		/* in case sysconf() does not change this */
	if ( (val = sysconf(name)) == -1)
		err_sys("sysconf error");
	return(val);
}

#ifdef	HAVE_SYS_SYSCTL_H
void Sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp,
	   void *newp, size_t newlen)
{
	if (sysctl(name, namelen, oldp, oldlenp, newp, newlen) == -1)
		err_sys("sysctl error");
}
#endif

void Unlink(const char *pathname)
{
	if (unlink(pathname) == -1)
		err_sys("unlink error for %s", pathname);
}

pid_t Wait(int *iptr)
{
	pid_t	pid;

	if ( (pid = wait(iptr)) == -1)
		err_sys("wait error");
	return(pid);
}

pid_t Waitpid(pid_t pid, int *iptr, int options)
{
	pid_t	retpid;

	if ( (retpid = waitpid(pid, iptr, options)) == -1)
		err_sys("waitpid error");
	return(retpid);
}

void Write(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
		err_sys("write error");
}

/***************************************************************************************
 * Functions: sock_ntop.c
***************************************************************************************/
char *sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char portstr[8];
	static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		if (ntohs(sin->sin_port) != 0) {
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return(str);
	}

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		str[0] = '[';
		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
			return(NULL);
		if (ntohs(sin6->sin6_port) != 0) {
			snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return(str);
		}
		return (str + 1);
	}
#endif

#ifdef	AF_UNIX
	case AF_UNIX: {
		struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
			strcpy(str, "(no pathname bound)");
		else
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		return(str);
	}
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK: {
		struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

		if (sdl->sdl_nlen > 0)
			snprintf(str, sizeof(str), "%*s (index %d)",
					 sdl->sdl_nlen, &sdl->sdl_data[0], sdl->sdl_index);
		else
			snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
		return(str);
	}
#endif
	default:
		snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);
	}
    return (NULL);
}

char *Sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char *ptr;

	if ((ptr = sock_ntop(sa, salen)) == NULL) {
		err_sys("sock_ntop error");	/* inet_ntop() sets errno */
	}
	return(ptr);
}

/***************************************************************************************
 * Functions: sock_bind_wild.c
***************************************************************************************/
int sock_bind_wild(int sockfd, int family)
{
	socklen_t	len;

	switch (family) {
	case AF_INET: {
		struct sockaddr_in	sin;

		bzero(&sin, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		sin.sin_port = htons(0);	/* bind ephemeral port */

		if (bind(sockfd, (SA *) &sin, sizeof(sin)) < 0)
			return(-1);
		len = sizeof(sin);
		if (getsockname(sockfd, (SA *) &sin, &len) < 0)
			return(-1);
		return(sin.sin_port);
	}

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	sin6;

		bzero(&sin6, sizeof(sin6));
		sin6.sin6_family = AF_INET6;
		sin6.sin6_addr = in6addr_any;
		sin6.sin6_port = htons(0);	/* bind ephemeral port */

		if (bind(sockfd, (SA *) &sin6, sizeof(sin6)) < 0)
			return(-1);
		len = sizeof(sin6);
		if (getsockname(sockfd, (SA *) &sin6, &len) < 0)
			return(-1);
		return(sin6.sin6_port);
	}
#endif
	}
	return(-1);
}

int Sock_bind_wild(int sockfd, int family)
{
	int		port;

	if ( (port = sock_bind_wild(sockfd, family)) < 0)
		err_sys("sock_bind_wild error");

	return(port);
}

/***************************************************************************************
 * Functions: sock_cmp_addr.c
***************************************************************************************/
int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen)
{
	if (sa1->sa_family != sa2->sa_family)
		return(-1);

	switch (sa1->sa_family) {
	case AF_INET: {
		return(memcmp( &((struct sockaddr_in *) sa1)->sin_addr,
					   &((struct sockaddr_in *) sa2)->sin_addr,
					   sizeof(struct in_addr)));
	}

#ifdef	IPV6
	case AF_INET6: {
		return(memcmp( &((struct sockaddr_in6 *) sa1)->sin6_addr,
					   &((struct sockaddr_in6 *) sa2)->sin6_addr,
					   sizeof(struct in6_addr)));
	}
#endif

#ifdef	AF_UNIX
	case AF_UNIX: {
		return(strcmp( ((struct sockaddr_un *) sa1)->sun_path,
					   ((struct sockaddr_un *) sa2)->sun_path));
	}
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK: {
		return(-1);		/* no idea what to compare here ? */
	}
#endif
	}
    return (-1);
}

/***************************************************************************************
 * Functions: sock_cmp_port.c
***************************************************************************************/
int sock_cmp_port(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen)
{
	if (sa1->sa_family != sa2->sa_family)
		return(-1);

	switch (sa1->sa_family) {
	case AF_INET: {
		return( ((struct sockaddr_in *) sa1)->sin_port ==
				((struct sockaddr_in *) sa2)->sin_port);
	}

#ifdef	IPV6
	case AF_INET6: {
		return( ((struct sockaddr_in6 *) sa1)->sin6_port ==
				((struct sockaddr_in6 *) sa2)->sin6_port);
	}
#endif

	}
    return (-1);
}

/***************************************************************************************
 * Functions: sock_get_port.c
***************************************************************************************/
int sock_get_port(const struct sockaddr *sa, socklen_t salen)
{
	switch (sa->sa_family) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *) sa;
			return(sin->sin_port);
		}
#ifdef	IPV6
		case AF_INET6: {
			struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;
			return(sin6->sin6_port);
		}
#endif
	}

	return(-1); /* ??? */
}

/***************************************************************************************
 * Functions: sock_ntop_host.c
***************************************************************************************/
char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		return(str);
	}

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
			return(NULL);
		return(str);
	}
#endif

#ifdef	AF_UNIX
	case AF_UNIX: {
		struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
			strcpy(str, "(no pathname bound)");
		else
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		return(str);
	}
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK: {
		struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

		if (sdl->sdl_nlen > 0)
			snprintf(str, sizeof(str), "%*s",
					 sdl->sdl_nlen, &sdl->sdl_data[0]);
		else
			snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
		return(str);
	}
#endif
	default:
		snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);
	}
    return (NULL);
}

char *Sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	char	*ptr;

	if ( (ptr = sock_ntop_host(sa, salen)) == NULL)
		err_sys("sock_ntop_host error");	/* inet_ntop() sets errno */
	return(ptr);
}

/***************************************************************************************
 * Functions: sock_set_addr.c
***************************************************************************************/
void sock_set_addr(struct sockaddr *sa, socklen_t salen, const void *addr)
{
	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		memcpy(&sin->sin_addr, addr, sizeof(struct in_addr));
		return;
	}

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		memcpy(&sin6->sin6_addr, addr, sizeof(struct in6_addr));
		return;
	}
#endif
	}

    return;
}

/***************************************************************************************
 * Functions: sock_set_port.c
***************************************************************************************/
void sock_set_port(struct sockaddr *sa, socklen_t salen, int port)
{
	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		sin->sin_port = port;
		return;
	}

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		sin6->sin6_port = port;
		return;
	}
#endif
	}

	return;
}

/***************************************************************************************
 * Functions: sock_set_wild.c
***************************************************************************************/
void sock_set_wild(struct sockaddr *sa, socklen_t salen)
{
	const void	*wildptr;

	switch (sa->sa_family) {
	case AF_INET: {
		static struct in_addr	in4addr_any;

		in4addr_any.s_addr = htonl(INADDR_ANY);
		wildptr = &in4addr_any;
		break;
	}

#ifdef	IPV6
	case AF_INET6: {
		wildptr = &in6addr_any;
		break;
	}
#endif

	default:
		return;
	}
	sock_set_addr(sa, salen, wildptr);
	return;
}

/***************************************************************************************
 * Functions: readn.c
 * Read "n" bytes from a descriptor.
***************************************************************************************/
ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}

ssize_t Readn(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;

	if ( (n = readn(fd, ptr, nbytes)) < 0)
		err_sys("readn error");
	return(n);
}

/***************************************************************************************
 * Functions: writen.c
 * Write "n" bytes to a descriptor.
***************************************************************************************/
ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}

void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes) {
		err_sys("writen error");
	}
}

/***************************************************************************************
 * Functions: readline.c
***************************************************************************************/
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

ssize_t my_read(int fd, char *ptr)
{
	if (read_cnt <= 0) {
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

ssize_t readlinebuf(void **vptrptr)
{
	if (read_cnt)
		*vptrptr = read_ptr;
	return(read_cnt);
}

ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
		err_sys("readline error");
	return(n);
}

/***************************************************************************************
 * Functions: wraplib.c
 * Wrapper functions for our own library functions.
 * Most are included in the source file for the function itself.
***************************************************************************************/
const char *Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const char	*ptr;

	if (strptr == NULL)		/* check for old code */
		err_quit("NULL 3rd argument to inet_ntop");
	if ( (ptr = inet_ntop(family, addrptr, strptr, len)) == NULL)
		err_sys("inet_ntop error");		/* sets errno */
	return(ptr);
}

void Inet_pton(int family, const char *strptr, void *addrptr)
{
	int n;

	if ( (n = inet_pton(family, strptr, addrptr)) < 0)
		err_sys("inet_pton error for %s", strptr);	/* errno set */
	else if (n == 0)
		err_quit("inet_pton error for %s", strptr);	/* errno not set */
}

/***************************************************************************************
 * Functions: str_echo.c
***************************************************************************************/
void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];

again:
	while ( (n = read(sockfd, buf, MAXLINE)) > 0) {
		Writen(sockfd, buf, n);
	}

	if (n < 0 && errno == EINTR) {
		goto again;
	} else if (n < 0) {
		err_sys("str_echo: read error");
	}
}

/***************************************************************************************
 * Functions: str_cli.c
***************************************************************************************/
void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		Writen(sockfd, sendline, strlen(sendline));

		if (Readline(sockfd, recvline, MAXLINE) == 0) {
			err_quit("str_cli: server terminated prematurely");
		}

		Fputs(recvline, stdout);
	}
}

/***************************************************************************************
 * Functions: signal.c
***************************************************************************************/
Sigfunc *signal(int signo, Sigfunc *func)
{
	struct sigaction	act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;	/* SunOS 4.x */
#endif
	} else {
#ifdef	SA_RESTART
		act.sa_flags |= SA_RESTART;		/* SVR4, 44BSD */
#endif
	}
	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
}

Sigfunc *Signal(int signo, Sigfunc *func)	/* for our signal() function */
{
	Sigfunc	*sigfunc;

	if ( (sigfunc = signal(signo, func)) == SIG_ERR)
		err_sys("signal error");
	return(sigfunc);
}

/***************************************************************************************
 * Functions: wrapstdio.c
 * Standard I/O wrapper functions.
***************************************************************************************/
void Fclose(FILE *fp)
{
	if (fclose(fp) != 0)
		err_sys("fclose error");
}

FILE * Fdopen(int fd, const char *type)
{
	FILE *fp;

	if ((fp = fdopen(fd, type)) == NULL)
		err_sys("fdopen error");

	return(fp);
}

char * Fgets(char *ptr, int n, FILE *stream)
{
	char *rptr;

	if ((rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
		err_sys("fgets error");

	return (rptr);
}

FILE * Fopen(const char *filename, const char *mode)
{
	FILE *fp;

	if ((fp = fopen(filename, mode)) == NULL)
		err_sys("fopen error");

	return(fp);
}

void Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF)
		err_sys("fputs error");
}

struct addrinfo *host_serv(const char *host, const char *serv, int family, int socktype)
{
	int				n;
	struct addrinfo	hints, *res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
	hints.ai_family = family;		/* AF_UNSPEC, AF_INET, AF_INET6, etc. */
	hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		return(NULL);

	return(res);	/* return pointer to first on linked list */
}
/* end host_serv */

/*
 * There is no easy way to pass back the integer return code from
 * getaddrinfo() in the function above, short of adding another argument
 * that is a pointer, so the easiest way to provide the wrapper function
 * is just to duplicate the simple function as we do here.
 */

struct addrinfo *Host_serv(const char *host, const char *serv, int family, int socktype)
{
	int				n;
	struct addrinfo	hints, *res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
	hints.ai_family = family;		/* 0, AF_INET, AF_INET6, etc. */
	hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("host_serv error for %s, %s: %s",
				 (host == NULL) ? "(no hostname)" : host,
				 (serv == NULL) ? "(no service name)" : serv,
				 gai_strerror(n));

	return(res);	/* return pointer to first on linked list */
}

int tcp_connect(const char *host, const char *serv)
{
	int				sockfd, n;
	struct addrinfo	hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("tcp_connect error for %s, %s: %s",
				 host, serv, gai_strerror(n));
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue;	/* ignore this one */

		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;		/* success */

		Close(sockfd);	/* ignore this one */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)	/* errno set from final connect() */
		err_sys("tcp_connect error for %s, %s", host, serv);

	freeaddrinfo(ressave);

	return(sockfd);
}

int Tcp_connect(const char *host, const char *serv)
{
	return(tcp_connect(host, serv));
}

int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	int				listenfd, n;
	const int		on = 1;
	struct addrinfo	hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("tcp_listen error for %s, %s: %s",
				 host, serv, gai_strerror(n));
	ressave = res;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue;		/* error, try next one */

		Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;			/* success */

		Close(listenfd);	/* bind error, close and try next one */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)	/* errno from final socket() or bind() */
		err_sys("tcp_listen error for %s, %s", host, serv);

	Listen(listenfd, LISTENQ);

	if (addrlenp)
		*addrlenp = res->ai_addrlen;	/* return size of protocol address */

	freeaddrinfo(ressave);

	return(listenfd);
}
/* end tcp_listen */

/*
 * We place the wrapper function here, not in wraplib.c, because some
 * XTI programs need to include wraplib.c, and it also defines
 * a Tcp_listen() function.
 */

int Tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	return(tcp_listen(host, serv, addrlenp));
}

int udp_client(const char *host, const char *serv, SA **saptr, socklen_t *lenp)
{
	int				sockfd, n;
	struct addrinfo	hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("udp_client error for %s, %s: %s",
				 host, serv, gai_strerror(n));
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd >= 0)
			break;		/* success */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)	/* errno set from final socket() */
		err_sys("udp_client error for %s, %s", host, serv);

	*saptr = Malloc(res->ai_addrlen);
	memcpy(*saptr, res->ai_addr, res->ai_addrlen);
	*lenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return(sockfd);
}

int Udp_client(const char *host, const char *serv, SA **saptr, socklen_t *lenptr)
{
	return(udp_client(host, serv, saptr, lenptr));
}

int udp_connect(const char *host, const char *serv)
{
	int				sockfd, n;
	struct addrinfo	hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("udp_connect error for %s, %s: %s",
				 host, serv, gai_strerror(n));
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue;	/* ignore this one */

		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;		/* success */

		Close(sockfd);	/* ignore this one */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)	/* errno set from final connect() */
		err_sys("udp_connect error for %s, %s", host, serv);

	freeaddrinfo(ressave);

	return(sockfd);
}

int Udp_connect(const char *host, const char *serv)
{
	int		n;

	if ( (n = udp_connect(host, serv)) < 0) {
		err_quit("udp_connect error for %s, %s: %s",
					 host, serv, gai_strerror(-n));
	}
	return(n);
}

int udp_server(const char *host, const char *serv, socklen_t *addrlenp)
{
	int				sockfd, n;
	struct addrinfo	hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("udp_server error for %s, %s: %s",
				 host, serv, gai_strerror(n));
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue;		/* error - try next one */

		if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;			/* success */

		Close(sockfd);		/* bind error - close and try next one */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)	/* errno from final socket() or bind() */
		err_sys("udp_server error for %s, %s", host, serv);

	if (addrlenp)
		*addrlenp = res->ai_addrlen;	/* return size of protocol address */

	freeaddrinfo(ressave);

	return(sockfd);
}

int Udp_server(const char *host, const char *serv, socklen_t *addrlenp)
{
	return(udp_server(host, serv, addrlenp));
}

