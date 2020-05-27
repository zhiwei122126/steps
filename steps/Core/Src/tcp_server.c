#include <lwip/tcp.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tcp_server.h"

const int client_count = 3;
const int client_recv_buf_size = 512;

static uint8_t  recv_buffer[client_count][client_recv_buf_size];


static uint32_t timer_par = 1;
static struct tcp_pcb *client_pcb[client_count] = {0,0,0};

int error_number = 0;


static int client_send(int client_idx, uint8_t *par,int size)
{
    tcp_write(client_pcb[client_idx], par, size, TCP_WRITE_FLAG_COPY );
    tcp_output(client_pcb[client_idx]);
    return 0;
}

/***********************************************************************
函数名称：tcp_server_recv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err)
功    能：TCP数据接收和发送
注    意：这是一个回调函数，当一个TCP段到达这个连接时会被调用
***********************************************************************/
static err_t client_recv(void *arg, struct tcp_pcb *cur_pcb,struct pbuf *pBuf,err_t err)
{
    struct pbuf *p_temp = pBuf;
    int i = 0;
    int client_idx = 0;
    // 没有数据。连接断开了
    if(NULL == pBuf){   
        for(i = 0; i < client_count; i++){
            if(client_pcb[i] == cur_pcb){
                tcp_close(cur_pcb);
                client_pcb[i] = NULL;
                return ERR_OK;
            }
        }
    }
    // 有数据
    for(i = 0; i < client_count; i++){
        if(client_pcb[i] == cur_pcb){
            client_idx = i;
            break;
        }
    }
    int recvd_byte_num = 0;

    while(p_temp != NULL) {
        recvd_byte_num += p_temp->tot_len;
        if(recvd_byte_num >= client_recv_buf_size){
            recvd_byte_num -=  p_temp->tot_len;
            break;
        }
        memcpy(&recv_buffer[client_idx][recvd_byte_num - p_temp->tot_len], p_temp->payload, p_temp->tot_len);
        tcp_recved(cur_pcb, p_temp->tot_len);
        p_temp = p_temp->next;
    }
    if(recvd_byte_num  > 0){
        // client_idx 接收到 recvd_byte_num 个字节的数据。处理数据
        tcp_server_send(&recv_buffer[client_idx][0],recvd_byte_num);
    }
    /* 释放该TCP段 */
    pbuf_free(pBuf);   

    return ERR_OK;
}

 /***********************************************************************
函数名称：client_err_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
功    能：回调函数
注    意：这是一个回调函数，当一个连接发生错误时被回调
***********************************************************************/

static void client_err_callback(void *arg, err_t err)
{
    
}
/***********************************************************************
函数名称：tcp_server_accept(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err)
功    能：回调函数
注    意：这是一个回调函数，当一个连接已经接受时会被调用
***********************************************************************/

static err_t tcp_server_accept(void *arg,struct tcp_pcb *new_pcb,err_t err)
{
    int i = 0;

    tcp_setprio(new_pcb, TCP_PRIO_MIN);  /* 设置回调函数优先级，当存在几个连接时特别重要,此函数必须调用*/
    tcp_recv(new_pcb,client_recv);   /* 设置TCP段到时的回调函数 */
    tcp_err(new_pcb,client_err_callback);
    for(i = 0; i < client_count; i++){
        if(client_pcb[i] == NULL){
            client_pcb[i] = new_pcb;
            err = ERR_OK;
            break;
        }
    }
    return err;
}



static void timer_callback(void *arg)
{
    // timer. send 
    char * hello = "hello from stm32f407 zzw";
    tcp_server_send(hello, strlen(hello) );
}


// 创建定时器，启动 1s 的定时器
// 创建 tcp server 套接字，兵设置它的回调函数。 
void tcp_server_init(void)      //定义TCP服务器线程
{
    struct tcp_pcb *server_pcb;

    /*****************************************************/
    server_pcb = tcp_new();                                 /* 建立通信的TCP控制块(pcb) */
    tcp_bind(server_pcb,IP_ADDR_ANY,TCP_SERVER_PORT);       /* 绑定本地IP地址和端口号（作为tcp服务器） */
    server_pcb = tcp_listen(server_pcb);                    /* 进入监听状态 */
    error_number = 1;
    tcp_accept(server_pcb,tcp_server_accept);              /* 设置有连接请求时的回调函数 */
    error_number = 2;
    //创建软件周期定时器。周期1s(1000个时钟节拍)，周期模式
    osTimerId_t timer_id = osTimerNew(timer_callback, osTimerPeriodic, &timer_par, NULL);
    if (timer_id == NULL) {
        // Periodic timer create fail.
        error_number = -1;
    }
    uint32_t  timerDelay = 500U;   // 10ms - per tick
    osStatus_t  status = osTimerStart(timer_id, timerDelay);       // start timer
    if (status != osOK) {
      // Timer could not be started
        error_number = -2;
    }
}

void tcp_server_send(uint8_t *par,int size)
{
    for(int i = 0; i < 3; i++){
        if(client_pcb[i] != NULL){
             client_send(i,par,size);
        }
    }
}
