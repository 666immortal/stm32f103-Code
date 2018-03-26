/*********************************************************************************************************
* ģ������: Capture.c
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
#include "Capture.h"
#include "stm32f10x_tim.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
//s_iCaptureSts�е�bit7��Ϊ������ɵı�־��bit6������ߵ�ƽ��־��bit5-0������ߵ�ƽ��ʱ������Ĵ���
static  u8  s_iCaptureSts = 0;     //����״̬ 
static  u16 s_iCaptureVal;         //����ֵ
 
/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static  void ConfigTIMxForCapture(u16 arr, u16 psc);    //���ö�ʱ��TIMx��ͨ�����벶��

/*********************************************************************************************************
* ��������: ConfigTIMxForCapture
* ��������: ���ö�ʱ��x��ͨ�����벶�� 
* �������: arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: �˴��ݶ�ʹ�ö�ʱ��TIM5��ͨ��1��PA0���������벶�񣬲��� PA0 �ϸߵ�ƽ��������KEY_UP��������ߵ�ƽ��
*********************************************************************************************************/
static  void ConfigTIMxForCapture(u16 arr, u16 psc)
{ 
  GPIO_InitTypeDef        GPIO_InitStructure;    //����ṹ��GPIO_InitStructure���������ò���ͨ����GPIO
  TIM_TimeBaseInitTypeDef TIMx_TimeBaseStructure;//����ṹ��TIMx_TimeBaseStructure���������ö�ʱ��TIMx�Ĳ���
  TIM_ICInitTypeDef       TIMx_ICInitStructure;  //����ṹTIMx_ICInitStructure,������ʼ������ͨ��
  NVIC_InitTypeDef        NVIC_InitStructure;    //����ṹ��NVIC_InitStructure�����������ж�NVIC�Ĳ���

  RCC_APB1PeriphClockCmd(USER_DEFINE_CAPTURE_TIM_CLK, ENABLE);                //ʹ�ܶ�ʱ��TIMx��ʱ��
  RCC_APB2PeriphClockCmd(USER_DEFINE_CAPTURE_GPIO_CLK, ENABLE);               //ʹ�ܲ����GPIO�˿�ʱ��
  
  GPIO_InitStructure.GPIO_Pin  = USER_DEFINE_CAPTURE_GPIO_PIN;                //���ò���ͨ����Ӧ������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;                               //��������ģʽ 
  GPIO_Init(USER_DEFINE_CAPTURE_GPIO_PORT, &GPIO_InitStructure);              //���ݲ�����ʼ�������GPIO�˿�
  GPIO_ResetBits(USER_DEFINE_CAPTURE_GPIO_PORT, USER_DEFINE_CAPTURE_GPIO_PIN);//�������Ӧ��������Ϊ�͵�ƽ
  
  //��ʱ��TIMx��ʼ��
  TIMx_TimeBaseStructure.TIM_Period        = arr;                     //�趨�������Զ���װֵ 
  TIMx_TimeBaseStructure.TIM_Prescaler     = psc;                     //����TIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  
  TIMx_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //����ʱ�ӷָ�
  TIMx_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;      //���ö�ʱ��TIMxΪ���ϼ���ģʽ
  TIM_TimeBaseInit(USER_DEFINE_CAPTURE_TIM, &TIMx_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
  //����TIMx�Ĳ���ͨ������
  //CC1S = 01��CC1ͨ��������Ϊ���� ������ͨ��IC1ӳ�䵽��ʱ������TI1��
  TIMx_ICInitStructure.TIM_Channel        = USER_DEFINE_CAPTURE_CHANNEL; 
  TIMx_ICInitStructure.TIM_ICPolarity     = TIM_ICPolarity_Rising;    //���ó�ʼ��Ϊ�����ز���
  TIMx_ICInitStructure.TIM_ICSelection    = TIM_ICSelection_DirectTI; //����Ϊֱ��ӳ�䵽TI1��
  TIMx_ICInitStructure.TIM_ICPrescaler    = TIM_ICPSC_DIV1;           //����ÿһ�������ض����񣬲�׽����Ƶ 
  TIMx_ICInitStructure.TIM_ICFilter       = 0x08;                     //���������˲���
  TIM_ICInit(USER_DEFINE_CAPTURE_TIM, &TIMx_ICInitStructure);         //��ʼ��TIMx���벶�����
  
  //�жϷ����ʼ��
  NVIC_InitStructure.NVIC_IRQChannel                   = USER_DEFINE_CAPTURE_TIM_IRQn;//���ö�ʱ��TIMx���ж�ͨ��
//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;           //������ռ���ȼ�Ϊ2
//NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;           //���ô����ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;      //ʹ���ж�NVIC���ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);                                     //���ݲ�����ʼ���ж�NVIC�Ĵ��� 
  
  //��������жϣ�����CC1IE�����жϣ�������Ч�źſ��Կ����жϣ�
  TIM_ITConfig(USER_DEFINE_CAPTURE_TIM, TIM_IT_Update | TIM_IT_CC1, ENABLE);  
  
  TIM_Cmd(USER_DEFINE_CAPTURE_TIM, ENABLE);                                   //ʹ�ܶ�ʱ��TIMx
}

/*********************************************************************************************************
* ��������: USER_DEFINE_CAPTURE_TIM_IRQHandler
* ��������: ��ʱ��TIMx�жϷ������
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void USER_DEFINE_CAPTURE_TIM_IRQHandler(void)
{ 
  if((s_iCaptureSts & 0x80) == 0)                             //���λΪ0����ʾ����δ���
  {  
    //�ߵ�ƽ����ʱ��TIMx����������¼�
    if (TIM_GetITStatus(USER_DEFINE_CAPTURE_TIM, TIM_IT_Update) != RESET)        
    {    
      if(s_iCaptureSts & 0x40)                                //�������������ǰһ���Ѿ����񵽸ߵ�ƽ��
      {
        //TIM_APR 16λԤװ��ֵ����CNT > 65536-1��2^16 - 1��ʱ�����
        //��������(s_iCaptureSts & 0X3F)++����0x40 �������������0
        if((s_iCaptureSts & 0x3F) == 0x3F)                    //�ﵽ���������ߵ�ƽ̫����
        {
          s_iCaptureSts |= 0x80;                              //ǿ�Ʊ�ǳɹ�������һ��
          s_iCaptureVal = 0xFFFF;                             //����ֵΪ0xFFFF
        } 
        else
        {
          s_iCaptureSts++;                                    //��Ǽ��������һ��
        }
      }
    }
    
    if (TIM_GetITStatus(USER_DEFINE_CAPTURE_TIM, TIM_IT_CC1) != RESET) //���������¼�
    { 
      if(s_iCaptureSts & 0x40)                            //bit6Ϊ1�����ϴβ��������أ���ô��β����½���
      {
        s_iCaptureSts |= 0x80;                            //��ɲ��񣬱�ǳɹ�����һ���½���
        s_iCaptureVal = TIM_GetCapture1(USER_DEFINE_CAPTURE_TIM);  //s_iCaptureVa��¼����ȽϼĴ�����ֵ
        //CC1P=0 ����Ϊ�����ز���Ϊ�´β�����׼��
        TIM_OC1PolarityConfig(USER_DEFINE_CAPTURE_TIM, TIM_ICPolarity_Rising);   
      }
      else                                                //bit6Ϊ0����ʾ�ϴ�û���������أ����ǵ�һ�β���������
      {
        s_iCaptureSts = 0;                                //����������
        s_iCaptureVal = 0;                                //����ֵΪ0
                                                             
        TIM_SetCounter(USER_DEFINE_CAPTURE_TIM, 0);       //���üĴ�����ֵΪ0
                                                             
        s_iCaptureSts |= 0x40;                            //bit6��Ϊ1����ǲ�����������
        
        TIM_OC1PolarityConfig(USER_DEFINE_CAPTURE_TIM, TIM_ICPolarity_Falling);  //CC1P=1 ����Ϊ�½��ز���
      }    
    } 
  }
 
  TIM_ClearITPendingBit(USER_DEFINE_CAPTURE_TIM, TIM_IT_CC1 | TIM_IT_Update);    //����жϱ�־λ
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitCapture
* ��������: Capture��ʼ������ 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void  InitCapture(void)
{
  //�������ﵽ���ת��ֵ0xFFFF��������������72MHz / ��72-1 + 1�� = 1Mhz��Ƶ�ʼ���
  ConfigTIMxForCapture(0xFFFF, 72-1);  
}

/*********************************************************************************************************
* ��������: GetCaptureVal
* ��������: ��ȡ����ʱ�䣬����ֵΪ1��ʾ����ɹ�����ʱ*pCapVal�������� 
* �������: void
* �������: pCalVal�����񵽵�ֵ�ĵ�ַ
* �� �� ֵ: ok��1-��ȡ�ɹ�
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
u8   GetCaptureVal(i32* pCapVal)
{
  u8  ok = 0;
  
  if(s_iCaptureSts & 0x80)              //���λΪ1����ʾ�ɹ�������һ��������
  {
    ok = 1;                             //����ɹ�
    (*pCapVal)  = s_iCaptureSts & 0x3F; //ȡ����6λ��������ֵ����(*pCapVal)���õ��������
    (*pCapVal) *= 65536;//��������������Ϊ2^16 = 65536����������������õ����ʱ���ܺͣ���1 / 1MHz = 1us Ϊ��λ��
    (*pCapVal) += s_iCaptureVal;        //�������һ�αȽϲ���Ĵ�����ֵ���õ��ܵĸߵ�ƽʱ��

    s_iCaptureSts = 0;                  //����Ϊ0��������һ�β���
  }

  return(ok);                           //�����Ƿ񲶻�ɹ��ı�־
}
