﻿/*********************************************************************************************************
* 模块名称: LED.h
* 摘    要: LED驱动
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
#ifndef _LED_H_
#define _LED_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define USER_DEFINE_LED0_GPIO_PORT   GPIOB                    //用户定义LED0的GPIO端口
#define USER_DEFINE_LED1_GPIO_PORT   GPIOE                    //用户定义LED1的GPIO端口
                                                              
#define USER_DEFINE_LED0_GPIO_PIN    GPIO_Pin_5               //用户定义LED0的引脚
#define USER_DEFINE_LED1_GPIO_PIN    GPIO_Pin_5               //用户定义LED1的引脚
                                                              
#define USER_DEFINE_LED0_GPIO_CLK    RCC_APB2Periph_GPIOB     //用户定义LED0的时钟
#define USER_DEFINE_LED1_GPIO_CLK    RCC_APB2Periph_GPIOE     //用户定义LED1的时钟

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void  InitLED(void);                    //初始化LED
void  LEDFlicker(u16 cnt);              //LED闪烁

#endif
