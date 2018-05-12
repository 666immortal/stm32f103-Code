/*********************************************************************************************************
* 模块名称: SPI.h
* 摘    要: SPI驱动
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
#ifndef _SPI_H_
#define _SPI_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define USER_DEFINE_SPI_CS_GPIO_PORT    GPIOB                   //用户定义SPI.CS的GPIO端口
#define USER_DEFINE_SPI_CS_GPIO_PIN     GPIO_Pin_12             //用户定义SPI.CS的引脚
#define USER_DEFINE_SPI_CS_GPIO_CLK     RCC_APB2Periph_GPIOB    //用户定义SPI.CS的时钟

#define USER_DEFINE_SPI_SCK_GPIO_PORT   GPIOB                   //用户定义SPI.SCK的GPIO端口
#define USER_DEFINE_SPI_SCK_GPIO_PIN    GPIO_Pin_13             //用户定义SPI.SCK的引脚
#define USER_DEFINE_SPI_SCK_GPIO_CLK    RCC_APB2Periph_GPIOB    //用户定义SPI.SCK的时钟

#define USER_DEFINE_SPI_MISO_GPIO_PORT  GPIOB                   //用户定义SPI.MISO的GPIO端口
#define USER_DEFINE_SPI_MISO_GPIO_PIN   GPIO_Pin_14             //用户定义SPI.MISO的引脚
#define USER_DEFINE_SPI_MISO_GPIO_CLK   RCC_APB2Periph_GPIOB    //用户定义SPI.MISO的时钟

#define USER_DEFINE_SPI_MOSI_GPIO_PORT  GPIOB                   //用户定义SPI.MOSI的GPIO端口
#define USER_DEFINE_SPI_MOSI_GPIO_PIN   GPIO_Pin_15             //用户定义SPI.MOSI的引脚
#define USER_DEFINE_SPI_MOSI_GPIO_CLK   RCC_APB2Periph_GPIOB    //用户定义SPI.MOSI的时钟

#define USER_DEFINE_SPI_PORT            SPI2                    //用户定义SPI的端口
#define USER_DEFINE_SPI_CLK             RCC_APB1Periph_SPI2     //用户定义SPI的时钟
#define USER_DEFINE_SPI_CPOL            SPI_CPOL_High           //用户定义SPI的时钟极性
#define USER_DEFINE_SPI_CPHA            SPI_CPHA_2Edge          //用户定义SPI的时钟相位

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/ 				  	    													  
void  InitSPI(void);                //初始化SPI端口
void  SPISetSpeed(u8 SpeedSet);     //设置SPI速度   
u8    SPIReadWriteByte(u8 TxData);  //SPI总线读写一个字节
 
#endif
