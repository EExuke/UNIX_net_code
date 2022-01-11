/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : 2_basic_socket.h
 *     FIRST CREATION DATE    : 2021/12/01
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/12/01
 *     FILE DESCRIPTION       : API exported to external moduels
** ************************************************************************** */
#ifndef _2_BASIC_SOCKET_H_
#define _2_BASIC_SOCKET_H_

/*------------------Macro Definition-------------------------*/
/*------------------End of Macro Definition------------------*/

/*------------------API Definition---------------------------*/
int byte_order();

int daytime_tcp_srv1();

int tcp_serv01();
int tcp_cli01(char *ip_addr);
int tcp_serv04();
int tcp_cli04(char *ip_addr);

void str_cli_select02(FILE *fp, int sockfd);
int tcp_serv_select01();
int tcp_serv_poll01();

int checkopts();

int udp_serv01();
int udp_cli01(char *ip_addr);
int udp_cli09(char *ip_addr);
int udp_serv_select01();

int hostent(char *host);
int daytimetcpcli1(char *hostname, char *servname);
int daytimetcpcli(char *hostname, char *servname);
int daytimetcpsrv1(char *servname);
int daytimetcpsrv2(char *hostname, char *servname);
int daytimeudpcli1(char *hostname, char *servname);
int daytimeudpsrv2(char *hostname, char *servname);
/*------------------End of API Definition--------------------*/

#endif /* End of _2_BASIC_SOCKET_H_ */

