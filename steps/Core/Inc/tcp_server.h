/********************************************************************
文件名称：TCP_SERVER.h
功    能：
编写时间：2013.4.25
编 写 人：
注    意：
*********************************************************************/
#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#define TCP_SERVER_PORT     1030

void tcp_server_send(uint8_t *par,int size);
void tcp_server_init(void);

#endif
