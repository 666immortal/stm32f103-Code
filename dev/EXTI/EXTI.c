/*********************************************************************************************************
* ģ������: EXTI.c
* ժ    Ҫ: 
* ��ǰ�汾: 1.0.0
* ��    ��: 
* �������: 2018��03��01��
* ��    ��:
* ע    ��: none                                                                  
**********************************************************************************************************
* ȡ���汾: 
* ��    ��:
* �������: 
* �޸�����:
* �޸��ļ�: 
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
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
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static u8 s_iKey0ExitFlag = FALSE;    //����KEY0�ⲿ�жϱ�־����ΪFALSE
static u8 s_iKey2ExitFlag = FALSE;    //����KEY2�ⲿ�жϱ�־����ΪFALSE

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static  void  ConfigEXTI(void);            //�����ⲿ�ж�EXTI

/*********************************************************************************************************
* ��������: ConfigEXTI
* ��������: �����ⲿ�ж�EXTI 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  void ConfigEXTI(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;          //����ṹ��EXTI_InitStructure,���������ⲿ�ж�EXTI�Ĳ���
  NVIC_InitTypeDef NVIC_InitStructure;          //����ṹ��NVIC_InitStructure�����������ж�NVIC�Ĳ���

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);             //ʹ��AFIO���ù���ģ���ʱ��
  
  //����ʹ�ð���KEY0�ⲿ�ж���
  GPIO_EXTILineConfig(USER_DEFINE_KEY0_EXIT_PORTSOURCE, USER_DEFINE_KEY0_EXIT_PINSOURCE);  

  EXTI_InitStructure.EXTI_Line    = USER_DEFINE_KEY0_EXIT_LINE;      //���ð���KEY0�ⲿ�ж���
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;             //����Ϊ�ж�����             
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            //����Ϊ�½��ش���ģʽ
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                          //ʹ���ж���
  EXTI_Init(&EXTI_InitStructure);                                    //���ݲ�����ʼ������EXTI�Ĵ���
                                                               
  NVIC_InitStructure.NVIC_IRQChannel        = USER_DEFINE_KEY0_EXIT_IRQ;  //���ð���KEY0�ⲿ�ж��ߵ�ͨ��
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x03;           //������ռ���ȼ�3
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x02;           //���������ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd     = ENABLE;                     //ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);                                         //���ݲ�����ʼ���ж�NVIC�ļĴ���

  //����ʹ�ð���KEY2�ⲿ�ж���
  GPIO_EXTILineConfig(USER_DEFINE_KEY2_EXIT_PORTSOURCE, USER_DEFINE_KEY2_EXIT_PINSOURCE);          

  EXTI_InitStructure.EXTI_Line    = USER_DEFINE_KEY2_EXIT_LINE;      //ѡ�񰴼�KEY2�ⲿ�ж���
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;             //����Ϊ�ж�����
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            //����Ϊ�½��ش���ģʽ
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                          //ʹ���ж���
  EXTI_Init(&EXTI_InitStructure);                                    //���ݲ�����ʼ������EXTI�Ĵ���
 
  NVIC_InitStructure.NVIC_IRQChannel        = USER_DEFINE_KEY2_EXIT_IRQ;  //���ð���KEY2�ⲿ�ж��ߵ�ͨ��
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x03;           //������ռ���ȼ�3 
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x02;           //���������ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd     = ENABLE;                     //ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);                                         //���ݲ�����ʼ���ж�NVIC�ļĴ���
}

/*********************************************************************************************************
* ��������: USER_DEFINE_KEY0_EXIT_IRQHandler
* ��������: ����KEY0�ⲿ�жϵ��жϷ����� 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void USER_DEFINE_KEY0_EXIT_IRQHandler(void)
{
  if(EXTI_GetITStatus(USER_DEFINE_KEY0_EXIT_LINE) == SET)   //��⵽����KEY0�ж��������жϷ���
  { 
    //����������
    if(USER_DEFINE_KEY_DOWN_LEVEL_KEY0 == GPIO_ReadInputDataBit(USER_DEFINE_KEY0_GPIO_PORT, USER_DEFINE_KEY0_GPIO_PIN))
    {            
      s_iKey0ExitFlag = TRUE;    //�����������Ҳ����ⲿ�ж�
    }
  }

  EXTI_ClearITPendingBit(USER_DEFINE_KEY0_EXIT_LINE);     //�������KEY0�ж����ϵ��жϱ�־λ
}

/*********************************************************************************************************
* ��������: USER_DEFINE_KEY2_EXIT_IRQHandler
* ��������: ����KEY2�ⲿ�жϵ��жϷ�����
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void USER_DEFINE_KEY2_EXIT_IRQHandler(void)
{
  if(EXTI_GetITStatus(USER_DEFINE_KEY2_EXIT_LINE) == SET)  //��⵽����KEY2�ж��������жϷ���
  { 
    //����������
    if(USER_DEFINE_KEY_DOWN_LEVEL_KEY2 == GPIO_ReadInputDataBit(USER_DEFINE_KEY2_GPIO_PORT, USER_DEFINE_KEY2_GPIO_PIN))
    {
      s_iKey2ExitFlag = TRUE;  //�����������Ҳ����ⲿ�ж�
    }
  }
  
  EXTI_ClearITPendingBit(USER_DEFINE_KEY2_EXIT_LINE);    //�������KEY2�ж����ϵ��жϱ�־λ                                            
}

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitEXTI
* ��������: ��ʼ���ⲿ�ж�EXTI 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: �����е�EXTI��ʼ�����������ڴ˺�����
*********************************************************************************************************/
void InitEXTI(void)
{
  ConfigEXTI();      //�����ⲿ�ж�EXTI 
}

/*********************************************************************************************************
* ��������: GetKey0ExitFlag
* ��������: ��ȡ����KEY0�ⲿ�жϱ�־  
* �������: void
* �������: void
* �� �� ֵ: s_iKey0ExitFlag
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
u8  GetKey0ExitFlag(void)
{
  return(s_iKey0ExitFlag);    //���ذ���KEY0�ⲿ�жϱ�־
}

/*********************************************************************************************************
* ��������: ClrKey0ExitFlag
* ��������: �������KEY0�ⲿ�жϱ�־ 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void  ClrKey0ExitFlag(void)
{
  s_iKey0ExitFlag = FALSE;    //������KEY0�ⲿ�жϱ�־����ΪFALSE
}

/*********************************************************************************************************
* ��������: GetKey2ExitFlag
* ��������: ��ȡ����KEY2�ⲿ�жϱ�־  
* �������: void
* �������: void
* �� �� ֵ: s_iKey2ExitFlag
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
u8  GetKey2ExitFlag(void)
{
  return(s_iKey2ExitFlag);    //���ذ���KEY2�ⲿ�жϱ�־
}

/*********************************************************************************************************
* ��������: ClrKey2ExitFlag
* ��������: �������KEY2�ⲿ�жϱ�־   
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void  ClrKey2ExitFlag(void)
{
  s_iKey2ExitFlag = FALSE;    //������KEY2�ⲿ�жϱ�־����ΪFALSE
}
