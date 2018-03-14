/*********************************************************************************************************
* ģ������: UART.h
* ժ    Ҫ: UART����
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
#ifndef _UART_H_
#define _UART_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include <stdio.h>
#include "DataType.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define UART_BUF_SIZE 100   //���û������Ĵ�С

#define USER_DEFINE_USART                        USART1                        //�û�����USART�Ķ˿�  
#define USER_DEFINE_USART_CLK                    RCC_APB2Periph_USART1         //�û�����USART��ʱ�� 
#define USER_DEFINE_USART_BAUDRATE               115200                        //�û�����USART�Ĳ�����
#define USER_DEFINE_USART_WORDLENGTH             USART_WordLength_8b           //�û�����USART������ֳ�
#define USER_DEFINE_USART_STOPBITS               USART_StopBits_1              //�û�����USART��ֹͣλ
#define USER_DEFINE_USART_PARITY                 USART_Parity_No               //�û�����USART����żУ��λ
#define USER_DEFINE_USART_MODE                   USART_Mode_Rx | USART_Mode_Tx //�û�����USART��ģʽ
#define USER_DEFINE_USART_HRDWAREFLOWCONTROL     USART_HardwareFlowControl_None//�û�����USART��Ӳ��������
  
#define USER_DEFINE_USART_TX_GPIO_PORT           GPIOA                   //�û�����USART.TX��GPIO�˿�
#define USER_DEFINE_USART_TX_GPIO_PIN            GPIO_Pin_9              //�û�����USART.TX������
#define USER_DEFINE_USART_TX_GPIO_CLK            RCC_APB2Periph_GPIOA    //�û�����USART.TX��ʱ��
#define USER_DEFINE_USART_TX_MODE                GPIO_Mode_AF_PP         //�û�����USART.TX��ģʽ
#define USER_DEFINE_USART_TX_SPEED               GPIO_Speed_50MHz        //�û�����USART.TX��I/O������
  
#define USER_DEFINE_USART_RX_GPIO_PORT           GPIOA                   //�û�����USART.RX��GPIO�˿� 
#define USER_DEFINE_USART_RX_GPIO_PIN            GPIO_Pin_10             //�û�����USART.RX������
#define USER_DEFINE_USART_RX_GPIO_CLK            RCC_APB2Periph_GPIOA    //�û�����USART.RX��ʱ��
#define USER_DEFINE_USART_RX_MODE                GPIO_Mode_IN_FLOATING   //�û�����USART.RX��ģʽ
#define USER_DEFINE_USART_RX_SPEED               GPIO_Speed_50MHz        //�û�����USART.RX��I/O������
  
#define USER_DEFINE_USART_IRQHandler             USART1_IRQHandler       //�û����崮�ڵ��жϷ�����
#define USER_DEFINE_USART_IRQ                    USART1_IRQn             //�û����崮�ڵ��ж�ͨ��

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void  InitUART(void);               //��ʼ��������ȱʡ�򿪴���
u8    WriteUART(u8* pBuf, u8 len);  //д���ڣ����سɹ�д������ݳ���
u8    ReadUART(u8* pBuf, u8 len);   //�����ڣ����ض��������ݳ���

#endif
