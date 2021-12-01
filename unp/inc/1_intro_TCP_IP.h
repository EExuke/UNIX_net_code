/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : 1_intro_TCP_IP.h
 *     FIRST CREATION DATE    : 2021/11/29
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/11/29
 *     FILE DESCRIPTION       : API exported to external moduels
** ************************************************************************** */
#ifndef _1_INTRO_TCP_IP_H_
#define _1_INTRO_TCP_IP_H_

/*------------------Macro Definition-------------------------*/
/*------------------End of Macro Definition------------------*/

/*------------------API Definition---------------------------*/
int daytime_tcp_cli(const char *ip_addr);
int daytime_tcp_cli6(const char *ip6_addr);
int daytime_tcp_srv();
/*------------------End of API Definition--------------------*/

#endif /* End of _1_INTRO_TCP_IP_H_ */

