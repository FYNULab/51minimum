#ifndef 	_LCD12864_h_
#define 	_LCD12864_h_
/**************************************************************
               iO口宏定义区
***************************************************************/
sbit CS =P2^5;//RS
sbit SID=P2^6;//RW
sbit SCK=P2^7;//E
//sbit PSB=P2^3;//E


extern void WriteCommand( unsigned char Cbyte );
extern void WriteData( unsigned char Dbyte );
extern void LcmInit( void );
extern void LcmClearTXT( void );
//extern void LcmClearBMP( void );

extern void PutStr(unsigned char row,unsigned char col,unsigned char *puts);
//extern void PutNum(unsigned char row,unsigned char col,unsigned int num);



#endif