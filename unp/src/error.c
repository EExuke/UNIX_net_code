/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : error.c
 *     FIRST CREATION DATE    : 2021/11/29
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/11/29
 *     FILE DESCRIPTION       : unix code err msgs Functions
** ************************************************************************** */

#include "unp.h"

#include <stdarg.h>  /* ANSI C header */
#include <syslog.h>  /* for syslog */

int daemon_proc;     /* set nonzero by daemon_init() */

/***************************************************************************************
 * Description   : print msg and return to caller
 *               : caller specifies "errnoflag" and "level"
 ***************************************************************************************/
static void err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int errno_save, n;
	char buf[MAXLINE + 1];

	errno_save = errno; //value caller might want printed
#ifdef HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap);  //safe
#else
	vsprintf(buf, fmt, ap);  //not safe
#endif /* HAVE_VSPRINTF */

	n = strlen(buf);
	if (errnoflag) {
		snprintf(buf+n, MAXLINE-n, ": %s", strerror(errno_save));
	}
	strcat(buf, "\n");

	if (daemon_proc) {
		syslog(level, "%s", buf);
	} else {
		fflush(stdout);  //in caese stdout is same to stderr
		fputs(buf, stderr);
		fflush(stderr);
	}
	return;
}

/***************************************************************************************
 * Description   : nonfatal error related to system call
 *               : print msg and return
 ***************************************************************************************/
void err_ret(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/***************************************************************************************
 * Description   : fatal error related to system call
 *               : print msg and terminate
 ***************************************************************************************/
void err_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/***************************************************************************************
 * Description   : fatal error related to system call
 *               : print msg, dump core, and terminate
 ***************************************************************************************/
void err_dump(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();    //dump core and terminate
	exit(1);    //shouldn't get here
}

/***************************************************************************************
 * Description   : nonfatal error unrelated to system call
 *               : print msg and return
 ***************************************************************************************/
void err_msg(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/***************************************************************************************
 * Description   : fatal error unrelated to system call
 *               : print msg and terminate
 ***************************************************************************************/
void err_quit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);    //shouldn't get here
}

