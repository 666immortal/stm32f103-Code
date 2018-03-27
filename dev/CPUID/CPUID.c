/*********************************************************************************************************
* ģ������: CPUID.c
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
#include "CPUID.h"
#include "UART.h"
#include <stm32f10x_conf.h>

/*********************************************************************************************************
*                                              �궨��
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
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitCPUID
* ��������: ��ʼ��CPUID 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��:
*********************************************************************************************************/
void  InitCPUID(void)
{
  
}

/*********************************************************************************************************
* ��������: ReadCPUID
* ��������: ����mcuisp�ĸ�ʽȡ��CPUID 
* �������: void
* �������: ����mcuisp�ĸ�ʽ��ţ�arrId[11]Ϊ���λ
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��:
*********************************************************************************************************/
void  ReadCPUID(u8* pMCUISPID)
{
  u32 arrID[3];                                 //32λ��CPUID
     
  //�ڹ̶���ַ��ȡSTM32Ψһ��CPUID
  arrID[0] = *(vu32*)(0x1FFFF7E8);              //31-00
  arrID[1] = *(vu32*)(0x1FFFF7EC);              //63-32
  arrID[2] = *(vu32*)(0x1FFFF7F0);              //95-64
  
  //����MCUISP�ĸ�ʽ���
  pMCUISPID[11] = (arrID[0] & 0x000000FF) >> 0; 
  pMCUISPID[10] = (arrID[0] & 0x0000FF00) >> 8;
  pMCUISPID[9]  = (arrID[0] & 0x00FF0000) >> 16;
  pMCUISPID[8]  = (arrID[0] & 0xFF000000) >> 24;

  pMCUISPID[7]  = (arrID[1] & 0x000000FF) >> 0;
  pMCUISPID[6]  = (arrID[1] & 0x0000FF00) >> 8;
  pMCUISPID[5]  = (arrID[1] & 0x00FF0000) >> 16;
  pMCUISPID[4]  = (arrID[1] & 0xFF000000) >> 24;

  pMCUISPID[3]  = (arrID[2] & 0x000000FF) >> 0;
  pMCUISPID[2]  = (arrID[2] & 0x0000FF00) >> 8;
  pMCUISPID[1]  = (arrID[2] & 0x00FF0000) >> 16;
  pMCUISPID[0]  = (arrID[2] & 0xFF000000) >> 24; 
}

/*********************************************************************************************************
* ��������: CheckCPUID
* ��������: ���STM32��ID�Ƿ�ƥ�䣬���ƥ���������ִ�У��������while��ѭ�� 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void  CheckCPUID(void)
{
  i8  i;                              //ѭ��������
  u8  arrMCUISPID[12];                //���ڴ��STM32��CPUID
  u8  idOk = FALSE;                   //������־λ����ʾCPUIDƥ���Ƿ�ɹ���0-ʧ�ܣ�1-�ɹ�

  u8  permitCPUID1[12];               //�洢DLʹ�õ�ϵͳ���CPUID
  u8  permitCPUID2[12];               //�洢LYQʹ�õ�ϵͳ���CPUID
  u8  permitCPUID3[12];               //�洢ս�����CPUID

  //DLʹ�õ�ϵͳ���CPUID
  permitCPUID1[11] = 0X30;            //96λ��оƬΨһ���кŵ������һ���ֽ�
  permitCPUID1[10] = 0XFF;
  permitCPUID1[9]  = 0XD7;
  permitCPUID1[8]  = 0X05;
  permitCPUID1[7]  = 0X43;
  permitCPUID1[6]  = 0X4E;
  permitCPUID1[5]  = 0X35;
  permitCPUID1[4]  = 0X38;
  permitCPUID1[3]  = 0X41;
  permitCPUID1[2]  = 0X77;
  permitCPUID1[1]  = 0X25;
  permitCPUID1[0]  = 0X43;            //96λ��оƬΨһ���кŵ����Ҷ�һ���ֽ�
    
  //LYQʹ�õ�ϵͳ���CPUID
  permitCPUID2[11] = 0X30;            //96λ��оƬΨһ���кŵ������һ���ֽ�
  permitCPUID2[10] = 0XFF;
  permitCPUID2[9]  = 0XD6;
  permitCPUID2[8]  = 0X05;
  permitCPUID2[7]  = 0X43;
  permitCPUID2[6]  = 0X4E;
  permitCPUID2[5]  = 0X35;
  permitCPUID2[4]  = 0X38;
  permitCPUID2[3]  = 0X22;
  permitCPUID2[2]  = 0X75;
  permitCPUID2[1]  = 0X23; 
  permitCPUID2[0]  = 0X43;            //96λ��оƬΨһ���кŵ����Ҷ�һ���ֽ�

  //ս�����CPUID
  permitCPUID3[11] = 0X32;            //96λ��оƬΨһ���кŵ������һ���ֽ�
  permitCPUID3[10] = 0XFF;
  permitCPUID3[9]  = 0XD7;
  permitCPUID3[8]  = 0X05;
  permitCPUID3[7]  = 0X4D;
  permitCPUID3[6]  = 0X59;
  permitCPUID3[5]  = 0X32;
  permitCPUID3[4]  = 0X32;
  permitCPUID3[3]  = 0X18;
  permitCPUID3[2]  = 0X71;
  permitCPUID3[1]  = 0X10;
  permitCPUID3[0]  = 0X51;            //96λ��оƬΨһ���кŵ����Ҷ�һ���ֽ�

  ReadCPUID(arrMCUISPID);             //��ȡ��ǰCPU��ID�����arrMCUISPID������
  
  printf("The CPUID is:");            //��ӡ��ʾ��Ϣ��The CPUID is:��
  
  for(i = 11; i >= 0; i--)            //ѭ����CPUID��ӡ���
  {
    printf("%02x", arrMCUISPID[i]);   //��CPUIDͨ��ʮ�����Ƶĸ�ʽ��ӡ���
  }
  
  printf("\r\n");                     //�������
  
  //����1���������е�CPUID
  for(i = 0; i < 12; i++)
  {
    if (permitCPUID1[i] == arrMCUISPID[i])      //DL��ϵͳ��CPUID���ȡ����CPUID�Ƚ�
    {
      if(11 == i)                               
      {
        idOk = TRUE;                            //CPUIDƥ��ɹ�
        printf("DL ID!\r\n");                   //�����DL ID��
      }    
    }
    else
    {
      break;                                    //CPUID��ƥ�䣬����ѭ��
    }
  }

  //����2���������е�CPUID
  for(i = 0; i < 12; i++)
  {
    if (permitCPUID2[i] == arrMCUISPID[i])      //LYQ��ϵͳ��CPUID���ȡ����CPUID�Ƚ�
    {
      if(11 == i)
      {
        idOk = TRUE;                            //CPUIDƥ��ɹ�
        printf("LYQ ID!\r\n");                  //�����LYQ ID��
      }    
    }
    else
    {
      break;                                    //CPUID��ƥ�䣬����ѭ��
    }
  }

  //����3���������е�CPUID
  for(i = 0; i < 12; i++)
  {
    if (permitCPUID3[i] == arrMCUISPID[i])      //ս�����CPUID���ȡ����CPUID�Ƚ�
    {
      if(11 == i)
      {
        idOk = TRUE;                            //CPUIDƥ��ɹ�
        printf("This is STM32 ID!\r\n");        //�����This is STM32 ID��
      }    
    }
    else
    {
      break;                                    //CPUID��ƥ�䣬����ѭ��
    }
  }

  //CPUID��ƥ�䣬�������ѭ��
  if(TRUE != idOk) 
  {
    printf("CheckCPUID failed!\r\n");           //�����CheckCPUID failed!��
    while(1);                                   //������ѭ��
  }
}