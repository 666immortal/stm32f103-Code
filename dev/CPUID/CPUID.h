/*********************************************************************************************************
* ģ������: CPUID.h
* ժ    Ҫ: CPUID����
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
#ifndef _CPU_ID_H_
#define _CPU_ID_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/


/*********************************************************************************************************
*                                              �ṹ�嶨��
*********************************************************************************************************/


/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void  InitCPUID(void);          //��ʼ��CPUID
void  ReadCPUID(u8* pMCUISPID); //��ȡCPUID
void  CheckCPUID(void);         //���STM32��ID�Ƿ�ƥ�䣬ֻ��ƥ�������²��ܼ�������ִ��

#endif