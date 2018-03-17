/*********************************************************************************************************
* ģ������: SPI.c
* ժ    Ҫ: 
* ��ǰ�汾: 1.0.0
* ��    ��: 
* �������: 2018��03��01�� 
* ��    ��: ������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ
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
#include "SPI.h"
#include "stm32f10x.h"

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
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitSPI
* ��������: ��ʼ��SPI
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void InitSPI(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;//����ṹ��GPIO_InitStructure,��������SPI��GPIO
  SPI_InitTypeDef  SPI_InitStructure; //����ṹSPI_InitStructure,��������SPI�Ĳ���

  if(USER_DEFINE_SPI_PORT == SPI1)    //����û�ѡ���SPI�˿�ΪSPI1
  {
    RCC_APB2PeriphClockCmd(USER_DEFINE_SPI_CLK,  ENABLE); //SPI1ʱ��ʹ��
  }
  else
  {
    RCC_APB1PeriphClockCmd(USER_DEFINE_SPI_CLK,  ENABLE); //SPI2/SPI3ʱ��ʹ��
  }
 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 //�����������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;               //����I/O������Ϊ50MHz
  
  RCC_APB2PeriphClockCmd(USER_DEFINE_SPI_SCK_GPIO_CLK, ENABLE);   //ʹ��SPI.SCK��ʱ��  
  GPIO_InitStructure.GPIO_Pin = USER_DEFINE_SPI_SCK_GPIO_PIN;     //����SPI.SCK������
  GPIO_Init(USER_DEFINE_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);  //���ݲ�����ʼ��SPI.SCK��GPIO�˿�

  RCC_APB2PeriphClockCmd(USER_DEFINE_SPI_MISO_GPIO_CLK, ENABLE);  //ʹ��SPI.MISO��ʱ��   
  GPIO_InitStructure.GPIO_Pin = USER_DEFINE_SPI_MISO_GPIO_PIN;    //����SPI.MISO������
  GPIO_Init(USER_DEFINE_SPI_MISO_GPIO_PORT, &GPIO_InitStructure); //���ݲ�����ʼ��SPI.MISO��GPIO�˿�

  RCC_APB2PeriphClockCmd(USER_DEFINE_SPI_MOSI_GPIO_CLK, ENABLE);  //ʹ��SPI.MOSI��ʱ�� 
  GPIO_InitStructure.GPIO_Pin = USER_DEFINE_SPI_MOSI_GPIO_PIN;    //����SPI.MOSI������
  GPIO_Init(USER_DEFINE_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure); //���ݲ�����ʼ��SPI.MOSI��GPIO�˿�

  GPIO_SetBits(USER_DEFINE_SPI_SCK_GPIO_PORT, USER_DEFINE_SPI_SCK_GPIO_PIN);  //��ʼ��SPI.SCK����ߵ�ƽ
  GPIO_SetBits(USER_DEFINE_SPI_MISO_GPIO_PORT, USER_DEFINE_SPI_MISO_GPIO_PIN);//��ʼ��SPI.MISO����ߵ�ƽ
  GPIO_SetBits(USER_DEFINE_SPI_MOSI_GPIO_PORT, USER_DEFINE_SPI_MOSI_GPIO_PIN);//��ʼ��SPI.MOSI����ߵ�ƽ

  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;       //����SPI����ģʽ:����Ϊ��SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
  SPI_InitStructure.SPI_CPOL = USER_DEFINE_SPI_CPOL;  //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
  SPI_InitStructure.SPI_CPHA = USER_DEFINE_SPI_CPHA;  //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
  //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   
  //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  
  //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  
  SPI_InitStructure.SPI_CRCPolynomial = 7;            //CRCֵ����Ķ���ʽ
  SPI_Init(USER_DEFINE_SPI_PORT, &SPI_InitStructure); //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
  SPI_Cmd(USER_DEFINE_SPI_PORT, ENABLE);              //ʹ��SPI����
  
  SPIReadWriteByte(0xff);                             //��������
}   
  
/*********************************************************************************************************
* ��������: SPISetSpeed
* ��������: ����SPI������
* �������: SPI_BaudRatePrescaler��SPI�Ĳ����ʷ�Ƶֵ
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: SPI �ٶ����ú���
*           SpeedSet:
*           SPI_BaudRatePrescaler_2   2��Ƶ   
*           SPI_BaudRatePrescaler_8   8��Ƶ   
*           SPI_BaudRatePrescaler_16  16��Ƶ  
*           SPI_BaudRatePrescaler_256 256��Ƶ       
*********************************************************************************************************/
void SPISetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); //�ж�����Ĳ����Ƿ���ȷ
  
  USER_DEFINE_SPI_PORT->CR1 &= 0XFFC7;                            //��ʼ��GPIO���ʵļĴ���                     
  USER_DEFINE_SPI_PORT->CR1 |= SPI_BaudRatePrescaler;             //����SPI2���� 
  
  SPI_Cmd(USER_DEFINE_SPI_PORT, ENABLE);                          //ʹ��SPI����
} 

/*********************************************************************************************************
* ��������: SPIReadWriteByte
* ��������: ��дһ���ֽ� 
* �������: TxData:Ҫд����ֽ�
* �������: void
* �� �� ֵ: ��ȡ�����ֽ�
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
u8 SPIReadWriteByte(u8 TxData)
{
  u8 reTry = 0;       //���Ա�־λ
  
  //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
  while(SPI_I2S_GetFlagStatus(USER_DEFINE_SPI_PORT, SPI_I2S_FLAG_TXE) == RESET) 
  {
    reTry++;          //ÿѭ��һ�μ�1
    
    if(reTry > 200)   //���ʱ�����
    {
      return 0;       //��д����ʧ��
    }
  }

  SPI_I2S_SendData(USER_DEFINE_SPI_PORT, TxData);     //ͨ������SPIx����һ������
  
  reTry = 0;          //����

  //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
  while (SPI_I2S_GetFlagStatus(USER_DEFINE_SPI_PORT, SPI_I2S_FLAG_RXNE) == RESET) 
  {
    reTry++;          //ÿѭ��һ�μ�1
    
    if(reTry > 200)   //���ʱ�����
    {
      return 0;       //��д����ʧ��
    }
  }
  
  return SPI_I2S_ReceiveData(USER_DEFINE_SPI_PORT);   //����ͨ��SPIx������յ�����
}
