#include<reg51.h>
#include<DS18B20.h>

#define _6nop(); {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
//定义适用于STC_Y3,STC_Y5的nop延时
#define STC_Y1 1	//89
#define STC_Y3 3	//12
#define STC_Y5 5	//15

sbit DS18B20_IO = DS18B20_IO_SET;

#ifndef STC_YX
#define STC_YX STC_Y1
#endif //如果没有定义STC单片机的指令集，则默认为STC_Y5指令集

#ifndef XTAL
#define XTAL 11.059200
#endif //如果没有定义晶振频率，则默认为11.0592M晶振

/*///////////////////////////////////////////////////////////////////////////////////
*函数名：DelayX10us
*函数功能：延时
*参数列表：
*   t
*       参数类型：unsigned char型整数
*       参数描述：要延时的时长，单位：10us
*返回值：无。
*版本：1.0
*作者：何相龙
*日期：2014年12月11日
*版本：1.1
*版本改动：重写函数，使之能在各种指令集、各晶振频率下工作
*作者：何相龙
*日期：2015年5月11日
*////////////////////////////////////////////////////////////////////////////////////
void DelayX10us(unsigned char t)
{
	unsigned int i;
		i = t * 2;
        while (--i)
		{
			_6nop();_6nop();_6nop();_6nop();_6nop();
			_6nop();_nop_();_nop_();_nop_();
		}
}
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：DS18B20_Init
*函数功能：初始化DS18B20
*参数列表：
*   无
*返回值：一个bit型变量，初始化是否正常（1；正常，0：失败）
*版本：1.0
*作者：何相龙
*日期：2015年5月11日
*////////////////////////////////////////////////////////////////////////////////////
bit DS18B20_Init()
{
	bit temp;
	EA=0;               //禁能中断，防止时序被干扰
	DS18B20_IO=0;       //拉低1-wine总线，发送Init信号
	DelayX10us(60);     //延时600us
	DS18B20_IO=1;       //释放总线
	DelayX10us(8);      //延时80us，等待器件响应
	temp=DS18B20_IO;    //读取总线
	while(!DS18B20_IO); //等待总线被释放
	EA=1;               //恢复中断使能
	return ~temp;       //返回获取到的初始化状态，为符合习惯，进行取反后再返回
}
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：DS18B20_Write
*函数功能：向DS18B20写一个unsigned char型的数据或命令
*参数列表：
*   dat
*       参数类型：unsigned char型整数
*       参数描述：要写入的数据或命令
*返回值：无
*版本：1.0
*作者：何相龙
*日期：2014年12月11日
*////////////////////////////////////////////////////////////////////////////////////
void DS18B20_Write(unsigned char dat)
{
	unsigned char mask;
	EA=0;                               //禁能中断，防止时序被干扰
	for(mask=0x01;mask!=0;mask<<=1)     //按位发送数据
	{
		if(dat&mask)                    //获取当前位是否为1
		{
			DS18B20_IO=0;               //位开始信号
			#if STC_YX == STC_Y1
			_nop_();                    //稍微延时，防止错误。对不同的单片机指令集做了不同的处理
			#else
            _6nop();_6nop();
			#endif
			DS18B20_IO=1;               //拉高总线，发送数据
			DelayX10us(8);              //延时80us
		}
		else                            //当前位为0
		{
			DS18B20_IO=0;               //拉低总线，发送数据
			DelayX10us(8);              //延时80us
			DS18B20_IO=1;               //释放总线
		}
			#if STC_YX == STC_Y1
			_nop_();_nop_();            //稍微延时，防止错误。对不同的单片机指令集做了不同的处理
			#else
            _6nop();_6nop();
			#endif
	}
	EA=1;                               //恢复中断使能
}
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：DS18B20_Read
*函数功能：从DS18B20中读取一位数据
*参数列表：
*   无
*版本：1.0
*作者：何相龙
*日期：2014年12月11日
*////////////////////////////////////////////////////////////////////////////////////
unsigned char DS18B20_Read()
{
	unsigned char mask,dat=0;
	EA=0;                               //禁能中断，防止时序被干扰
	for(mask=0x01;mask!=0;mask<<=1)     //按位读取数据
	{
		DS18B20_IO=0;                   //拉低总线，发送起始信号
        #if STC_YX == STC_Y1
        _nop_();_nop_();                //稍微延时，防止错误。对不同的单片机指令集做了不同的处理
        #else
        _6nop();_6nop();
        #endif

		DS18B20_IO=1;                   //释放总线
		DelayX10us(1);                  //延时10us
		if(DS18B20_IO)dat|=mask;        //读取总线，获取1位数据被保存
		DelayX10us(5);                  //延时50us，准备读取下一位数据
	}
	EA=1;                               //恢复中断使能
	return dat;
}
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：DS18B20_Start
*函数功能：启动DS18B20温度转换。
*参数列表：
*   *addr
*       参数类型：unsigned char型指针（连续8个unsigned char的数据）
*       参数描述：64字节的器件编号。特别的，Skip ROM时addr[0]请传入0x00
*返回值：一个bit型变量，启动是否正常（1；正常，0：失败）
*版本：1.0
*作者：何相龙
*日期：2014年12月11日
*////////////////////////////////////////////////////////////////////////////////////
bit DS18B20_Start(unsigned char *addr)
{
	if(!DS18B20_Init())return 0;            //发送初始化信号，如果初始化错误，返回0
	if(addr[0]==0x00)DS18B20_Write(0xCC);   //如果addr[0]为0x00，发送Skip ROM指令
	else                                    //否则，发送Match ROM指令，并发送器件ROM号
	{
		DS18B20_Write(0x55);
		DS18B20_Write(addr[0]);
		DS18B20_Write(addr[1]);
		DS18B20_Write(addr[2]);
		DS18B20_Write(addr[3]);
		DS18B20_Write(addr[4]);
		DS18B20_Write(addr[5]);
		DS18B20_Write(addr[6]);
		DS18B20_Write(addr[7]);
	}
	DS18B20_Write(0x44);                    //发送启动温度转换指令
	return 1;
}
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：DS18B20_GetTemp
*函数功能：读取DS18B20，获得温度。
*参数列表：
*   *addr
*       参数类型：unsigned char型指针（连续8个unsigned char的数据）
*       参数描述：64字节的器件编号。特别的，Skip ROM时addr[0]请传入0x00
*返回值：一个double型变量，获取到的温度值。（发生错误时，返回-999）
*版本：1.0
*作者：何相龙
*日期：2014年12月11日
*版本：1.1
*版本改动：修改返回值类型，直接返回double型数据
*作者：何相龙
*日期：2015年5月11日
*////////////////////////////////////////////////////////////////////////////////////
double DS18B20_GetTemp(unsigned char *addr)
{
	int temp;                                   //储存温度值技术中级数据
	unsigned char temp1,temp2;                  //储存温度值技术中级数据
	unsigned int temp3;                         //储存温度值技术中级数据
	double tempo;                               //储存最终得到的温度值
	bit flag = 0;                               //正负标记位
	if(!DS18B20_Init())return -999;             //发送初始化信号，如果初始化错误，返回-999
	if(addr[0] == 0x00)DS18B20_Write(0xCC);     //如果addr[0]为0x00，发送Skip ROM指令
	else                                        //否则，发送Match ROM指令，并发送器件ROM号
	{
		DS18B20_Write(0x55);
		DS18B20_Write(addr[0]);
		DS18B20_Write(addr[1]);
		DS18B20_Write(addr[2]);
		DS18B20_Write(addr[3]);
		DS18B20_Write(addr[4]);
		DS18B20_Write(addr[5]);
		DS18B20_Write(addr[6]);
		DS18B20_Write(addr[7]);
	}
	DS18B20_Write(0xBE);                        //发送读取温度指令
	temp1=DS18B20_Read();                       //读第一个unsigned char型数据，并暂存
	temp2=DS18B20_Read();                       //读第二个unsigned char型数据，并暂存
	temp3=(unsigned int)temp2;                  //将两位unsigned char型数据放到一个unsigned int型变量中
	temp3 <<= 8;
	temp3|=(unsigned int)temp1;
	DelayX10us(15);                             //延时150us
	if(temp3&0x8000)                            //如果得到的温度值是负值
	{
		temp3 &= 0x07FF;                        //获取温度值的绝对值
		temp = -temp3;
		flag = 1;                               //设置负值标志
	}
	else
	{
		temp3 &= 0x07FF;                        //获取温度值的绝对值
		temp = temp3;
	}
	tempo = temp >> 4;                          //处理温度值，存储到double型变量tempo中
	tempo += (temp & 0x08) * 0.5 + (temp & 0x04) * 0.25 + (temp & 0x02) * 0.125 + (temp & 0x01) * 0.6275;
	return tempo;
}