/*********************************************************************************************************
* ģ������: DAC.c
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
#include "DAC.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/


/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/


/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/


/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitDAC
* ��������: ��ʼ��DACͨ��1���
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void InitDAC(void)
{ 
  DAC_InitTypeDef  DAC_InitStructure;     //����ṹ��DAC_InitStructure����������DAC  
	GPIO_InitTypeDef GPIO_InitStructure;    //����ṹ��GPIO_InitStructure����������DACͨ����GPIO
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	        //ʹ��DAC��ʱ��
	RCC_APB2PeriphClockCmd(USER_DEFINE_DAC_GPIO_CLK, ENABLE );    //ʹ��DACͨ����GPIO�˿�ʱ��
   
	GPIO_InitStructure.GPIO_Pin = USER_DEFINE_DAC_GPIO_PIN;				//����GPIO������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		            //ģ������ģʽ
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //����I/O������50MHz
 	GPIO_Init(USER_DEFINE_DAC_GPIO, &GPIO_InitStructure);         //���ݲ�����ʼ��GPIO
  
	GPIO_SetBits(USER_DEFINE_DAC_GPIO, USER_DEFINE_DAC_GPIO_PIN); //����ָ�������ݶ˿�λ
					
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;	                        //���ò�ʹ�ô�������
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;           //���ò�ʹ�ò��η���
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; //���Ρ���ֵ����
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable ;	          //DAC�������ر� BOFF1=1
  DAC_Init(USER_DEFINE_DAC_CHANNEL, &DAC_InitStructure);	                  //��ʼ��DACͨ��

	DAC_Cmd(USER_DEFINE_DAC_CHANNEL, ENABLE);            //ʹ��DAC
  
  if(USER_DEFINE_DAC_CHANNEL == DAC_Channel_1)         //����ͨ��1
  {
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);           //����DACͨ��1��ֵ���Ҳ���12λ�Ҷ������ݸ�ʽ
  }
  else if(USER_DEFINE_DAC_CHANNEL == DAC_Channel_2)    //����ͨ��2
  {
    DAC_SetChannel2Data(DAC_Align_12b_R, 0);           //����DACͨ��2��ֵ���Ҳ���12λ�Ҷ������ݸ�ʽ
  }   
}

/*********************************************************************************************************
* ��������: SetDACVol
* ��������: ����DACͨ�������ѹ
* �������: volnum:0-3300,����0-3.3V
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void SetDACVol(u16 volnum)
{
	float temp = volnum;                               //�����������ֵ��temp
  
	temp /= 1000;                                      //tempֵΪ���õĵ�ѹֵ
  
	temp = temp * 4096 / 3.3;                          //ͨ����ʽ����DOR�Ĵ�����ֵ
  
  if(USER_DEFINE_DAC_CHANNEL == DAC_Channel_1)       //����ͨ��1
  {
    DAC_SetChannel1Data(DAC_Align_12b_R, temp);      //����DACͨ��1��ֵ���Ҳ���12λ�Ҷ������ݸ�ʽ
  }
  else if(USER_DEFINE_DAC_CHANNEL == DAC_Channel_2)  //����ͨ��2
  {
    DAC_SetChannel2Data(DAC_Align_12b_R, temp);      //����DACͨ��2��ֵ���Ҳ���12λ�Ҷ������ݸ�ʽ
  }
}