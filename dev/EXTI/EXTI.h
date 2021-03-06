﻿/*********************************************************************************************************
* 模块名称: EXTI.h
* 摘    要: 外部中断EXTI驱动
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
#ifndef _EXTI_H_
#define _EXTI_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define USER_DEFINE_KEY0_EXIT_LINE        EXTI_Line4            //用户定义按键KEY0外部中断线

#define USER_DEFINE_KEY0_EXIT_PORTSOURCE  GPIO_PortSourceGPIOE  //用户定义按键KEY0外部中断的事件输出GPIO端口
#define USER_DEFINE_KEY0_EXIT_PINSOURCE   GPIO_PinSource4       //用户定义按键KEY0外部中断的事件输出引脚
                                                                
#define USER_DEFINE_KEY0_EXIT_IRQ         EXTI4_IRQn            //用户定义按键KEY0外部中断的中断通道
#define USER_DEFINE_KEY0_EXIT_IRQHandler  EXTI4_IRQHandler      //用户定义按键KEY0外部中断的中断服务函数
                                                                                                                             
#define USER_DEFINE_KEY2_EXIT_LINE        EXTI_Line2            //用户定义按键KEY2外部中断线

#define USER_DEFINE_KEY2_EXIT_PORTSOURCE  GPIO_PortSourceGPIOE  //用户定义按键KEY2外部中断的事件输出GPIO端口
#define USER_DEFINE_KEY2_EXIT_PINSOURCE   GPIO_PinSource2       //用户定义按键KEY2外部中断的事件输出引脚
                                                                
#define USER_DEFINE_KEY2_EXIT_IRQ         EXTI2_IRQn            //用户定义按键KEY2外部中断的中断通道
#define USER_DEFINE_KEY2_EXIT_IRQHandler  EXTI2_IRQHandler      //用户定义按键KEY2外部中断的中断服务函数

/*********************************************************************************************************
*                                              结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void	InitEXTI(void);		//初始化外部中断EXTI

u8    GetKey0ExitFlag(void);  //获取按键KEY0外部中断标志 
void  ClrKey0ExitFlag(void);  //清除按键KEY0外部中断标志

u8    GetKey2ExitFlag(void);  //获取按键KEY2外部中断标志 
void  ClrKey2ExitFlag(void);  //清除按键KEY2外部中断标志

#endif
