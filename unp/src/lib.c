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
void *
Calloc(size_t n, size_t size)
{
	void	*ptr;

	if ( (ptr = calloc(n, size)) == NULL)
		err_sys("calloc error");
	return(ptr);
}

void
Close(int fd)
{
	if (close(fd) == -1)
		err_sys("close error");
}

void
Dup2(int fd1, int fd2)
{
	if (dup2(fd1, fd2) == -1)
		err_sys("dup2 error");
}

int
Fcntl(int fd, int cmd, int arg)
{
	int	n;

	if ( (n = fcntl(fd, cmd, arg)) == -1)
		err_sys("fcntl error");
	return(n);
}

void
Gettimeofday(struct timeval *tv, void *foo)
{
	if (gettimeofday(tv, foo) == -1)
		err_sys("gettimeofday error");
	return;
}

int
Ioctl(int fd, int request, void *arg)
{
	int		n;

	if ( (n = ioctl(fd, request, arg)) == -1)
		err_sys("ioctl error");
	return(n);	/* streamio of I_LIST returns value */
}

pid_t
Fork(void)
{
	pid_t	pid;

	if ( (pid = fork()) == -1)
		err_sys("fork error");
	return(pid);
}

void *
Malloc(size_t size)
{
	void	*ptr;

	if ( (ptr = malloc(size)) == NULL)
		err_sys("malloc error");
	return(ptr);
}

int
Mkstemp(char *template)
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

#include	<sys/mman.h>

void *
Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	void	*ptr;

	if ( (ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1))
		err_sys("mmap error");
	return(ptr);
}

int
Open(const char *pathname, int oflag, mode_t mode)
{
	int		fd;

	if ( (fd = open(pathname, oflag, mode)) == -1)
		err_sys("open error for %s", pathname);
	return(fd);
}

void
Pipe(int *fds)
{
	if (pipe(fds) < 0)
		err_sys("pipe error");
}

ssize_t
Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t		n;

	if ( (n = read(fd, ptr, nbytes)) == -1)
		err_sys("read error");
	return(n);
}

void
Sigaddset(sigset_t *set, int signo)
{
	if (sigaddset(set, signo) == -1)
		err_sys("sigaddset error");
}

void
Sigdelset(sigset_t *set, int signo)
{
	if (sigdelset(set, signo) == -1)
		err_sys("sigdelset error");
}

void
Sigemptyset(sigset_t *set)
{
	if (sigemptyset(set) == -1)
		err_sys("sigemptyset error");
}

void
Sigfillset(sigset_t *set)
{
	if (sigfillset(set) == -1)
		err_sys("sigfillset error");
}

int
Sigismember(const sigset_t *set, int signo)
{
	int		n;

	if ( (n = sigismember(set, signo)) == -1)
		err_sys("sigismember error");
	return(n);
}

void
Sigpending(sigset_t *set)
{
	if (sigpending(set) == -1)
		err_sys("sigpending error");
}

void
Sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	if (sigprocmask(how, set, oset) == -1)
		err_sys("sigprocmask error");
}

char *
Strdup(const char *str)
{
	char	*ptr;

	if ( (ptr = strdup(str)) == NULL)
		err_sys("strdup error");
	return(ptr);
}

long
Sysconf(int name)
{
	long	val;

	errno = 0;		/* in case sysconf() does not change this */
	if ( (val = sysconf(name)) == -1)
		err_sys("sysconf error");
	return(val);
}

#ifdef	HAVE_SYS_SYSCTL_H
void
Sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp,
	   void *newp, size_t newlen)
{
	if (sysctl(name, namelen, oldp, oldlenp, newp, newlen) == -1)
		err_sys("sysctl error");
}
#endif

void
Unlink(const char *pathname)
{
	if (unlink(pathname) == -1)
		err_sys("unlink error for %s", pathname);
}

pid_t
Wait(int *iptr)
{
	pid_t	pid;

	if ( (pid = wait(iptr)) == -1)
		err_sys("wait error");
	return(pid);
}

pid_t
Waitpid(pid_t pid, int *iptr, int options)
{
	pid_t	retpid;

	if ( (retpid = waitpid(pid, iptr, options)) == -1)
		err_sys("waitpid error");
	return(retpid);
}

void
Write(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
		err_sys("write error");
}

