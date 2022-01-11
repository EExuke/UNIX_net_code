/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : 4_client_server_model.h
 *     FIRST CREATION DATE    : 2022/01/11
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2022/01/11
 *     FILE DESCRIPTION       : API exported to external moduels
** ************************************************************************** */
#ifndef _4_CLIENT_SERVER_MODEL_H_
#define _4_CLIENT_SERVER_MODEL_H_

#include <sys/resource.h>

/*------------------Macro Definition-------------------------*/
#define MAXN    16384

/*------------------End of Macro Definition------------------*/

/*------------------API Definition---------------------------*/
int test_client(const char *hostname, const char *service, int nchildren, int nloops, int nbytes);

#ifndef HAVE_GETRUSAGE_PROTO
int getrusage(int, struct rusage *);
#endif

int model_serv01(char *hostname, char *service);
/*------------------End of API Definition--------------------*/

#endif /* End of _4_CLIENT_SERVER_MODEL_H_ */

