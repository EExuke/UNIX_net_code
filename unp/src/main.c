/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : main.c
 *     FIRST CREATION DATE    : 2021/11/28
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/11/28
 *     FILE DESCRIPTION       :
** ************************************************************************** */

#include "/home/xuke/bin/dbg.h"
#include "unp.h"
#include "1_intro_TCP_IP.h"
#include "2_basic_socket.h"

/***************************************************************************************
 * Function Name : main
 * Description   : test and run unp programs
 * @param [in ]  :
 * @param [out]  : NULL
 * @return       : 0
 ***************************************************************************************/
int main(int argc, const char *argv[])
{
	my_debug_msg("1:========================================");
	my_debug_msg("| test 1-1: daytime_tcp_cli ==>");
	daytime_tcp_cli("127.0.0.1");

	my_debug_msg("| test 1-2: daytime_tcp_cli6 ==>");
	daytime_tcp_cli6("::1");

	/*my_debug_msg("| test 1-3: daytime_tcp_srv ==>");*/
	/*daytime_tcp_srv();  //have loop*/

	my_debug_msg("2:========================================");
	my_debug_msg("| test 2-1: byte_order ==>");
	byte_order();

	/*my_debug_msg("| test 2-2: daytime_tcp_srv1 ==>");*/
	/*daytime_tcp_srv1();  //have loop*/

	/*my_debug_msg("| test 2-3: tcp_serv01 ==>");*/
	/*tcp_serv01();  //have loop*/

	/*my_debug_msg("| test 2-4: tcp_serv04 ==>");*/
	/*tcp_serv04();  //have loop*/

	return 0;
}

