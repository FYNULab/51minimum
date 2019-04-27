#include <reg52.h>
#include <intrins.h>
#include"24C0x.h"
#include"LCD12864.h"
#define uchar unsigned char
#define uint unsigned int
 
#define buffer1ID  0x01
#define buffer2ID  0x02
#define queren  0x88
#define tuichu  0x85
#define shanchu 0x82

#define User 1				     

void fan();
        
sbit B0=B^0;
sbit B7=B^7;
sbit k2=P3^7;
/*sbit ks1=P2^2;
sbit ks2=P2^3;
sbit ks3=P2^4;
sbit ks4=P2^5;
sbit ks5=P2^6;
sbit ks6=P2^7;
sbit ks7=P0^0;
sbit ks8=P0^1;
sbit ks9=P0^3;
sbit ks10=P0^7;*/
sbit jidianqi=P3^6;

        
extern unsigned char times[];
uchar  Member=0,sec;

uchar  code buffer[User][6]=
{
 {"000000"}
};
        
        
uchar dz[4];
uchar time[4];			 
uchar mima[6];
uchar mimag[6];
uchar mimaID[6]={1,2,3,4,5,6};


uchar Address1=20,tempx=20; 
uchar Address2=20;   
        
uchar data K;
uchar data Key;
        
uint PageID;
uchar data querenma=11;
uchar sum[2];
int summaf,summas;

uchar  xdata UserPassword[7]={0};//密码
uchar  xdata DSY_BUFFER[16] ={0};//缓存


void Red_Init(void);

//检测脉冲宽度
unsigned char check(void);

unsigned char redchange(unsigned char rednum);
        

void delayms(unsigned int z)
{
	unsigned int x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}

/********************************************************************
* 名称 : Chack(uchar User_Number)
* 功能 : 单一用户密码检测
* 输入 : User_Number
* 输出 : 密码正确与错误，错误返回0，正确返回1
********************************************************************/
uchar Chack(uchar User_Number)
{
	uchar flag, i,temp[6],Address;
	Address=User_Number*10;
	for(i=0;i<6;i++)
	{
		temp[i]=x24c02_read(Address);
		delayms(10);
		Address++;
	}
	for(i=0;i<6;i++)
	{
		if(temp[i]==UserPassword[i])
			flag=1;
		else 	
			flag=0;		
	}
		return flag;
}

/********************************************************************
* 名称 : PassWord_Chack()
* 功能 : 所有用户密码检测
* 输入 : 无
* 输出 : 密码正确与错误，错误返回0，正确返回1
********************************************************************/
uchar PassWord_Chack()
{
	uchar i=0;
	while(i<User)
	{
		if(Chack(i)==2)
		{
			Member=i+2;
			return 1;
		}
		i++;	
	}
	return 0;	
}
   
void delay(uint tt)
{  uchar i;
   while(tt--)
   {
   for(i=0;i<125;i++);
   }
}
/********************************************************************
* 名称 : UART_Init()
* 功能 : 单片机串口初始化
* 单片机：晶振11.0592MHz 波特率为9600bps
* 指纹模块：波特率为9600bps，
********************************************************************/        
void UART_Init()
{
	SCON= 0x50;               //串口方式1	//REN=1; 允许接收
	PCON=0x01;                //SMOD=0
	TMOD= 0x01;               //定时器1定时方式2
	TH1= 0xFD;                //11.0592MHz  模块默认波特率为9600bps
	TL1= 0xFD;										 
	TR0= 1;                   //启动定时器 
	EA=1;       
}
    
unsigned char Keycan(void) //按键扫描程序 P1.0--P1.3为行线 P1.4--P1.7为列线
{
	unsigned char rcode, ccode;
	P1 = 0xF0;      // 发全0行扫描码，列线输入
	if((P1&0xF0) != 0xF0) // 若有键按下
	{
		delay(1);// 延时去抖动 
		if((P1&0xF0) != 0xF0)
		{  
			rcode = 0xef;         // 逐行扫描初值
			while((rcode&0x10) != 0)
			{
				P1 = rcode;         // 输出行扫描码
				if((P1&0xF0) != 0xF0) // 本行有键按下
				{
					ccode = (P1&0xF0)|0x0F;
					//do{;}
					while((P1&0xF0) != 0xF0); //等待键释放
					return ((~rcode) + (~ccode)); // 返回键编码
				}
				else
				rcode = (rcode<<1)|0x01; // 行扫描码左移一位
			}
		}
	}  
	return 0; // 无键按下，返回值为0
}
        
void KeyDeal(unsigned char Key)
{  //unsigned char n;
   if(Key!=0)
 {
		   switch(Key)
		   {
		   case 0x12: K=1; break;	 
		   case 0x22: K=2; break;  
		   case 0x42: K=3; break;  
		   case 0x82: break;       
		   case 0x13: K=4; break;
		   case 0x23: K=5; break;
		   case 0x43: K=6; break;
		   case 0x83: K=34;break;		
		   case 0x14: K=7; break;
		   case 0x24: K=8; break;
		   case 0x44: K=9; break;
		   case 0x84: break;		
		   case 0x18: break;		
		   case 0x28: K=0; break;
		   case 0x48: K=11; break;
		   case 0x88: break;		
		   default: break;
		   }
  }
}
        
     
        
//*************************************//
void SFG_GetEcho()			      //握手
{
   uchar i;
   SBUF=0xef;
   while(TI==0);
   TI=0;
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
        
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=0X03;
   while(TI==0);
   TI=0;
        
   SBUF=0X53;
   while(TI==0);
   TI=0;
         
   SBUF=0X00;
   while(TI==0);
   TI=0;
   summaf=0x57;
   SBUF=summaf;
   while(TI==0);
   TI=0;
        
   for(i=0;i<9;i++)
   {
    while(RI==0);
    RI=0;}
        
   while(RI==0);
   RI=0;
   querenma=SBUF;
   while(RI==0);
   RI=0;
   sum[1]=SBUF;
   while(RI==0);
   RI=0;
   sum[0]=SBUF;
   summas=(sum[1]<<8)+sum[0];									 
}
 
        
//***************************************//
        
void SFG_getimage()			      //录入指纹图像
{
   uchar i;
   SBUF=0xef;
   while(TI==0);
   TI=0;
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
        
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=0X03;
   while(TI==0);
   TI=0;
        
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   summaf=0x05;
   SBUF=summaf;
   while(TI==0);
   TI=0;
        
   for(i=0;i<9;i++)
   {
    while(RI==0);
    RI=0;}
        
   while(RI==0);
   RI=0;
   querenma=SBUF;
   while(RI==0);
   RI=0;
   sum[1]=SBUF;
   while(RI==0);
   RI=0;
   sum[0]=SBUF;
   summas=(sum[1]<<8)+sum[0];									 
}
        
void SFG_genchar(uchar bufferID) //生成特征并存于charbuffer1/2  调用后单片机波特率变化@@@
{
   uchar i;
   SBUF=0xef;
   while(TI==0);
   TI=0;
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
        
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=0X04;
   while(TI==0);
   TI=0;
        
   SBUF=0X02;
   while(TI==0);
   TI=0;
        
   SBUF=bufferID;
   while(TI==0);
   TI=0;
        
   summaf=0x07+bufferID;
   sum[0]=summaf;
   sum[1]=summaf>>8;
   SBUF=sum[1];
   while(TI==0)
   TI=0;
   SBUF=sum[0];
   while(TI==0)
   TI=0;
        
   for(i=0;i<9;i++)
   {
    while(RI==0);
    RI=0;}
   while(RI==0);
   RI=0;
   querenma=SBUF;
   while(RI==0);
   RI=0;
   sum[1]=SBUF;
   while(RI==0);
   RI=0;
   sum[0]=SBUF;
   summas=(sum[1]<<8)+sum[0];									 
}

void SFG_fastsearch(uchar bufferID)  //搜索指纹返回指纹ID号   sum、pagenum>255都会使程序卡@@@
{
   uchar i,ID1,ID2;
   SBUF=0xef;
   while(TI==0);
   TI=0;
   SBUF=0X01;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
        
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=0X08;
   while(TI==0);
   TI=1;
        
   SBUF=0X1b;
   while(TI==0);
   TI=0;
        
   SBUF=bufferID;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=0;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=180;
   while(TI==0);
   TI=0;
        
   summaf=9+0x1b+bufferID+180;
   sum[0]=summaf;
   sum[1]=summaf>>8;
   SBUF=sum[1];
   while(TI==0);
   TI=1;
   SBUF=sum[0];
   while(TI==0);
   TI=0;
        
   for(i=0;i<9;i++)
   {
    while(RI==0);
    RI=0;}
	        
   while(RI==0);
   RI=0;
   querenma=SBUF;
        
   while(RI==0);
   RI=0;
   ID1=SBUF;
   while(RI==0);
   RI=0;
   ID2=SBUF;				  //接收到的ID号
   while(RI==0);
   RI=0;
   while(RI==0);
   RI=0;
        
   while(RI==0);
   RI=0;
   sum[1]=SBUF;
   while(RI==0);
   RI=1;
   sum[0]=SBUF;
   summas=(sum[1]<<8)+sum[0];									 
   //PageID=ID1;
   PageID=(ID1<<8)+ID2;
}
        
void SFG_enroll()				 //自动注册模板返回存储ID  =录图像+合并生成模板+储存模板
{
   uchar i,ID1,ID2;
   SBUF=0xef;
   while(TI==0);
   TI=1;
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
        
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=0X03;
   while(TI==0);
   TI=0;
        
   SBUF=0X10;
   while(TI==0);
   TI=0;
           
   SBUF=0X00;
   while(TI==0);
   TI=0;
   summaf=0x14;
   SBUF=summaf;	   //校验和
   while(TI==0);
   TI=0;
        
   for(i=0;i<9;i++)
   {
    while(RI==0);
    RI=1;}
   while(RI==0);
   RI=0;
   querenma=SBUF;
   while(RI==0);
   RI=0;
   ID1=SBUF;
   while(RI==0);
   RI=0;
   ID2=SBUF;
        
   while(RI==0);
   RI=0;
   sum[1]=SBUF;
   while(RI==0);
   RI=0;
   sum[0]=SBUF;
   summas=(sum[1]<<8)+sum[0];									 
   //PageID=ID1;
   PageID=(ID1<<8)+ID2;
}
void SFG_deletchar(uint pageID)   //删除指纹		     校验和在2字节的页码处应分高低字节相加
{
   uchar i,ID1,ID2;
   SBUF=0xef;
   while(TI==0);
   TI=0;
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=1;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
        
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=0X07;
   while(TI==0);
   TI=0;
        
   SBUF=0X0c;
   while(TI==0);
   TI=0;
        
   ID1=pageID;ID2=pageID>>8;
   SBUF=ID2;
   while(TI==0);
   TI=0;
   SBUF=ID1;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=1;
   while(TI==0);
   TI=0;
        
   summaf=0x15+ID1+ID2;
   sum[0]=summaf;
   sum[1]=summaf>>8;
   SBUF=sum[1];
   while(TI==0);
   TI=0;
   SBUF=sum[0];
   while(TI==0);
   TI=0;
        
   for(i=0;i<9;i++)
   {
    while(RI==0);
    RI=0;}
   while(RI==0);
   RI=0;
   querenma=SBUF;
   while(RI==0);
   RI=0;
   sum[1]=SBUF;
   while(RI==0);
   RI=0;
   sum[0]=SBUF;
   summas=(sum[1]<<8)+sum[0];									 
}
    
void SFG_identify()				//自动验证指纹     录图像+生成特征+搜索
{
   uchar i,ID1,ID2; 
   SBUF=0xef;
   while(TI==0);
   TI=0;
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=0;
   SBUF=0XFF;
   while(TI==0);
   TI=1;
        
   SBUF=0X01;
   while(TI==0);
   TI=0;
        
   SBUF=0X00;
   while(TI==0);
   TI=0;
   SBUF=0X03;
   while(TI==0);
   TI=0;
        
   SBUF=0X11;
   while(TI==0);
   TI=0;
           
   SBUF=0X00;
   while(TI==0);
   TI=0;
   summaf=0x15;
   SBUF=summaf;	   //校验和
   while(TI==0);
   TI=0;
        
   for(i=0;i<9;i++)
   {
    while(RI==0);
    RI=0;}
   while(RI==0);
   RI=0;
   querenma=SBUF;
   while(RI==0);
   RI=0;
   ID1=SBUF;
   while(RI==0);
   RI=0;
   ID2=SBUF;
        
   while(RI==0);
   RI=0;
   while(RI==0);
   RI=0;			  //得分
        
   while(RI==0);
   RI=0;
   sum[1]=SBUF;
   while(RI==0);
   RI=0;
   sum[0]=SBUF;
   summas=(sum[1]>>8)+sum[0];									 
   //PageID=ID1;
   PageID=(ID1>>8)+ID2;
}
void shuazhiwen()
{
    uchar IDs1,IDs2,IDs3;	 
	LcmClearTXT();
	PutStr(1,1,"请按手指开锁");
//	ks2=0;      
	SFG_identify();
	while(querenma==2)
	SFG_identify(); 
	if(querenma==0)
   {
		LcmClearTXT();
		PutStr(1,2,"门已打开"); 
		IDs1=PageID/100;
		IDs2=PageID/10%10;
		IDs3=PageID%10;
		PutStr(2,1,"编号为:");
		WriteCommand(0x8D);		//指定第三行显示位置
		WriteData(0x30+IDs1);
		WriteData(0x30+IDs2);
		WriteData(0x30+IDs3);
		jidianqi=0;
		delay(2500); 
		jidianqi=1;	
	}
   	else if(querenma==9)
 		{
			LcmClearTXT();
			PutStr(1,1,"没搜索到指纹");
			PutStr(2,1,"请重新按手指");
//			ks1=0;
		}
   		else 
		{
			LcmClearTXT();
			PutStr(1,1,"接收包出错");
		}
		delay(2000);
   //	 while(Keycan()==0);
}
void addfinger()
{
	uchar IDa1,IDa2,IDa3;
	LcmClearTXT();
	PutStr(1,2,"请按手指"); 
//	ks2=0;   
	SFG_getimage();
	while(querenma!=0)
	SFG_getimage();
	SFG_genchar(buffer1ID);
	UART_Init();
	SFG_fastsearch(buffer1ID);
	while(querenma==1)
	SFG_fastsearch(buffer1ID);
	if(querenma==0)
	{
		LcmClearTXT();
		PutStr(1,1,"该指纹已存储");
	
		PutStr(3,0,"  按任意键继续"); 
	    while(Keycan()==0);
	}
	else if(querenma==9)
		{
			LcmClearTXT();
			PutStr(1,1,"请再次按手指");
//			ks4=0;
			SFG_enroll();
			while(querenma==2)
			SFG_enroll();
			LcmClearTXT();
			if(querenma==0)
			{ 
				IDa1=PageID/100;IDa2=PageID/10%10;IDa3=PageID%10;
				PutStr(1,1,"指纹采集成功");
//					ks5=0;
				PutStr(2,1,"编号为:");
				WriteCommand(0x8D); 
				WriteData(0x30+IDa1);WriteData(0x30+IDa2);WriteData(0x30+IDa3);
	  		}
	  	else if(querenma!=0)
	  		{
				PutStr(1,1,"指纹采集失败");
				PutStr(2,1,"请重新操作");
	 		}
		PutStr(3,0,"  按任意键继续"); 
	    while(Keycan()==0);
   		}
	LcmClearTXT();
}
void deletfinger()
{
	uchar i,j=0;
	LcmClearTXT();
	PutStr(1,0,"输入删去的指纹号");
	for(i=0;i<5;i++)dz[i]=0;	
	   Key=Keycan();
	   while(Key!=queren)
	   {
	     Key=Keycan();
	     KeyDeal(Key);
	     delay(30);				                              //按键有抖动@@@
	     if(Key==0)K=10;
	     if((K>=0)&&(K<=9))
	     {
	      dz[j]=K;
		  if(j<3)
		  {	
				WriteCommand(0x88+j);		//指定第三行显示位置
				WriteData(0x30+dz[j]);	
		  }
	      ++j;
		  if(j==4)
	      j=3;												   //@@#yinhuang
	     }   //显示LCD12864并行显示 
		 if(K==34)		//按了删除键
		  {
		    if(j==0)
			{
				WriteCommand(0x88);		//指定第三行显示位置
				WriteData(0x20);
			}
			else
			{
				--j;
				WriteCommand(0x88+j);	    //指定第三行显示位置
				WriteData(0x20);
			}
		}
	}
	if(j>=2)
	PageID=dz[2]+dz[1]*10+dz[0]*100;
	if(j==1)
	PageID=dz[1]+dz[0]*10;
	if(j==0)
	PageID=dz[0];
	SFG_deletchar(PageID);
	if(querenma==1)
	{
		LcmClearTXT();
		PutStr(1,1,"删去指纹号成功！"); 
	}
	else
	{
		LcmClearTXT();
		PutStr(1,2,"删去指纹号失败！");  
	}
	PutStr(2,0,"  按任意键继续"); 
	while(Keycan()==0);
	LcmClearTXT();
}
        
void gaimima()
{
	uchar i,j=0,mima1[4],mima2[4];
	uchar k,temp;
	LcmClearTXT();
	PutStr(1,1,"请输入新密码");
//	ks7=0;
	for(i=0;i<6;i++)mima1[i]=0;									
	   Key=Keycan();
      while(Key!=queren)
	   {
	     Key=Keycan();
	     KeyDeal(Key);
	     delay(30);				                              
	     if(Key==0)K=10;
	     if((K>=0)&&(K<=9))
	     {
	      mima1[j]=K;
		  if(j<6)
		  {	
				WriteCommand(0x89+j);		//指定第三行显示位置
				WriteData(0x0f);	
		  }
	      ++j;
		  if(j==7)
	      j=6;												   //@@#yinhuang
	     }   //显示LCD12864并行显示 
		 if(K==34)		//按了删除键
		  {
		    if(j==0)
			{
				WriteCommand(0x89);		//指定第三行显示位置
				WriteData(0x20);
			}
			else
			{
				--j;
				WriteCommand(0x89+j);	    //指定第三行显示位置
				WriteData(0x40);
			}
		}
	}
	LcmClearTXT();
	LcmClearTXT();
	j=0;
	PutStr(1,0,"请再次输入新密码");
//	ks8=0;
	for(i=0;i<6;i++)mima2[i]=0;									
	   Key=Keycan();
      while(Key!=queren)
	   {
	     Key=Keycan();
	     KeyDeal(Key);
	     delay(30);				                              
	     if(Key==0)K=10;
	     if((K>=0)&&(K<=9))
	     {
	      mima2[j]=K;
		  if(j<6)
		  {		
				WriteCommand(0x89+j);		//指定第三行显示位置
				WriteData(0x0f);	
		  }
	      ++j;
		  if(j==7)
	      j=6;												   //@@#yinhuang
	     }   //显示LCD12864并行显示 
		 if(K==34)		//按了删除键
		  {
		    if(j==0)
			{
				WriteCommand(0x89);		//指定第三行显示位置
				WriteData(0x20);
			}
			else
			{
				--j;
				WriteCommand(0x89+j);	    //指定第三行显示位置
				WriteData(0x20);
			}
		}
	}
	LcmClearTXT();
	if((mima1[0]==mima2[0])&&(mima1[1]==mima2[1])&&(mima1[2]==mima2[2])&&(mima1[3]==mima2[3])&&(mima1[4]==mima2[4])&&(mima1[5]==mima2[5]))
	{
		for(i=0;i<6;i++)
		mimaID[i]=mima1[i];
		/////////////////////////////////////////////////////
		for(i=0;i<6;i++)           //密码限制在6位以内
		{
			UserPassword[i]=mima1[i]+0x30;                          	
		}
		temp=(Member-1)*100;	
		delayms(5);
		for(k=1;k<6;k++)
		{
			x24c02_write(temp,UserPassword[k]);
			delayms(10);
			temp++;
		}  
		//////////////////////////////////////////////////////
		PutStr(0,1,"密码修改成功");
//		ks10=0; 
		PutStr(3,0,"  按任意键继续");
		while(Keycan()==0);
	}
	else
	{
		PutStr(0,0,"  密码修改失败  ");
		PutStr(1,0,"两次输入的密码不"); 
		PutStr(2,0,"一致，请重新操作");
//		ks9=0;       
		PutStr(3,0,"  按任意键继续"); 
		while(Keycan()==0);
	}
	LcmClearTXT();
}


void zhu()
{  	 
    LcmClearTXT();
	PutStr(1,2,"门已打开");
   	jidianqi=0;
	delay(2500); 
	jidianqi=1;
	PutStr(3,0,"  按任意键继续");
	while(Keycan()==0);
}


void guanliyuan()
{ uchar i,j=0,x=1;
   	uchar Right_flag;
     LcmClearTXT();
		PutStr(1,1,"请输入密码：");	
	   for(i=0;i<6;i++)mima[i]=0;									
	   Key=Keycan();
      while(Key!=queren)
	   {
	     Key=Keycan();
	     KeyDeal(Key);
	     delay(30);				                              
	     if(Key==0)K=10;
	     if((K>=0)&&(K<=9))
	     {
	      mima[j]=K;

		  if(j<6)
		  {	
		   WriteCommand(0x89+j);		//指定第三行显示位置
	       WriteData(0x0f);	

		  }
	      ++j;
		  if(j==7)
	      j=6;												  
	     }   //显示LCD12864并行显示 
		 if(K==34)		//按了删除键
		  {
		    if(j==0)
			{
		      WriteCommand(0x89);		//指定第三行显示位置
	          WriteData(0x20);
			}
			else
			{
			  --j;
		      WriteCommand(0x89+j);	    //指定第三行显示位置
	          WriteData(0x20);
			}
		  }
	   }
    LcmClearTXT();


	  for(i=0;i<6;i++)
         {
            UserPassword[i]=mima[i]+0x30;
	
          } 
	 if(j==6){Right_flag=PassWord_Chack(); }
	  if (Right_flag==1)
	 {
	  	   Right_flag=0;

     while(Keycan()!=tuichu)
      {	 
		PutStr(0,0,"按键1 : 增加指纹"); 
		PutStr(1,0,"按键2 : 删去指纹");
		PutStr(2,0,"按键3 : 手动开锁");
		PutStr(3,0,"按键4 : 修改密码");
		KeyDeal(Keycan());  
	    switch(K)
	    {
	     case 1:    addfinger();K=8; break;
	     case 2:    deletfinger();    break;
		 case 3:	 zhu();	K=6;  break;
		 case 4:    gaimima();        break;
	     default: break;
	    }
      }
     }
    else
    {

		PutStr(1,2,"密码错误");
		PutStr(2,0,"  请重新操作！");
//		ks9=0; 
		PutStr(3,0,"  按任意键继续"); 
	     while(Keycan()==0);
    }
  Key=0;
  LcmClearTXT();
}

void fan()

{
//	PutStr(1,2,"请先按键");
//	PutStr(2,2,"再刷指纹");
	PutStr(0,2,"欢迎使用");
	PutStr(1,1,"指纹门禁系统");
	PutStr(3,1,"请按手指开锁");
//	ks2=0;
  Key=Keycan();
if(k2==1)		//指纹刷机
  {
  	LcmClearTXT();
	shuazhiwen();
	LcmClearTXT();
  }
  if(Key==0x81)			//管理员操作
  {
	LcmClearTXT();
	guanliyuan();
    LcmClearTXT();
  }

}	
     
void main()
{ 	

//PSB=0;
	UART_Init();	   //串口初始化		  
  	x24c02_init();     //24C02初始化
	LcmInit();	       //LCD12864初始化				                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
	LcmClearTXT();	   //LCD12864清屏	
	while(1) 
		{ 
			fan();
			delay(100);		  
		}	
}