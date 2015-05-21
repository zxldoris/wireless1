#include<reg51.h>
#include<DS18B20.h>
sbit OUT = P0^0;
unsigned char addr = 0x00;                          //Skip ROM
void Delay100ms();
void UART_Conf(unsigned int baud);
extern void UART_SendString(unsigned char *dat,unsigned char len);
extern void fb_38k();
int main()
{
	double temperature;                             //获取到的温度存储在这个变量
	int temp;                                       //中间临时变量
	char SendString[10]={0};
	unsigned char i = 10;
	EA=1;
	ET0=1;
	UART_Conf(600);
	fb_38k();                                //串口通信配置函数
	while(1)
	{
		                       

		if(! DS18B20_Start(&addr))                  //尝试初始化
		{
			UART_SendString("ERROR\n", 6);          //如果失败，串口发送ERROR
			Delay100ms();                          //延时1秒
		}
		else
		{
			Delay100ms();                          //延时1秒
			temperature = DS18B20_GetTemp(&addr);   //读取DS18B20，获取温度信息
			temp = (int) temperature;               //处理温度数据
			SendString[0] = temp / 10 % 10 + '0';
			SendString[1] = temp % 10 + '0';
			SendString[2] = '.';
			temp = (temperature - temp) * 100;
			SendString[3] = temp / 10 + '0';
			SendString[4] = ' ';
			SendString[5] = ' ';
			SendString[6] = SendString[0];
			SendString[7] = SendString[1];
			SendString[8] = '.';
			SendString[9] = SendString[3];
			SendString[10] = ' ';
			SendString[11] = ' ';
			SendString[12] = SendString[0];
			SendString[13] = SendString[1];
			SendString[14] = '.';
			SendString[15] = SendString[3];
			UART_SendString(SendString, 16);         //通过串口发送温度数据
		}
	}
}
void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 5;
	j = 52;
	k = 195;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
