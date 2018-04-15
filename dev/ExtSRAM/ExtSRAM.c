/*********************************************************************************************************
* ģ������: ExtSRAM.c
* ժ    Ҫ: 
* ��ǰ�汾: 1.0.0
* ��    ��: 
* �������: 2018��03��01��
* ��    ��:
* ע    ��: ʹ��NOR/SRAM�� Bank1.sector3,��ַλHADDR[27,26]=10 
*           ��IS61LV25616/IS62WV25616,��ַ�߷�ΧΪA0~A17,��IS61LV51216/IS62WV51216,��ַ�߷�ΧΪA0~A18                                                                             
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
#include "ExtSRAM.h"
#include <stm32f10x_conf.h>
#include "LCD.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
//ʹ�õ�FSMC�Ĵ洢��BANK1������3������IS62WV51216����������3��Ӧ�Ŀ�ʼ��ַ��0x68000000
#define BANK1_SRAM3_ADDR    ((u32)(0x68000000))    

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
//__attribute__������ָ���������������ԣ�at(0X68000000)������˵����at�����ñ����ľ��Ե�ַ
//�����ñ�������0X68000000�����ַ��s_arrSRAM[250000]�ǲ����õ�����
static u32 s_arrSRAM[250000] __attribute__((at(0X68000000)));

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
//���ⲿSRAMָ����ַ��ʼ��д��ָ�����ȵ�����
static void ExtSRAMWriteBuf(u8* pBuf, u32 writeAddr, u32 numByteToWrite);
//���ⲿSRAMָ����ַ��ʼ����ȡָ�����ȵ�����
static void ExtSRAMReadBuf(u8* pBuf, u32 readAddr, u32 numByteToRead);   

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/                              
/*********************************************************************************************************
* ��������: ExtSRAMWriteBuf
* ��������: ��ָ����ַ��ʼ,����д��n���ֽ�
* �������: pBuf: �ֽ�ָ�룬writeAddr: Ҫд���ֽڵ���ʼ��ַ��numByteToWrite: Ҫд����ֽ���
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��:
*********************************************************************************************************/
static void ExtSRAMWriteBuf(u8* pBuf, u32 writeAddr, u32 numByteToWrite)
{
  u32 n = numByteToWrite;                           //n��Ҫд����ֽ���
  
  for(; n != 0; n--)                                //ÿд��һ���ֽڣ�n��1
  {                        
    *(vu8*)(BANK1_SRAM3_ADDR + writeAddr) = *pBuf;  //��Ҫд���ֽڵ���ʼ��ַ��ʼ�����д������
    
    writeAddr++;                                    //Ҫд���ֽڵĵ�ַ��1
    pBuf++;                                         //�ֽ�ָ���1
  }   
}                                          

/*********************************************************************************************************
* ��������: ExtSRAMReadBuf
* ��������: ��ָ����ַ��ʼ,��������n���ֽ�
* �������: pBuf:�ֽ�ָ��, readAddr:Ҫ�����ֽڵ���ʼ��ַ,numByteToRead:Ҫ�������ֽ���
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��:
*********************************************************************************************************/
static void ExtSRAMReadBuf(u8* pBuf, u32 readAddr, u32 numByteToRead)
{
  u32 n = numByteToRead;                            //n��Ҫ�������ֽ���

  for(;n != 0; n--)                                 //ÿ����һ���ֽڣ�n��1
  {                          
    *pBuf = *(vu8*)(BANK1_SRAM3_ADDR + readAddr);   //��Ҫ�����ֽڵ���ʼ��ַ��ʼ�������������
    
    readAddr++;                                     //Ҫ�����ֽڵĵ�ַ��1
    pBuf++;                                         //�ֽ�ָ���1
  }  
} 

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitExtSRAM
* ��������: ��ʼ���ⲿSRAM
* �������: void 
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��:
*********************************************************************************************************/
void InitExtSRAM(void)
{  
  i32 i;   //����һ��i32���͵ı���i����Ϊѭ��������

  GPIO_InitTypeDef               GPIO_InitStructure;     //����ṹ��GPIO_InitStructure,��������FSMC��GPIO
  //����ṹ��FSMC_NORSRAMInitStructure����������FSMC���ɱ侲̬�洢���������Ĳ���
  FSMC_NORSRAMInitTypeDef        FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;        //����ṹ��readWriteTiming����������FSMC��ʱ�����
 
  //ʹ��GPIOD��GPIOE��GPIOF��GPIOG��ʱ�ӣ�����ԭ��ͼ������ʹ��GPIO�˿ڣ�
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |     
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);     //ʹ��FSMC��ʱ��
  
  //��鿴 stm32f10x_gpio.h ����� GPIO_Pin_x ��Ӧ�� uint16_t ���͵���ֵ��Ȼ����ӡ�
  //���µ�GPIOD���Ű�˳�����ζ�ӦFSMC_D2��D3��NOE��NWE��D13��D14��D15��A16��A17��A18��D0��D1
  GPIO_InitStructure.GPIO_Pin   = 0xFF33;           //���ö�Ӧ�����ţ�PD0��1��4��5��8-15
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����I/O������Ϊ50MHz
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //�����������ģʽ
  GPIO_Init(GPIOD, &GPIO_InitStructure);            //���ݲ�����ʼ��GPIOD�˿�

  //���µ�GPIOE���Ű�˳�����ζ�ӦFSMC_NBL0��NBL1��D4-D12
  GPIO_InitStructure.GPIO_Pin = 0xFF83;             //���ö�Ӧ�����ţ�PE0��1��7-15
  GPIO_Init(GPIOE, &GPIO_InitStructure);            //���ݲ�����ʼ��GPIOE�˿�

  //���µ�GPIOF���Ű�˳�����ζ�ӦFSMC_A0-A5��A6-A9
  GPIO_InitStructure.GPIO_Pin = 0xF03F;             //���ö�Ӧ�����ţ�PF0-5��PF12-15
  GPIO_Init(GPIOF, &GPIO_InitStructure);            //���ݲ�����ʼ��GPIOF�˿�

  //���µ�GPIOG���Ű�˳�����ζ�ӦFSMC_A10-A15��NE3
  GPIO_InitStructure.GPIO_Pin = 0x043F;             //���ö�Ӧ�����ţ�PG0-5��9��10��12
  GPIO_Init(GPIOG, &GPIO_InitStructure);            //���ݲ�����ʼ��GPIOG�˿�
           
  readWriteTiming.FSMC_AddressSetupTime = 0x00;     //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime  = 0x00;     //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�  
  readWriteTiming.FSMC_DataSetupTime    = 0x03;     //���ݱ���ʱ�䣨DATAST��Ϊ3��HCLK 4/72M=55ns(��EM��SRAMоƬ)   
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;//���߻ָ�ʱ��
  readWriteTiming.FSMC_CLKDivision = 0x00;          //ʱ�ӷ�Ƶ����
  readWriteTiming.FSMC_DataLatency = 0x00;          //���ݲ���ʱ��
  readWriteTiming.FSMC_AccessMode  = FSMC_AccessMode_A; //�첽ģʽA��FSMC_NOR������ʱ��ģʽ��  
 
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;                      //ʹ����FSMC��BANK1������3
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;    //��ֹ��ַ/�����߸���
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;               //�洢������ΪSRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;      //�洢�����ݿ��Ϊ16bit  
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;  //�ر�ͻ��ģʽ����
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;//��ַ�ߵĵȴ�״̬��ƽ
  //���첽����״̬ʱ���رյȴ��źţ����������첽���棩
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;                //��ʹ�ܻ���ģʽ
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//�ڵȴ�״̬ǰ���͵ȴ��ź�  
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;     //���ô洢��дʹ��
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;            //�رյȴ��ź�
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;        //��ʹ��ʱ����չģʽ��ʹ�ö�дͬʱ�� 
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;            //��ʹ��ͻ��дģʽ
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;        //���ö�д����ʱ�򣨹ر���չģʽ����£�
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;            //����д����ʱ�򣨴���չģʽ����£�

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //���ݲ�����ʼ��FSMC����

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  //ʹ��BANK1������3
  
  for(i = 0; i < 250000; i++) //i��0������249999
  {
    s_arrSRAM[i] = i;         //i��ŵ�����s_arrSRAM[i]                   
  }
}
    
/*********************************************************************************************************
* ��������: ExtSRAMTestWrite
* ��������: ���Ժ���,��ָ����ַд��1���ֽ�
* �������: addr:��ַ,data:Ҫд�������
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��:
*********************************************************************************************************/
void ExtSRAMTestWrite(u32 addr, u8 data)
{         
  ExtSRAMWriteBuf(&data, addr, 1);  //�ӵ�ַaddr����ʼд��1�ֽڵ�����data��&dataΪ���ݵĵ�ַ��
}

/*********************************************************************************************************
* ��������: ExtSRAMTestWrite
* ��������: ��ȡ1���ֽ�
* �������: addr:��ַ
* �������: void
* �� �� ֵ: ��ȡ��������
* ��������: 2018��03��01��
* ע    ��:
*********************************************************************************************************/
u8 ExtSRAMTestRead(u32 addr)
{
  u8 data;                        //����һ��u8���͵ı���data��Ϊ���ݴ���
  
  ExtSRAMReadBuf(&data, addr, 1); //�ӵ�ַaddr����ʼ����1�ֽڵ����ݣ���ŵ�data                 
  
  return data;                    //����data
}  
   
/*********************************************************************************************************
* ��������: ExtSRAMTest1
* ��������: �ⲿ�ڴ����(���֧��1M�ֽ��ڴ����)
* �������: x,y
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��:
*********************************************************************************************************/
void ExtSRAMTest1(u16 x, u16 y)
{  
  u32  i = 0;                           //����һ��u32���͵ı���i����Ϊѭ��������
  u8  temp = 0;                         //����һ��u8���͵ı���temp����Ϊ���ݴ���
  u8  sval = 0;                         //����һ��u8���͵ı���sval����¼�ڵ�ַ0������������               
  
  //��LCD�ϵ�ָ���������x��y����һ�������СΪwidth��239��heigh��y + 16�ĵط�
  //��ʾ�����СΪ16���ַ���"Ex Memory Test:   0KB"
  LCDShowString(x, y, 239, y + 16, 16, "Ex Memory Test:   0KB");
    
  for(i = 0; i < 1024 * 1024; i += 4096)//��0��ַ��ʼ��ÿ��4K�ֽ�,д��һ������,�ܹ�д��256������,�պ���1M�ֽ�
  {
    ExtSRAMWriteBuf(&temp, i, 1);       //�ڵ�ַi��д��1�ֽڵ�����temp
    
    temp++;                             //temp��1
  }
        
  for(i = 0; i < 1024 * 1024; i += 4096)//���ζ���֮ǰд�������,����У�� 
  {
    ExtSRAMReadBuf(&temp, i, 1);        //�ӵ�ַi������1�ֽڵ����ݷ���temp
    
    if(i == 0)
    {
      sval = temp;                      //��¼�ڵ�ַ0���������ݵ�sval
    }
    else if(temp <= sval)               //���������ĳ�����ݱȵ�ַ0������������С
    {
      break;
    } 
 
    //��LCD�ϵ�ָ���������x + 15 * 8��y����ʾλ��Ϊ4�������СΪ16������(u16)(temp - sval + 1) * 4
    //����0�������ķǵ�����ʾ�����ڡ�Test����������ʾ�ڴ������
    LCDShowxNum(x + 15 * 8, y, (u16)(temp - sval + 1) * 4, 4, 16, 0);  
  }  
}

/*********************************************************************************************************
* ��������: ExtSRAMTest2
* ��������: �ⲿ�ڴ����(���֧��1M�ֽ��ڴ����)
* �������: void
* �������: void
* �� �� ֵ: ��ȡ��������
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void ExtSRAMTest2(void)
{  
  i32 i;                                                  //����һ��i32���͵ı���i����Ϊѭ��������
      
  //�˴���һ��BUG����ʹ��ExtSRAMTest1��,����ExtSRAMTest2���������ÿ��1024����һ��i��s_arrSRAM[i]�������
  //����s_arrSRAM[i] - i = n��nΪ�ڼ��γ���BUG������ͨ������д�����δ�BUG������˳����ʾ��249999
  //��ԭ��������ExtSRAMTest1�ĸ�ֵ�и�ÿ1024��Ԫ�ص�8λ������n��
  for(i = 0; i < 250000; i += 1024)
  {
    s_arrSRAM[i] = i;
  }
  
  for(i = 0; i < 250000; i++)                             //������s_arrSRAM[]��0��249999λ�����ζԱ�
  {
    if(i != s_arrSRAM[i])                                 //��Ԥ�ȴ�������ݲ���
    {
      //��LCD�ϵ�ָ���������x��30��y��210����һ�������СΪwidth��200��heigh��16�ĵط���ʾ�ַ���"SRAM Test Failed!"
      LCDShowString(30, 210, 200, 16, 16, "SRAM Test Failed!");

      break;
    }
   
    //��LCD�ϵ�ָ��λ��x��30��y��190����ʾλ��Ϊ6�������СΪ16������s_arrSRAM[i]
    LCDShowxNum(30, 190, s_arrSRAM[i], 6, 16, 0);
  }
}   
