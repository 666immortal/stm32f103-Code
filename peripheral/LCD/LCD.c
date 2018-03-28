/*********************************************************************************************************
* 模块名称: LCD.c
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

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "LCD.h"
#include "stdlib.h"
#include "Font.h" 
#include "UART.h"
#include "SysTick.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
//LCD控制块，管理LCD相关的重要参数
typedef struct  
{
  u16 width;          //LCD 宽度
  u16 height;         //LCD 高度
  u16 id;             //LCD ID
  u8  dir;            //横竖屏控制标志位：0，竖屏；1，横屏
  u16 wramcmd;        //开始写GRAM指令
  u16 setXCmd;        //设置x坐标指令
  u16 setYCmd;        //设置y坐标指令 
}StructLCDCtrlBlock; 

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static  StructLCDCtrlBlock s_structLCDCB; //LCD控制块，管理LCD相关的重要参数，默认为竖屏
static  u16 s_iPointColor = 0x0000;       //画笔颜色
static  u16 s_iBackColor  = 0xFFFF;       //背景色

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void LCD_WR_REG(u16 regVal);                     //写寄存器函数 
static void LCD_WR_DATA(u16 data);                      //写LCD数据 
static u16  LCD_RD_DATA(void);                          //读LCD数据 
static void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);//写寄存器
static u16  LCD_ReadReg(u16 LCD_Reg);                   //读寄存器 
static void LCD_WriteRAM_Prepare(void);                 //开始写GRAM
static void LCD_WriteRAM(u16 RGB_Code);                 //向LCD写入一个颜色值
static u16  LCD_BGR2RGB(u16 c);                         //通过该函数转换将GBR格式的数据转换成RGB格式
static void opt_delay(u8 i);                            //延时函数
static u32  LCDPower(u8 m, u8 n);                       //求m的n次幂函数

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: LCD_WR_REG
* 函数功能: 写寄存器函数 
* 输入参数: regVal:寄存器值 
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意:
*********************************************************************************************************/
static void LCD_WR_REG(u16 regVal)
{   
  LCD->LCD_REG = regVal;        //写入要写的寄存器序号   
}

/*********************************************************************************************************
* 函数名称: LCD_WR_DATA
* 函数功能: 写LCD数据 
* 输入参数: data:要写入的值 
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意:
*********************************************************************************************************/
static void LCD_WR_DATA(u16 data)
{ 
  LCD->LCD_RAM = data;          //向LCD写入数据
}

/*********************************************************************************************************
* 函数名称: LCD_RD_DATA
* 函数功能: 读LCD数据 
* 输入参数: void
* 输出参数: void
* 返 回 值: 读取到的数据
* 创建日期: 2018年03月01日 
* 注    意:
*********************************************************************************************************/
static u16 LCD_RD_DATA(void)
{
  vu16 ram;                     //防止变量被优化
        
  ram = LCD->LCD_RAM;           //读取LCD的数据并存入ram中
        
  return ram;                   //返回读取到的数据
}

/*********************************************************************************************************
* 函数名称: LCD_WriteReg
* 函数功能: 写寄存器 
* 输入参数: LCD_Reg:寄存器地址，LCD_RegValue:要写入的数据
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意:
*********************************************************************************************************/
static void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
  LCD->LCD_REG = LCD_Reg;       //写入要写的寄存器序号  
  LCD->LCD_RAM = LCD_RegValue;  //写入数据  
} 

/*********************************************************************************************************
* 函数名称: LCD_ReadReg
* 函数功能: 读寄存器 
* 输入参数: LCD_Reg:寄存器地址 
* 输出参数: void
* 返 回 值: 读到的数据
* 创建日期: 2018年03月01日 
* 注    意:
*********************************************************************************************************/
static u16 LCD_ReadReg(u16 LCD_Reg)
{ 
  LCD_WR_REG(LCD_Reg);          //写入要读的寄存器序号
  DelayNus(5);                  //延时5us
          
  return LCD_RD_DATA();         //返回读到的值
}  

/*********************************************************************************************************
* 函数名称: LCD_WriteRAM_Prepare
* 函数功能: 开始写GRAM 
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意:
*********************************************************************************************************/
static void LCD_WriteRAM_Prepare(void)
{
  LCD->LCD_REG = s_structLCDCB.wramcmd; //向LCD寄存器写入开始写GRAM指令
}

/*********************************************************************************************************
* 函数名称: LCD_WriteRAM
* 函数功能: 向LCD写入一个颜色值
* 输入参数: RGB_Code:颜色值 
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意:
*********************************************************************************************************/
static void LCD_WriteRAM(u16 RGB_Code)
{ 
  LCD->LCD_RAM = RGB_Code;              //向LCD写入一个颜色值
}

/*********************************************************************************************************
* 函数名称: LCD_BGR2RGB
* 函数功能: 从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式，通过该函数转换 
* 输入参数: c:GBR格式的颜色值 
* 输出参数: void
* 返 回 值: rgb-RGB格式的颜色值
* 创建日期: 2018年03月01日 
* 注    意:
*********************************************************************************************************/
static u16 LCD_BGR2RGB(u16 c)
{
  u16  rgb;                               //用来存放RGB颜色数据
  u16  r;                                 //用来存放红色的数据
  u16  g;                                 //用来存放绿色的数据
  u16  b;                                 //用来存放蓝色的数据
  
  b = (c >> 0) & 0x1f;                    //从c中取出蓝色的数据（低5位）存入b中
  g = (c >> 5) & 0x3f;                    //从c中取出绿色的数据（中间6位）存入g中
  r = (c >> 11) & 0x1f;                   //从c中取出红色的数据（高5位）存入r中
  rgb = (b << 11) + (g << 5) + (r << 0);  //将三种颜色的数据组合起来存进rgb中
  
  return rgb;                             //返回转化好的RGB颜色数据
} 
/*********************************************************************************************************
* 函数名称: opt_delay
* 函数功能: 简单的延时函数
* 输入参数: i-延时的时间长度
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 当mdk -O1时间优化时需要设置
*********************************************************************************************************/
static void opt_delay(u8 i)
{
  while(i--)                              //循环递减，直至i为0
  {
    
  }
}

/*********************************************************************************************************
* 函数名称: LCDPower
* 函数功能: 求m的n次幂函数 
* 输入参数: m为底，n为幂 
* 输出参数: void
* 返 回 值: m的n次方
* 创建日期: 2018年03月01日 
* 注    意:
*********************************************************************************************************/
static u32 LCDPower(u8 m, u8 n)
{
  u32 result = 1;                         //定义用来存放结果的变量       
                                          
  while(n--)                              //随着每次循环，n递减，直至为0
  {                                       
    result *= m;                          //循环n次，相当于n个m相乘
  }                                       
                                          
  return result;                          //返回m的n次幂的值
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: InitTFTLCD
* 函数功能: 初始化LCD
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 该初始化函数可以初始化各种ILI93XX液晶,但其他函数基于ILI9320，在其他型号的驱动芯片上没有测试
*********************************************************************************************************/
void InitTFTLCD(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;                  //定义结构体GPIO_InitStructure,用来配置LCD的GPIO
  //定义结构体FSMC_NORSRAMInitStructure用来配置LCD的参数
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure; 
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;       //定义结构体readWriteTiming来配置LCD的读时序
  FSMC_NORSRAMTimingInitTypeDef  writeTiming;           //定义结构体writeTiming来配置LCD的写时序
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);     //使能FSMC的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能GPIOB以及AFIO复用功能时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); //使能GPIOD以及AFIO复用功能时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //使能GPIOE以及AFIO复用功能时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE); //使能GPIOG以及AFIO复用功能时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;         //设置LCD背光灯的引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置I/O口速率为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);            //根据参数初始化LCD背光灯的GPIO端口
  
  //PORTD复用推挽输出  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5 | GPIO_Pin_8 | 
                                GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置I/O口速率为50MHz
  GPIO_Init(GPIOD, &GPIO_InitStructure);            //根据参数初始化LCD背光灯的GPIO端口
  
  //PORTE复用推挽输出  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 | 
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出模式  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置I/O口速率为50MHz
  GPIO_Init(GPIOE, &GPIO_InitStructure);            //根据参数初始化LCD背光灯的GPIO端口

  //设置LCD的命令/数据标志位引脚PG0和片选引脚PG12
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_12;   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置I/O口速率为50MHz
  GPIO_Init(GPIOG, &GPIO_InitStructure);            //根据参数初始化LCD命令/数据标志位和片选的GPIO端口

  readWriteTiming.FSMC_AddressSetupTime = 0x01;     //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;      //地址保持时间（ADDHLD）模式A未用到  
  //数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
  readWriteTiming.FSMC_DataSetupTime = 0x0f;    
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;  //模式A 
    

  writeTiming.FSMC_AddressSetupTime = 0x00;             //地址建立时间（ADDSET）为1个HCLK  
  writeTiming.FSMC_AddressHoldTime = 0x00;              //地址保持时间
  writeTiming.FSMC_DataSetupTime = 0x03;                //数据保存时间为4个HCLK
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;        //总线翻转周期
  writeTiming.FSMC_CLKDivision = 0x00;                  //HCLK的分频系数
  writeTiming.FSMC_DataLatency = 0x00;                  //数据延迟时间
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;      //模式A 

 
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;                       //使用了FSMC的BANK1的区域4
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;     //禁止地址/数据线复用
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;                //存储器类型为SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;       //存储器数据宽度为16bit  
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;   //关闭突发模式访问
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low; //地址线的等待状态电平
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable; //关闭等待信号
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;                 //不使能环回模式
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//在等待状态前发送等待信号  
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;      //设置存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;             //关闭等待信号
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;          //不使用时序扩展模式，使用读写同时序 
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;             //不使用突发写模式
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;         //设置读写访问时序（关闭扩展模式情况下）
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;                 //设置写访问时序（打开扩展模式情况下）

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); //初始化FSMC配置

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE); //使能BANK1 
 
  DelayNms(50); //delay 50 ms 
  
  s_structLCDCB.id = LCD_ReadReg(0x0000); //读ID（9320/9325/9328/4531/4535等IC）   
  //读到ID不正确,新增s_structLCDCB.id == 0X9300判断，因为9341在未被复位的情况下会被读成9300
  if(s_structLCDCB.id< 0XFF || s_structLCDCB.id == 0XFFFF || s_structLCDCB.id == 0X9300)
  { 
    //尝试9341 ID的读取  
    LCD_WR_REG(0XD3); 
    s_structLCDCB.id = LCD_RD_DATA();       //dummy read  
    s_structLCDCB.id = LCD_RD_DATA();       //读到0X00
    s_structLCDCB.id = LCD_RD_DATA();       //读取93  
    s_structLCDCB.id <<= 8;                 //左移8位，空出低8位
    s_structLCDCB.id |= LCD_RD_DATA();      //读取41  
    
    if(s_structLCDCB.id != 0X9341)          //非9341,尝试是不是6804
    { 
      LCD_WR_REG(0XBF);    
      
      s_structLCDCB.id = LCD_RD_DATA();     //dummy read    
      s_structLCDCB.id = LCD_RD_DATA();     //读回0X01         
      s_structLCDCB.id = LCD_RD_DATA();     //读回0XD0           
      s_structLCDCB.id = LCD_RD_DATA();     //这里读回0X68 
      s_structLCDCB.id <<= 8;               //左移8位，空出低8位
      s_structLCDCB.id |= LCD_RD_DATA();    //这里读回0X04    
      if(s_structLCDCB.id != 0X6804)        //也不是6804,尝试看看是不是NT35310
      { 
        LCD_WR_REG(0XD4);     
        
        s_structLCDCB.id = LCD_RD_DATA();   //dummy read  
        s_structLCDCB.id = LCD_RD_DATA();   //读回0X01   
        s_structLCDCB.id = LCD_RD_DATA();   //读回0X53  
        s_structLCDCB.id <<= 8;             //左移8位，空出低8位 
        s_structLCDCB.id |= LCD_RD_DATA();  //这里读回0X10   
        if(s_structLCDCB.id != 0X5310)      //也不是NT35310,尝试看看是不是NT35510
        {
          LCD_WR_REG(0XDA00);  
          
          s_structLCDCB.id = LCD_RD_DATA(); //读回0X00   
          
          LCD_WR_REG(0XDB00);  
          
          s_structLCDCB.id = LCD_RD_DATA(); //读回0X80
          s_structLCDCB.id <<= 8;           //左移8位，空出低8位  
          
          LCD_WR_REG(0XDC00);  
          
          s_structLCDCB.id |= LCD_RD_DATA();//读回0X00   
          
          if(s_structLCDCB.id == 0x8000)
          {
            s_structLCDCB.id = 0x5510;      //NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
          }
          if(s_structLCDCB.id != 0X5510)      //也不是NT5510,尝试看看是不是SSD1963
          {
            LCD_WR_REG(0XA1);
            s_structLCDCB.id = LCD_RD_DATA();//dummy read  
            s_structLCDCB.id = LCD_RD_DATA();  //读回0X57
            s_structLCDCB.id <<= 8;           //左移8位，空出低8位   
            s_structLCDCB.id |= LCD_RD_DATA();  //读回0X61  
            if(s_structLCDCB.id == 0X5761)
            {
              s_structLCDCB.id = 0X1963;//SSD1963读回的ID是5761H,为方便区分,我们强制设置为1963  
            }  
          }
        }
      }
    }    
  } 
  printf("LCD ID:%x\r\n", s_structLCDCB.id); //在串口打印LCD的ID 
  
  //下面根据不同LCD的ID选择不同的初始化序列对LCD进行初始化。这些初始化序列由LCD供应商提供
  //给客户，我们直接使用这些序列即可，不需要深入研究
  if(s_structLCDCB.id == 0X9341)  //9341初始化
  {   
    LCD_WR_REG(0xCF);  
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0xC1); 
    LCD_WR_DATA(0X30); 
    LCD_WR_REG(0xED);  
    LCD_WR_DATA(0x64); 
    LCD_WR_DATA(0x03); 
    LCD_WR_DATA(0X12); 
    LCD_WR_DATA(0X81); 
    LCD_WR_REG(0xE8);  
    LCD_WR_DATA(0x85); 
    LCD_WR_DATA(0x10); 
    LCD_WR_DATA(0x7A); 
    LCD_WR_REG(0xCB);  
    LCD_WR_DATA(0x39); 
    LCD_WR_DATA(0x2C); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x34); 
    LCD_WR_DATA(0x02); 
    LCD_WR_REG(0xF7);  
    LCD_WR_DATA(0x20); 
    LCD_WR_REG(0xEA);  
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x00); 
    LCD_WR_REG(0xC0);   //Power control 
    LCD_WR_DATA(0x1B);  //VRH[5:0] 
    LCD_WR_REG(0xC1);   //Power control 
    LCD_WR_DATA(0x01);  //SAP[2:0];BT[3:0] 
    LCD_WR_REG(0xC5);   //VCM control 
    LCD_WR_DATA(0x30);  //3F
    LCD_WR_DATA(0x30);  //3C
    LCD_WR_REG(0xC7);   //VCM control2 
    LCD_WR_DATA(0XB7); 
    LCD_WR_REG(0x36);   // Memory Access Control 
    LCD_WR_DATA(0x48); 
    LCD_WR_REG(0x3A);   
    LCD_WR_DATA(0x55); 
    LCD_WR_REG(0xB1);   
    LCD_WR_DATA(0x00);   
    LCD_WR_DATA(0x1A); 
    LCD_WR_REG(0xB6);   // Display Function Control 
    LCD_WR_DATA(0x0A); 
    LCD_WR_DATA(0xA2); 
    LCD_WR_REG(0xF2);   // 3Gamma Function Disable 
    LCD_WR_DATA(0x00); 
    LCD_WR_REG(0x26);   //Gamma curve selected 
    LCD_WR_DATA(0x01); 
    LCD_WR_REG(0xE0);   //Set Gamma 
    LCD_WR_DATA(0x0F); 
    LCD_WR_DATA(0x2A); 
    LCD_WR_DATA(0x28); 
    LCD_WR_DATA(0x08); 
    LCD_WR_DATA(0x0E); 
    LCD_WR_DATA(0x08); 
    LCD_WR_DATA(0x54); 
    LCD_WR_DATA(0XA9); 
    LCD_WR_DATA(0x43); 
    LCD_WR_DATA(0x0A); 
    LCD_WR_DATA(0x0F); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x00);      
    LCD_WR_REG(0XE1);   //Set Gamma 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x15); 
    LCD_WR_DATA(0x17); 
    LCD_WR_DATA(0x07); 
    LCD_WR_DATA(0x11); 
    LCD_WR_DATA(0x06); 
    LCD_WR_DATA(0x2B); 
    LCD_WR_DATA(0x56); 
    LCD_WR_DATA(0x3C); 
    LCD_WR_DATA(0x05); 
    LCD_WR_DATA(0x10); 
    LCD_WR_DATA(0x0F); 
    LCD_WR_DATA(0x3F); 
    LCD_WR_DATA(0x3F); 
    LCD_WR_DATA(0x0F); 
    LCD_WR_REG(0x2B); 
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x3f);
    LCD_WR_REG(0x2A); 
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xef);   
    LCD_WR_REG(0x11);   //Exit Sleep
    DelayNms(120);
    LCD_WR_REG(0x29);   //display on  
  }
  else if(s_structLCDCB.id == 0x6804) //6804初始化
  {
    LCD_WR_REG(0X11);
    DelayNms(20);
    LCD_WR_REG(0XD0);   //VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
    LCD_WR_DATA(0X07); 
    LCD_WR_DATA(0X42); 
    LCD_WR_DATA(0X1D); 
    LCD_WR_REG(0XD1);   //VCOMH VCOM_AC amplitude setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X1a);
    LCD_WR_DATA(0X09); 
    LCD_WR_REG(0XD2);   //Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0X22);
    LCD_WR_REG(0XC0);   //REV SM GS 
    LCD_WR_DATA(0X10);
    LCD_WR_DATA(0X3B);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X02);
    LCD_WR_DATA(0X11);
    
    LCD_WR_REG(0XC5);   //Frame rate setting = 72HZ  when setting 0x03
    LCD_WR_DATA(0X03);
    
    LCD_WR_REG(0XC8);   //Gamma setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X25);
    LCD_WR_DATA(0X21);
    LCD_WR_DATA(0X05);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X0a);
    LCD_WR_DATA(0X65);
    LCD_WR_DATA(0X25);
    LCD_WR_DATA(0X77);
    LCD_WR_DATA(0X50);
    LCD_WR_DATA(0X0f);
    LCD_WR_DATA(0X00);    
              
    LCD_WR_REG(0XF8);
    LCD_WR_DATA(0X01);    

    LCD_WR_REG(0XFE);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X02);
    
    LCD_WR_REG(0X20);   //Exit invert mode

    LCD_WR_REG(0X36);
    LCD_WR_DATA(0X08);  //原来是a
    
    LCD_WR_REG(0X3A);
    LCD_WR_DATA(0X55);  //16位模式    
    LCD_WR_REG(0X2B);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0X3F);
    
    LCD_WR_REG(0X2A);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0XDF);
    DelayNms(120);
    LCD_WR_REG(0X29);    
  }
  else if(s_structLCDCB.id == 0x5310)//5310初始化
  { 
    LCD_WR_REG(0xED);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0xFE);

    LCD_WR_REG(0xEE);
    LCD_WR_DATA(0xDE);
    LCD_WR_DATA(0x21);

    LCD_WR_REG(0xF1);
    LCD_WR_DATA(0x01);
    LCD_WR_REG(0xDF);
    LCD_WR_DATA(0x10);

    //VCOMvoltage//
    LCD_WR_REG(0xC4);
    LCD_WR_DATA(0x8F);  //5f

    LCD_WR_REG(0xC6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE2);
    LCD_WR_DATA(0xE2);
    LCD_WR_DATA(0xE2);
    LCD_WR_REG(0xBF);
    LCD_WR_DATA(0xAA);

    LCD_WR_REG(0xB0);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x19);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x21);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0x80);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x96);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB4);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x96);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA1);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB5);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5E);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xAC);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDC);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x90);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xEB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDC);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xBA);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0x20);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xFF);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC2);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x39);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x23);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x23);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC4);
    LCD_WR_DATA(0x62);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x84);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x95);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE6);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC5);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x65);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x76);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC6);
    LCD_WR_DATA(0x20);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x16);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x21);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x46);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x52);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x7A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA(0x16);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x22);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x52);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x7A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE2);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x61);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x79);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x97);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xE3);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x62);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x78);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x97);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE4);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x74);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x84);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x93);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBE);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xE5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x74);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x84);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x93);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBE);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDC);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE6);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x76);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE7);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x76);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x67);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x67);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x87);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x23); 
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x87);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xAA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE9);
    LCD_WR_DATA(0xAA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x00);
    LCD_WR_DATA(0xAA);

    LCD_WR_REG(0xCF);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xF9);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);  //66

    LCD_WR_REG(0x11);
    DelayNms(100);
    LCD_WR_REG(0x29);
    LCD_WR_REG(0x35);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x51);
    LCD_WR_DATA(0xFF);
    LCD_WR_REG(0x53);
    LCD_WR_DATA(0x2C);
    LCD_WR_REG(0x55);
    LCD_WR_DATA(0x82);
    LCD_WR_REG(0x2c);
  }
  else if(s_structLCDCB.id == 0x5510)//5510初始化
  {
    LCD_WriteReg(0xF000,0x55);
    LCD_WriteReg(0xF001,0xAA);
    LCD_WriteReg(0xF002,0x52);
    LCD_WriteReg(0xF003,0x08);
    LCD_WriteReg(0xF004,0x01);
    //AVDD Set AVDD 5.2V
    LCD_WriteReg(0xB000,0x0D);
    LCD_WriteReg(0xB001,0x0D);
    LCD_WriteReg(0xB002,0x0D);
    //AVDD ratio
    LCD_WriteReg(0xB600,0x34);
    LCD_WriteReg(0xB601,0x34);
    LCD_WriteReg(0xB602,0x34);
    //AVEE -5.2V
    LCD_WriteReg(0xB100,0x0D);
    LCD_WriteReg(0xB101,0x0D);
    LCD_WriteReg(0xB102,0x0D);
    //AVEE ratio
    LCD_WriteReg(0xB700,0x34);
    LCD_WriteReg(0xB701,0x34);
    LCD_WriteReg(0xB702,0x34);
    //VCL -2.5V
    LCD_WriteReg(0xB200,0x00);
    LCD_WriteReg(0xB201,0x00);
    LCD_WriteReg(0xB202,0x00);
    //VCL ratio
    LCD_WriteReg(0xB800,0x24);
    LCD_WriteReg(0xB801,0x24);
    LCD_WriteReg(0xB802,0x24);
    //VGH 15V (Free pump)
    LCD_WriteReg(0xBF00,0x01);
    LCD_WriteReg(0xB300,0x0F);
    LCD_WriteReg(0xB301,0x0F);
    LCD_WriteReg(0xB302,0x0F);
    //VGH ratio
    LCD_WriteReg(0xB900,0x34);
    LCD_WriteReg(0xB901,0x34);
    LCD_WriteReg(0xB902,0x34);
    //VGL_REG -10V
    LCD_WriteReg(0xB500,0x08);
    LCD_WriteReg(0xB501,0x08);
    LCD_WriteReg(0xB502,0x08);
    LCD_WriteReg(0xC200,0x03);
    //VGLX ratio
    LCD_WriteReg(0xBA00,0x24);
    LCD_WriteReg(0xBA01,0x24);
    LCD_WriteReg(0xBA02,0x24);
    //VGMP/VGSP 4.5V/0V
    LCD_WriteReg(0xBC00,0x00);
    LCD_WriteReg(0xBC01,0x78);
    LCD_WriteReg(0xBC02,0x00);
    //VGMN/VGSN -4.5V/0V
    LCD_WriteReg(0xBD00,0x00);
    LCD_WriteReg(0xBD01,0x78);
    LCD_WriteReg(0xBD02,0x00);
    //VCOM
    LCD_WriteReg(0xBE00,0x00);
    LCD_WriteReg(0xBE01,0x64);
    //Gamma Setting
    LCD_WriteReg(0xD100,0x00);
    LCD_WriteReg(0xD101,0x33);
    LCD_WriteReg(0xD102,0x00);
    LCD_WriteReg(0xD103,0x34);
    LCD_WriteReg(0xD104,0x00);
    LCD_WriteReg(0xD105,0x3A);
    LCD_WriteReg(0xD106,0x00);
    LCD_WriteReg(0xD107,0x4A);
    LCD_WriteReg(0xD108,0x00);
    LCD_WriteReg(0xD109,0x5C);
    LCD_WriteReg(0xD10A,0x00);
    LCD_WriteReg(0xD10B,0x81);
    LCD_WriteReg(0xD10C,0x00);
    LCD_WriteReg(0xD10D,0xA6);
    LCD_WriteReg(0xD10E,0x00);
    LCD_WriteReg(0xD10F,0xE5);
    LCD_WriteReg(0xD110,0x01);
    LCD_WriteReg(0xD111,0x13);
    LCD_WriteReg(0xD112,0x01);
    LCD_WriteReg(0xD113,0x54);
    LCD_WriteReg(0xD114,0x01);
    LCD_WriteReg(0xD115,0x82);
    LCD_WriteReg(0xD116,0x01);
    LCD_WriteReg(0xD117,0xCA);
    LCD_WriteReg(0xD118,0x02);
    LCD_WriteReg(0xD119,0x00);
    LCD_WriteReg(0xD11A,0x02);
    LCD_WriteReg(0xD11B,0x01);
    LCD_WriteReg(0xD11C,0x02);
    LCD_WriteReg(0xD11D,0x34);
    LCD_WriteReg(0xD11E,0x02);
    LCD_WriteReg(0xD11F,0x67);
    LCD_WriteReg(0xD120,0x02);
    LCD_WriteReg(0xD121,0x84);
    LCD_WriteReg(0xD122,0x02);
    LCD_WriteReg(0xD123,0xA4);
    LCD_WriteReg(0xD124,0x02);
    LCD_WriteReg(0xD125,0xB7);
    LCD_WriteReg(0xD126,0x02);
    LCD_WriteReg(0xD127,0xCF);
    LCD_WriteReg(0xD128,0x02);
    LCD_WriteReg(0xD129,0xDE);
    LCD_WriteReg(0xD12A,0x02);
    LCD_WriteReg(0xD12B,0xF2);
    LCD_WriteReg(0xD12C,0x02);
    LCD_WriteReg(0xD12D,0xFE);
    LCD_WriteReg(0xD12E,0x03);
    LCD_WriteReg(0xD12F,0x10);
    LCD_WriteReg(0xD130,0x03);
    LCD_WriteReg(0xD131,0x33);
    LCD_WriteReg(0xD132,0x03);
    LCD_WriteReg(0xD133,0x6D);
    LCD_WriteReg(0xD200,0x00);
    LCD_WriteReg(0xD201,0x33);
    LCD_WriteReg(0xD202,0x00);
    LCD_WriteReg(0xD203,0x34);
    LCD_WriteReg(0xD204,0x00);
    LCD_WriteReg(0xD205,0x3A);
    LCD_WriteReg(0xD206,0x00);
    LCD_WriteReg(0xD207,0x4A);
    LCD_WriteReg(0xD208,0x00);
    LCD_WriteReg(0xD209,0x5C);
    LCD_WriteReg(0xD20A,0x00);

    LCD_WriteReg(0xD20B,0x81);
    LCD_WriteReg(0xD20C,0x00);
    LCD_WriteReg(0xD20D,0xA6);
    LCD_WriteReg(0xD20E,0x00);
    LCD_WriteReg(0xD20F,0xE5);
    LCD_WriteReg(0xD210,0x01);
    LCD_WriteReg(0xD211,0x13);
    LCD_WriteReg(0xD212,0x01);
    LCD_WriteReg(0xD213,0x54);
    LCD_WriteReg(0xD214,0x01);
    LCD_WriteReg(0xD215,0x82);
    LCD_WriteReg(0xD216,0x01);
    LCD_WriteReg(0xD217,0xCA);
    LCD_WriteReg(0xD218,0x02);
    LCD_WriteReg(0xD219,0x00);
    LCD_WriteReg(0xD21A,0x02);
    LCD_WriteReg(0xD21B,0x01);
    LCD_WriteReg(0xD21C,0x02);
    LCD_WriteReg(0xD21D,0x34);
    LCD_WriteReg(0xD21E,0x02);
    LCD_WriteReg(0xD21F,0x67);
    LCD_WriteReg(0xD220,0x02);
    LCD_WriteReg(0xD221,0x84);
    LCD_WriteReg(0xD222,0x02);
    LCD_WriteReg(0xD223,0xA4);
    LCD_WriteReg(0xD224,0x02);
    LCD_WriteReg(0xD225,0xB7);
    LCD_WriteReg(0xD226,0x02);
    LCD_WriteReg(0xD227,0xCF);
    LCD_WriteReg(0xD228,0x02);
    LCD_WriteReg(0xD229,0xDE);
    LCD_WriteReg(0xD22A,0x02);
    LCD_WriteReg(0xD22B,0xF2);
    LCD_WriteReg(0xD22C,0x02);
    LCD_WriteReg(0xD22D,0xFE);
    LCD_WriteReg(0xD22E,0x03);
    LCD_WriteReg(0xD22F,0x10);
    LCD_WriteReg(0xD230,0x03);
    LCD_WriteReg(0xD231,0x33);
    LCD_WriteReg(0xD232,0x03);
    LCD_WriteReg(0xD233,0x6D);
    LCD_WriteReg(0xD300,0x00);
    LCD_WriteReg(0xD301,0x33);
    LCD_WriteReg(0xD302,0x00);
    LCD_WriteReg(0xD303,0x34);
    LCD_WriteReg(0xD304,0x00);
    LCD_WriteReg(0xD305,0x3A);
    LCD_WriteReg(0xD306,0x00);
    LCD_WriteReg(0xD307,0x4A);
    LCD_WriteReg(0xD308,0x00);
    LCD_WriteReg(0xD309,0x5C);
    LCD_WriteReg(0xD30A,0x00);

    LCD_WriteReg(0xD30B,0x81);
    LCD_WriteReg(0xD30C,0x00);
    LCD_WriteReg(0xD30D,0xA6);
    LCD_WriteReg(0xD30E,0x00);
    LCD_WriteReg(0xD30F,0xE5);
    LCD_WriteReg(0xD310,0x01);
    LCD_WriteReg(0xD311,0x13);
    LCD_WriteReg(0xD312,0x01);
    LCD_WriteReg(0xD313,0x54);
    LCD_WriteReg(0xD314,0x01);
    LCD_WriteReg(0xD315,0x82);
    LCD_WriteReg(0xD316,0x01);
    LCD_WriteReg(0xD317,0xCA);
    LCD_WriteReg(0xD318,0x02);
    LCD_WriteReg(0xD319,0x00);
    LCD_WriteReg(0xD31A,0x02);
    LCD_WriteReg(0xD31B,0x01);
    LCD_WriteReg(0xD31C,0x02);
    LCD_WriteReg(0xD31D,0x34);
    LCD_WriteReg(0xD31E,0x02);
    LCD_WriteReg(0xD31F,0x67);
    LCD_WriteReg(0xD320,0x02);
    LCD_WriteReg(0xD321,0x84);
    LCD_WriteReg(0xD322,0x02);
    LCD_WriteReg(0xD323,0xA4);
    LCD_WriteReg(0xD324,0x02);
    LCD_WriteReg(0xD325,0xB7);
    LCD_WriteReg(0xD326,0x02);
    LCD_WriteReg(0xD327,0xCF);
    LCD_WriteReg(0xD328,0x02);
    LCD_WriteReg(0xD329,0xDE);
    LCD_WriteReg(0xD32A,0x02);
    LCD_WriteReg(0xD32B,0xF2);
    LCD_WriteReg(0xD32C,0x02);
    LCD_WriteReg(0xD32D,0xFE);
    LCD_WriteReg(0xD32E,0x03);
    LCD_WriteReg(0xD32F,0x10);
    LCD_WriteReg(0xD330,0x03);
    LCD_WriteReg(0xD331,0x33);
    LCD_WriteReg(0xD332,0x03);
    LCD_WriteReg(0xD333,0x6D);
    LCD_WriteReg(0xD400,0x00);
    LCD_WriteReg(0xD401,0x33);
    LCD_WriteReg(0xD402,0x00);
    LCD_WriteReg(0xD403,0x34);
    LCD_WriteReg(0xD404,0x00);
    LCD_WriteReg(0xD405,0x3A);
    LCD_WriteReg(0xD406,0x00);
    LCD_WriteReg(0xD407,0x4A);
    LCD_WriteReg(0xD408,0x00);
    LCD_WriteReg(0xD409,0x5C);
    LCD_WriteReg(0xD40A,0x00);
    LCD_WriteReg(0xD40B,0x81);

    LCD_WriteReg(0xD40C,0x00);
    LCD_WriteReg(0xD40D,0xA6);
    LCD_WriteReg(0xD40E,0x00);
    LCD_WriteReg(0xD40F,0xE5);
    LCD_WriteReg(0xD410,0x01);
    LCD_WriteReg(0xD411,0x13);
    LCD_WriteReg(0xD412,0x01);
    LCD_WriteReg(0xD413,0x54);
    LCD_WriteReg(0xD414,0x01);
    LCD_WriteReg(0xD415,0x82);
    LCD_WriteReg(0xD416,0x01);
    LCD_WriteReg(0xD417,0xCA);
    LCD_WriteReg(0xD418,0x02);
    LCD_WriteReg(0xD419,0x00);
    LCD_WriteReg(0xD41A,0x02);
    LCD_WriteReg(0xD41B,0x01);
    LCD_WriteReg(0xD41C,0x02);
    LCD_WriteReg(0xD41D,0x34);
    LCD_WriteReg(0xD41E,0x02);
    LCD_WriteReg(0xD41F,0x67);
    LCD_WriteReg(0xD420,0x02);
    LCD_WriteReg(0xD421,0x84);
    LCD_WriteReg(0xD422,0x02);
    LCD_WriteReg(0xD423,0xA4);
    LCD_WriteReg(0xD424,0x02);
    LCD_WriteReg(0xD425,0xB7);
    LCD_WriteReg(0xD426,0x02);
    LCD_WriteReg(0xD427,0xCF);
    LCD_WriteReg(0xD428,0x02);
    LCD_WriteReg(0xD429,0xDE);
    LCD_WriteReg(0xD42A,0x02);
    LCD_WriteReg(0xD42B,0xF2);
    LCD_WriteReg(0xD42C,0x02);
    LCD_WriteReg(0xD42D,0xFE);
    LCD_WriteReg(0xD42E,0x03);
    LCD_WriteReg(0xD42F,0x10);
    LCD_WriteReg(0xD430,0x03);
    LCD_WriteReg(0xD431,0x33);
    LCD_WriteReg(0xD432,0x03);
    LCD_WriteReg(0xD433,0x6D);
    LCD_WriteReg(0xD500,0x00);
    LCD_WriteReg(0xD501,0x33);
    LCD_WriteReg(0xD502,0x00);
    LCD_WriteReg(0xD503,0x34);
    LCD_WriteReg(0xD504,0x00);
    LCD_WriteReg(0xD505,0x3A);
    LCD_WriteReg(0xD506,0x00);
    LCD_WriteReg(0xD507,0x4A);
    LCD_WriteReg(0xD508,0x00);
    LCD_WriteReg(0xD509,0x5C);
    LCD_WriteReg(0xD50A,0x00);
    LCD_WriteReg(0xD50B,0x81);

    LCD_WriteReg(0xD50C,0x00);
    LCD_WriteReg(0xD50D,0xA6);
    LCD_WriteReg(0xD50E,0x00);
    LCD_WriteReg(0xD50F,0xE5);
    LCD_WriteReg(0xD510,0x01);
    LCD_WriteReg(0xD511,0x13);
    LCD_WriteReg(0xD512,0x01);
    LCD_WriteReg(0xD513,0x54);
    LCD_WriteReg(0xD514,0x01);
    LCD_WriteReg(0xD515,0x82);
    LCD_WriteReg(0xD516,0x01);
    LCD_WriteReg(0xD517,0xCA);
    LCD_WriteReg(0xD518,0x02);
    LCD_WriteReg(0xD519,0x00);
    LCD_WriteReg(0xD51A,0x02);
    LCD_WriteReg(0xD51B,0x01);
    LCD_WriteReg(0xD51C,0x02);
    LCD_WriteReg(0xD51D,0x34);
    LCD_WriteReg(0xD51E,0x02);
    LCD_WriteReg(0xD51F,0x67);
    LCD_WriteReg(0xD520,0x02);
    LCD_WriteReg(0xD521,0x84);
    LCD_WriteReg(0xD522,0x02);
    LCD_WriteReg(0xD523,0xA4);
    LCD_WriteReg(0xD524,0x02);
    LCD_WriteReg(0xD525,0xB7);
    LCD_WriteReg(0xD526,0x02);
    LCD_WriteReg(0xD527,0xCF);
    LCD_WriteReg(0xD528,0x02);
    LCD_WriteReg(0xD529,0xDE);
    LCD_WriteReg(0xD52A,0x02);
    LCD_WriteReg(0xD52B,0xF2);
    LCD_WriteReg(0xD52C,0x02);
    LCD_WriteReg(0xD52D,0xFE);
    LCD_WriteReg(0xD52E,0x03);
    LCD_WriteReg(0xD52F,0x10);
    LCD_WriteReg(0xD530,0x03);
    LCD_WriteReg(0xD531,0x33);
    LCD_WriteReg(0xD532,0x03);
    LCD_WriteReg(0xD533,0x6D);
    LCD_WriteReg(0xD600,0x00);
    LCD_WriteReg(0xD601,0x33);
    LCD_WriteReg(0xD602,0x00);
    LCD_WriteReg(0xD603,0x34);
    LCD_WriteReg(0xD604,0x00);
    LCD_WriteReg(0xD605,0x3A);
    LCD_WriteReg(0xD606,0x00);
    LCD_WriteReg(0xD607,0x4A);
    LCD_WriteReg(0xD608,0x00);
    LCD_WriteReg(0xD609,0x5C);
    LCD_WriteReg(0xD60A,0x00);
    LCD_WriteReg(0xD60B,0x81);

    LCD_WriteReg(0xD60C,0x00);
    LCD_WriteReg(0xD60D,0xA6);
    LCD_WriteReg(0xD60E,0x00);
    LCD_WriteReg(0xD60F,0xE5);
    LCD_WriteReg(0xD610,0x01);
    LCD_WriteReg(0xD611,0x13);
    LCD_WriteReg(0xD612,0x01);
    LCD_WriteReg(0xD613,0x54);
    LCD_WriteReg(0xD614,0x01);
    LCD_WriteReg(0xD615,0x82);
    LCD_WriteReg(0xD616,0x01);
    LCD_WriteReg(0xD617,0xCA);
    LCD_WriteReg(0xD618,0x02);
    LCD_WriteReg(0xD619,0x00);
    LCD_WriteReg(0xD61A,0x02);
    LCD_WriteReg(0xD61B,0x01);
    LCD_WriteReg(0xD61C,0x02);
    LCD_WriteReg(0xD61D,0x34);
    LCD_WriteReg(0xD61E,0x02);
    LCD_WriteReg(0xD61F,0x67);
    LCD_WriteReg(0xD620,0x02);
    LCD_WriteReg(0xD621,0x84);
    LCD_WriteReg(0xD622,0x02);
    LCD_WriteReg(0xD623,0xA4);
    LCD_WriteReg(0xD624,0x02);
    LCD_WriteReg(0xD625,0xB7);
    LCD_WriteReg(0xD626,0x02);
    LCD_WriteReg(0xD627,0xCF);
    LCD_WriteReg(0xD628,0x02);
    LCD_WriteReg(0xD629,0xDE);
    LCD_WriteReg(0xD62A,0x02);
    LCD_WriteReg(0xD62B,0xF2);
    LCD_WriteReg(0xD62C,0x02);
    LCD_WriteReg(0xD62D,0xFE);
    LCD_WriteReg(0xD62E,0x03);
    LCD_WriteReg(0xD62F,0x10);
    LCD_WriteReg(0xD630,0x03);
    LCD_WriteReg(0xD631,0x33);
    LCD_WriteReg(0xD632,0x03);
    LCD_WriteReg(0xD633,0x6D);
    //LV2 Page 0 enable
    LCD_WriteReg(0xF000,0x55);
    LCD_WriteReg(0xF001,0xAA);
    LCD_WriteReg(0xF002,0x52);
    LCD_WriteReg(0xF003,0x08);
    LCD_WriteReg(0xF004,0x00);
    //Display control
    LCD_WriteReg(0xB100, 0xCC);
    LCD_WriteReg(0xB101, 0x00);
    //Source hold time
    LCD_WriteReg(0xB600,0x05);
    //Gate EQ control
    LCD_WriteReg(0xB700,0x70);
    LCD_WriteReg(0xB701,0x70);
    //Source EQ control (Mode 2)
    LCD_WriteReg(0xB800,0x01);
    LCD_WriteReg(0xB801,0x03);
    LCD_WriteReg(0xB802,0x03);
    LCD_WriteReg(0xB803,0x03);
    //Inversion mode (2-dot)
    LCD_WriteReg(0xBC00,0x02);
    LCD_WriteReg(0xBC01,0x00);
    LCD_WriteReg(0xBC02,0x00);
    //Timing control 4H w/ 4-delay
    LCD_WriteReg(0xC900,0xD0);
    LCD_WriteReg(0xC901,0x02);
    LCD_WriteReg(0xC902,0x50);
    LCD_WriteReg(0xC903,0x50);
    LCD_WriteReg(0xC904,0x50);
    LCD_WriteReg(0x3500,0x00);
    LCD_WriteReg(0x3A00,0x55);  //16-bit/pixel
    LCD_WR_REG(0x1100);
    DelayNus(120);
    LCD_WR_REG(0x2900);
  }
  else if(s_structLCDCB.id == 0x9325)//9325初始化
  {
    LCD_WriteReg(0x00E5,0x78F0); 
    LCD_WriteReg(0x0001,0x0100); 
    LCD_WriteReg(0x0002,0x0700); 
    LCD_WriteReg(0x0003,0x1030); 
    LCD_WriteReg(0x0004,0x0000); 
    LCD_WriteReg(0x0008,0x0202);  
    LCD_WriteReg(0x0009,0x0000);
    LCD_WriteReg(0x000A,0x0000); 
    LCD_WriteReg(0x000C,0x0000); 
    LCD_WriteReg(0x000D,0x0000);
    LCD_WriteReg(0x000F,0x0000);
    //power on sequence VGHVGL
    LCD_WriteReg(0x0010,0x0000);   
    LCD_WriteReg(0x0011,0x0007);  
    LCD_WriteReg(0x0012,0x0000);  
    LCD_WriteReg(0x0013,0x0000); 
    LCD_WriteReg(0x0007,0x0000); 
    //vgh 
    LCD_WriteReg(0x0010,0x1690);   
    LCD_WriteReg(0x0011,0x0227);
    //delayms(100);
    //vregiout 
    LCD_WriteReg(0x0012,0x009D); //0x001b
    //delayms(100); 
    //vom amplitude
    LCD_WriteReg(0x0013,0x1900);
    //delayms(100); 
    //vom H
    LCD_WriteReg(0x0029,0x0025); 
    LCD_WriteReg(0x002B,0x000D); 
    //gamma
    LCD_WriteReg(0x0030,0x0007);
    LCD_WriteReg(0x0031,0x0303);
    LCD_WriteReg(0x0032,0x0003);// 0006
    LCD_WriteReg(0x0035,0x0206);
    LCD_WriteReg(0x0036,0x0008);
    LCD_WriteReg(0x0037,0x0406); 
    LCD_WriteReg(0x0038,0x0304);//0200
    LCD_WriteReg(0x0039,0x0007); 
    LCD_WriteReg(0x003C,0x0602);// 0504
    LCD_WriteReg(0x003D,0x0008); 
    //ram
    LCD_WriteReg(0x0050,0x0000); 
    LCD_WriteReg(0x0051,0x00EF);
    LCD_WriteReg(0x0052,0x0000); 
    LCD_WriteReg(0x0053,0x013F);  
    LCD_WriteReg(0x0060,0xA700); 
    LCD_WriteReg(0x0061,0x0001); 
    LCD_WriteReg(0x006A,0x0000); 
    //
    LCD_WriteReg(0x0080,0x0000); 
    LCD_WriteReg(0x0081,0x0000); 
    LCD_WriteReg(0x0082,0x0000); 
    LCD_WriteReg(0x0083,0x0000); 
    LCD_WriteReg(0x0084,0x0000); 
    LCD_WriteReg(0x0085,0x0000); 
    //
    LCD_WriteReg(0x0090,0x0010); 
    LCD_WriteReg(0x0092,0x0600); 
    
    LCD_WriteReg(0x0007,0x0133);
    LCD_WriteReg(0x00,0x0022);//
  }else if(s_structLCDCB.id == 0x9328)//9328初始化
  {
    LCD_WriteReg(0x00EC,0x108F);  // internal timeing      
    LCD_WriteReg(0x00EF,0x1234);  // ADD        
    //LCD_WriteReg(0x00e7,0x0010);      
    //LCD_WriteReg(0x0000,0x0001);//开启内部时钟
    LCD_WriteReg(0x0001,0x0100);     
    LCD_WriteReg(0x0002,0x0700);  //电源开启                    
    //LCD_WriteReg(0x0003,(1<<3) | (1<<4) );   //65K  RGB
    //DRIVE TABLE(寄存器 03H)
    //BIT3=AM BIT4:5=ID0:1
    //AM ID0 ID1   FUNCATION
    // 0  0   0     R->L D->U
    // 1  0   0     D->U  R->L
    // 0  1   0     L->R D->U
    // 1  1   0    D->U  L->R
    // 0  0   1     R->L U->D
    // 1  0   1    U->D  R->L
    // 0  1   1    L->R U->D 正常就用这个.
    // 1  1   1     U->D  L->R
    LCD_WriteReg(0x0003,(1<<12) | (3<<4) | (0<<3) );//65K    
    LCD_WriteReg(0x0004,0x0000);                                   
    LCD_WriteReg(0x0008,0x0202);             
    LCD_WriteReg(0x0009,0x0000);         
    LCD_WriteReg(0x000a,0x0000);//display setting         
    LCD_WriteReg(0x000c,0x0001);//display setting          
    LCD_WriteReg(0x000d,0x0000);//0f3c          
    LCD_WriteReg(0x000f,0x0000);
    //电源配置
    LCD_WriteReg(0x0010,0x0000);   
    LCD_WriteReg(0x0011,0x0007);
    LCD_WriteReg(0x0012,0x0000);                                                                 
    LCD_WriteReg(0x0013,0x0000);                 
    LCD_WriteReg(0x0007,0x0001);                 
    DelayNms(50); 
    LCD_WriteReg(0x0010,0x1490);   
    LCD_WriteReg(0x0011,0x0227);
    DelayNms(50); 
    LCD_WriteReg(0x0012,0x008A);                  
    DelayNms(50); 
    LCD_WriteReg(0x0013,0x1a00);   
    LCD_WriteReg(0x0029,0x0006);
    LCD_WriteReg(0x002b,0x000d);
    DelayNms(50); 
    LCD_WriteReg(0x0020,0x0000);                                                            
    LCD_WriteReg(0x0021,0x0000);           
    DelayNms(50); 
    //伽马校正
    LCD_WriteReg(0x0030,0x0000); 
    LCD_WriteReg(0x0031,0x0604);   
    LCD_WriteReg(0x0032,0x0305);
    LCD_WriteReg(0x0035,0x0000);
    LCD_WriteReg(0x0036,0x0C09); 
    LCD_WriteReg(0x0037,0x0204);
    LCD_WriteReg(0x0038,0x0301);        
    LCD_WriteReg(0x0039,0x0707);     
    LCD_WriteReg(0x003c,0x0000);
    LCD_WriteReg(0x003d,0x0a0a);
    DelayNms(50); 
    LCD_WriteReg(0x0050,0x0000); //水平GRAM起始位置 
    LCD_WriteReg(0x0051,0x00ef); //水平GRAM终止位置                    
    LCD_WriteReg(0x0052,0x0000); //垂直GRAM起始位置                    
    LCD_WriteReg(0x0053,0x013f); //垂直GRAM终止位置  
 
    LCD_WriteReg(0x0060,0xa700);        
    LCD_WriteReg(0x0061,0x0001); 
    LCD_WriteReg(0x006a,0x0000);
    LCD_WriteReg(0x0080,0x0000);
    LCD_WriteReg(0x0081,0x0000);
    LCD_WriteReg(0x0082,0x0000);
    LCD_WriteReg(0x0083,0x0000);
    LCD_WriteReg(0x0084,0x0000);
    LCD_WriteReg(0x0085,0x0000);
      
    LCD_WriteReg(0x0090,0x0010);     
    LCD_WriteReg(0x0092,0x0600);  
    //开启显示设置    
    LCD_WriteReg(0x0007,0x0133); 
  }
  else if(s_structLCDCB.id == 0x9320)//9320初始化
  {
    LCD_WriteReg(0x00,0x0000);
    LCD_WriteReg(0x01,0x0100);  //Driver Output Contral.
    LCD_WriteReg(0x02,0x0700);  //LCD Driver Waveform Contral.
    LCD_WriteReg(0x03,0x1030);//Entry Mode Set.
    //LCD_WriteReg(0x03,0x1018);  //Entry Mode Set.
  
    LCD_WriteReg(0x04,0x0000);  //Scalling Contral.
    LCD_WriteReg(0x08,0x0202);  //Display Contral 2.(0x0207)
    LCD_WriteReg(0x09,0x0000);  //Display Contral 3.(0x0000)
    LCD_WriteReg(0x0a,0x0000);  //Frame Cycle Contal.(0x0000)
    LCD_WriteReg(0x0c,(1<<0));  //Extern Display Interface Contral 1.(0x0000)
    LCD_WriteReg(0x0d,0x0000);  //Frame Maker Position.
    LCD_WriteReg(0x0f,0x0000);  //Extern Display Interface Contral 2.      
    DelayNms(50); 
    LCD_WriteReg(0x07,0x0101);  //Display Contral.
    DelayNms(50);                   
    LCD_WriteReg(0x10,(1<<12) | (0<<8) | (1<<7) | (1<<6) | (0<<4));  //Power Control 1.(0x16b0)
    LCD_WriteReg(0x11,0x0007);                //Power Control 2.(0x0001)
    LCD_WriteReg(0x12,(1<<8) | (1<<4) | (0<<0));        //Power Control 3.(0x0138)
    LCD_WriteReg(0x13,0x0b00);                //Power Control 4.
    LCD_WriteReg(0x29,0x0000);                //Power Control 7.
  
    LCD_WriteReg(0x2b,(1<<14) | (1<<4));      
    LCD_WriteReg(0x50,0);  //Set X Star
    //水平GRAM终止位置Set X End.
    LCD_WriteReg(0x51,239);  //Set Y Star
    LCD_WriteReg(0x52,0);  //Set Y End.t.
    LCD_WriteReg(0x53,319);  //
  
    LCD_WriteReg(0x60,0x2700);  //Driver Output Control.
    LCD_WriteReg(0x61,0x0001);  //Driver Output Control.
    LCD_WriteReg(0x6a,0x0000);  //Vertical Srcoll Control.
  
    LCD_WriteReg(0x80,0x0000);  //Display Position? Partial Display 1.
    LCD_WriteReg(0x81,0x0000);  //RAM Address Start? Partial Display 1.
    LCD_WriteReg(0x82,0x0000);  //RAM Address End-Partial Display 1.
    LCD_WriteReg(0x83,0x0000);  //Displsy Position? Partial Display 2.
    LCD_WriteReg(0x84,0x0000);  //RAM Address Start? Partial Display 2.
    LCD_WriteReg(0x85,0x0000);  //RAM Address End? Partial Display 2.
  
    LCD_WriteReg(0x90,(0<<7) | (16<<0));  //Frame Cycle Contral.(0x0013)
    LCD_WriteReg(0x92,0x0000);  //Panel Interface Contral 2.(0x0000)
    LCD_WriteReg(0x93,0x0001);  //Panel Interface Contral 3.
    LCD_WriteReg(0x95,0x0110);  //Frame Cycle Contral.(0x0110)
    LCD_WriteReg(0x97,(0<<8));  //
    LCD_WriteReg(0x98,0x0000);  //Frame Cycle Contral.     
    LCD_WriteReg(0x07,0x0173);  //(0x0173)
  }
  else if(s_structLCDCB.id == 0X9331)//9331初始化    
  {
    LCD_WriteReg(0x00E7, 0x1014);
    LCD_WriteReg(0x0001, 0x0100); // set SS and SM bit
    LCD_WriteReg(0x0002, 0x0200); // set 1 line inversion
    LCD_WriteReg(0x0003,(1<<12) | (3<<4) | (1<<3));//65K    
    //LCD_WriteReg(0x0003, 0x1030); // set GRAM write direction and BGR=1.
    LCD_WriteReg(0x0008, 0x0202); // set the back porch and front porch
    LCD_WriteReg(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
    LCD_WriteReg(0x000A, 0x0000); // FMARK function
    LCD_WriteReg(0x000C, 0x0000); // RGB interface setting
    LCD_WriteReg(0x000D, 0x0000); // Frame marker Position
    LCD_WriteReg(0x000F, 0x0000); // RGB interface polarity
    //*************Power On sequence ****************//
    LCD_WriteReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WriteReg(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
    LCD_WriteReg(0x0012, 0x0000); // VREG1OUT voltage
    LCD_WriteReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
    DelayNms(200); // Dis-charge capacitor power voltage
    LCD_WriteReg(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WriteReg(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
    DelayNms(50); // Delay 50ms
    LCD_WriteReg(0x0012, 0x000C); // Internal reference voltage= Vci;
    DelayNms(50); // Delay 50ms
    LCD_WriteReg(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
    LCD_WriteReg(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
    LCD_WriteReg(0x002B, 0x000B); // Set Frame Rate
    DelayNms(50); // Delay 50ms
    LCD_WriteReg(0x0020, 0x0000); // GRAM horizontal Address
    LCD_WriteReg(0x0021, 0x013f); // GRAM Vertical Address
    // ----------- Adjust the Gamma Curve ----------//
    LCD_WriteReg(0x0030, 0x0000);
    LCD_WriteReg(0x0031, 0x0106);
    LCD_WriteReg(0x0032, 0x0000);
    LCD_WriteReg(0x0035, 0x0204);
    LCD_WriteReg(0x0036, 0x160A);
    LCD_WriteReg(0x0037, 0x0707);
    LCD_WriteReg(0x0038, 0x0106);
    LCD_WriteReg(0x0039, 0x0707);
    LCD_WriteReg(0x003C, 0x0402);
    LCD_WriteReg(0x003D, 0x0C0F);
    //------------------ Set GRAM area ---------------//
    LCD_WriteReg(0x0050, 0x0000); // Horizontal GRAM Start Address
    LCD_WriteReg(0x0051, 0x00EF); // Horizontal GRAM End Address
    LCD_WriteReg(0x0052, 0x0000); // Vertical GRAM Start Address
    LCD_WriteReg(0x0053, 0x013F); // Vertical GRAM Start Address
    LCD_WriteReg(0x0060, 0x2700); // Gate Scan Line
    LCD_WriteReg(0x0061, 0x0001); // NDL,VLE, REV 
    LCD_WriteReg(0x006A, 0x0000); // set scrolling line
    //-------------- Partial Display Control ---------//
    LCD_WriteReg(0x0080, 0x0000);
    LCD_WriteReg(0x0081, 0x0000);
    LCD_WriteReg(0x0082, 0x0000);
    LCD_WriteReg(0x0083, 0x0000);
    LCD_WriteReg(0x0084, 0x0000);
    LCD_WriteReg(0x0085, 0x0000);
    //-------------- Panel Control -------------------//
    LCD_WriteReg(0x0090, 0x0010);
    LCD_WriteReg(0x0092, 0x0600);
    LCD_WriteReg(0x0007, 0x0133); // 262K color and display ON
  }
  else if(s_structLCDCB.id == 0x5408)//5408初始化
  {
    LCD_WriteReg(0x01,0x0100);                  
    LCD_WriteReg(0x02,0x0700);//LCD Driving Waveform Contral 
    LCD_WriteReg(0x03,0x1030);//Entry Mode设置      
    //指针从左至右自上而下的自动增模式
    //Normal Mode(Window Mode disable)
    //RGB格式
    //16位数据2次传输的8总线设置
    LCD_WriteReg(0x04,0x0000); //Scalling Control register     
    LCD_WriteReg(0x08,0x0207); //Display Control 2 
    LCD_WriteReg(0x09,0x0000); //Display Control 3   
    LCD_WriteReg(0x0A,0x0000); //Frame Cycle Control   
    LCD_WriteReg(0x0C,0x0000); //External Display Interface Control 1 
    LCD_WriteReg(0x0D,0x0000); //Frame Maker Position     
    LCD_WriteReg(0x0F,0x0000); //External Display Interface Control 2 
    DelayNms(20);
    //TFT 液晶彩色图像显示方法14
    LCD_WriteReg(0x10,0x16B0); //0x14B0 //Power Control 1
    LCD_WriteReg(0x11,0x0001); //0x0007 //Power Control 2
    LCD_WriteReg(0x17,0x0001); //0x0000 //Power Control 3
    LCD_WriteReg(0x12,0x0138); //0x013B //Power Control 4
    LCD_WriteReg(0x13,0x0800); //0x0800 //Power Control 5
    LCD_WriteReg(0x29,0x0009); //NVM read data 2
    LCD_WriteReg(0x2a,0x0009); //NVM read data 3
    LCD_WriteReg(0xa4,0x0000);   
    LCD_WriteReg(0x50,0x0000); //设置操作窗口的X轴开始列
    LCD_WriteReg(0x51,0x00EF); //设置操作窗口的X轴结束列
    LCD_WriteReg(0x52,0x0000); //设置操作窗口的Y轴开始行
    LCD_WriteReg(0x53,0x013F); //设置操作窗口的Y轴结束行
    LCD_WriteReg(0x60,0x2700); //Driver Output Control
    //设置屏幕的点数以及扫描的起始行
    LCD_WriteReg(0x61,0x0001); //Driver Output Control
    LCD_WriteReg(0x6A,0x0000); //Vertical Scroll Control
    LCD_WriteReg(0x80,0x0000); //Display Position – Partial Display 1
    LCD_WriteReg(0x81,0x0000); //RAM Address Start – Partial Display 1
    LCD_WriteReg(0x82,0x0000); //RAM address End - Partial Display 1
    LCD_WriteReg(0x83,0x0000); //Display Position – Partial Display 2
    LCD_WriteReg(0x84,0x0000); //RAM Address Start – Partial Display 2
    LCD_WriteReg(0x85,0x0000); //RAM address End – Partail Display2
    LCD_WriteReg(0x90,0x0013); //Frame Cycle Control
    LCD_WriteReg(0x92,0x0000);  //Panel Interface Control 2
    LCD_WriteReg(0x93,0x0003); //Panel Interface control 3
    LCD_WriteReg(0x95,0x0110);  //Frame Cycle Control
    LCD_WriteReg(0x07,0x0173);     
    DelayNms(50);
  }  
  else if(s_structLCDCB.id == 0x1505)//1505初始化
  {
    // second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
    LCD_WriteReg(0x0007,0x0000);
    DelayNms(50); 
    LCD_WriteReg(0x0012,0x011C);//0x011A   why need to set several times?
    LCD_WriteReg(0x00A4,0x0001);//NVM   
    LCD_WriteReg(0x0008,0x000F);
    LCD_WriteReg(0x000A,0x0008);
    LCD_WriteReg(0x000D,0x0008);      
    //伽马校正
    LCD_WriteReg(0x0030,0x0707);
    LCD_WriteReg(0x0031,0x0007); //0x0707
    LCD_WriteReg(0x0032,0x0603); 
    LCD_WriteReg(0x0033,0x0700); 
    LCD_WriteReg(0x0034,0x0202); 
    LCD_WriteReg(0x0035,0x0002); //?0x0606
    LCD_WriteReg(0x0036,0x1F0F);
    LCD_WriteReg(0x0037,0x0707); //0x0f0f  0x0105
    LCD_WriteReg(0x0038,0x0000); 
    LCD_WriteReg(0x0039,0x0000); 
    LCD_WriteReg(0x003A,0x0707); 
    LCD_WriteReg(0x003B,0x0000); //0x0303
    LCD_WriteReg(0x003C,0x0007); //?0x0707
    LCD_WriteReg(0x003D,0x0000); //0x1313//0x1f08
    DelayNms(50); 
    LCD_WriteReg(0x0007,0x0001);
    LCD_WriteReg(0x0017,0x0001);//开启电源
    DelayNms(50); 
    //电源配置
    LCD_WriteReg(0x0010,0x17A0); 
    LCD_WriteReg(0x0011,0x0217);//reference voltage VC[2:0]   Vciout = 1.00*Vcivl
    LCD_WriteReg(0x0012,0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
    LCD_WriteReg(0x0013,0x0F00);//VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
    LCD_WriteReg(0x002A,0x0000);  
    LCD_WriteReg(0x0029,0x000A);//0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
    LCD_WriteReg(0x0012,0x013E);// 0x013C  power supply on
    //Coordinates Control//
    LCD_WriteReg(0x0050,0x0000);//0x0e00
    LCD_WriteReg(0x0051,0x00EF); 
    LCD_WriteReg(0x0052,0x0000); 
    LCD_WriteReg(0x0053,0x013F); 
    //Pannel Image Control//
    LCD_WriteReg(0x0060,0x2700); 
    LCD_WriteReg(0x0061,0x0001); 
    LCD_WriteReg(0x006A,0x0000); 
    LCD_WriteReg(0x0080,0x0000); 
    //Partial Image Control//
    LCD_WriteReg(0x0081,0x0000); 
    LCD_WriteReg(0x0082,0x0000); 
    LCD_WriteReg(0x0083,0x0000); 
    LCD_WriteReg(0x0084,0x0000); 
    LCD_WriteReg(0x0085,0x0000); 
    //Panel Interface Control//
    LCD_WriteReg(0x0090,0x0013);//0x0010 frenqucy
    LCD_WriteReg(0x0092,0x0300); 
    LCD_WriteReg(0x0093,0x0005); 
    LCD_WriteReg(0x0095,0x0000); 
    LCD_WriteReg(0x0097,0x0000); 
    LCD_WriteReg(0x0098,0x0000); 
  
    LCD_WriteReg(0x0001,0x0100); 
    LCD_WriteReg(0x0002,0x0700); 
    LCD_WriteReg(0x0003,0x1038);//扫描方向 上->下  左->右 
    LCD_WriteReg(0x0004,0x0000); 
    LCD_WriteReg(0x000C,0x0000); 
    LCD_WriteReg(0x000F,0x0000); 
    LCD_WriteReg(0x0020,0x0000); 
    LCD_WriteReg(0x0021,0x0000); 
    LCD_WriteReg(0x0007,0x0021); 
    DelayNms(20);
    LCD_WriteReg(0x0007,0x0061); 
    DelayNms(20);
    LCD_WriteReg(0x0007,0x0173); 
    DelayNms(20);
  }
  else if(s_structLCDCB.id == 0xB505)//B505初始化
  {
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    
    LCD_WriteReg(0x00a4,0x0001);
    DelayNms(20);      
    LCD_WriteReg(0x0060,0x2700);
    LCD_WriteReg(0x0008,0x0202);
    
    LCD_WriteReg(0x0030,0x0214);
    LCD_WriteReg(0x0031,0x3715);
    LCD_WriteReg(0x0032,0x0604);
    LCD_WriteReg(0x0033,0x0e16);
    LCD_WriteReg(0x0034,0x2211);
    LCD_WriteReg(0x0035,0x1500);
    LCD_WriteReg(0x0036,0x8507);
    LCD_WriteReg(0x0037,0x1407);
    LCD_WriteReg(0x0038,0x1403);
    LCD_WriteReg(0x0039,0x0020);
    
    LCD_WriteReg(0x0090,0x001a);
    LCD_WriteReg(0x0010,0x0000);
    LCD_WriteReg(0x0011,0x0007);
    LCD_WriteReg(0x0012,0x0000);
    LCD_WriteReg(0x0013,0x0000);
    DelayNms(20);
    
    LCD_WriteReg(0x0010,0x0730);
    LCD_WriteReg(0x0011,0x0137);
    DelayNms(20);
    
    LCD_WriteReg(0x0012,0x01b8);
    DelayNms(20);
    
    LCD_WriteReg(0x0013,0x0f00);
    LCD_WriteReg(0x002a,0x0080);
    LCD_WriteReg(0x0029,0x0048);
    DelayNms(20);
    
    LCD_WriteReg(0x0001,0x0100);
    LCD_WriteReg(0x0002,0x0700);
    LCD_WriteReg(0x0003,0x1038);//扫描方向 上->下  左->右 
    LCD_WriteReg(0x0008,0x0202);
    LCD_WriteReg(0x000a,0x0000);
    LCD_WriteReg(0x000c,0x0000);
    LCD_WriteReg(0x000d,0x0000);
    LCD_WriteReg(0x000e,0x0030);
    LCD_WriteReg(0x0050,0x0000);
    LCD_WriteReg(0x0051,0x00ef);
    LCD_WriteReg(0x0052,0x0000);
    LCD_WriteReg(0x0053,0x013f);
    LCD_WriteReg(0x0060,0x2700);
    LCD_WriteReg(0x0061,0x0001);
    LCD_WriteReg(0x006a,0x0000);
    //LCD_WriteReg(0x0080,0x0000);
    //LCD_WriteReg(0x0081,0x0000);
    LCD_WriteReg(0x0090,0X0011);
    LCD_WriteReg(0x0092,0x0600);
    LCD_WriteReg(0x0093,0x0402);
    LCD_WriteReg(0x0094,0x0002);
    DelayNms(20);
    
    LCD_WriteReg(0x0007,0x0001);
    DelayNms(20);
    LCD_WriteReg(0x0007,0x0061);
    LCD_WriteReg(0x0007,0x0173);
    
    LCD_WriteReg(0x0020,0x0000);
    LCD_WriteReg(0x0021,0x0000);    
    LCD_WriteReg(0x00,0x22);  
  }
  else if(s_structLCDCB.id == 0xC505)//C505初始化
  {
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    DelayNms(20);      
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x00a4,0x0001);
    DelayNms(20);      
    LCD_WriteReg(0x0060,0x2700);
    LCD_WriteReg(0x0008,0x0806);
    
    LCD_WriteReg(0x0030,0x0703);//gamma setting
    LCD_WriteReg(0x0031,0x0001);
    LCD_WriteReg(0x0032,0x0004);
    LCD_WriteReg(0x0033,0x0102);
    LCD_WriteReg(0x0034,0x0300);
    LCD_WriteReg(0x0035,0x0103);
    LCD_WriteReg(0x0036,0x001F);
    LCD_WriteReg(0x0037,0x0703);
    LCD_WriteReg(0x0038,0x0001);
    LCD_WriteReg(0x0039,0x0004);   
    
    LCD_WriteReg(0x0090, 0x0015);  //80Hz
    LCD_WriteReg(0x0010, 0X0410);  //BT,AP
    LCD_WriteReg(0x0011,0x0247);  //DC1,DC0,VC
    LCD_WriteReg(0x0012, 0x01BC);
    LCD_WriteReg(0x0013, 0x0e00);
    DelayNms(120);
    LCD_WriteReg(0x0001, 0x0100);
    LCD_WriteReg(0x0002, 0x0200);
    LCD_WriteReg(0x0003, 0x1030);
    
    LCD_WriteReg(0x000A, 0x0008);
    LCD_WriteReg(0x000C, 0x0000);
    
    LCD_WriteReg(0x000E, 0x0020);
    LCD_WriteReg(0x000F, 0x0000);
    LCD_WriteReg(0x0020, 0x0000);  //H Start
    LCD_WriteReg(0x0021, 0x0000);  //V Start
    LCD_WriteReg(0x002A,0x003D);  //vcom2
    DelayNms(20);
    LCD_WriteReg(0x0029, 0x002d);
    LCD_WriteReg(0x0050, 0x0000);
    LCD_WriteReg(0x0051, 0xD0EF);
    LCD_WriteReg(0x0052, 0x0000);
    LCD_WriteReg(0x0053, 0x013F);
    LCD_WriteReg(0x0061, 0x0000);
    LCD_WriteReg(0x006A, 0x0000);
    LCD_WriteReg(0x0092,0x0300); 
 
    LCD_WriteReg(0x0093, 0x0005);
    LCD_WriteReg(0x0007, 0x0100);
  }
  else if(s_structLCDCB.id == 0x4531)//4531初始化
  {
    LCD_WriteReg(0X00,0X0001);   
    DelayNms(10);   
    LCD_WriteReg(0X10,0X1628);   
    LCD_WriteReg(0X12,0X000e);//0x0006    
    LCD_WriteReg(0X13,0X0A39);   
    DelayNms(10);   
    LCD_WriteReg(0X11,0X0040);   
    LCD_WriteReg(0X15,0X0050);   
    DelayNms(10);   
    LCD_WriteReg(0X12,0X001e);//16    
    DelayNms(10);   
    LCD_WriteReg(0X10,0X1620);   
    LCD_WriteReg(0X13,0X2A39);   
    DelayNms(10);   
    LCD_WriteReg(0X01,0X0100);   
    LCD_WriteReg(0X02,0X0300);   
    LCD_WriteReg(0X03,0X1038);//改变方向的   
    LCD_WriteReg(0X08,0X0202);   
    LCD_WriteReg(0X0A,0X0008);   
    LCD_WriteReg(0X30,0X0000);   
    LCD_WriteReg(0X31,0X0402);   
    LCD_WriteReg(0X32,0X0106);   
    LCD_WriteReg(0X33,0X0503);   
    LCD_WriteReg(0X34,0X0104);   
    LCD_WriteReg(0X35,0X0301);   
    LCD_WriteReg(0X36,0X0707);   
    LCD_WriteReg(0X37,0X0305);   
    LCD_WriteReg(0X38,0X0208);   
    LCD_WriteReg(0X39,0X0F0B);   
    LCD_WriteReg(0X41,0X0002);   
    LCD_WriteReg(0X60,0X2700);   
    LCD_WriteReg(0X61,0X0001);   
    LCD_WriteReg(0X90,0X0210);   
    LCD_WriteReg(0X92,0X010A);   
    LCD_WriteReg(0X93,0X0004);   
    LCD_WriteReg(0XA0,0X0100);   
    LCD_WriteReg(0X07,0X0001);   
    LCD_WriteReg(0X07,0X0021);   
    LCD_WriteReg(0X07,0X0023);   
    LCD_WriteReg(0X07,0X0033);   
    LCD_WriteReg(0X07,0X0133);   
    LCD_WriteReg(0XA0,0X0000); 
  }
  else if(s_structLCDCB.id == 0x4535)//4535初始化
  {            
    LCD_WriteReg(0X15,0X0030);   
    LCD_WriteReg(0X9A,0X0010);   
    LCD_WriteReg(0X11,0X0020);   
    LCD_WriteReg(0X10,0X3428);   
    LCD_WriteReg(0X12,0X0002);//16    
    LCD_WriteReg(0X13,0X1038);   
    DelayNms(40);   
    LCD_WriteReg(0X12,0X0012);//16    
    DelayNms(40);   
    LCD_WriteReg(0X10,0X3420);   
    LCD_WriteReg(0X13,0X3038);   
    DelayNms(70);   
    LCD_WriteReg(0X30,0X0000);   
    LCD_WriteReg(0X31,0X0402);   
    LCD_WriteReg(0X32,0X0307);   
    LCD_WriteReg(0X33,0X0304);   
    LCD_WriteReg(0X34,0X0004);   
    LCD_WriteReg(0X35,0X0401);   
    LCD_WriteReg(0X36,0X0707);   
    LCD_WriteReg(0X37,0X0305);   
    LCD_WriteReg(0X38,0X0610);   
    LCD_WriteReg(0X39,0X0610); 
      
    LCD_WriteReg(0X01,0X0100);   
    LCD_WriteReg(0X02,0X0300);   
    LCD_WriteReg(0X03,0X1030);//改变方向的   
    LCD_WriteReg(0X08,0X0808);   
    LCD_WriteReg(0X0A,0X0008);   
    LCD_WriteReg(0X60,0X2700);   
    LCD_WriteReg(0X61,0X0001);   
    LCD_WriteReg(0X90,0X013E);   
    LCD_WriteReg(0X92,0X0100);   
    LCD_WriteReg(0X93,0X0100);   
    LCD_WriteReg(0XA0,0X3000);   
    LCD_WriteReg(0XA3,0X0010);   
    LCD_WriteReg(0X07,0X0001);   
    LCD_WriteReg(0X07,0X0021);   
    LCD_WriteReg(0X07,0X0023);   
    LCD_WriteReg(0X07,0X0033);   
    LCD_WriteReg(0X07,0X0133);   
  }else if(s_structLCDCB.id == 0X1963)//1963初始化
  {
    //Set PLL with OSC = 10MHz (hardware)
    //Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
    LCD_WR_REG(0xE2);    
    LCD_WR_DATA(0x1D);                     //参数1 
    LCD_WR_DATA(0x02);                     //参数2 Divider M = 2, PLL = 300/(M+1) = 100MHz
    LCD_WR_DATA(0x04);                     //参数3 Validate M and N values   
    DelayNus(100);                  
    LCD_WR_REG(0xE0);                      // Start PLL command
    LCD_WR_DATA(0x01);                     // enable PLL
    DelayNms(10);                 
    LCD_WR_REG(0xE0);                      // Start PLL command again
    LCD_WR_DATA(0x03);                     // now, use PLL output as system clock  
    DelayNms(12);                   
    LCD_WR_REG(0x01);                      //软复位
    DelayNms(10);                 
                      
    LCD_WR_REG(0xE6);                      //设置像素频率,33Mhz
    LCD_WR_DATA(0x2F);                  
    LCD_WR_DATA(0xFF);                  
    LCD_WR_DATA(0xFF);                  
                      
    LCD_WR_REG(0xB0);                      //设置LCD模式
    LCD_WR_DATA(0x20);                     //24位模式
    LCD_WR_DATA(0x00);                     //TFT 模式 
  
    LCD_WR_DATA((SSD_HOR_RESOLUTION-1)>>8);//设置LCD水平像素
    LCD_WR_DATA(SSD_HOR_RESOLUTION-1);     
    LCD_WR_DATA((SSD_VER_RESOLUTION-1)>>8);//设置LCD垂直像素
    LCD_WR_DATA(SSD_VER_RESOLUTION-1);     
    LCD_WR_DATA(0x00);                     //RGB序列 
    
    LCD_WR_REG(0xB4);                      //Set horizontal period
    LCD_WR_DATA((SSD_HT-1)>>8);
    LCD_WR_DATA(SSD_HT-1);
    LCD_WR_DATA(SSD_HPS>>8);
    LCD_WR_DATA(SSD_HPS);
    LCD_WR_DATA(SSD_HOR_PULSE_WIDTH-1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xB6);                      //Set vertical period
    LCD_WR_DATA((SSD_VT-1)>>8);
    LCD_WR_DATA(SSD_VT-1);
    LCD_WR_DATA(SSD_VPS>>8);
    LCD_WR_DATA(SSD_VPS);
    LCD_WR_DATA(SSD_VER_FRONT_PORCH-1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    
    LCD_WR_REG(0xF0);                     //设置SSD1963与CPU接口为16bit  
    LCD_WR_DATA(0x03);                    //16-bit(565 format) data for 16bpp 
                     
    LCD_WR_REG(0x29);                     //开启显示
    //设置PWM输出  背光通过占空比可调 
    LCD_WR_REG(0xD0);                     //设置自动白平衡DBC
    LCD_WR_DATA(0x00);                    //disable
                      
    LCD_WR_REG(0xBE);                     //配置PWM输出
    LCD_WR_DATA(0x05);                    //1设置PWM频率
    LCD_WR_DATA(0xFE);                    //2设置PWM占空比
    LCD_WR_DATA(0x01);                    //3设置C
    LCD_WR_DATA(0x00);                    //4设置D
    LCD_WR_DATA(0x00);                    //5设置E 
    LCD_WR_DATA(0x00);                    //6设置F 
                        
    LCD_WR_REG(0xB8);                     //设置GPIO配置
    LCD_WR_DATA(0x03);                    //2个IO口设置成输出
    LCD_WR_DATA(0x01);                    //GPIO使用正常的IO功能 
    LCD_WR_REG(0xBA);                   
    LCD_WR_DATA(0X01);                    //GPIO[1:0]=01,控制LCD方向
    
    LCDSSDSetBackLight(100);              //背光设置为最亮
  }                   
  LCDSetDispDir(0);                       //设置LCD为竖屏显示
  BACK_LIGHT_ON();                        //开启背光灯
  LCDClear(WHITE);                        //将屏幕背景色刷新为白色
} 

/*********************************************************************************************************
* 函数名称: GetLCDID
* 函数功能: 获取LCD的ID
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
u16  GetLCDID(void)
{
  return s_structLCDCB.id;                 //返回LCD的ID
}

/*********************************************************************************************************
* 函数名称: LCDDispOn
* 函数功能: LCD开启显示
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDDispOn(void)
{             
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X6804 || 
     s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X1963)    //如果LCD的ID为9341|6804|5310|1963
  {
    LCD_WR_REG(0X29);                                             //开启显示  
  }  
  else if(s_structLCDCB.id == 0X5510)                             //如果ID为5510
  {
    LCD_WR_REG(0X2900);                                           //开启显示
  }
  else                                                            //如果为其他
  {
    LCD_WriteReg(0X07, 0x0173);                                   //开启显示
  }
}   

/*********************************************************************************************************
* 函数名称: LCDDispOff
* 函数功能: LCD关闭显示
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDDispOff(void)
{     
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X6804 ||  
     s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X1963)    //如果LCD的ID为9341|6804|5310|1963
  {                                                               
    LCD_WR_REG(0X28);                                             //关闭显示                                 
  }                                                               
  else if(s_structLCDCB.id == 0X5510)                             //如果ID为5510
  {                                                               
    LCD_WR_REG(0X2800);                                           //关闭显示                                 
  }                                                               
  else                                                            //如果为其他
  {
    LCD_WriteReg(0X07,0x0);                                       //关闭显示
  }    
} 

/*********************************************************************************************************
* 函数名称: SetPointColor
* 函数功能: 设置画笔的颜色
* 输入参数: color-画笔的颜色
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void SetPointColor(u16 color)
{
  s_iPointColor = color;          //设置画笔为传入函数的颜色
}

/*********************************************************************************************************
* 函数名称: SetBackColor
* 函数功能: 设置背景色
* 输入参数: color-要填充背景的颜色
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void SetBackColor(u16 color)
{
  s_iBackColor = color;           //设置背景色为传入函数的颜色
}

/*********************************************************************************************************
* 函数名称: LCDClear
* 函数功能: 清屏
* 输入参数: color:要清屏的填充色
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDClear(u16 color)
{
  u32 index = 0;                                                //用来扫描LCD屏上每一个点的光标  
  u32 totalPoint;                                               //用来存放LCD屏的总像素点数
  
  totalPoint = s_structLCDCB.width * s_structLCDCB.height;      //LCD屏的宽度*高度，得到总点数
  
  if((s_structLCDCB.id == 0X6804) && (s_structLCDCB.dir == 1))  //6804横屏的时候特殊处理  
  {                
    s_structLCDCB.dir = 0;                                      //将横竖屏标志位设置为竖屏
    s_structLCDCB.setXCmd = 0X2A;                               //设置X坐标指令
    s_structLCDCB.setYCmd = 0X2B;                               //设置Y坐标指令 
    
    LCDSetCursor(0x00, 0x0000);                                 //设置光标位置 
    
    s_structLCDCB.dir = 1;                                      //将横竖屏标志位设置为横屏   
    s_structLCDCB.setXCmd = 0X2B;                               //设置X坐标指令
    s_structLCDCB.setYCmd = 0X2A;                               //设置Y坐标指令      
  }
  else
  {
    LCDSetCursor(0x00,0x0000);                                  //设置光标位置 
  }
  
  LCD_WriteRAM_Prepare();                                       //开始写入GRAM     
  
  for(index = 0; index < totalPoint; index++)                   //循环，直至光标index达到LCD屏的总像素点数
  {
    LCD->LCD_RAM = color;                                       //写入数据，显示颜色
  }
} 

/*********************************************************************************************************
* 函数名称: LCDSetCursor
* 函数功能: 设置光标位置
* 输入参数: xPos:横坐标，yPos:纵坐标
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDSetCursor(u16 xPos, u16 yPos)
{   
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310)//如果LCD的ID为9341或5310
  {        
    LCD_WR_REG(s_structLCDCB.setXCmd);        //向横坐标位置寄存器写命令
    LCD_WR_DATA(xPos >> 8);                   //横坐标位置右移八位
    LCD_WR_DATA(xPos & 0XFF);                 //横坐标按位与0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);        //向纵坐标位置寄存器写命令
    LCD_WR_DATA(yPos >> 8);                   //纵坐标位置右移八位
    LCD_WR_DATA(yPos & 0XFF);                 //纵坐标按位与0XFF
  }   
  else if(s_structLCDCB.id == 0X6804)         //如果LCD的ID为6804
  {
    if(s_structLCDCB.dir == 1)                //横竖屏控制标志位为横屏
    {
      xPos = s_structLCDCB.width - 1 - xPos;  //横屏时处理  
    }  
    LCD_WR_REG(s_structLCDCB.setXCmd);        //向横坐标位置寄存器写命令
    LCD_WR_DATA(xPos >> 8);                   //横坐标位置右移八位
    LCD_WR_DATA(xPos & 0XFF);                 //横坐标按位与0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);        //向纵坐标位置寄存器写命令
    LCD_WR_DATA(yPos >> 8);                   //纵坐标位置右移八位
    LCD_WR_DATA(yPos & 0XFF);                 //纵坐标按位与0XFF
  }
  else if(s_structLCDCB.id == 0X1963)         //如果LCD的ID为1963
  {             
    if(s_structLCDCB.dir == 0)                //横竖屏控制标志位为竖屏
    {
      xPos = s_structLCDCB.width - 1 - xPos;  //竖屏时处理
      
      LCD_WR_REG(s_structLCDCB.setXCmd);      //向横坐标位置寄存器写命令
      LCD_WR_DATA(0);                         //写入数据0
      LCD_WR_DATA(0);                         //写入数据0
      LCD_WR_DATA(xPos >> 8);                 //横坐标位置右移八位
      LCD_WR_DATA(xPos&0XFF);                 //横坐标按位与0XFF
    }
    else
    {
      LCD_WR_REG(s_structLCDCB.setXCmd);            //向横坐标位置寄存器写命令
      LCD_WR_DATA(xPos >> 8);                       //横坐标位置右移八位
      LCD_WR_DATA(xPos & 0XFF);                     //横坐标按位与0XFF
      LCD_WR_DATA((s_structLCDCB.width - 1) >> 8);  //屏幕的宽度减1，再右移八位
      LCD_WR_DATA((s_structLCDCB.width - 1) & 0XFF);//屏幕的宽度减1，在按位与0XFF
    }  
    LCD_WR_REG(s_structLCDCB.setYCmd);              //向纵坐标位置寄存器写命令
    LCD_WR_DATA(yPos >> 8);                         //纵坐标位置右移八位
    LCD_WR_DATA(yPos & 0XFF);                       //纵坐标按位与0XFF
    LCD_WR_DATA((s_structLCDCB.height - 1) >> 8);   //屏幕的宽度减1，再右移八位
    LCD_WR_DATA((s_structLCDCB.height - 1) & 0XFF); //屏幕的宽度减1，在按位与0XFF           
  }
  else if(s_structLCDCB.id == 0X5510)         //如果LCD的ID为5510
  {
    LCD_WR_REG(s_structLCDCB.setXCmd);        //向横坐标位置寄存器写命令
    LCD_WR_DATA(xPos >> 8);                   //横坐标位置右移八位
    LCD_WR_REG(s_structLCDCB.setXCmd + 1);    //向横坐标下一个位置寄存器写命令
    LCD_WR_DATA(xPos & 0XFF);                 //横坐标按位与0XFF   
    LCD_WR_REG(s_structLCDCB.setYCmd);        //向纵坐标位置寄存器写命令
    LCD_WR_DATA(yPos >> 8);                   //纵坐标位置右移八位
    LCD_WR_REG(s_structLCDCB.setYCmd + 1);    //向纵坐标下一个位置寄存器写命令
    LCD_WR_DATA(yPos & 0XFF);                 //纵坐标按位与0XFF
  }
  else
  {
    if(s_structLCDCB.dir == 1)                //横竖屏控制标志位为横屏
    {
      xPos = s_structLCDCB.width - 1 - xPos;  //横屏时处理
    }
    
    LCD_WriteReg(s_structLCDCB.setXCmd, xPos);//向横坐标位置寄存器写命令
    LCD_WriteReg(s_structLCDCB.setYCmd, yPos);//向纵坐标位置寄存器写命令
  }
}

/*********************************************************************************************************
* 函数名称: LCDDrawPoint
* 函数功能: 画点
* 输入参数: x,y:坐标
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: s_iPointColor:此点的颜色
*********************************************************************************************************/
void LCDDrawPoint(u16 x,u16 y)
{
  LCDSetCursor(x,y);            //设置光标位置 
  LCD_WriteRAM_Prepare();       //开始写入GRAM
  
  LCD->LCD_RAM = s_iPointColor; //写入数据，显示颜色
}

/*********************************************************************************************************
* 函数名称: LCDFastDrawPoint
* 函数功能: 快速画点
* 输入参数: x,y:坐标，color:颜色
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: s_iPointColor:此点的颜色,该函数使用宏定义进行画点，画点速度相对较快
*********************************************************************************************************/
void LCDFastDrawPoint(u16 x, u16 y, u16 color)
{     
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310) //如果LCD的ID为9341或5310
  {
    LCD_WR_REG(s_structLCDCB.setXCmd);      //向横坐标位置寄存器写命令
    LCD_WR_DATA(x >> 8);                    //横坐标位置右移八位
    LCD_WR_DATA(x & 0XFF);                  //横坐标按位与0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);      //向纵坐标位置寄存器写命令
    LCD_WR_DATA(y >> 8);                    //纵坐标位置右移八位
    LCD_WR_DATA(y & 0XFF);                  //纵坐标按位与0XFF
  } 
  else if(s_structLCDCB.id == 0X5510)       //如果LCD的ID为5510
  { 
    LCD_WR_REG(s_structLCDCB.setXCmd);      //向横坐标位置寄存器写命令
    LCD_WR_DATA(x >> 8);                    //横坐标位置右移八位
    LCD_WR_REG(s_structLCDCB.setXCmd + 1);  //向横坐标下一个位置寄存器写命令
    LCD_WR_DATA(x & 0XFF);                  //横坐标按位与0XFF   
    LCD_WR_REG(s_structLCDCB.setYCmd);      //向纵坐标位置寄存器写命令
    LCD_WR_DATA(y >> 8);                    //纵坐标位置右移八位
    LCD_WR_REG(s_structLCDCB.setYCmd + 1);  //向纵坐标下一个位置寄存器写命令
    LCD_WR_DATA(y & 0XFF);                  //纵坐标按位与0XFF
  } 
  else if(s_structLCDCB.id == 0X1963)       //如果LCD的ID为1963
  { 
    if(s_structLCDCB.dir == 0)              //横竖屏控制标志位为竖屏
    { 
      x = s_structLCDCB.width - 1 - x;      //竖屏时处理
    } 
    
    LCD_WR_REG(s_structLCDCB.setXCmd);      //向横坐标位置寄存器写命令
    LCD_WR_DATA(x >> 8);                    //横坐标位置右移八位
    LCD_WR_DATA(x & 0XFF);                  //横坐标按位与0XFF
    LCD_WR_DATA(x >> 8);                    //横坐标位置右移八位
    LCD_WR_DATA(x & 0XFF);                  //横坐标按位与0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);      //向纵坐标位置寄存器写命令
    LCD_WR_DATA(y >> 8);                    //纵坐标位置右移八位
    LCD_WR_DATA(y & 0XFF);                  //纵坐标按位与0XFF
    LCD_WR_DATA(y >> 8);                    //纵坐标位置右移八位
    LCD_WR_DATA(y & 0XFF);                  //纵坐标按位与0XFF
  } 
  else if(s_structLCDCB.id == 0X6804)       //如果LCD的ID为6804
  {         
    if(s_structLCDCB.dir == 1)              //横竖屏控制标志位为竖屏
    { 
      x = s_structLCDCB.width - 1 - x;      //横屏时处理
    } 
    
    LCD_WR_REG(s_structLCDCB.setXCmd);      //向横坐标位置寄存器写命令
    LCD_WR_DATA(x >> 8);                    //横坐标位置右移八位
    LCD_WR_DATA(x & 0XFF);                  //横坐标按位与0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);      //向纵坐标位置寄存器写命令
    LCD_WR_DATA(y >> 8);                    //纵坐标位置右移八位
    LCD_WR_DATA(y & 0XFF);                  //纵坐标按位与0XFF
  }
  else
  {
    if(s_structLCDCB.dir == 1)              //横竖屏控制标志位为竖屏
    {
      x = s_structLCDCB.width - 1 - x;      //横屏其实就是调转x,y坐标
    }
    
    LCD_WriteReg(s_structLCDCB.setXCmd, x); //设置x轴指令
    LCD_WriteReg(s_structLCDCB.setYCmd, y); //设置y轴指令
  }       
  
  LCD->LCD_REG = s_structLCDCB.wramcmd;     //开始写GRAM指令
  LCD->LCD_RAM = color;                     //将颜色写入RAM
}

/*********************************************************************************************************
* 函数名称: LCDReadPoint
* 函数功能: 读取个某点的颜色值
* 输入参数: x,y:坐标
* 输出参数: void
* 返 回 值: 此点的颜色
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
u16 LCDReadPoint(u16 x, u16 y)
{
  u16 r = 0;  //
  u16 g = 0;  //
  u16 b = 0;  //
  
  if(x >= s_structLCDCB.width || y >= s_structLCDCB.height)
  {
    return 0;                                                     //超过了范围,直接返回
  }    
  
  LCDSetCursor(x,y);                                              //将光标移到指定位置
  
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X6804 || 
     s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X1963)
  {
    LCD_WR_REG(0X2E);                                             //9341/6804/3510/1963 发送读GRAM指令
  }
  else if(s_structLCDCB.id == 0X5510)                             //如果LCD的ID为5510
  {
    LCD_WR_REG(0X2E00);                                           //5510 发送读GRAM指令
  }
  else
  { 
    LCD_WR_REG(0X22);                                             //其他IC发送读GRAM指令
  }
  
  if(s_structLCDCB.id == 0X9320)                                  //如果LCD的ID为9320
  {
    opt_delay(2);                                                 //延时2us      
  }
  
  r = LCD_RD_DATA();                                              //dummy Read，无效的读取值
  
  if(s_structLCDCB.id == 0X1963)
  {
    return r;                                                     //1963直接读就可以 
  }
  
  opt_delay(2);                                                   //延时2us    
  
  r = LCD_RD_DATA();                                              //实际坐标颜色  
  
  //9341/NT35310/NT35510要分2次读出
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X5510)    
  {
    opt_delay(2);                                                 //延时2us    
    
    b = LCD_RD_DATA();                          
    
    //对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
    g = r & 0XFF;     
    g <<= 8;
  } 
  
  if(s_structLCDCB.id == 0X9325 || s_structLCDCB.id == 0X4535 || s_structLCDCB.id == 0X4531 ||  
     s_structLCDCB.id == 0XB505 || s_structLCDCB.id == 0XC505)
  {
    return r; //这几种IC直接返回颜色值
  }
  else if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X5510)
  {
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  //ILI9341/NT35310/NT35510需要公式转换一下
  }
  else 
  {
    return LCD_BGR2RGB(r);  //其他IC
  }
}       

/*********************************************************************************************************
* 函数名称: LCDDrawCircle
* 函数功能: 在指定位置画一个指定大小的圆
* 输入参数: (x,y):中心点，r:半径
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDDrawCircle(u16 x0,u16 y0,u8 r)
{
  int a;                        
  int b;                        //用来存放半径
  int di;
  
  a = 0;
  b = r;    
  di = 3 - (r << 1);             //判断下个点位置的标志
  
  while(a <= b)
  {
    LCDDrawPoint(x0 + a, y0 - b); //5
    LCDDrawPoint(x0 + b, y0 - a); //0           
    LCDDrawPoint(x0 + b, y0 + a); //4   
    LCDDrawPoint(x0 + a, y0 + b); //6 
    LCDDrawPoint(x0 - a, y0 + b); //1       
    LCDDrawPoint(x0 - b, y0 + a); 
    LCDDrawPoint(x0 - a, y0 - b); //2 
    LCDDrawPoint(x0 - b, y0 - a); //7    
    
    a++;
    
    //使用Bresenham算法画圆     
    if(di < 0)
    {
      di += 4 * a + 6;
    }      
    else
    {
      di += 10 + 4 * (a - b);   
      b--;
    }     
  }
} 

/*********************************************************************************************************
* 函数名称: LCDDrawLine
* 函数功能: 画线
* 输入参数: (x1,y1):起点坐标；(x2,y2):终点坐标
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDDrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
  u16 t; 
  int xerr = 0, yerr = 0, delta_x, delta_y, distance; 
  int incx, incy, uRow, uCol; 
  
  delta_x = x2 - x1;                  //计算坐标增量 
  delta_y = y2 - y1;              
  uRow = x1;              
  uCol = y1;              
                
  if(delta_x > 0)                     //当x的增量大于0
  {             
    incx = 1;                         //设置x轴单步方向为正方向
  }             
  else if(delta_x == 0)               //当x的增量等于0
  {             
    incx = 0;                         //垂直线 
  }             
  else                                //当x的增量小于0
  {             
    incx = -1;                        //设置x轴单步方向为负方向
    delta_x = -delta_x;               //取绝对值
  }               
                
  if(delta_y > 0)                     //当y的增量大于0
  {             
    incy = 1;                         //设置y轴单步方向为正方向
  }                 
  else if(delta_y == 0)               //当y的增量等于0
  {             
    incy = 0;                         //水平线 
  }             
  else                                //当y的增量小于0
  {             
    incy = -1;                        //设置y轴单步方向为负方向
    delta_y = -delta_y;               //取绝对值
  } 
  
  if(delta_x > delta_y)               //比较x和y的增量大小，选取较大的增量为基本增量坐标轴
  {             
    distance = delta_x;               //x的增量较大，则选取x的增量为基本增量坐标轴
  }    
  else
  {
    distance = delta_y;               //y的增量较大，则选取y的增量为基本增量坐标轴
  }
  
  for(t = 0; t <= distance + 1; t++)  //画线输出 
  {  
    LCDDrawPoint(uRow, uCol);         //画点 
    xerr += delta_x ; 
    yerr += delta_y ;
    
    if(xerr > distance) 
    { 
      xerr -= distance; 
      uRow += incx; 
    } 
    
    if(yerr > distance) 
    { 
      yerr -= distance; 
      uCol += incy; 
    } 
  }  
} 

/*********************************************************************************************************
* 函数名称: LCDDrawRect
* 函数功能: 画矩形
* 输入参数: (x1,y1),(x2,y2):矩形的对角坐标
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDDrawRect(u16 x1, u16 y1, u16 x2, u16 y2)
{
  //调用画线函数画出矩形的四条边
  LCDDrawLine(x1,y1,x2,y1);         //画上边的长
  LCDDrawLine(x1,y1,x1,y2);         //画左边的宽
  LCDDrawLine(x1,y2,x2,y2);         //画下边的长
  LCDDrawLine(x2,y1,x2,y2);         //画右边的宽
}

/*********************************************************************************************************
* 函数名称: LCDFill
* 函数功能: 在指定区域内填充单个颜色
* 输入参数: (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)，color:要填充的颜色
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDFill(u16 startX, u16 startY, u16 endX, u16 endY, u16 color)
{      
  u16 i, j;
  u16 xlen = 0;
  u16 temp;
  
  if((s_structLCDCB.id == 0X6804) && (s_structLCDCB.dir == 1))  //6804横屏的时候特殊处理  
  {
    temp = startX;
    startX = startY;
    startY = s_structLCDCB.width - endX - 1;    
    endX = endY;
    endY = s_structLCDCB.width - temp - 1;
    s_structLCDCB.dir = 0;   
    s_structLCDCB.setXCmd = 0X2A;
    s_structLCDCB.setYCmd = 0X2B;           
    LCDFill(startX, startY, endX, endY, color);  
    s_structLCDCB.dir = 1;   
    s_structLCDCB.setXCmd = 0X2B;
    s_structLCDCB.setYCmd = 0X2A;     
  }
  else
  {
    xlen = endX - startX + 1;   
    for(i = startY; i <= endY; i++)
    {
      LCDSetCursor(startX, i);  //设置光标位置 
      LCD_WriteRAM_Prepare();   //开始写入GRAM    
      
      for(j = 0; j < xlen; j++)
      {
        LCD->LCD_RAM = color;  //写入数据，显示颜色
      }         
    }
  }
}  

/*********************************************************************************************************
* 函数名称: LCDColorFill
* 函数功能: 在指定区域内填充指定颜色块
* 输入参数: (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)，color:要填充的颜色
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDColorFill(u16 startX, u16 startY, u16 endX, u16 endY, u16* pColor)
{  
  u16 height, width;                        //用来存放指定区域的高度和宽度
  u16 i, j;                                 //循环计数器
            
  width = endX - startX + 1;                //得到填充的宽度
  height = endY - startY + 1;               //得到填充的高度
  
   for(i = 0; i < height; i++)
  {
    LCDSetCursor(startX, startY + i);       //设置光标位置 
    LCD_WriteRAM_Prepare();                 //开始写入GRAM
    
    for(j = 0; j < width; j++)
    {
      LCD->LCD_RAM = pColor[i * width + j]; //写入数据
    }      
  }      
}  

/*********************************************************************************************************
* 函数名称: LCDShowChar
* 函数功能: 在指定位置显示一个字符
* 输入参数: x,y:起始坐标，num:要显示的字符:" "→"~"，size:字体大小 12/16/24，mode:叠加方式(1)非叠加方式(0)
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{                  
  u8 temp;                                                      //用来存放
  u8 t1, t;                                                     //循环计数器
  u16 y0 = y;                                                   //用来存放纵坐标的起始位置
  u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);    //得到字体一个字符对应点阵集所占的字节数  
  
  num = num - ' ';          //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库偏移量）
  
  for(t = 0; t < csize; t++)
  {   
    if(size == 12)
    {
      temp = asc2_1206[num][t];               //调用1206字体
    }             
    else if(size== 16)              
    {             
      temp=asc2_1608[num][t];                 //调用1608字体
    }             
    else if(size == 24)             
    {             
      temp=asc2_2412[num][t];                 //调用2412字体
    }             
    else              
    {             
      return;                                 //没有字库
    }
    for(t1 = 0; t1 < 8; t1++)   
    {          
      if(temp & 0x80)
      {
        LCDFastDrawPoint(x, y, s_iPointColor);//以画笔的颜色快速画点
      }
      else if(mode == 0)
      {
        LCDFastDrawPoint(x, y, s_iBackColor); //以背光灯的颜色快速画点
      }
      
      temp <<= 1;                             //左移一位
      y++;                                    //纵坐标加1
                
      if(y >= s_structLCDCB.height)           //如果纵左边大于LCD屏的高度
      {         
        return;                               //超区域了，跳出函数
      }         
      if((y - y0) == size)          
      {         
        y = y0;                               //纵坐标重新回到起点
        x++;                                  //横坐标加1
                  
        if(x >= s_structLCDCB.width)          //如果横坐标大于LCD屏的宽度
        {         
          return;                             //超区域了，跳出函数
        }         
        break;                                //跳出循环
      }
    }     
  }                    
} 

/*********************************************************************************************************
* 函数名称: LCDShowNum
* 函数功能: 显示数字,高位为0,则不显示
* 输入参数: x,y:起始坐标，len :数字的位数，size:字体大小 12/16/24，color:颜色，num:数值(0~4294967295)
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{           
  u8 t;                                                       //循环计数器
  u8 temp;                                                    //用于存放数字的各个位
  u8 enshow = 0;                                              //用于判断是否为高位0的标志位
  
  for(t = 0; t < len; t++)                                    //循环直至显示数字的所有位
  {
    temp = (num / LCDPower(10, len - t - 1)) % 10;            //从高到低依次取出数字的各个位
    
    if(enshow == 0 && t < (len - 1))                          //没有超出数字的长度且设置了高位0标志
    {
      if(temp == 0)                                           //取出的数字为0
      {
        LCDShowChar(x + (size / 2) * t, y, ' ', size, 0);     //高位0用空格代替
        continue;                                             //跳过本次循环，进入下一次循环
      }
      else
      {
        enshow = 1;                                           //清除高位0标志
      }               
    }   
    
    LCDShowChar(x + (size / 2) * t, y, temp + '0', size, 0);  //调用显示字符函数显示数字
  }
} 

/*********************************************************************************************************
* 函数名称: LCDShowxNum
* 函数功能: 显示数字,高位为0,还是显示
* 输入参数: x,y:起始坐标，num:数值(0~999999999)，len:长度(即要显示的位数)，size:字体大小 12/16/24
*           mode：[7]:0,不填充;1,填充0；[6:1]:保留；[0]:0,非叠加显示;1,叠加显示
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{  
  u8 t;                                                                //循环计数器
  u8 temp;                                                             //用于存放数字的各个位
  u8 enshow = 0;                                                       //用于判断是否为高位0的标志位
                                                                       
  for(t = 0; t < len; t++)                                             //循环直至显示数字的所有位
  {                                                                    
    temp = (num / LCDPower(10, len - t - 1)) % 10;                     //从高到低依次取出数字的各个位
                                                                       
    if(enshow == 0 && t < (len - 1))                                   //没有超出数字的长度且设置了高位0标志
    {                                                                  
      if(temp == 0)                                                    //取出的数字为0
      {                                                                
        if(mode & 0X80)                                                
        {                                                              
          LCDShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);  //显示0
        }                                                              
        else                                                           
        {                                                              
          LCDShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);  //高位0用空格代替
        }                                                              
        continue;                                                      //跳过本次循环，进入下一次循环                         
      }                                                                
      else
      {
        enshow = 1;                                                    //清除高位0标志
      }
    }   
    LCDShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01); //调用显示字符函数显示数字
  }
} 

/*********************************************************************************************************
* 函数名称: LCDShowString
* 函数功能: 显示字符串
* 输入参数: x,y:起点坐标，width,height:区域大小，size:字体大小 12/16/24，*p:字符串起始地址
* 输出参数:
* 返 回 值: 
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8* p)
{         
  u8 x0   = x;                        //存放横向起点
  width  += x;                        //将宽度转化为横向末地址
  height += y;                        //将高度转化为纵向末地址
  
  while((*p <= '~') && (*p >= ' '))   //判断是不是非法字符!
  {       
    if(x >= width)
    {
      x = x0;                         //横坐标指向横向起点
      y += size;                      //纵坐标指向下一行
    }
    if(y >= height)
    {
      break;                          //纵坐标超出区域退出  
    }  
    
    LCDShowChar(x, y, *p, size, 1);   //调用显示字符函数显示一个字符
    
    x += size / 2;                    //x光标移向下一位
    p++;                              //字符串指针加1
  }  
}

/*********************************************************************************************************
* 函数名称: LCDSSDSetBackLight
* 函数功能: SSD1963 背光设置
* 输入参数: pwm:背光等级,0~100.越大越亮
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/
void LCDSSDSetBackLight(u8 pwm)
{  
  LCD_WR_REG(0xBE);       //配置PWM输出
  LCD_WR_DATA(0x05);      //设置PWM频率
  LCD_WR_DATA(pwm * 2.55);//设置PWM占空比
  LCD_WR_DATA(0x01);      
  LCD_WR_DATA(0xFF);      
  LCD_WR_DATA(0x00);      
  LCD_WR_DATA(0x00);      
}

/*********************************************************************************************************
* 函数名称: LCDSetScanDir
* 函数功能: 设置LCD的自动扫描方向
* 输入参数: pwm:背光等级,0~100.越大越亮
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
*           所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常
*           dir:0~7,代表8个方向(具体定义见lcd.h)
*           9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963等IC已经实际测试
*********************************************************************************************************/       
void LCDSetScanDir(u8 dir)
{
  u16 regval = 0; //用于存放寄存器的值
  u16 dirreg = 0; //用于存放屏幕的方向
  u16 temp;  
  
  //横屏时，对6804和1963不改变扫描方向！竖屏时1963改变方向
  if((s_structLCDCB.dir == 1 && s_structLCDCB.id != 0X6804 && s_structLCDCB.id != 0X1963) || 
     (s_structLCDCB.dir == 0 && s_structLCDCB.id == 0X1963))
  {         
    switch(dir)//方向转换
    {
      case 0:
        dir = 6;
        break;
      case 1:
        dir = 7;
        break;
      case 2:
        dir = 4;
        break;
      case 3:
        dir = 5;
        break;
      case 4:
        dir = 1;
        break;
      case 5:
        dir = 0;
        break;
      case 6:
        dir = 3;
        break;
      case 7:
        dir = 2;
        break;       
    }
  } 
  //9341/6804/5310/5510/1963,特殊处理
  if(s_structLCDCB.id == 0x9341 || s_structLCDCB.id == 0X6804 || s_structLCDCB.id == 0X5310 || 
     s_structLCDCB.id == 0X5510 || s_structLCDCB.id == 0X1963)
  {
    switch(dir)
    {
      case L2R_U2D://从左到右,从上到下
        regval |= (0 << 7) | (0 << 6) | (0 << 5); 
        break;
      case L2R_D2U://从左到右,从下到上
        regval |= (1 << 7) | (0 << 6) | (0 << 5); 
        break;
      case R2L_U2D://从右到左,从上到下
        regval |= (0 << 7) | (1 << 6) | (0 << 5); 
        break;
      case R2L_D2U://从右到左,从下到上
        regval |= (1 << 7) | (1 << 6) | (0 << 5); 
        break;   
      case U2D_L2R://从上到下,从左到右
        regval |= (0 << 7) | (0 << 6) | (1 << 5); 
        break;
      case U2D_R2L://从上到下,从右到左
        regval |= (0 << 7) | (1 << 6) | (1 << 5); 
        break;
      case D2U_L2R://从下到上,从左到右
        regval |= (1 << 7) | (0 << 6) | (1 << 5); 
        break;
      case D2U_R2L://从下到上,从右到左
        regval |= (1 << 7) | (1 << 6) | (1 << 5); 
        break;   
    }
    
    if(s_structLCDCB.id == 0X5510)
    {
      dirreg = 0X3600;  
    }  
    else 
    {
      dirreg = 0X36;
    }
    
    if((s_structLCDCB.id != 0X5310) && (s_structLCDCB.id != 0X5510) && (s_structLCDCB.id != 0X1963))
    {
      regval |= 0X08;                                   //5310/5510/1963不需要BGR
    }
       
    if(s_structLCDCB.id == 0X6804)
    {
      regval |= 0x02;                                   //6804的BIT6和9341的反了
    }
    
    LCD_WriteReg(dirreg, regval);
    
    if(s_structLCDCB.id != 0X1963)                      //1963不做坐标处理
    {
      if(regval & 0X20)
      {
        if(s_structLCDCB.width < s_structLCDCB.height)  //交换X,Y
        {
          temp = s_structLCDCB.width;
          s_structLCDCB.width = s_structLCDCB.height;
          s_structLCDCB.height = temp;
        }
      }
      else  
      {
        if(s_structLCDCB.width>s_structLCDCB.height)    //交换X,Y
        {
          temp = s_structLCDCB.width;
          s_structLCDCB.width = s_structLCDCB.height;
          s_structLCDCB.height = temp;
        }
      }  
    }
    
    if(s_structLCDCB.id == 0X5510)
    {
      LCD_WR_REG(s_structLCDCB.setXCmd);
      LCD_WR_DATA(0); 
      LCD_WR_REG(s_structLCDCB.setXCmd + 1);
      LCD_WR_DATA(0); 
      LCD_WR_REG(s_structLCDCB.setXCmd + 2);
      LCD_WR_DATA((s_structLCDCB.width - 1) >> 8); 
      LCD_WR_REG(s_structLCDCB.setXCmd + 3);
      LCD_WR_DATA((s_structLCDCB.width - 1) & 0XFF); 
      LCD_WR_REG(s_structLCDCB.setYCmd);
      LCD_WR_DATA(0); 
      LCD_WR_REG(s_structLCDCB.setYCmd + 1);
      LCD_WR_DATA(0); 
      LCD_WR_REG(s_structLCDCB.setYCmd + 2);
      LCD_WR_DATA((s_structLCDCB.height - 1) >> 8); 
      LCD_WR_REG(s_structLCDCB.setYCmd + 3);
      LCD_WR_DATA((s_structLCDCB.height - 1) & 0XFF);
    }
    else
    {
      LCD_WR_REG(s_structLCDCB.setXCmd); 
      LCD_WR_DATA(0);
      LCD_WR_DATA(0);
      LCD_WR_DATA((s_structLCDCB.width - 1) >> 8);
      LCD_WR_DATA((s_structLCDCB.width - 1) & 0XFF);
      LCD_WR_REG(s_structLCDCB.setYCmd); 
      LCD_WR_DATA(0);
      LCD_WR_DATA(0);
      LCD_WR_DATA((s_structLCDCB.height - 1) >> 8);
      LCD_WR_DATA((s_structLCDCB.height - 1) & 0XFF);  
    }
  }
  else 
  {
    switch(dir)
    {
      case L2R_U2D://从左到右,从上到下
        regval |= (1 << 5) | (1 << 4) | (0 << 3); 
        break;
      case L2R_D2U://从左到右,从下到上
        regval |= (0 << 5) | (1 << 4) | (0 << 3); 
        break;
      case R2L_U2D://从右到左,从上到下
        regval |= (1 << 5) | (0 << 4) | (0 << 3);
        break;
      case R2L_D2U://从右到左,从下到上
        regval |= (0 << 5) | (0 << 4) | (0 << 3); 
        break;   
      case U2D_L2R://从上到下,从左到右
        regval |= (1 << 5) | (1 << 4) | (1 << 3); 
        break;
      case U2D_R2L://从上到下,从右到左
        regval |= (1 << 5) | (0 << 4) | (1 << 3); 
        break;
      case D2U_L2R://从下到上,从左到右
        regval |= (0 << 5) | (1 << 4) | (1 << 3); 
        break;
      case D2U_R2L://从下到上,从右到左
        regval |= (0 << 5) | (0 << 4) | (1 << 3); 
        break;   
    } 
    dirreg = 0X03;
    regval |= 1 << 12; 
    LCD_WriteReg(dirreg, regval);
  }
} 

/*********************************************************************************************************
* 函数名称: LCDSetDispDir
* 函数功能: 设置LCD显示方向
* 输入参数: dir:0,竖屏；1,横屏
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/   
void LCDSetDispDir(u8 dir)
{
  if(dir == 0)  //竖屏
  {
    s_structLCDCB.dir = 0;                  //将横竖屏标志位设置为竖屏
    s_structLCDCB.width = 240;              //设置LCD屏的宽度为240
    s_structLCDCB.height = 320;             //设置LCD屏的高度为320
    
    if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X6804 || s_structLCDCB.id == 0X5310)
    {
      s_structLCDCB.wramcmd = 0X2C;         //设置写入GRAM的指令 
      s_structLCDCB.setXCmd = 0X2A;         //设置写X坐标指令
      s_structLCDCB.setYCmd = 0X2B;         //设置写Y坐标指令     
      if(s_structLCDCB.id == 0X6804 || s_structLCDCB.id == 0X5310)
      {
        s_structLCDCB.width = 320;          //设置LCD屏的宽度320
        s_structLCDCB.height = 480;         //设置LCD屏的高度480 
      }
    }
    else if(s_structLCDCB.id == 0x5510)
    {
      s_structLCDCB.wramcmd = 0X2C00;       //设置写入GRAM的指令 
      s_structLCDCB.setXCmd = 0X2A00;       //设置写X坐标指令
      s_structLCDCB.setYCmd = 0X2B00;       //设置写Y坐标指令 
      s_structLCDCB.width = 480;            //设置LCD屏的宽度480
      s_structLCDCB.height = 800;           //设置LCD屏的高度800 ;
    }
    else if(s_structLCDCB.id == 0X1963)
    {
      s_structLCDCB.wramcmd = 0X2C;         //设置写入GRAM的指令 
      s_structLCDCB.setXCmd = 0X2B;         //设置写X坐标指令
      s_structLCDCB.setYCmd = 0X2A;         //设置写Y坐标指令
      s_structLCDCB.width = 480;            //设置LCD屏的宽度480
      s_structLCDCB.height = 800;           //设置LCD屏的高度800  
    }
    else
    {
      s_structLCDCB.wramcmd = 0X22;         //设置写入GRAM的指令 
      s_structLCDCB.setXCmd = 0X20;         //设置写X坐标指令
      s_structLCDCB.setYCmd = 0X21;         //设置写Y坐标指令   
    }
  }else                                     //将横竖屏标志位设置为横屏
  {            
    s_structLCDCB.dir = 1;                  //将横竖屏标志位设置为横屏
    s_structLCDCB.width = 320;              //设置LCD屏的宽度为320
    s_structLCDCB.height = 240;             //设置LCD屏的高度为240
    
    if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310)
    {
      s_structLCDCB.wramcmd = 0X2C;         //设置写入GRAM的指令 
      s_structLCDCB.setXCmd = 0X2A;         //设置写X坐标指令
      s_structLCDCB.setYCmd = 0X2B;         //设置写Y坐标指令      
    }
    else if(s_structLCDCB.id == 0X6804)   
    {
      s_structLCDCB.wramcmd = 0X2C;         //设置写入GRAM的指令 
      s_structLCDCB.setXCmd = 0X2B;         //设置写X坐标指令
      s_structLCDCB.setYCmd = 0X2A;         //设置写Y坐标指令  
    }
    else if(s_structLCDCB.id == 0x5510)
    {
      s_structLCDCB.wramcmd = 0X2C00;       //设置写入GRAM的指令 
      s_structLCDCB.setXCmd = 0X2A00;       //设置写X坐标指令
      s_structLCDCB.setYCmd = 0X2B00;       //设置写Y坐标指令  
      s_structLCDCB.width = 800;
      s_structLCDCB.height = 480;
    }
    else if(s_structLCDCB.id == 0X1963)
    {
      s_structLCDCB.wramcmd = 0X2C;         //设置写入GRAM的指令 
      s_structLCDCB.setXCmd = 0X2A;         //设置写X坐标指令
      s_structLCDCB.setYCmd = 0X2B;         //设置写Y坐标指令
      s_structLCDCB.width = 800;            //设置LCD屏的宽度800
      s_structLCDCB.height = 480;           //设置LCD屏的高度480  
    }
    else
    {
      s_structLCDCB.wramcmd = 0X22;         //设置写入GRAM的指令 
      s_structLCDCB.setXCmd = 0X21;         //设置写X坐标指令
      s_structLCDCB.setYCmd = 0X20;         //设置写Y坐标指令   
    }
    
    if(s_structLCDCB.id == 0X6804 || s_structLCDCB.id == 0X5310)
    {    
      s_structLCDCB.width = 480;            //设置LCD屏的宽度为480
      s_structLCDCB.height = 320;           //设置LCD屏的高度为320
    }
  } 
  
  LCDSetScanDir(DFT_SCAN_DIR);              //默认扫描方向
}   

/*********************************************************************************************************
* 函数名称: LCDSetWindow
* 函数功能: 设置窗口,并自动设置画点坐标到窗口左上角(sx,sy)
* 输入参数: sx, sy:窗口起始坐标(左上角)，width, height:窗口宽度和高度,必须大于0，窗体大小:width * height
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日 
* 注    意: 
*********************************************************************************************************/   
void LCDSetWindow(u16 sx, u16 sy, u16 width, u16 height)
{    
  u8  hsareg, heareg, vsareg, veareg;
  u16 hsaval, heaval, vsaval, veaval; 
  u16 twidth, theight;
  
  twidth  = sx + width - 1;
  theight = sy + height - 1;
  
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X6804 || 
     (s_structLCDCB.dir == 1 && s_structLCDCB.id == 0X1963))
  {
    LCD_WR_REG(s_structLCDCB.setXCmd); 
    LCD_WR_DATA(sx >> 8); 
    LCD_WR_DATA(sx & 0XFF);   
    LCD_WR_DATA(twidth >> 8); 
    LCD_WR_DATA(twidth & 0XFF);  
    LCD_WR_REG(s_structLCDCB.setYCmd); 
    LCD_WR_DATA(sy >> 8); 
    LCD_WR_DATA(sy & 0XFF); 
    LCD_WR_DATA(theight >> 8); 
    LCD_WR_DATA(theight & 0XFF); 
  }
  else if(s_structLCDCB.id == 0X1963)//1963竖屏特殊处理
  {
    sx = s_structLCDCB.width - width - sx; 
    height = sy + height - 1; 
    LCD_WR_REG(s_structLCDCB.setXCmd); 
    LCD_WR_DATA(sx >> 8); 
    LCD_WR_DATA(sx & 0XFF);   
    LCD_WR_DATA((sx + width - 1) >> 8); 
    LCD_WR_DATA((sx + width - 1) & 0XFF);  
    LCD_WR_REG(s_structLCDCB.setYCmd); 
    LCD_WR_DATA(sy >> 8); 
    LCD_WR_DATA(sy & 0XFF); 
    LCD_WR_DATA(height >> 8); 
    LCD_WR_DATA(height & 0XFF);     
  }
  else if(s_structLCDCB.id == 0X5510)
  {
    LCD_WR_REG(s_structLCDCB.setXCmd);
    LCD_WR_DATA(sx >> 8);  
    LCD_WR_REG(s_structLCDCB.setXCmd + 1);
    LCD_WR_DATA(sx & 0XFF);    
    LCD_WR_REG(s_structLCDCB.setXCmd + 2);
    LCD_WR_DATA(twidth >> 8);   
    LCD_WR_REG(s_structLCDCB.setXCmd + 3);
    LCD_WR_DATA(twidth & 0XFF);   
    LCD_WR_REG(s_structLCDCB.setYCmd);
    LCD_WR_DATA(sy >> 8);   
    LCD_WR_REG(s_structLCDCB.setYCmd + 1);
    LCD_WR_DATA(sy & 0XFF);  
    LCD_WR_REG(s_structLCDCB.setYCmd + 2);
    LCD_WR_DATA(theight >> 8);   
    LCD_WR_REG(s_structLCDCB.setYCmd + 3);
    LCD_WR_DATA(theight & 0XFF);  
  }
  else  //其他驱动IC
  {
    if(s_structLCDCB.dir == 1)//如果横竖屏标志位为横屏
    {
      //窗口值
      hsaval = sy;        
      heaval = theight;
      vsaval = s_structLCDCB.width - twidth - 1;
      veaval = s_structLCDCB.width - sx - 1;        
    }else
    { 
      hsaval = sx;        
      heaval = twidth;
      vsaval = sy;
      veaval = theight;
    } 
    hsareg = 0X50;
    heareg = 0X51;                  //水平方向窗口寄存器    
    vsareg = 0X52;                  
    veareg = 0X53;                  //垂直方向窗口寄存器
    
    //设置寄存器值
    LCD_WriteReg(hsareg, hsaval);
    LCD_WriteReg(heareg, heaval);
    LCD_WriteReg(vsareg, vsaval);
    LCD_WriteReg(veareg, veaval);    
    LCDSetCursor(sx, sy);           //设置光标位置
  }
}