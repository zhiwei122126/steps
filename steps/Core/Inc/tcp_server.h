/********************************************************************
�ļ����ƣ�TCP_SERVER.h
��    �ܣ�
��дʱ�䣺2013.4.25
�� д �ˣ�
ע    �⣺
*********************************************************************/
#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#define TCP_SERVER_PORT     1030

void tcp_server_send(uint8_t *par,int size);
void tcp_server_init(void);

#endif
