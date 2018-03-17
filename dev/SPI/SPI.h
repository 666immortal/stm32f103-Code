/*********************************************************************************************************
* ģ������: SPI.h
* ժ    Ҫ: SPI����
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
#ifndef _SPI_H_
#define _SPI_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define USER_DEFINE_SPI_CS_GPIO_PORT    GPIOB                   //�û�����SPI.CS��GPIO�˿�
#define USER_DEFINE_SPI_CS_GPIO_PIN     GPIO_Pin_12             //�û�����SPI.CS������
#define USER_DEFINE_SPI_CS_GPIO_CLK     RCC_APB2Periph_GPIOB    //�û�����SPI.CS��ʱ��

#define USER_DEFINE_SPI_SCK_GPIO_PORT   GPIOB                   //�û�����SPI.SCK��GPIO�˿�
#define USER_DEFINE_SPI_SCK_GPIO_PIN    GPIO_Pin_13             //�û�����SPI.SCK������
#define USER_DEFINE_SPI_SCK_GPIO_CLK    RCC_APB2Periph_GPIOB    //�û�����SPI.SCK��ʱ��

#define USER_DEFINE_SPI_MISO_GPIO_PORT  GPIOB                   //�û�����SPI.MISO��GPIO�˿�
#define USER_DEFINE_SPI_MISO_GPIO_PIN   GPIO_Pin_14             //�û�����SPI.MISO������
#define USER_DEFINE_SPI_MISO_GPIO_CLK   RCC_APB2Periph_GPIOB    //�û�����SPI.MISO��ʱ��

#define USER_DEFINE_SPI_MOSI_GPIO_PORT  GPIOB                   //�û�����SPI.MOSI��GPIO�˿�
#define USER_DEFINE_SPI_MOSI_GPIO_PIN   GPIO_Pin_15             //�û�����SPI.MOSI������
#define USER_DEFINE_SPI_MOSI_GPIO_CLK   RCC_APB2Periph_GPIOB    //�û�����SPI.MOSI��ʱ��

#define USER_DEFINE_SPI_PORT            SPI2                    //�û�����SPI�Ķ˿�
#define USER_DEFINE_SPI_CLK             RCC_APB1Periph_SPI2     //�û�����SPI��ʱ��
#define USER_DEFINE_SPI_CPOL            SPI_CPOL_High           //�û�����SPI��ʱ�Ӽ���
#define USER_DEFINE_SPI_CPHA            SPI_CPHA_2Edge          //�û�����SPI��ʱ����λ

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/ 				  	    													  
void  InitSPI(void);                //��ʼ��SPI�˿�
void  SPISetSpeed(u8 SpeedSet);     //����SPI�ٶ�   
u8    SPIReadWriteByte(u8 TxData);  //SPI���߶�дһ���ֽ�
 
#endif
