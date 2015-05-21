#include<reg51.h>
#include<config.h>
#define BUFFMAX 64
typedef unsigned char uint8;
typedef unsigned int uint16;
uint8 pdata UART_Buff[BUFFMAX];
uint8 UART_BuffIndex=0;
bit UART_SendFlag,UART_ResiveStringEndFlag,UART_ResiveStringFlag;

void UART_Conf(uint16 baud) //UART设置函数（buad：欲设置的波特率）
{
	TL1=TH1=256-11059200/12/32/baud;
	EA=1;
	ES=1;
	TMOD&=0X0F;
	TMOD|=0X20;
	SCON=0X50;
	TR1=1;
}
void UART_SendString(uint8 *dat,uint8 len)  //串口通信：发送一个字符串（dat：要发送的字符串，len：字符串长度）
{
	unsigned char i = 0xFF;
	while(len)
	{
		len--;
		SBUF=*dat;
		dat++;
		while(!UART_SendFlag);
		UART_SendFlag=0;
		while(i--);
	}
}
void interrupt_UART() interrupt 4   //串口中断函数
{
	if(TI)
	{
		TI=0;
		UART_SendFlag=1;
	}
	if(RI)
	{
		RI=0;
		UART_Buff[UART_BuffIndex]=SBUF;
		UART_ResiveStringFlag=1;
		UART_BuffIndex++;
	}
}