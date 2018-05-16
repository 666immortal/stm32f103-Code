/*********************************************************************************************************
* 模块名称: PWM.c
* 摘    要: 
* 当前版本: 1.0.0
* 作    者: 
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
#include "PWM.h"
#include "stm32f10x_tim.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static  i16 s_iDutyCycle = 0;  //用于存放占空比
       
/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
#ifdef USE_DEFINE_TIM3_PB0_OUTPUT_PWM
static  void ConfigTimer3ForPWMPB0(u16 arr, u16 psc);    //TIM3.CH3的PB0用于产生PWM波
#endif

#ifdef USE_DEFINE_TIM3_PB5_OUTPUT_PWM
static  void ConfigTimer3ForPWMPB5(u16 arr, u16 psc);    //TIM3.CH2部分重映像PB5用于产生PWM波
#endif

#ifdef USER_DEFINE_TIM3_PC6_OUTPUT_PWM
static  void ConfigTimer3ForPWMPC6(u16 arr, u16 psc);    //TIM3.CH1完全重映像PC6用于产生PWM波
#endif

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: ConfigTimer3ForPWMPB00
* 函数功能: TIM3-CH3-PB0输出，TIM3没有重映像 
* 输入参数: arr：自动重装值；psc：时钟预分频数
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
#ifdef USE_DEFINE_TIM3_PB0_OUTPUT_PWM

static  void ConfigTimer3ForPWMPB0(u16 arr, u16 psc)
{
  GPIO_InitTypeDef GPIO_InitStructure;            //定义结构体GPIO_InitStructure,用来配置TIM3的GPIO
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; //定义结构体TIM_TimeBaseStructure,用来配置TIM3的基本参数
  TIM_OCInitTypeDef  TIM_OCInitStructure;         //定义结构体TIM_OCInitStructure,用来配置TIM3的通道参数

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    //使能TIMI3的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //使能TIM3.CH3的引脚时钟

  //设置该引脚为复用推挽输出模式,输出TIM3.CH3的PWM脉冲波形
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;             //设置TIM3.CH3的引脚 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;        //复用推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //设置I/O口速率为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);                  //根据参数初始化TIM3的GPIO端口
 
  TIM_TimeBaseStructure.TIM_Period        = arr;          //设置自动重装载周期值
  TIM_TimeBaseStructure.TIM_Prescaler     = psc;          //设置预分频值为不分频
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;            //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //设置计数模式为向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);         //根据指定的参数初始化TIM3的基本参数
  
  //PWM2模式，TIM_CounterMode_Up模式下，TIM3_CNT < TIM3_CCRx时为无效电平（低电平）
  TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM2;        //选择PWM2模式
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  TIM_OCInitStructure.TIM_Pulse       = 0;                      //设置待装入捕获比较寄存器的脉冲值
  TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;    //设置极性，OC2高电平有效 
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);                      //根据指定的参数初始化外设TIM3的通道

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);             //TIM3.CH3通道预装载使能
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);                           //使能TIM3在ARR上的预装载寄存器
  
  TIM_Cmd(TIM3, ENABLE);                                        //使能TIM3
}

#endif

/*********************************************************************************************************
* 函数名称: ConfigTimer3ForPWMPB05
* 函数功能: TIM3-CH2-PB5输出，TIM3部分映像 
* 输入参数: arr：自动重装值；psc：时钟预分频数
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
#ifdef USE_DEFINE_TIM3_PB5_OUTPUT_PWM

static  void ConfigTimer3ForPWMPB5(u16 arr, u16 psc)
{
  GPIO_InitTypeDef GPIO_InitStructure;            //定义结构体GPIO_InitStructure,用来配置TIM3的GPIO
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; //定义结构体TIM_TimeBaseStructure,用来配置TIM3的基本参数
  TIM_OCInitTypeDef  TIM_OCInitStructure;         //定义结构体TIM_OCInitStructure,用来配置TIM3的通道参数

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    //使能TIM3的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //使能TIM3.CH2的引脚时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //使能AFIO复用功能模块时钟
  //注意：GPIO_PinRemapConfig必须放在RCC_APBXPeriphClockCmd的后面才可以
  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);    //TIM3部分重映射TIM3.CH2->PB5

  //设置该引脚为复用推挽输出模式,输出TIM3.CH2的PWM脉冲波形
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;             //设置TIM3.CH2的引脚  
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;        //复用推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //设置I/O口速率为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);                  //根据参数初始化TIM3的GPIO端口
 
  TIM_TimeBaseStructure.TIM_Period        = arr;          //设置自动重装载周期值
  TIM_TimeBaseStructure.TIM_Prescaler     = psc;          //设置预分频值为不分频
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;            //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //设置计数模式为向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);         //根据指定的参数初始化TIM3的基本参数
  
  //PWM2模式，TIM_CounterMode_Up模式下，TIMx_CNT < TIMx_CCRx时为无效电平（高电平）  
  TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM2;        //选择PWM2模式
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  //TIM_OCInitStructure.TIM_Pulse       = 0;                    //设置待装入捕获比较寄存器的脉冲值
  TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;    //设置极性，OC2低电平有效 
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);                      //根据参数初始化TIM3的通道

  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);             //TIM.CH2通道预装载使能
  
  //TIM_ARRPreloadConfig(TIM3, ENABLE);                         //使能TIM3在ARR上的预装载寄存器
  
  TIM_Cmd(TIM3, ENABLE);                                        //使能TIM3
}

#endif

/*********************************************************************************************************
* 函数名称: ConfigTimer3ForPWMPC06
* 函数功能: TIM3-CH1-PC06输出，TIM3完全映像 
* 输入参数: arr：自动重装值；psc：时钟预分频数
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
#ifdef USER_DEFINE_TIM3_PC6_OUTPUT_PWM

static  void ConfigTimer3ForPWMPC6(u16 arr, u16 psc)
{
  GPIO_InitTypeDef GPIO_InitStructure;            //定义结构体GPIO_InitStructure,用来配置TIM3的GPIO
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; //定义结构体TIM_TimeBaseStructure,用来配置TIM3的基本参数
  TIM_OCInitTypeDef  TIM_OCInitStructure;         //定义结构体TIM_OCInitStructure,用来配置TIM3的通道参数

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    //使能TIM3的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   //使能TIM3.CH1的引脚时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //使能AFIO复用功能模块时钟
  //注意：GPIO_PinRemapConfig必须放在RCC_APBXPeriphClockCmd的后面才可以
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);       //TIM3全映射TIM3.CH1->PC6

  //设置该引脚为复用推挽输出模式,输出TIM3.CH1的PWM脉冲波形
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;             //设置TIM3.CH1的引脚 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;        //复用推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //设置I/O口速率为50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);                  //根据参数初始化TIM3的GPIO端口
 
  TIM_TimeBaseStructure.TIM_Period        = arr;          //设置自动重装载周期值
  TIM_TimeBaseStructure.TIM_Prescaler     = psc;          //设置预分频值为不分频
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;            //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //设置计数模式为向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);         //根据参数初始化TIM3的基本参数
  
  //PWM1模式，TIM_CounterMode_Up模式下，TIMx_CNT < TIMx_CCRx时为有效电平（低电平）  
  TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;        //选择PWM1模式
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  TIM_OCInitStructure.TIM_Pulse       = 0;                      //设置待装入捕获比较寄存器的脉冲值
  TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_Low;     //设置极性，OC1 低电平有效 
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);                      //根据参数初始化外设TIM3的通道

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);             //TIM3.CH1通道预装载使能
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);                           //使能TIM3在ARR上的预装载寄存器
  
  TIM_Cmd(TIM3, ENABLE);                                        //使能TIM3
}

#endif

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: InitPWM
* 函数功能: 初始化PWM
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
void InitPWM(void)
{
  #ifdef USE_DEFINE_TIM3_PB0_OUTPUT_PWM
  ConfigTimer3ForPWMPB0(599, 999);  //TIM3 PWM初始化, 72000000/(999+1)/(599+1) = 120Hz
  TIM_SetCompare3(TIM3, 0);         //初始值为0
  #endif

  #ifdef USE_DEFINE_TIM3_PB5_OUTPUT_PWM
  ConfigTimer3ForPWMPB5(599, 999);  //TIM3 PWM初始化, 72000000/(999+1)/(599+1) = 120Hz
  TIM_SetCompare2(TIM3, 0);         //初始值为0
  #endif

  #ifdef USER_DEFINE_TIM3_PC6_OUTPUT_PWM
  ConfigTimer3ForPWMPC6(599, 999);  //TIM3 PWM初始化, 72000000/(999+1)/(599+1) = 120Hz
  TIM_SetCompare1(TIM3, 0);         //初始值为0
  #endif
}

/*********************************************************************************************************
* 函数名称: SetPWM
* 函数功能: 设置占空比
* 输入参数: val：占空比
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
void  SetPWM(i16 val)
{
  s_iDutyCycle = val;                   //获取占空比的值
  
  #ifdef USE_DEFINE_TIM3_PB0_OUTPUT_PWM
  TIM_SetCompare3(TIM3, s_iDutyCycle);	//设置占空比
  #endif
    
  #ifdef USE_DEFINE_TIM3_PB5_OUTPUT_PWM
  TIM_SetCompare2(TIM3, s_iDutyCycle);	//设置占空比	
  #endif

  #ifdef USER_DEFINE_TIM3_PC6_OUTPUT_PWM
  TIM_SetCompare1(TIM3, s_iDutyCycle);	//设置占空比	
  #endif
}

/*********************************************************************************************************
* 函数名称: IncPWMDutyCycle
* 函数功能: 递增占空比，每次递增10，一直增到高电平输出为止（KEY02-PE02） 
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
void  IncPWMDutyCycle(void)
{
  if(s_iDutyCycle >= 600)               //如果占空比不小于600
  {
    s_iDutyCycle = 600;                 //保持占空比值为600
  }
  else
  {
    s_iDutyCycle += 10;                 //占空比加10
  }

  #ifdef USE_DEFINE_TIM3_PB0_OUTPUT_PWM
  TIM_SetCompare3(TIM3, s_iDutyCycle);	//设置占空比	
  #endif
    
  #ifdef USE_DEFINE_TIM3_PB5_OUTPUT_PWM
  TIM_SetCompare2(TIM3, s_iDutyCycle);	//设置占空比	
  #endif

  #ifdef USER_DEFINE_TIM3_PC6_OUTPUT_PWM
  TIM_SetCompare1(TIM3, s_iDutyCycle);	//设置占空比	
  #endif
}

/*********************************************************************************************************
* 函数名称: DecPWMDutyCycle
* 函数功能: 递减占空比，每次递减10，一直减到低电平输出为止（KEY01-PE04） 
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意: 
*********************************************************************************************************/
void  DecPWMDutyCycle(void)
{
  if(s_iDutyCycle <= 0)                 //如果占空比不大于0
  {                                     
    s_iDutyCycle = 0;                   //保持占空比值为0
  }                                     
  else                                  
  {                                     
    s_iDutyCycle = s_iDutyCycle - 10;   //占空比减10
  }
  
  #ifdef USE_DEFINE_TIM3_PB0_OUTPUT_PWM
  TIM_SetCompare3(TIM3, s_iDutyCycle);	//设置占空比	
  #endif
    
  #ifdef USE_DEFINE_TIM3_PB5_OUTPUT_PWM
  TIM_SetCompare2(TIM3, s_iDutyCycle);	//设置占空比	
  #endif

  #ifdef USER_DEFINE_TIM3_PC6_OUTPUT_PWM
  TIM_SetCompare1(TIM3, s_iDutyCycle);	//设置占空比	
  #endif
}
