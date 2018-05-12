/*********************************************************************************************************
* 模块名称: LCD.h
* 摘    要: 
* 当前版本: 1.0.0
* 作    者: 666immortal
* 完成日期: 2018年03月01日 
* 内    容:
* 注    意: none                                                                  
**********************************************************************************************************
* 取代版本: 
* 作    者:
* 完成日期: 
* 修改内容:
* 修改文件: 
*********************************************************************************************************/
#ifndef _LCD_H_
#define _LCD_H_	

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "stdlib.h"
#include "DataType.h"
#include "stm32f10x.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
//LCD背光点亮，PB0
#define BACK_LIGHT_ON()   GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);    

//使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A10作为数据命令区分线 
//注意设置时STM32内部会右移一位对齐!  
#define LCD_BASE        ((u32)(0x6C000000 | 0x000007FE))    //LCD的基地址
#define LCD             ((LCD_TypeDef *) LCD_BASE)          //将LCD的基地址强制转化为LCD_TypeDef类型的指针

#define DFT_SCAN_DIR  L2R_U2D                               //默认的扫描方向

//画笔颜色
#define WHITE   0xFFFF    //白色
#define BLACK   0x0000    //黑色 
#define BLUE    0x001F    //蓝色 
#define BRED    0XF81F    //蓝红混色
#define GRED    0XFFE0    //绿红混色
#define GBLUE   0X07FF    //绿蓝混色
#define RED     0xF800    //红色
#define MAGENTA 0xF81F    //品红色
#define GREEN   0x07E0    //绿色
#define CYAN    0x7FFF    //蓝绿色
#define YELLOW  0xFFE0    //黄色
#define BROWN   0XBC40    //棕色
#define BRRED   0XFC07    //棕红色
#define GRAY    0X8430    //灰色

//GUI颜色
#define DARKBLUE  0X01CF  //深蓝色
#define LIGHTBLUE 0X7D7C  //浅蓝色  
#define GRAYBLUE  0X5458  //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN  0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY       0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE   0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE      0X2B12 //浅棕蓝色(选择条目的反色)

//LCD分辨率设置
#define SSD_HOR_RESOLUTION  800 //LCD水平分辨率
#define SSD_VER_RESOLUTION  480 //LCD垂直分辨率

//LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH 1   //水平脉宽
#define SSD_HOR_BACK_PORCH  46  //水平前廊
#define SSD_HOR_FRONT_PORCH 210 //水平后廊

#define SSD_VER_PULSE_WIDTH 1   //垂直脉宽
#define SSD_VER_BACK_PORCH  23  //垂直前廊
#define SSD_VER_FRONT_PORCH 22  //垂直前廊

//如下几个参数，自动计算
#define SSD_HT  (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_BACK_PORCH)
#define SSD_VT  (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
//LCD地址结构体,两个成员分别用来写命令和写数据
typedef struct
{
  vu16 LCD_REG;   //向LCD写命令
  vu16 LCD_RAM;   //向LCD写数据
}LCD_TypeDef;

typedef enum
{
  L2R_U2D = 0,    //从左到右,从上到下
  L2R_D2U,        //从左到右,从下到上
  R2L_U2D,        //从右到左,从上到下
  R2L_D2U,        //从右到左,从下到上
  U2D_L2R,        //从上到下,从左到右
  U2D_R2L,        //从上到下,从右到左
  D2U_L2R,        //从下到上,从左到右
  D2U_R2L         //从下到上,从右到左
}EnumDirection; 

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/    															  
void InitTFTLCD(void);                                                      //初始化LCD
u16  GetLCDID(void);                                                        //获取LCD的ID
void LCDDispOn(void);                                                       //LCD开启显示
void LCDDispOff(void);                                                      //LCD关闭显示
void SetPointColor(u16 color);                                              //设置画笔颜色
void SetBackColor(u16 color);                                               //设置背景颜色
                   
void LCDClear(u16 color);                                                   //清屏
void LCDSetCursor(u16 xPos, u16 yPos);                                      //设置光标
void LCDDrawPoint(u16 x, u16 y);                                            //画点
void LCDFastDrawPoint(u16 x, u16 y, u16 color);                             //快速画点
u16  LCDReadPoint(u16 x, u16 y);                                            //读点 
void LCDDrawCircle(u16 x0, u16 y0, u8 r);                                   //画圆
void LCDDrawLine(u16 x1, u16 y1, u16 x2, u16 y2);                           //画线
void LCDDrawRect(u16 x1, u16 y1, u16 x2, u16 y2);                           //画矩形
void LCDFill(u16 startX, u16 startY, u16 endX, u16 endY, u16 color);        //填充单色
void LCDColorFill(u16 startX, u16 startY, u16 endX, u16 endY, u16* pColor); //填充指定颜色
void LCDShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode);                   //显示一个字符
void LCDShowNum(u16 x, u16 y, u32 num, u8 len, u8 size);                    //显示一个数字
void LCDShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode);          //显示数字
void LCDShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8* p);    //显示一个字符串,12/16字体

void LCDSSDSetBackLight(u8 pwm);                                            //SSD1963 背光控制
void LCDSetScanDir(u8 dir);                                                 //设置屏扫描方向
void LCDSetDispDir(u8 dir);                                                 //设置屏幕显示方向
void LCDSetWindow(u16 sx, u16 sy, u16 width, u16 height);                   //设置窗口

#endif  
