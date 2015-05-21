#include<reg52.h>
sbit OUT=P0^0; //P1.0���������
sfr AUXR  = 0x8E; //Auxiliary Register  T0x12 T1x12 UART_M0x6 BRTR S2SMOD BRTx12 EXTRAM S1BRS  0000,0000
void fb_38k()
{
	EA = 1;
AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x02;		//���ö�ʱ��ģʽ
	TL0 = 0x6E;		//���ö�ʱ��ֵ
	TH0 = 0x6E;		//���ö�ʱ����ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;		
}
void T0_time() interrupt 1
{
	OUT=~OUT; 
} 