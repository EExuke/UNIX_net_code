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

/***************************************************************************************
 * wrapsock functions
 ***************************************************************************************/
int Socket(int family, int type, int protocol)
{
	int n;
	if ((n = socket(family, type, protocol)) < 0) {
		err_sys("socket error");
	}
	return n;
}

/***************************************************************************************
 * wrappthread functions
 ***************************************************************************************/
void Pthread_mutex_lock(pthread_mutex_t *mptr)
{
	int n;

	if ((n = pthread_mutex_lock(mptr)) == 0) {
		return;
	}
	errno = n;
	err_sys("pthread_mutex_lock error");
}

