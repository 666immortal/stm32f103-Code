/*********************************************************************************************************
* 模块名称: EXTI.c
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
#include "EXTI.h"
#include "UART.h"
#include "DataType.h"
#include "KeyOne.h"
#include <stm32f10x_conf.h>
#include "LED.h"
#include "Timer.h"
#include "SysTick.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static u8 s_iKey0ExitFlag = FALSE;    //按键KEY0外部中断标志设置为FALSE
static u8 s_iKey2ExitFlag = FALSE;    //按键KEY2外部中断标志设置为FALSE

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static  void  ConfigEXTI(void);            //配置外部中断EXTI

/*********************************************************************************************************
* 函数名称: ConfigEXTI
* 函数功能: 配置外部中断EXTI 
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
static  void ConfigEXTI(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;          //定义结构体EXTI_InitStructure,用来配置外部中断EXTI的参数
  NVIC_InitTypeDef NVIC_InitStructure;          //定义结构体NVIC_InitStructure，用来配置中断NVIC的参数

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);             //使能AFIO复用功能模块的时钟
  
  //配置使用按键KEY0外部中断线
  GPIO_EXTILineConfig(USER_DEFINE_KEY0_EXIT_PORTSOURCE, USER_DEFINE_KEY0_EXIT_PINSOURCE);  

  EXTI_InitStructure.EXTI_Line    = USER_DEFINE_KEY0_EXIT_LINE;      //设置按键KEY0外部中断线
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;             //设置为中断请求             
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            //设置为下降沿触发模式
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                          //使能中断线
  EXTI_Init(&EXTI_InitStructure);                                    //根据参数初始化外设EXTI寄存器
                                                               
  NVIC_InitStructure.NVIC_IRQChannel        = USER_DEFINE_KEY0_EXIT_IRQ;  //设置按键KEY0外部中断线的通道
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x03;           //设置抢占优先级3
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x02;           //设置子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd     = ENABLE;                     //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);                                         //根据参数初始化中断NVIC的寄存器

  //配置使用按键KEY2外部中断线
  GPIO_EXTILineConfig(USER_DEFINE_KEY2_EXIT_PORTSOURCE, USER_DEFINE_KEY2_EXIT_PINSOURCE);          

  EXTI_InitStructure.EXTI_Line    = USER_DEFINE_KEY2_EXIT_LINE;      //选择按键KEY2外部中断线
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;             //设置为中断请求
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            //设置为下降沿触发模式
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                          //使能中断线
  EXTI_Init(&EXTI_InitStructure);                                    //根据参数初始化外设EXTI寄存器
 
  NVIC_InitStructure.NVIC_IRQChannel        = USER_DEFINE_KEY2_EXIT_IRQ;  //设置按键KEY2外部中断线的通道
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x03;           //设置抢占优先级3 
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x02;           //设置子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd     = ENABLE;                     //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);                                         //根据参数初始化中断NVIC的寄存器
}

/*********************************************************************************************************
* 函数名称: USER_DEFINE_KEY0_EXIT_IRQHandler
* 函数功能: 按键KEY0外部中断的中断服务函数 
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
void USER_DEFINE_KEY0_EXIT_IRQHandler(void)
{
  if(EXTI_GetITStatus(USER_DEFINE_KEY0_EXIT_LINE) == SET)   //检测到按键KEY0中断线上有中断发生
  { 
    //按键被按下
    if(USER_DEFINE_KEY_DOWN_LEVEL_KEY0 == GPIO_ReadInputDataBit(USER_DEFINE_KEY0_GPIO_PORT, USER_DEFINE_KEY0_GPIO_PIN))
    {            
      s_iKey0ExitFlag = TRUE;    //按键被按下且产生外部中断
    }
  }

  EXTI_ClearITPendingBit(USER_DEFINE_KEY0_EXIT_LINE);     //清除按键KEY0中断线上的中断标志位
}

/*********************************************************************************************************
* 函数名称: USER_DEFINE_KEY2_EXIT_IRQHandler
* 函数功能: 按键KEY2外部中断的中断服务函数
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
void USER_DEFINE_KEY2_EXIT_IRQHandler(void)
{
  if(EXTI_GetITStatus(USER_DEFINE_KEY2_EXIT_LINE) == SET)  //检测到按键KEY2中断线上有中断发生
  { 
    //按键被按下
    if(USER_DEFINE_KEY_DOWN_LEVEL_KEY2 == GPIO_ReadInputDataBit(USER_DEFINE_KEY2_GPIO_PORT, USER_DEFINE_KEY2_GPIO_PIN))
    {
      s_iKey2ExitFlag = TRUE;  //按键被按下且产生外部中断
    }
  }
  
  EXTI_ClearITPendingBit(USER_DEFINE_KEY2_EXIT_LINE);    //清除按键KEY2中断线上的中断标志位                                            
}

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: InitEXTI
* 函数功能: 初始化外部中断EXTI 
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 将所有的EXTI初始化函数都放在此函数中
*********************************************************************************************************/
void InitEXTI(void)
{
  ConfigEXTI();      //配置外部中断EXTI 
}

/*********************************************************************************************************
* 函数名称: GetKey0ExitFlag
* 函数功能: 获取按键KEY0外部中断标志  
* 输入参数: void
* 输出参数: void
* 返 回 值: s_iKey0ExitFlag
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
u8  GetKey0ExitFlag(void)
{
  return(s_iKey0ExitFlag);    //返回按键KEY0外部中断标志
}

/*********************************************************************************************************
* 函数名称: ClrKey0ExitFlag
* 函数功能: 清除按键KEY0外部中断标志 
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
void  ClrKey0ExitFlag(void)
{
  s_iKey0ExitFlag = FALSE;    //将按键KEY0外部中断标志设置为FALSE
}

/*********************************************************************************************************
* 函数名称: GetKey2ExitFlag
* 函数功能: 获取按键KEY2外部中断标志  
* 输入参数: void
* 输出参数: void
* 返 回 值: s_iKey2ExitFlag
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
u8  GetKey2ExitFlag(void)
{
  return(s_iKey2ExitFlag);    //返回按键KEY2外部中断标志
}

/*********************************************************************************************************
* 函数名称: ClrKey2ExitFlag
* 函数功能: 清除按键KEY2外部中断标志   
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
void  ClrKey2ExitFlag(void)
{
  s_iKey2ExitFlag = FALSE;    //将按键KEY2外部中断标志设置为FALSE
}
