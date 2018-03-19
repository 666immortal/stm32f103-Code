/*********************************************************************************************************
* ģ������: W25Qxx.c
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
#include "W25Qxx.h" 
#include "SPI.h"
#include "SysTick.h"   

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
//W25Qxxָ���
#define W25X_WRITE_ENABLE            0x06     //дʹ��
#define W25X_WRITE_DISABLE           0x04     //д����
#define W25X_READ_STS_REG            0x05     //��״̬�Ĵ���
#define W25X_WRITE_STS_REG           0x01     //д״̬�Ĵ���
#define W25X_READ_DATA               0x03     //������
#define W25X_FAST_READ_DATA          0x0B     //���ٶ�����
#define W25X_FAST_READ_DUAL          0x3B     //˫���ٶȶ�����
#define W25X_PAGE_PROGRAM            0x02     //дҳ����
#define W25X_BLOCK_ERASE             0xD8     //������
#define W25X_SECTOR_ERASE            0x20     //��������
#define W25X_CHIP_ERASE              0xC7     //����оƬ
#define W25X_POWER_DOWN              0xB9     //����ģʽ
#define W25X_RELEASE_POWER_DOWN      0xAB     //�ͷŵ���ģʽ
#define W25X_DEVICE_ID               0xAB     //�豸ID
#define W25X_MANUFACT_DEVICE_ID      0x90     //������ID
#define W25X_JEDEC_DEVICE_ID         0x9F     //JEDEC��׼ID

//ѡ��W25Qxx��Ƭѡ
#define CLR_W25Qxx_CS()	GPIO_ResetBits(USER_DEFINE_SPI_CS_GPIO_PORT, USER_DEFINE_SPI_CS_GPIO_PIN)
//ȡ��W25Qxx��Ƭѡ
#define SET_W25Qxx_CS()	GPIO_SetBits  (USER_DEFINE_SPI_CS_GPIO_PORT, USER_DEFINE_SPI_CS_GPIO_PIN)

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static u8 SPI_FLASH_BUF[4096];                //SPI������

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static  u8   W25QxxReadSR(void);              //��״̬�Ĵ��� 
static  void W25QxxWriteSR(u8 sr);            //д״̬�Ĵ���

static  void W25QxxWriteEnable(void);         //дʹ�� 
static  void W25QxxWriteDisable(void);        //д����

static  void W25QxxWaitBusy(void);            //�ȴ�����

//��ָ����ַ��ʼд�����256�ֽڵ�����
static  void W25QxxWritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
//�޼���дW25Qxx,��ָ����ַ��ʼд��ָ�����ȵ�����
static  void W25QxxWriteNoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);

static  void W25QxxEraseChip(void);           //��Ƭ����
static  void W25QxxEraseSector(u32 Dst_Addr); //��������

static  void W25QxxPowerDown(void);           //�������ģʽ
static  void W25QxxWakeUp(void);              //����

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: W25QxxReadSR
* ��������: ��ȡW25Qxx��״̬�Ĵ��� 
* �������: void
* �������: void
* �� �� ֵ: ״̬�Ĵ���
* ��������: 2018��03��01��
* ע    ��: BIT7  6   5   4   3   2   1   0
*           SPR   RV  TB BP2 BP1 BP0 WEL BUSY
*           SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
*           TB,BP2,BP1,BP0:FLASH����д��������
*           WEL:дʹ������
*           BUSY:æ���λ(1,æ;0,����)
*           Ĭ��:0x00
*********************************************************************************************************/
static  u8 W25QxxReadSR(void)   
{  
  u8 byte=0;                                //������żĴ�����״̬
  
  CLR_W25Qxx_CS();                          //ѡ��W25Qxx��Ƭѡ
  SPIReadWriteByte(W25X_READ_STS_REG);      //���Ͷ�ȡ״̬�Ĵ�������    
  byte = SPIReadWriteByte(0Xff);            //��ȡһ���ֽ�
  SET_W25Qxx_CS();                          //ȡ��W25Qxx��Ƭѡ

  return byte;                              //���ؼĴ�����״̬
} 

/*********************************************************************************************************
* ��������: W25QxxWriteSR
* ��������: дW25Qxx״̬�Ĵ��� 
* �������: sr����д���״̬�Ĵ���
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д
*********************************************************************************************************/
static  void W25QxxWriteSR(u8 sr)   
{   
  CLR_W25Qxx_CS();                          //ѡ��W25Qxx��Ƭѡ
  SPIReadWriteByte(W25X_WRITE_STS_REG);     //����дȡ״̬�Ĵ�������    
  SPIReadWriteByte(sr);                     //д��һ���ֽ�  
  SET_W25Qxx_CS();                          //ȡ��W25Qxx��Ƭѡ
} 
  
/*********************************************************************************************************
* ��������: W25QxxWriteEnable
* ��������: дʹ�ܣ���WEL��λ 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��:   
*********************************************************************************************************/ 
static  void W25QxxWriteEnable(void)   
{
  CLR_W25Qxx_CS();                            //ѡ��W25Qxx��Ƭѡ
  SPIReadWriteByte(W25X_WRITE_ENABLE);        //����дʹ��ָ��
  SET_W25Qxx_CS();                            //ȡ��W25Qxx��Ƭѡ
} 

/*********************************************************************************************************
* ��������: W25QxxWriteDisable
* ��������: д��ֹ����WEL���� 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/ 
static  void W25QxxWriteDisable(void)   
{  
  CLR_W25Qxx_CS();                            //ѡ��W25Qxx��Ƭѡ
  SPIReadWriteByte(W25X_WRITE_DISABLE);       //����д��ָֹ��    
  SET_W25Qxx_CS();                            //ȡ��W25Qxx��Ƭѡ
}
 
/*********************************************************************************************************
* ��������: W25QxxWritePage
* ��������: ��һҳ(0~65535)��д������256���ֽڵ����ݣ���ָ����ַ��ʼд�����256�ֽڵ����� 
* �������: pBuffer:���ݴ洢����WriteAddr:��ʼд��ĵ�ַ(24bit)
*           NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  void W25QxxWritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u16 i;                                      //ѭ��������
  W25QxxWriteEnable();                        //дʹ�ܣ�����WEL��λ 
  CLR_W25Qxx_CS();                            //ѡ��W25Qxx��Ƭѡ

  SPIReadWriteByte(W25X_PAGE_PROGRAM);        //����дҳ����   
  SPIReadWriteByte((u8)((WriteAddr) >> 16));  //����24bit��ַ    
  SPIReadWriteByte((u8)((WriteAddr) >> 8));   
  SPIReadWriteByte((u8)WriteAddr);   
  for(i = 0; i < NumByteToWrite; i++)
  {
    SPIReadWriteByte(pBuffer[i]);             //ѭ��д��
  }
  
  SET_W25Qxx_CS();                            //ȡ��W25Qxx��Ƭѡ

  W25QxxWaitBusy();                           //�ȴ�д�����
} 

/*********************************************************************************************************
* ��������: W25QXXWriteNoCheck
* ��������: �޼���дW25Qxx,��ָ����ַ��ʼд��ָ�����ȵ�����
* �������: pBuffer:���ݴ洢��,WriteAddr:��ʼд��ĵ�ַ(24bit),NumByteToWrite:Ҫд����ֽ���(���65535)
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
*           �����Զ���ҳ���� 
*           ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
*********************************************************************************************************/
static  void W25QxxWriteNoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)   
{
  u16 pageRemain;                       //���ڴ�ŵ�ҳʣ����ֽ���
  pageRemain = 256 - WriteAddr % 256;   //��ҳʣ����ֽ���
  if(NumByteToWrite <= pageRemain)
  {
    pageRemain = NumByteToWrite;        //Ҫд����ֽ���������256���ֽ�
  }
  
  while(1)
  { 
    //��ָ����ַ��ʼд������  
    W25QxxWritePage(pBuffer, WriteAddr, pageRemain);
    
    if(NumByteToWrite == pageRemain)
    {
      break;                            //д�����
    }
    else
    {
      pBuffer   += pageRemain;
      WriteAddr += pageRemain;

      NumByteToWrite -= pageRemain;     //��ȥ�Ѿ�д���˵��ֽ���
      
      if(NumByteToWrite > 256)
      {
        pageRemain = 256;               //һ�ο���д��256���ֽ�
      }
      else
      {
        pageRemain = NumByteToWrite;    //����256���ֽ���
      }
    }
  }
} 

/*********************************************************************************************************
* ��������: W25QXXEraseChip
* ��������: ��������оƬ 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: W25X16:25s 
*           W25X32:40s 
*           W25X64:40s 
*           �ȴ�ʱ�䳬��
*********************************************************************************************************/
static  void W25QxxEraseChip(void)   
{                                             
  W25QxxWriteEnable();                  //дʹ�ܣ�����WEL��λ
  W25QxxWaitBusy();                     //�ȴ�����   
  CLR_W25Qxx_CS();                      //ѡ��W25Qxx��Ƭѡ
  SPIReadWriteByte(W25X_CHIP_ERASE);    //����Ƭ��������  
  SET_W25Qxx_CS();                      //ȡ��W25Qxx��Ƭѡ

  W25QxxWaitBusy();                     //�ȴ�оƬ�������
}  
 
/*********************************************************************************************************
* ��������: W25QXXEraseSector
* ��������: ����һ������ 
* �������: Dst_Addr:������ַ 0~511 for w25x16
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: ����һ������������ʱ��:150ms
*********************************************************************************************************/
static  void W25QxxEraseSector(u32 Dst_Addr)   
{   
  Dst_Addr *= 4096;
  W25QxxWriteEnable();                        //дʹ�ܣ�����WEL��λ
  W25QxxWaitBusy();                           //�ȴ�����
  CLR_W25Qxx_CS();                            //ѡ��W25Qxx��Ƭѡ
  SPIReadWriteByte(W25X_SECTOR_ERASE);        //������������ָ�� 
  SPIReadWriteByte((u8)((Dst_Addr) >> 16));   //����24bit��ַ    
  SPIReadWriteByte((u8)((Dst_Addr) >> 8));   
  SPIReadWriteByte((u8)Dst_Addr);  
  SET_W25Qxx_CS();                            //ȡ��W25Qxx��Ƭѡ
  W25QxxWaitBusy();                           //�ȴ��������
}  

/*********************************************************************************************************
* ��������: W25QXXWaitBusy
* ��������: �ȴ����� 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  void W25QxxWaitBusy(void)   
{   
  while((W25QxxReadSR() & 0x01) == 0x01)     //�ȴ�BUSYλ���
  {
    
  }
}
  
/*********************************************************************************************************
* ��������: W25QXXPowerDown
* ��������: �������ģʽ 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  void W25QxxPowerDown(void)   
{ 
  CLR_W25Qxx_CS();                            //ѡ��W25Qxx��Ƭѡ
  SPIReadWriteByte(W25X_POWER_DOWN);          //���͵���ģʽ����  
  SET_W25Qxx_CS();                            //ȡ��W25Qxx��Ƭѡ
  DelayNus(3);                                //��ʱ3us���ȴ�TPD  
}
   
/*********************************************************************************************************
* ��������: W25QXXWakeUp
* ��������: ����
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  void W25QxxWakeUp(void)   
{  
  CLR_W25Qxx_CS();                              //ѡ��W25Qxx��Ƭѡ
  SPIReadWriteByte(W25X_RELEASE_POWER_DOWN);    //�����ͷŵ���ģʽ����
  SET_W25Qxx_CS();                              //ȡ��W25Qxx��Ƭѡ
  DelayNus(3);                                  //��ʱ3us���ȴ�TRES1
}   

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitW25Qxx
* ��������: ��ʼ��W25QXX 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 4KbytesΪһ��Sector           
*           16������Ϊ1��Block
*           W25X16
*           ����Ϊ2M�ֽ�,����32��Block,512��Sector 
*********************************************************************************************************/
void InitW25Qxx(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;//����ṹ��GPIO_InitStructure����������W25QxxƬѡ��GPIO
  
  RCC_APB2PeriphClockCmd(USER_DEFINE_SPI_CS_GPIO_CLK, ENABLE);   //ʹ��W25QxxƬѡ��ʱ��

  GPIO_InitStructure.GPIO_Pin   = USER_DEFINE_SPI_CS_GPIO_PIN;   //����25QxxƬѡ������    
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;              //�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;              //����I/O������Ϊ50MHz
  GPIO_Init(USER_DEFINE_SPI_CS_GPIO_PORT, &GPIO_InitStructure);  //���ݲ�����ʼ��25QxxƬѡ��GPIO�˿�

  SET_W25Qxx_CS();                                               //ȡ��W25Qxx��Ƭѡ
  InitSPI();                                                     //��ʼ��SPI
  SPISetSpeed(SPI_BaudRatePrescaler_2);                          //����Ϊ18Mʱ��,����ģʽ
}

/*********************************************************************************************************
* ��������: W25QxxReadID
* ��������: ��ȡоƬID
* �������: void
* �������: void
* �� �� ֵ: ��ȡ����оƬ��ID
* ��������: 2018��03��01��
* ע    ��: 0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
*           0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
*           0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
*           0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
*           0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	
*********************************************************************************************************/
u16 W25QxxReadID(void)
{
  u16 ID = 0;                                 //�������оƬ��ID
                    
  CLR_W25Qxx_CS();                            //ѡ��W25Qxx��Ƭѡ
                    
  SPIReadWriteByte(0x90);                     //���Ͷ�ȡID����    
  SPIReadWriteByte(0x00);         
  SPIReadWriteByte(0x00);         
  SPIReadWriteByte(0x00);         
  ID |= SPIReadWriteByte(0xFF) << 8;          //����һ�ζ�ȡ�����ֽ�����8λ������ID�߰�λ
  ID |= SPIReadWriteByte(0xFF);               //���ڶ��ζ�ȡ�����ֽڸ���ID�Ͱ�λ
  SET_W25Qxx_CS();                            //ȡ��W25Qxx��Ƭѡ
        
  return ID;                                  //���ض�ȡ����оƬ��ID
}

/*********************************************************************************************************
* ��������: W25QxxRead
* ��������: ��ȡSPI FLASH����ָ����ַ��ʼ��ȡָ�����ȵ����� 
* �������: pBuffer:���ݴ洢����ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)��NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
* �������: pBuffer
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void W25QxxRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)   
{ 
  u16 i;
  
  CLR_W25Qxx_CS();                            //ѡ��W25Qxx��Ƭѡ
  
  SPIReadWriteByte(W25X_READ_DATA);           //���Ͷ�ȡ����   
  SPIReadWriteByte((u8)((ReadAddr) >> 16));   //����24bit��ַ    
  SPIReadWriteByte((u8)((ReadAddr) >> 8));   
  SPIReadWriteByte((u8)ReadAddr);   
  
  for(i = 0; i < NumByteToRead; i++)
  { 
    pBuffer[i] = SPIReadWriteByte(0XFF);      //ѭ������  
  }
  
  SET_W25Qxx_CS();                            //ȡ��W25Qxx��Ƭѡ
} 

/*********************************************************************************************************
* ��������: W25QxxWrite
* ��������: дSPI FLASH����ָ����ַ��ʼд��ָ�����ȵ����ݣ��ú������������� 
* �������: pBuffer:���ݴ洢����WriteAddr:��ʼд��ĵ�ַ(24bit)��NumByteToWrite:Ҫд����ֽ���(���65535)
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/ 
void W25QxxWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)   
{ 
  u32 secPos;                                                   //���ڴ���׵�ַWriteAddr���ڵ�����
  u16 secOff;                                                   //���ڴ���׵�ַWriteAddr��������ƫ��
  u16 secRemain;                                                //���ڴ������ʣ��ĳ���
  u16 i;                                                        //ѭ��������
                                
  secPos = WriteAddr / 4096;                                    //������ַ
  secOff = WriteAddr % 4096;                                    //�������ڵ�ƫ��
  secRemain = 4096 - secOff;                                    //����ʣ��ռ��С   
                                
  if(NumByteToWrite <= secRemain)                               //�ж�Ҫд�����ݳ����Ƿ񳬹�������ʣ�µĳ���
  {                               
    secRemain = NumByteToWrite;                                 //������4096���ֽڣ����������
  } 
    
  while(1)  
  { 
    W25QxxRead(SPI_FLASH_BUF, secPos * 4096, 4096);             //�����������������ݣ�������SPI_FLASH_BUF��
    for(i = 0; i < secRemain; i++)                              //У������
    {
      if(SPI_FLASH_BUF[secOff + i] != 0XFF)                     //�ж�����ʣ��Ŀռ��Ƿ�����������
      {
        break;                                                  //���������ݣ���Ҫ���� 
      }                                                         
    }                                                                                                                      
    if(i < secRemain)                                           //�����Ҫ����
    {                                                           
      W25QxxEraseSector(secPos);                                //�����������
                              
      for(i = 0; i < secRemain; i++)                            //����
      {
        SPI_FLASH_BUF[i + secOff] = pBuffer[i];                 //���������ݴ����pBuffer��
      }
      
      W25QxxWriteNoCheck(SPI_FLASH_BUF, secPos * 4096, 4096);   //д����������  
    }
    else
    {
      W25QxxWriteNoCheck(pBuffer, WriteAddr, secRemain);        //д�Ѿ������˵�,ֱ��д������ʣ������.
    }

    if(NumByteToWrite == secRemain)
    {
      break;                                                    //д�������
    }                                                         
    else                                                        //д��δ����
    {                                                         
      secPos++;                                                 //������ַ��1
      secOff = 0;                                               //ƫ��λ��Ϊ0
                            
      pBuffer += secRemain;                                     //ָ��ƫ��
      WriteAddr += secRemain;                                   //д��ַƫ��  
      NumByteToWrite -= secRemain;                              //�ֽ����ݼ�
                                
      if(NumByteToWrite > 4096)                         
      {                         
        secRemain = 4096;                                       //��һ����������д����
      }                         
      else                          
      {                         
        secRemain = NumByteToWrite;                             //��һ����������д����
      }
    }
  }
} 
