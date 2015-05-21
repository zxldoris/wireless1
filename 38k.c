#include<reg52.h>
sbit OUT=P0^0; //P1.0口输出方波
sfr AUXR  = 0x8E; //Auxiliary Register  T0x12 T1x12 UART_M0x6 BRTR S2SMOD BRTx12 EXTRAM S1BRS  0000,0000
void fb_38k()
{
	EA = 1;
AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x02;		//设置定时器模式
	TL0 = 0x6E;		//设置定时初值
	TH0 = 0x6E;		//设置定时重载值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;		
}
void T0_time() interrupt 1
{
	OUT=~OUT; 
} 