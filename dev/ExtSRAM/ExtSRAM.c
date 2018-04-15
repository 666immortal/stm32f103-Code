/*********************************************************************************************************
* 模块名称: ExtSRAM.c
* 摘    要: 
* 当前版本: 1.0.0
* 作    者: 666immortal
* 完成日期: 2018年03月01日
* 内    容:
* 注    意: 使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
*           对IS61LV25616/IS62WV25616,地址线范围为A0~A17,对IS61LV51216/IS62WV51216,地址线范围为A0~A18                                                                             
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
#include "ExtSRAM.h"
#include <stm32f10x_conf.h>
#include "LCD.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
//使用的FSMC的存储块BANK1的区域3来控制IS62WV51216，其中区域3对应的开始地址是0x68000000
#define BANK1_SRAM3_ADDR    ((u32)(0x68000000))    

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
//__attribute__：用来指定变量的特殊属性；at(0X68000000)：属性说明；at：设置变量的绝对地址
//即设置变量处于0X68000000这个地址，s_arrSRAM[250000]是测试用的数组
static u32 s_arrSRAM[250000] __attribute__((at(0X68000000)));

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
//从外部SRAM指定地址开始，写入指定长度的数据
static void ExtSRAMWriteBuf(u8* pBuf, u32 writeAddr, u32 numByteToWrite);
//从外部SRAM指定地址开始，读取指定长度的数据
static void ExtSRAMReadBuf(u8* pBuf, u32 readAddr, u32 numByteToRead);   

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/                              
/*********************************************************************************************************
* 函数名称: ExtSRAMWriteBuf
* 函数功能: 从指定地址开始,连续写入n个字节
* 输入参数: pBuf: 字节指针，writeAddr: 要写入字节的起始地址，numByteToWrite: 要写入的字节数
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意:
*********************************************************************************************************/
static void ExtSRAMWriteBuf(u8* pBuf, u32 writeAddr, u32 numByteToWrite)
{
  u32 n = numByteToWrite;                           //n：要写入的字节数
  
  for(; n != 0; n--)                                //每写入一个字节，n减1
  {                        
    *(vu8*)(BANK1_SRAM3_ADDR + writeAddr) = *pBuf;  //从要写入字节的起始地址开始，逐个写入数据
    
    writeAddr++;                                    //要写入字节的地址加1
    pBuf++;                                         //字节指针加1
  }   
}                                          

/*********************************************************************************************************
* 函数名称: ExtSRAMReadBuf
* 函数功能: 从指定地址开始,连续读出n个字节
* 输入参数: pBuf:字节指针, readAddr:要读出字节的起始地址,numByteToRead:要读出的字节数
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意:
*********************************************************************************************************/
static void ExtSRAMReadBuf(u8* pBuf, u32 readAddr, u32 numByteToRead)
{
  u32 n = numByteToRead;                            //n：要读出的字节数

  for(;n != 0; n--)                                 //每读出一个字节，n减1
  {                          
    *pBuf = *(vu8*)(BANK1_SRAM3_ADDR + readAddr);   //从要读出字节的起始地址开始，逐个读出数据
    
    readAddr++;                                     //要读出字节的地址加1
    pBuf++;                                         //字节指针加1
  }  
} 

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: InitExtSRAM
* 函数功能: 初始化外部SRAM
* 输入参数: void 
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意:
*********************************************************************************************************/
void InitExtSRAM(void)
{  
  i32 i;   //定义一个i32类型的变量i，作为循环计数器

  GPIO_InitTypeDef               GPIO_InitStructure;     //定义结构体GPIO_InitStructure,用来配置FSMC的GPIO
  //定义结构体FSMC_NORSRAMInitStructure，用来配置FSMC（可变静态存储控制器）的参数
  FSMC_NORSRAMInitTypeDef        FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;        //定义结构体readWriteTiming，用来配置FSMC的时序参数
 
  //使能GPIOD、GPIOE、GPIOF、GPIOG的时钟（根据原理图来配置使用GPIO端口）
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |     
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);     //使能FSMC的时钟
  
  //请查看 stm32f10x_gpio.h 来算出 GPIO_Pin_x 对应的 uint16_t 类型的数值，然后相加。
  //以下的GPIOD引脚按顺序依次对应FSMC_D2、D3、NOE、NWE、D13、D14、D15、A16、A17、A18、D0、D1
  GPIO_InitStructure.GPIO_Pin   = 0xFF33;           //设置对应的引脚：PD0、1、4、5、8-15
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置I/O口速率为50MHz
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //复用推挽输出模式
  GPIO_Init(GPIOD, &GPIO_InitStructure);            //根据参数初始化GPIOD端口

  //以下的GPIOE引脚按顺序依次对应FSMC_NBL0、NBL1、D4-D12
  GPIO_InitStructure.GPIO_Pin = 0xFF83;             //设置对应的引脚：PE0、1、7-15
  GPIO_Init(GPIOE, &GPIO_InitStructure);            //根据参数初始化GPIOE端口

  //以下的GPIOF引脚按顺序依次对应FSMC_A0-A5、A6-A9
  GPIO_InitStructure.GPIO_Pin = 0xF03F;             //设置对应的引脚：PF0-5、PF12-15
  GPIO_Init(GPIOF, &GPIO_InitStructure);            //根据参数初始化GPIOF端口

  //以下的GPIOG引脚按顺序依次对应FSMC_A10-A15、NE3
  GPIO_InitStructure.GPIO_Pin = 0x043F;             //设置对应的引脚：PG0-5、9、10、12
  GPIO_Init(GPIOG, &GPIO_InitStructure);            //根据参数初始化GPIOG端口
           
  readWriteTiming.FSMC_AddressSetupTime = 0x00;     //地址建立时间（ADDSET）为1个HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime  = 0x00;     //地址保持时间（ADDHLD）模式A未用到  
  readWriteTiming.FSMC_DataSetupTime    = 0x03;     //数据保持时间（DATAST）为3个HCLK 4/72M=55ns(对EM的SRAM芯片)   
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;//总线恢复时间
  readWriteTiming.FSMC_CLKDivision = 0x00;          //时钟分频因子
  readWriteTiming.FSMC_DataLatency = 0x00;          //数据产生时间
  readWriteTiming.FSMC_AccessMode  = FSMC_AccessMode_A; //异步模式A（FSMC_NOR控制器时序模式）  
 
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;                      //使用了FSMC的BANK1的区域3
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;    //禁止地址/数据线复用
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;               //存储器类型为SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;      //存储器数据宽度为16bit  
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;  //关闭突发模式访问
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;//地址线的等待状态电平
  //在异步传输状态时，关闭等待信号（仅适用于异步闪存）
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;                //不使能环回模式
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//在等待状态前发送等待信号  
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;     //设置存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;            //关闭等待信号
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;        //不使用时序扩展模式，使用读写同时序 
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;            //不使用突发写模式
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;        //设置读写访问时序（关闭扩展模式情况下）
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;            //设置写访问时序（打开扩展模式情况下）

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //根据参数初始化FSMC配置

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  //使能BANK1的区域3
  
  for(i = 0; i < 250000; i++) //i从0计数到249999
  {
    s_arrSRAM[i] = i;         //i存放到数组s_arrSRAM[i]                   
  }
}
    
/*********************************************************************************************************
* 函数名称: ExtSRAMTestWrite
* 函数功能: 测试函数,在指定地址写入1个字节
* 输入参数: addr:地址,data:要写入的数据
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意:
*********************************************************************************************************/
void ExtSRAMTestWrite(u32 addr, u8 data)
{         
  ExtSRAMWriteBuf(&data, addr, 1);  //从地址addr处开始写入1字节的数据data（&data为数据的地址）
}

/*********************************************************************************************************
* 函数名称: ExtSRAMTestWrite
* 函数功能: 读取1个字节
* 输入参数: addr:地址
* 输出参数: void
* 返 回 值: 读取到的数据
* 创建日期: 2018年03月01日
* 注    意:
*********************************************************************************************************/
u8 ExtSRAMTestRead(u32 addr)
{
  u8 data;                        //定义一个u8类型的变量data作为数据储器
  
  ExtSRAMReadBuf(&data, addr, 1); //从地址addr处开始读出1字节的数据，存放到data                 
  
  return data;                    //返回data
}  
   
/*********************************************************************************************************
* 函数名称: ExtSRAMTest1
* 函数功能: 外部内存测试(最大支持1M字节内存测试)
* 输入参数: x,y
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意:
*********************************************************************************************************/
void ExtSRAMTest1(u16 x, u16 y)
{  
  u32  i = 0;                           //定义一个u32类型的变量i，作为循环计数器
  u8  temp = 0;                         //定义一个u8类型的变量temp，作为数据储器
  u8  sval = 0;                         //定义一个u8类型的变量sval，记录在地址0处读到的数据               
  
  //在LCD上的指定起点坐标x，y处的一块区域大小为width：239，heigh：y + 16的地方
  //显示字体大小为16的字符串"Ex Memory Test:   0KB"
  LCDShowString(x, y, 239, y + 16, 16, "Ex Memory Test:   0KB");
    
  for(i = 0; i < 1024 * 1024; i += 4096)//从0地址开始，每隔4K字节,写入一个数据,总共写入256个数据,刚好是1M字节
  {
    ExtSRAMWriteBuf(&temp, i, 1);       //在地址i处写入1字节的数据temp
    
    temp++;                             //temp加1
  }
        
  for(i = 0; i < 1024 * 1024; i += 4096)//依次读出之前写入的数据,进行校验 
  {
    ExtSRAMReadBuf(&temp, i, 1);        //从地址i处读出1字节的数据放入temp
    
    if(i == 0)
    {
      sval = temp;                      //记录在地址0读到的数据到sval
    }
    else if(temp <= sval)               //后面读出的某个数据比地址0处读到的数据小
    {
      break;
    } 
 
    //在LCD上的指定起点坐标x + 15 * 8，y处显示位数为4，字体大小为16的数字(u16)(temp - sval + 1) * 4
    //其中0代表不填充的非叠加显示，即在“Test：”后面显示内存的容量
    LCDShowxNum(x + 15 * 8, y, (u16)(temp - sval + 1) * 4, 4, 16, 0);  
  }  
}

/*********************************************************************************************************
* 函数名称: ExtSRAMTest2
* 函数功能: 外部内存测试(最大支持1M字节内存测试)
* 输入参数: void
* 输出参数: void
* 返 回 值: 读取到的数据
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
void ExtSRAMTest2(void)
{  
  i32 i;                                                  //定义一个i32类型的变量i，作为循环计数器
      
  //此处是一个BUG，在使用ExtSRAMTest1后,再用ExtSRAMTest2函数会出现每隔1024出现一次i与s_arrSRAM[i]不相符，
  //其中s_arrSRAM[i] - i = n，n为第几次出现BUG，所以通过重新写入屏蔽此BUG，可以顺利显示到249999
  //（原因是在用ExtSRAMTest1的赋值中给每1024个元素低8位加上了n）
  for(i = 0; i < 250000; i += 1024)
  {
    s_arrSRAM[i] = i;
  }
  
  for(i = 0; i < 250000; i++)                             //从数组s_arrSRAM[]的0到249999位置依次对比
  {
    if(i != s_arrSRAM[i])                                 //与预先存入的数据不符
    {
      //在LCD上的指定起点坐标x：30，y：210处的一块区域大小为width：200，heigh：16的地方显示字符串"SRAM Test Failed!"
      LCDShowString(30, 210, 200, 16, 16, "SRAM Test Failed!");

      break;
    }
   
    //在LCD上的指定位置x：30，y：190处显示位数为6，字体大小为16的数字s_arrSRAM[i]
    LCDShowxNum(30, 190, s_arrSRAM[i], 6, 16, 0);
  }
}   
