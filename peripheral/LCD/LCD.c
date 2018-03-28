/*********************************************************************************************************
* ģ������: LCD.c
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
#include "LCD.h"
#include "stdlib.h"
#include "Font.h" 
#include "UART.h"
#include "SysTick.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
//LCD���ƿ飬����LCD��ص���Ҫ����
typedef struct  
{
  u16 width;          //LCD ���
  u16 height;         //LCD �߶�
  u16 id;             //LCD ID
  u8  dir;            //���������Ʊ�־λ��0��������1������
  u16 wramcmd;        //��ʼдGRAMָ��
  u16 setXCmd;        //����x����ָ��
  u16 setYCmd;        //����y����ָ�� 
}StructLCDCtrlBlock; 

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static  StructLCDCtrlBlock s_structLCDCB; //LCD���ƿ飬����LCD��ص���Ҫ������Ĭ��Ϊ����
static  u16 s_iPointColor = 0x0000;       //������ɫ
static  u16 s_iBackColor  = 0xFFFF;       //����ɫ

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void LCD_WR_REG(u16 regVal);                     //д�Ĵ������� 
static void LCD_WR_DATA(u16 data);                      //дLCD���� 
static u16  LCD_RD_DATA(void);                          //��LCD���� 
static void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);//д�Ĵ���
static u16  LCD_ReadReg(u16 LCD_Reg);                   //���Ĵ��� 
static void LCD_WriteRAM_Prepare(void);                 //��ʼдGRAM
static void LCD_WriteRAM(u16 RGB_Code);                 //��LCDд��һ����ɫֵ
static u16  LCD_BGR2RGB(u16 c);                         //ͨ���ú���ת����GBR��ʽ������ת����RGB��ʽ
static void opt_delay(u8 i);                            //��ʱ����
static u32  LCDPower(u8 m, u8 n);                       //��m��n���ݺ���

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: LCD_WR_REG
* ��������: д�Ĵ������� 
* �������: regVal:�Ĵ���ֵ 
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��:
*********************************************************************************************************/
static void LCD_WR_REG(u16 regVal)
{   
  LCD->LCD_REG = regVal;        //д��Ҫд�ļĴ������   
}

/*********************************************************************************************************
* ��������: LCD_WR_DATA
* ��������: дLCD���� 
* �������: data:Ҫд���ֵ 
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��:
*********************************************************************************************************/
static void LCD_WR_DATA(u16 data)
{ 
  LCD->LCD_RAM = data;          //��LCDд������
}

/*********************************************************************************************************
* ��������: LCD_RD_DATA
* ��������: ��LCD���� 
* �������: void
* �������: void
* �� �� ֵ: ��ȡ��������
* ��������: 2018��03��01�� 
* ע    ��:
*********************************************************************************************************/
static u16 LCD_RD_DATA(void)
{
  vu16 ram;                     //��ֹ�������Ż�
        
  ram = LCD->LCD_RAM;           //��ȡLCD�����ݲ�����ram��
        
  return ram;                   //���ض�ȡ��������
}

/*********************************************************************************************************
* ��������: LCD_WriteReg
* ��������: д�Ĵ��� 
* �������: LCD_Reg:�Ĵ�����ַ��LCD_RegValue:Ҫд�������
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��:
*********************************************************************************************************/
static void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
  LCD->LCD_REG = LCD_Reg;       //д��Ҫд�ļĴ������  
  LCD->LCD_RAM = LCD_RegValue;  //д������  
} 

/*********************************************************************************************************
* ��������: LCD_ReadReg
* ��������: ���Ĵ��� 
* �������: LCD_Reg:�Ĵ�����ַ 
* �������: void
* �� �� ֵ: ����������
* ��������: 2018��03��01�� 
* ע    ��:
*********************************************************************************************************/
static u16 LCD_ReadReg(u16 LCD_Reg)
{ 
  LCD_WR_REG(LCD_Reg);          //д��Ҫ���ļĴ������
  DelayNus(5);                  //��ʱ5us
          
  return LCD_RD_DATA();         //���ض�����ֵ
}  

/*********************************************************************************************************
* ��������: LCD_WriteRAM_Prepare
* ��������: ��ʼдGRAM 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��:
*********************************************************************************************************/
static void LCD_WriteRAM_Prepare(void)
{
  LCD->LCD_REG = s_structLCDCB.wramcmd; //��LCD�Ĵ���д�뿪ʼдGRAMָ��
}

/*********************************************************************************************************
* ��������: LCD_WriteRAM
* ��������: ��LCDд��һ����ɫֵ
* �������: RGB_Code:��ɫֵ 
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��:
*********************************************************************************************************/
static void LCD_WriteRAM(u16 RGB_Code)
{ 
  LCD->LCD_RAM = RGB_Code;              //��LCDд��һ����ɫֵ
}

/*********************************************************************************************************
* ��������: LCD_BGR2RGB
* ��������: ��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��ͨ���ú���ת�� 
* �������: c:GBR��ʽ����ɫֵ 
* �������: void
* �� �� ֵ: rgb-RGB��ʽ����ɫֵ
* ��������: 2018��03��01�� 
* ע    ��:
*********************************************************************************************************/
static u16 LCD_BGR2RGB(u16 c)
{
  u16  rgb;                               //�������RGB��ɫ����
  u16  r;                                 //������ź�ɫ������
  u16  g;                                 //���������ɫ������
  u16  b;                                 //���������ɫ������
  
  b = (c >> 0) & 0x1f;                    //��c��ȡ����ɫ�����ݣ���5λ������b��
  g = (c >> 5) & 0x3f;                    //��c��ȡ����ɫ�����ݣ��м�6λ������g��
  r = (c >> 11) & 0x1f;                   //��c��ȡ����ɫ�����ݣ���5λ������r��
  rgb = (b << 11) + (g << 5) + (r << 0);  //��������ɫ����������������rgb��
  
  return rgb;                             //����ת���õ�RGB��ɫ����
} 
/*********************************************************************************************************
* ��������: opt_delay
* ��������: �򵥵���ʱ����
* �������: i-��ʱ��ʱ�䳤��
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: ��mdk -O1ʱ���Ż�ʱ��Ҫ����
*********************************************************************************************************/
static void opt_delay(u8 i)
{
  while(i--)                              //ѭ���ݼ���ֱ��iΪ0
  {
    
  }
}

/*********************************************************************************************************
* ��������: LCDPower
* ��������: ��m��n���ݺ��� 
* �������: mΪ�ף�nΪ�� 
* �������: void
* �� �� ֵ: m��n�η�
* ��������: 2018��03��01�� 
* ע    ��:
*********************************************************************************************************/
static u32 LCDPower(u8 m, u8 n)
{
  u32 result = 1;                         //����������Ž���ı���       
                                          
  while(n--)                              //����ÿ��ѭ����n�ݼ���ֱ��Ϊ0
  {                                       
    result *= m;                          //ѭ��n�Σ��൱��n��m���
  }                                       
                                          
  return result;                          //����m��n���ݵ�ֵ
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitTFTLCD
* ��������: ��ʼ��LCD
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: �ó�ʼ���������Գ�ʼ������ILI93XXҺ��,��������������ILI9320���������ͺŵ�����оƬ��û�в���
*********************************************************************************************************/
void InitTFTLCD(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;                  //����ṹ��GPIO_InitStructure,��������LCD��GPIO
  //����ṹ��FSMC_NORSRAMInitStructure��������LCD�Ĳ���
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure; 
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;       //����ṹ��readWriteTiming������LCD�Ķ�ʱ��
  FSMC_NORSRAMTimingInitTypeDef  writeTiming;           //����ṹ��writeTiming������LCD��дʱ��
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);     //ʹ��FSMC��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��GPIOB�Լ�AFIO���ù���ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); //ʹ��GPIOD�Լ�AFIO���ù���ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //ʹ��GPIOE�Լ�AFIO���ù���ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE); //ʹ��GPIOG�Լ�AFIO���ù���ʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;         //����LCD����Ƶ�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����I/O������Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);            //���ݲ�����ʼ��LCD����Ƶ�GPIO�˿�
  
  //PORTD�����������  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5 | GPIO_Pin_8 | 
                                GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����I/O������Ϊ50MHz
  GPIO_Init(GPIOD, &GPIO_InitStructure);            //���ݲ�����ʼ��LCD����Ƶ�GPIO�˿�
  
  //PORTE�����������  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 | 
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������ģʽ  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����I/O������Ϊ50MHz
  GPIO_Init(GPIOE, &GPIO_InitStructure);            //���ݲ�����ʼ��LCD����Ƶ�GPIO�˿�

  //����LCD������/���ݱ�־λ����PG0��Ƭѡ����PG12
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_12;   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����I/O������Ϊ50MHz
  GPIO_Init(GPIOG, &GPIO_InitStructure);            //���ݲ�����ʼ��LCD����/���ݱ�־λ��Ƭѡ��GPIO�˿�

  readWriteTiming.FSMC_AddressSetupTime = 0x01;     //��ַ����ʱ�䣨ADDSET��Ϊ2��HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;      //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�  
  //���ݱ���ʱ��Ϊ16��HCLK,��ΪҺ������IC�Ķ����ݵ�ʱ���ٶȲ���̫�죬�����1289���IC��
  readWriteTiming.FSMC_DataSetupTime = 0x0f;    
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;  //ģʽA 
    

  writeTiming.FSMC_AddressSetupTime = 0x00;             //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK  
  writeTiming.FSMC_AddressHoldTime = 0x00;              //��ַ����ʱ��
  writeTiming.FSMC_DataSetupTime = 0x03;                //���ݱ���ʱ��Ϊ4��HCLK
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;        //���߷�ת����
  writeTiming.FSMC_CLKDivision = 0x00;                  //HCLK�ķ�Ƶϵ��
  writeTiming.FSMC_DataLatency = 0x00;                  //�����ӳ�ʱ��
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;      //ģʽA 

 
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;                       //ʹ����FSMC��BANK1������4
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;     //��ֹ��ַ/�����߸���
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;                //�洢������ΪSRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;       //�洢�����ݿ��Ϊ16bit  
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;   //�ر�ͻ��ģʽ����
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low; //��ַ�ߵĵȴ�״̬��ƽ
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable; //�رյȴ��ź�
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;                 //��ʹ�ܻ���ģʽ
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//�ڵȴ�״̬ǰ���͵ȴ��ź�  
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;      //���ô洢��дʹ��
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;             //�رյȴ��ź�
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;          //��ʹ��ʱ����չģʽ��ʹ�ö�дͬʱ�� 
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;             //��ʹ��ͻ��дģʽ
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;         //���ö�д����ʱ�򣨹ر���չģʽ����£�
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;                 //����д����ʱ�򣨴���չģʽ����£�

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); //��ʼ��FSMC����

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE); //ʹ��BANK1 
 
  DelayNms(50); //delay 50 ms 
  
  s_structLCDCB.id = LCD_ReadReg(0x0000); //��ID��9320/9325/9328/4531/4535��IC��   
  //����ID����ȷ,����s_structLCDCB.id == 0X9300�жϣ���Ϊ9341��δ����λ������»ᱻ����9300
  if(s_structLCDCB.id< 0XFF || s_structLCDCB.id == 0XFFFF || s_structLCDCB.id == 0X9300)
  { 
    //����9341 ID�Ķ�ȡ  
    LCD_WR_REG(0XD3); 
    s_structLCDCB.id = LCD_RD_DATA();       //dummy read  
    s_structLCDCB.id = LCD_RD_DATA();       //����0X00
    s_structLCDCB.id = LCD_RD_DATA();       //��ȡ93  
    s_structLCDCB.id <<= 8;                 //����8λ���ճ���8λ
    s_structLCDCB.id |= LCD_RD_DATA();      //��ȡ41  
    
    if(s_structLCDCB.id != 0X9341)          //��9341,�����ǲ���6804
    { 
      LCD_WR_REG(0XBF);    
      
      s_structLCDCB.id = LCD_RD_DATA();     //dummy read    
      s_structLCDCB.id = LCD_RD_DATA();     //����0X01         
      s_structLCDCB.id = LCD_RD_DATA();     //����0XD0           
      s_structLCDCB.id = LCD_RD_DATA();     //�������0X68 
      s_structLCDCB.id <<= 8;               //����8λ���ճ���8λ
      s_structLCDCB.id |= LCD_RD_DATA();    //�������0X04    
      if(s_structLCDCB.id != 0X6804)        //Ҳ����6804,���Կ����ǲ���NT35310
      { 
        LCD_WR_REG(0XD4);     
        
        s_structLCDCB.id = LCD_RD_DATA();   //dummy read  
        s_structLCDCB.id = LCD_RD_DATA();   //����0X01   
        s_structLCDCB.id = LCD_RD_DATA();   //����0X53  
        s_structLCDCB.id <<= 8;             //����8λ���ճ���8λ 
        s_structLCDCB.id |= LCD_RD_DATA();  //�������0X10   
        if(s_structLCDCB.id != 0X5310)      //Ҳ����NT35310,���Կ����ǲ���NT35510
        {
          LCD_WR_REG(0XDA00);  
          
          s_structLCDCB.id = LCD_RD_DATA(); //����0X00   
          
          LCD_WR_REG(0XDB00);  
          
          s_structLCDCB.id = LCD_RD_DATA(); //����0X80
          s_structLCDCB.id <<= 8;           //����8λ���ճ���8λ  
          
          LCD_WR_REG(0XDC00);  
          
          s_structLCDCB.id |= LCD_RD_DATA();//����0X00   
          
          if(s_structLCDCB.id == 0x8000)
          {
            s_structLCDCB.id = 0x5510;      //NT35510���ص�ID��8000H,Ϊ��������,����ǿ������Ϊ5510
          }
          if(s_structLCDCB.id != 0X5510)      //Ҳ����NT5510,���Կ����ǲ���SSD1963
          {
            LCD_WR_REG(0XA1);
            s_structLCDCB.id = LCD_RD_DATA();//dummy read  
            s_structLCDCB.id = LCD_RD_DATA();  //����0X57
            s_structLCDCB.id <<= 8;           //����8λ���ճ���8λ   
            s_structLCDCB.id |= LCD_RD_DATA();  //����0X61  
            if(s_structLCDCB.id == 0X5761)
            {
              s_structLCDCB.id = 0X1963;//SSD1963���ص�ID��5761H,Ϊ��������,����ǿ������Ϊ1963  
            }  
          }
        }
      }
    }    
  } 
  printf("LCD ID:%x\r\n", s_structLCDCB.id); //�ڴ��ڴ�ӡLCD��ID 
  
  //������ݲ�ͬLCD��IDѡ��ͬ�ĳ�ʼ�����ж�LCD���г�ʼ������Щ��ʼ��������LCD��Ӧ���ṩ
  //���ͻ�������ֱ��ʹ����Щ���м��ɣ�����Ҫ�����о�
  if(s_structLCDCB.id == 0X9341)  //9341��ʼ��
  {   
    LCD_WR_REG(0xCF);  
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0xC1); 
    LCD_WR_DATA(0X30); 
    LCD_WR_REG(0xED);  
    LCD_WR_DATA(0x64); 
    LCD_WR_DATA(0x03); 
    LCD_WR_DATA(0X12); 
    LCD_WR_DATA(0X81); 
    LCD_WR_REG(0xE8);  
    LCD_WR_DATA(0x85); 
    LCD_WR_DATA(0x10); 
    LCD_WR_DATA(0x7A); 
    LCD_WR_REG(0xCB);  
    LCD_WR_DATA(0x39); 
    LCD_WR_DATA(0x2C); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x34); 
    LCD_WR_DATA(0x02); 
    LCD_WR_REG(0xF7);  
    LCD_WR_DATA(0x20); 
    LCD_WR_REG(0xEA);  
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x00); 
    LCD_WR_REG(0xC0);   //Power control 
    LCD_WR_DATA(0x1B);  //VRH[5:0] 
    LCD_WR_REG(0xC1);   //Power control 
    LCD_WR_DATA(0x01);  //SAP[2:0];BT[3:0] 
    LCD_WR_REG(0xC5);   //VCM control 
    LCD_WR_DATA(0x30);  //3F
    LCD_WR_DATA(0x30);  //3C
    LCD_WR_REG(0xC7);   //VCM control2 
    LCD_WR_DATA(0XB7); 
    LCD_WR_REG(0x36);   // Memory Access Control 
    LCD_WR_DATA(0x48); 
    LCD_WR_REG(0x3A);   
    LCD_WR_DATA(0x55); 
    LCD_WR_REG(0xB1);   
    LCD_WR_DATA(0x00);   
    LCD_WR_DATA(0x1A); 
    LCD_WR_REG(0xB6);   // Display Function Control 
    LCD_WR_DATA(0x0A); 
    LCD_WR_DATA(0xA2); 
    LCD_WR_REG(0xF2);   // 3Gamma Function Disable 
    LCD_WR_DATA(0x00); 
    LCD_WR_REG(0x26);   //Gamma curve selected 
    LCD_WR_DATA(0x01); 
    LCD_WR_REG(0xE0);   //Set Gamma 
    LCD_WR_DATA(0x0F); 
    LCD_WR_DATA(0x2A); 
    LCD_WR_DATA(0x28); 
    LCD_WR_DATA(0x08); 
    LCD_WR_DATA(0x0E); 
    LCD_WR_DATA(0x08); 
    LCD_WR_DATA(0x54); 
    LCD_WR_DATA(0XA9); 
    LCD_WR_DATA(0x43); 
    LCD_WR_DATA(0x0A); 
    LCD_WR_DATA(0x0F); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x00);      
    LCD_WR_REG(0XE1);   //Set Gamma 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x15); 
    LCD_WR_DATA(0x17); 
    LCD_WR_DATA(0x07); 
    LCD_WR_DATA(0x11); 
    LCD_WR_DATA(0x06); 
    LCD_WR_DATA(0x2B); 
    LCD_WR_DATA(0x56); 
    LCD_WR_DATA(0x3C); 
    LCD_WR_DATA(0x05); 
    LCD_WR_DATA(0x10); 
    LCD_WR_DATA(0x0F); 
    LCD_WR_DATA(0x3F); 
    LCD_WR_DATA(0x3F); 
    LCD_WR_DATA(0x0F); 
    LCD_WR_REG(0x2B); 
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x3f);
    LCD_WR_REG(0x2A); 
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xef);   
    LCD_WR_REG(0x11);   //Exit Sleep
    DelayNms(120);
    LCD_WR_REG(0x29);   //display on  
  }
  else if(s_structLCDCB.id == 0x6804) //6804��ʼ��
  {
    LCD_WR_REG(0X11);
    DelayNms(20);
    LCD_WR_REG(0XD0);   //VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
    LCD_WR_DATA(0X07); 
    LCD_WR_DATA(0X42); 
    LCD_WR_DATA(0X1D); 
    LCD_WR_REG(0XD1);   //VCOMH VCOM_AC amplitude setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X1a);
    LCD_WR_DATA(0X09); 
    LCD_WR_REG(0XD2);   //Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0X22);
    LCD_WR_REG(0XC0);   //REV SM GS 
    LCD_WR_DATA(0X10);
    LCD_WR_DATA(0X3B);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X02);
    LCD_WR_DATA(0X11);
    
    LCD_WR_REG(0XC5);   //Frame rate setting = 72HZ  when setting 0x03
    LCD_WR_DATA(0X03);
    
    LCD_WR_REG(0XC8);   //Gamma setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X25);
    LCD_WR_DATA(0X21);
    LCD_WR_DATA(0X05);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X0a);
    LCD_WR_DATA(0X65);
    LCD_WR_DATA(0X25);
    LCD_WR_DATA(0X77);
    LCD_WR_DATA(0X50);
    LCD_WR_DATA(0X0f);
    LCD_WR_DATA(0X00);    
              
    LCD_WR_REG(0XF8);
    LCD_WR_DATA(0X01);    

    LCD_WR_REG(0XFE);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X02);
    
    LCD_WR_REG(0X20);   //Exit invert mode

    LCD_WR_REG(0X36);
    LCD_WR_DATA(0X08);  //ԭ����a
    
    LCD_WR_REG(0X3A);
    LCD_WR_DATA(0X55);  //16λģʽ    
    LCD_WR_REG(0X2B);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0X3F);
    
    LCD_WR_REG(0X2A);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0XDF);
    DelayNms(120);
    LCD_WR_REG(0X29);    
  }
  else if(s_structLCDCB.id == 0x5310)//5310��ʼ��
  { 
    LCD_WR_REG(0xED);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0xFE);

    LCD_WR_REG(0xEE);
    LCD_WR_DATA(0xDE);
    LCD_WR_DATA(0x21);

    LCD_WR_REG(0xF1);
    LCD_WR_DATA(0x01);
    LCD_WR_REG(0xDF);
    LCD_WR_DATA(0x10);

    //VCOMvoltage//
    LCD_WR_REG(0xC4);
    LCD_WR_DATA(0x8F);  //5f

    LCD_WR_REG(0xC6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE2);
    LCD_WR_DATA(0xE2);
    LCD_WR_DATA(0xE2);
    LCD_WR_REG(0xBF);
    LCD_WR_DATA(0xAA);

    LCD_WR_REG(0xB0);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x19);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x21);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0x80);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x96);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB4);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x96);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA1);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB5);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5E);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xAC);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDC);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x90);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xEB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDC);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xBA);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0x20);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xFF);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC2);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x39);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x23);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x23);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC4);
    LCD_WR_DATA(0x62);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x84);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x95);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE6);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC5);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x65);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x76);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC6);
    LCD_WR_DATA(0x20);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x16);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x21);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x46);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x52);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x7A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA(0x16);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x22);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x52);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x7A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE2);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x61);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x79);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x97);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xE3);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x62);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x78);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x97);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE4);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x74);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x84);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x93);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBE);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xE5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x74);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x84);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x93);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBE);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDC);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE6);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x76);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE7);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x76);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x67);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x67);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x87);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x23); 
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x87);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xAA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE9);
    LCD_WR_DATA(0xAA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x00);
    LCD_WR_DATA(0xAA);

    LCD_WR_REG(0xCF);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xF9);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);  //66

    LCD_WR_REG(0x11);
    DelayNms(100);
    LCD_WR_REG(0x29);
    LCD_WR_REG(0x35);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x51);
    LCD_WR_DATA(0xFF);
    LCD_WR_REG(0x53);
    LCD_WR_DATA(0x2C);
    LCD_WR_REG(0x55);
    LCD_WR_DATA(0x82);
    LCD_WR_REG(0x2c);
  }
  else if(s_structLCDCB.id == 0x5510)//5510��ʼ��
  {
    LCD_WriteReg(0xF000,0x55);
    LCD_WriteReg(0xF001,0xAA);
    LCD_WriteReg(0xF002,0x52);
    LCD_WriteReg(0xF003,0x08);
    LCD_WriteReg(0xF004,0x01);
    //AVDD Set AVDD 5.2V
    LCD_WriteReg(0xB000,0x0D);
    LCD_WriteReg(0xB001,0x0D);
    LCD_WriteReg(0xB002,0x0D);
    //AVDD ratio
    LCD_WriteReg(0xB600,0x34);
    LCD_WriteReg(0xB601,0x34);
    LCD_WriteReg(0xB602,0x34);
    //AVEE -5.2V
    LCD_WriteReg(0xB100,0x0D);
    LCD_WriteReg(0xB101,0x0D);
    LCD_WriteReg(0xB102,0x0D);
    //AVEE ratio
    LCD_WriteReg(0xB700,0x34);
    LCD_WriteReg(0xB701,0x34);
    LCD_WriteReg(0xB702,0x34);
    //VCL -2.5V
    LCD_WriteReg(0xB200,0x00);
    LCD_WriteReg(0xB201,0x00);
    LCD_WriteReg(0xB202,0x00);
    //VCL ratio
    LCD_WriteReg(0xB800,0x24);
    LCD_WriteReg(0xB801,0x24);
    LCD_WriteReg(0xB802,0x24);
    //VGH 15V (Free pump)
    LCD_WriteReg(0xBF00,0x01);
    LCD_WriteReg(0xB300,0x0F);
    LCD_WriteReg(0xB301,0x0F);
    LCD_WriteReg(0xB302,0x0F);
    //VGH ratio
    LCD_WriteReg(0xB900,0x34);
    LCD_WriteReg(0xB901,0x34);
    LCD_WriteReg(0xB902,0x34);
    //VGL_REG -10V
    LCD_WriteReg(0xB500,0x08);
    LCD_WriteReg(0xB501,0x08);
    LCD_WriteReg(0xB502,0x08);
    LCD_WriteReg(0xC200,0x03);
    //VGLX ratio
    LCD_WriteReg(0xBA00,0x24);
    LCD_WriteReg(0xBA01,0x24);
    LCD_WriteReg(0xBA02,0x24);
    //VGMP/VGSP 4.5V/0V
    LCD_WriteReg(0xBC00,0x00);
    LCD_WriteReg(0xBC01,0x78);
    LCD_WriteReg(0xBC02,0x00);
    //VGMN/VGSN -4.5V/0V
    LCD_WriteReg(0xBD00,0x00);
    LCD_WriteReg(0xBD01,0x78);
    LCD_WriteReg(0xBD02,0x00);
    //VCOM
    LCD_WriteReg(0xBE00,0x00);
    LCD_WriteReg(0xBE01,0x64);
    //Gamma Setting
    LCD_WriteReg(0xD100,0x00);
    LCD_WriteReg(0xD101,0x33);
    LCD_WriteReg(0xD102,0x00);
    LCD_WriteReg(0xD103,0x34);
    LCD_WriteReg(0xD104,0x00);
    LCD_WriteReg(0xD105,0x3A);
    LCD_WriteReg(0xD106,0x00);
    LCD_WriteReg(0xD107,0x4A);
    LCD_WriteReg(0xD108,0x00);
    LCD_WriteReg(0xD109,0x5C);
    LCD_WriteReg(0xD10A,0x00);
    LCD_WriteReg(0xD10B,0x81);
    LCD_WriteReg(0xD10C,0x00);
    LCD_WriteReg(0xD10D,0xA6);
    LCD_WriteReg(0xD10E,0x00);
    LCD_WriteReg(0xD10F,0xE5);
    LCD_WriteReg(0xD110,0x01);
    LCD_WriteReg(0xD111,0x13);
    LCD_WriteReg(0xD112,0x01);
    LCD_WriteReg(0xD113,0x54);
    LCD_WriteReg(0xD114,0x01);
    LCD_WriteReg(0xD115,0x82);
    LCD_WriteReg(0xD116,0x01);
    LCD_WriteReg(0xD117,0xCA);
    LCD_WriteReg(0xD118,0x02);
    LCD_WriteReg(0xD119,0x00);
    LCD_WriteReg(0xD11A,0x02);
    LCD_WriteReg(0xD11B,0x01);
    LCD_WriteReg(0xD11C,0x02);
    LCD_WriteReg(0xD11D,0x34);
    LCD_WriteReg(0xD11E,0x02);
    LCD_WriteReg(0xD11F,0x67);
    LCD_WriteReg(0xD120,0x02);
    LCD_WriteReg(0xD121,0x84);
    LCD_WriteReg(0xD122,0x02);
    LCD_WriteReg(0xD123,0xA4);
    LCD_WriteReg(0xD124,0x02);
    LCD_WriteReg(0xD125,0xB7);
    LCD_WriteReg(0xD126,0x02);
    LCD_WriteReg(0xD127,0xCF);
    LCD_WriteReg(0xD128,0x02);
    LCD_WriteReg(0xD129,0xDE);
    LCD_WriteReg(0xD12A,0x02);
    LCD_WriteReg(0xD12B,0xF2);
    LCD_WriteReg(0xD12C,0x02);
    LCD_WriteReg(0xD12D,0xFE);
    LCD_WriteReg(0xD12E,0x03);
    LCD_WriteReg(0xD12F,0x10);
    LCD_WriteReg(0xD130,0x03);
    LCD_WriteReg(0xD131,0x33);
    LCD_WriteReg(0xD132,0x03);
    LCD_WriteReg(0xD133,0x6D);
    LCD_WriteReg(0xD200,0x00);
    LCD_WriteReg(0xD201,0x33);
    LCD_WriteReg(0xD202,0x00);
    LCD_WriteReg(0xD203,0x34);
    LCD_WriteReg(0xD204,0x00);
    LCD_WriteReg(0xD205,0x3A);
    LCD_WriteReg(0xD206,0x00);
    LCD_WriteReg(0xD207,0x4A);
    LCD_WriteReg(0xD208,0x00);
    LCD_WriteReg(0xD209,0x5C);
    LCD_WriteReg(0xD20A,0x00);

    LCD_WriteReg(0xD20B,0x81);
    LCD_WriteReg(0xD20C,0x00);
    LCD_WriteReg(0xD20D,0xA6);
    LCD_WriteReg(0xD20E,0x00);
    LCD_WriteReg(0xD20F,0xE5);
    LCD_WriteReg(0xD210,0x01);
    LCD_WriteReg(0xD211,0x13);
    LCD_WriteReg(0xD212,0x01);
    LCD_WriteReg(0xD213,0x54);
    LCD_WriteReg(0xD214,0x01);
    LCD_WriteReg(0xD215,0x82);
    LCD_WriteReg(0xD216,0x01);
    LCD_WriteReg(0xD217,0xCA);
    LCD_WriteReg(0xD218,0x02);
    LCD_WriteReg(0xD219,0x00);
    LCD_WriteReg(0xD21A,0x02);
    LCD_WriteReg(0xD21B,0x01);
    LCD_WriteReg(0xD21C,0x02);
    LCD_WriteReg(0xD21D,0x34);
    LCD_WriteReg(0xD21E,0x02);
    LCD_WriteReg(0xD21F,0x67);
    LCD_WriteReg(0xD220,0x02);
    LCD_WriteReg(0xD221,0x84);
    LCD_WriteReg(0xD222,0x02);
    LCD_WriteReg(0xD223,0xA4);
    LCD_WriteReg(0xD224,0x02);
    LCD_WriteReg(0xD225,0xB7);
    LCD_WriteReg(0xD226,0x02);
    LCD_WriteReg(0xD227,0xCF);
    LCD_WriteReg(0xD228,0x02);
    LCD_WriteReg(0xD229,0xDE);
    LCD_WriteReg(0xD22A,0x02);
    LCD_WriteReg(0xD22B,0xF2);
    LCD_WriteReg(0xD22C,0x02);
    LCD_WriteReg(0xD22D,0xFE);
    LCD_WriteReg(0xD22E,0x03);
    LCD_WriteReg(0xD22F,0x10);
    LCD_WriteReg(0xD230,0x03);
    LCD_WriteReg(0xD231,0x33);
    LCD_WriteReg(0xD232,0x03);
    LCD_WriteReg(0xD233,0x6D);
    LCD_WriteReg(0xD300,0x00);
    LCD_WriteReg(0xD301,0x33);
    LCD_WriteReg(0xD302,0x00);
    LCD_WriteReg(0xD303,0x34);
    LCD_WriteReg(0xD304,0x00);
    LCD_WriteReg(0xD305,0x3A);
    LCD_WriteReg(0xD306,0x00);
    LCD_WriteReg(0xD307,0x4A);
    LCD_WriteReg(0xD308,0x00);
    LCD_WriteReg(0xD309,0x5C);
    LCD_WriteReg(0xD30A,0x00);

    LCD_WriteReg(0xD30B,0x81);
    LCD_WriteReg(0xD30C,0x00);
    LCD_WriteReg(0xD30D,0xA6);
    LCD_WriteReg(0xD30E,0x00);
    LCD_WriteReg(0xD30F,0xE5);
    LCD_WriteReg(0xD310,0x01);
    LCD_WriteReg(0xD311,0x13);
    LCD_WriteReg(0xD312,0x01);
    LCD_WriteReg(0xD313,0x54);
    LCD_WriteReg(0xD314,0x01);
    LCD_WriteReg(0xD315,0x82);
    LCD_WriteReg(0xD316,0x01);
    LCD_WriteReg(0xD317,0xCA);
    LCD_WriteReg(0xD318,0x02);
    LCD_WriteReg(0xD319,0x00);
    LCD_WriteReg(0xD31A,0x02);
    LCD_WriteReg(0xD31B,0x01);
    LCD_WriteReg(0xD31C,0x02);
    LCD_WriteReg(0xD31D,0x34);
    LCD_WriteReg(0xD31E,0x02);
    LCD_WriteReg(0xD31F,0x67);
    LCD_WriteReg(0xD320,0x02);
    LCD_WriteReg(0xD321,0x84);
    LCD_WriteReg(0xD322,0x02);
    LCD_WriteReg(0xD323,0xA4);
    LCD_WriteReg(0xD324,0x02);
    LCD_WriteReg(0xD325,0xB7);
    LCD_WriteReg(0xD326,0x02);
    LCD_WriteReg(0xD327,0xCF);
    LCD_WriteReg(0xD328,0x02);
    LCD_WriteReg(0xD329,0xDE);
    LCD_WriteReg(0xD32A,0x02);
    LCD_WriteReg(0xD32B,0xF2);
    LCD_WriteReg(0xD32C,0x02);
    LCD_WriteReg(0xD32D,0xFE);
    LCD_WriteReg(0xD32E,0x03);
    LCD_WriteReg(0xD32F,0x10);
    LCD_WriteReg(0xD330,0x03);
    LCD_WriteReg(0xD331,0x33);
    LCD_WriteReg(0xD332,0x03);
    LCD_WriteReg(0xD333,0x6D);
    LCD_WriteReg(0xD400,0x00);
    LCD_WriteReg(0xD401,0x33);
    LCD_WriteReg(0xD402,0x00);
    LCD_WriteReg(0xD403,0x34);
    LCD_WriteReg(0xD404,0x00);
    LCD_WriteReg(0xD405,0x3A);
    LCD_WriteReg(0xD406,0x00);
    LCD_WriteReg(0xD407,0x4A);
    LCD_WriteReg(0xD408,0x00);
    LCD_WriteReg(0xD409,0x5C);
    LCD_WriteReg(0xD40A,0x00);
    LCD_WriteReg(0xD40B,0x81);

    LCD_WriteReg(0xD40C,0x00);
    LCD_WriteReg(0xD40D,0xA6);
    LCD_WriteReg(0xD40E,0x00);
    LCD_WriteReg(0xD40F,0xE5);
    LCD_WriteReg(0xD410,0x01);
    LCD_WriteReg(0xD411,0x13);
    LCD_WriteReg(0xD412,0x01);
    LCD_WriteReg(0xD413,0x54);
    LCD_WriteReg(0xD414,0x01);
    LCD_WriteReg(0xD415,0x82);
    LCD_WriteReg(0xD416,0x01);
    LCD_WriteReg(0xD417,0xCA);
    LCD_WriteReg(0xD418,0x02);
    LCD_WriteReg(0xD419,0x00);
    LCD_WriteReg(0xD41A,0x02);
    LCD_WriteReg(0xD41B,0x01);
    LCD_WriteReg(0xD41C,0x02);
    LCD_WriteReg(0xD41D,0x34);
    LCD_WriteReg(0xD41E,0x02);
    LCD_WriteReg(0xD41F,0x67);
    LCD_WriteReg(0xD420,0x02);
    LCD_WriteReg(0xD421,0x84);
    LCD_WriteReg(0xD422,0x02);
    LCD_WriteReg(0xD423,0xA4);
    LCD_WriteReg(0xD424,0x02);
    LCD_WriteReg(0xD425,0xB7);
    LCD_WriteReg(0xD426,0x02);
    LCD_WriteReg(0xD427,0xCF);
    LCD_WriteReg(0xD428,0x02);
    LCD_WriteReg(0xD429,0xDE);
    LCD_WriteReg(0xD42A,0x02);
    LCD_WriteReg(0xD42B,0xF2);
    LCD_WriteReg(0xD42C,0x02);
    LCD_WriteReg(0xD42D,0xFE);
    LCD_WriteReg(0xD42E,0x03);
    LCD_WriteReg(0xD42F,0x10);
    LCD_WriteReg(0xD430,0x03);
    LCD_WriteReg(0xD431,0x33);
    LCD_WriteReg(0xD432,0x03);
    LCD_WriteReg(0xD433,0x6D);
    LCD_WriteReg(0xD500,0x00);
    LCD_WriteReg(0xD501,0x33);
    LCD_WriteReg(0xD502,0x00);
    LCD_WriteReg(0xD503,0x34);
    LCD_WriteReg(0xD504,0x00);
    LCD_WriteReg(0xD505,0x3A);
    LCD_WriteReg(0xD506,0x00);
    LCD_WriteReg(0xD507,0x4A);
    LCD_WriteReg(0xD508,0x00);
    LCD_WriteReg(0xD509,0x5C);
    LCD_WriteReg(0xD50A,0x00);
    LCD_WriteReg(0xD50B,0x81);

    LCD_WriteReg(0xD50C,0x00);
    LCD_WriteReg(0xD50D,0xA6);
    LCD_WriteReg(0xD50E,0x00);
    LCD_WriteReg(0xD50F,0xE5);
    LCD_WriteReg(0xD510,0x01);
    LCD_WriteReg(0xD511,0x13);
    LCD_WriteReg(0xD512,0x01);
    LCD_WriteReg(0xD513,0x54);
    LCD_WriteReg(0xD514,0x01);
    LCD_WriteReg(0xD515,0x82);
    LCD_WriteReg(0xD516,0x01);
    LCD_WriteReg(0xD517,0xCA);
    LCD_WriteReg(0xD518,0x02);
    LCD_WriteReg(0xD519,0x00);
    LCD_WriteReg(0xD51A,0x02);
    LCD_WriteReg(0xD51B,0x01);
    LCD_WriteReg(0xD51C,0x02);
    LCD_WriteReg(0xD51D,0x34);
    LCD_WriteReg(0xD51E,0x02);
    LCD_WriteReg(0xD51F,0x67);
    LCD_WriteReg(0xD520,0x02);
    LCD_WriteReg(0xD521,0x84);
    LCD_WriteReg(0xD522,0x02);
    LCD_WriteReg(0xD523,0xA4);
    LCD_WriteReg(0xD524,0x02);
    LCD_WriteReg(0xD525,0xB7);
    LCD_WriteReg(0xD526,0x02);
    LCD_WriteReg(0xD527,0xCF);
    LCD_WriteReg(0xD528,0x02);
    LCD_WriteReg(0xD529,0xDE);
    LCD_WriteReg(0xD52A,0x02);
    LCD_WriteReg(0xD52B,0xF2);
    LCD_WriteReg(0xD52C,0x02);
    LCD_WriteReg(0xD52D,0xFE);
    LCD_WriteReg(0xD52E,0x03);
    LCD_WriteReg(0xD52F,0x10);
    LCD_WriteReg(0xD530,0x03);
    LCD_WriteReg(0xD531,0x33);
    LCD_WriteReg(0xD532,0x03);
    LCD_WriteReg(0xD533,0x6D);
    LCD_WriteReg(0xD600,0x00);
    LCD_WriteReg(0xD601,0x33);
    LCD_WriteReg(0xD602,0x00);
    LCD_WriteReg(0xD603,0x34);
    LCD_WriteReg(0xD604,0x00);
    LCD_WriteReg(0xD605,0x3A);
    LCD_WriteReg(0xD606,0x00);
    LCD_WriteReg(0xD607,0x4A);
    LCD_WriteReg(0xD608,0x00);
    LCD_WriteReg(0xD609,0x5C);
    LCD_WriteReg(0xD60A,0x00);
    LCD_WriteReg(0xD60B,0x81);

    LCD_WriteReg(0xD60C,0x00);
    LCD_WriteReg(0xD60D,0xA6);
    LCD_WriteReg(0xD60E,0x00);
    LCD_WriteReg(0xD60F,0xE5);
    LCD_WriteReg(0xD610,0x01);
    LCD_WriteReg(0xD611,0x13);
    LCD_WriteReg(0xD612,0x01);
    LCD_WriteReg(0xD613,0x54);
    LCD_WriteReg(0xD614,0x01);
    LCD_WriteReg(0xD615,0x82);
    LCD_WriteReg(0xD616,0x01);
    LCD_WriteReg(0xD617,0xCA);
    LCD_WriteReg(0xD618,0x02);
    LCD_WriteReg(0xD619,0x00);
    LCD_WriteReg(0xD61A,0x02);
    LCD_WriteReg(0xD61B,0x01);
    LCD_WriteReg(0xD61C,0x02);
    LCD_WriteReg(0xD61D,0x34);
    LCD_WriteReg(0xD61E,0x02);
    LCD_WriteReg(0xD61F,0x67);
    LCD_WriteReg(0xD620,0x02);
    LCD_WriteReg(0xD621,0x84);
    LCD_WriteReg(0xD622,0x02);
    LCD_WriteReg(0xD623,0xA4);
    LCD_WriteReg(0xD624,0x02);
    LCD_WriteReg(0xD625,0xB7);
    LCD_WriteReg(0xD626,0x02);
    LCD_WriteReg(0xD627,0xCF);
    LCD_WriteReg(0xD628,0x02);
    LCD_WriteReg(0xD629,0xDE);
    LCD_WriteReg(0xD62A,0x02);
    LCD_WriteReg(0xD62B,0xF2);
    LCD_WriteReg(0xD62C,0x02);
    LCD_WriteReg(0xD62D,0xFE);
    LCD_WriteReg(0xD62E,0x03);
    LCD_WriteReg(0xD62F,0x10);
    LCD_WriteReg(0xD630,0x03);
    LCD_WriteReg(0xD631,0x33);
    LCD_WriteReg(0xD632,0x03);
    LCD_WriteReg(0xD633,0x6D);
    //LV2 Page 0 enable
    LCD_WriteReg(0xF000,0x55);
    LCD_WriteReg(0xF001,0xAA);
    LCD_WriteReg(0xF002,0x52);
    LCD_WriteReg(0xF003,0x08);
    LCD_WriteReg(0xF004,0x00);
    //Display control
    LCD_WriteReg(0xB100, 0xCC);
    LCD_WriteReg(0xB101, 0x00);
    //Source hold time
    LCD_WriteReg(0xB600,0x05);
    //Gate EQ control
    LCD_WriteReg(0xB700,0x70);
    LCD_WriteReg(0xB701,0x70);
    //Source EQ control (Mode 2)
    LCD_WriteReg(0xB800,0x01);
    LCD_WriteReg(0xB801,0x03);
    LCD_WriteReg(0xB802,0x03);
    LCD_WriteReg(0xB803,0x03);
    //Inversion mode (2-dot)
    LCD_WriteReg(0xBC00,0x02);
    LCD_WriteReg(0xBC01,0x00);
    LCD_WriteReg(0xBC02,0x00);
    //Timing control 4H w/ 4-delay
    LCD_WriteReg(0xC900,0xD0);
    LCD_WriteReg(0xC901,0x02);
    LCD_WriteReg(0xC902,0x50);
    LCD_WriteReg(0xC903,0x50);
    LCD_WriteReg(0xC904,0x50);
    LCD_WriteReg(0x3500,0x00);
    LCD_WriteReg(0x3A00,0x55);  //16-bit/pixel
    LCD_WR_REG(0x1100);
    DelayNus(120);
    LCD_WR_REG(0x2900);
  }
  else if(s_structLCDCB.id == 0x9325)//9325��ʼ��
  {
    LCD_WriteReg(0x00E5,0x78F0); 
    LCD_WriteReg(0x0001,0x0100); 
    LCD_WriteReg(0x0002,0x0700); 
    LCD_WriteReg(0x0003,0x1030); 
    LCD_WriteReg(0x0004,0x0000); 
    LCD_WriteReg(0x0008,0x0202);  
    LCD_WriteReg(0x0009,0x0000);
    LCD_WriteReg(0x000A,0x0000); 
    LCD_WriteReg(0x000C,0x0000); 
    LCD_WriteReg(0x000D,0x0000);
    LCD_WriteReg(0x000F,0x0000);
    //power on sequence VGHVGL
    LCD_WriteReg(0x0010,0x0000);   
    LCD_WriteReg(0x0011,0x0007);  
    LCD_WriteReg(0x0012,0x0000);  
    LCD_WriteReg(0x0013,0x0000); 
    LCD_WriteReg(0x0007,0x0000); 
    //vgh 
    LCD_WriteReg(0x0010,0x1690);   
    LCD_WriteReg(0x0011,0x0227);
    //delayms(100);
    //vregiout 
    LCD_WriteReg(0x0012,0x009D); //0x001b
    //delayms(100); 
    //vom amplitude
    LCD_WriteReg(0x0013,0x1900);
    //delayms(100); 
    //vom H
    LCD_WriteReg(0x0029,0x0025); 
    LCD_WriteReg(0x002B,0x000D); 
    //gamma
    LCD_WriteReg(0x0030,0x0007);
    LCD_WriteReg(0x0031,0x0303);
    LCD_WriteReg(0x0032,0x0003);// 0006
    LCD_WriteReg(0x0035,0x0206);
    LCD_WriteReg(0x0036,0x0008);
    LCD_WriteReg(0x0037,0x0406); 
    LCD_WriteReg(0x0038,0x0304);//0200
    LCD_WriteReg(0x0039,0x0007); 
    LCD_WriteReg(0x003C,0x0602);// 0504
    LCD_WriteReg(0x003D,0x0008); 
    //ram
    LCD_WriteReg(0x0050,0x0000); 
    LCD_WriteReg(0x0051,0x00EF);
    LCD_WriteReg(0x0052,0x0000); 
    LCD_WriteReg(0x0053,0x013F);  
    LCD_WriteReg(0x0060,0xA700); 
    LCD_WriteReg(0x0061,0x0001); 
    LCD_WriteReg(0x006A,0x0000); 
    //
    LCD_WriteReg(0x0080,0x0000); 
    LCD_WriteReg(0x0081,0x0000); 
    LCD_WriteReg(0x0082,0x0000); 
    LCD_WriteReg(0x0083,0x0000); 
    LCD_WriteReg(0x0084,0x0000); 
    LCD_WriteReg(0x0085,0x0000); 
    //
    LCD_WriteReg(0x0090,0x0010); 
    LCD_WriteReg(0x0092,0x0600); 
    
    LCD_WriteReg(0x0007,0x0133);
    LCD_WriteReg(0x00,0x0022);//
  }else if(s_structLCDCB.id == 0x9328)//9328��ʼ��
  {
    LCD_WriteReg(0x00EC,0x108F);  // internal timeing      
    LCD_WriteReg(0x00EF,0x1234);  // ADD        
    //LCD_WriteReg(0x00e7,0x0010);      
    //LCD_WriteReg(0x0000,0x0001);//�����ڲ�ʱ��
    LCD_WriteReg(0x0001,0x0100);     
    LCD_WriteReg(0x0002,0x0700);  //��Դ����                    
    //LCD_WriteReg(0x0003,(1<<3) | (1<<4) );   //65K  RGB
    //DRIVE TABLE(�Ĵ��� 03H)
    //BIT3=AM BIT4:5=ID0:1
    //AM ID0 ID1   FUNCATION
    // 0  0   0     R->L D->U
    // 1  0   0     D->U  R->L
    // 0  1   0     L->R D->U
    // 1  1   0    D->U  L->R
    // 0  0   1     R->L U->D
    // 1  0   1    U->D  R->L
    // 0  1   1    L->R U->D �����������.
    // 1  1   1     U->D  L->R
    LCD_WriteReg(0x0003,(1<<12) | (3<<4) | (0<<3) );//65K    
    LCD_WriteReg(0x0004,0x0000);                                   
    LCD_WriteReg(0x0008,0x0202);             
    LCD_WriteReg(0x0009,0x0000);         
    LCD_WriteReg(0x000a,0x0000);//display setting         
    LCD_WriteReg(0x000c,0x0001);//display setting          
    LCD_WriteReg(0x000d,0x0000);//0f3c          
    LCD_WriteReg(0x000f,0x0000);
    //��Դ����
    LCD_WriteReg(0x0010,0x0000);   
    LCD_WriteReg(0x0011,0x0007);
    LCD_WriteReg(0x0012,0x0000);                                                                 
    LCD_WriteReg(0x0013,0x0000);                 
    LCD_WriteReg(0x0007,0x0001);                 
    DelayNms(50); 
    LCD_WriteReg(0x0010,0x1490);   
    LCD_WriteReg(0x0011,0x0227);
    DelayNms(50); 
    LCD_WriteReg(0x0012,0x008A);                  
    DelayNms(50); 
    LCD_WriteReg(0x0013,0x1a00);   
    LCD_WriteReg(0x0029,0x0006);
    LCD_WriteReg(0x002b,0x000d);
    DelayNms(50); 
    LCD_WriteReg(0x0020,0x0000);                                                            
    LCD_WriteReg(0x0021,0x0000);           
    DelayNms(50); 
    //٤��У��
    LCD_WriteReg(0x0030,0x0000); 
    LCD_WriteReg(0x0031,0x0604);   
    LCD_WriteReg(0x0032,0x0305);
    LCD_WriteReg(0x0035,0x0000);
    LCD_WriteReg(0x0036,0x0C09); 
    LCD_WriteReg(0x0037,0x0204);
    LCD_WriteReg(0x0038,0x0301);        
    LCD_WriteReg(0x0039,0x0707);     
    LCD_WriteReg(0x003c,0x0000);
    LCD_WriteReg(0x003d,0x0a0a);
    DelayNms(50); 
    LCD_WriteReg(0x0050,0x0000); //ˮƽGRAM��ʼλ�� 
    LCD_WriteReg(0x0051,0x00ef); //ˮƽGRAM��ֹλ��                    
    LCD_WriteReg(0x0052,0x0000); //��ֱGRAM��ʼλ��                    
    LCD_WriteReg(0x0053,0x013f); //��ֱGRAM��ֹλ��  
 
    LCD_WriteReg(0x0060,0xa700);        
    LCD_WriteReg(0x0061,0x0001); 
    LCD_WriteReg(0x006a,0x0000);
    LCD_WriteReg(0x0080,0x0000);
    LCD_WriteReg(0x0081,0x0000);
    LCD_WriteReg(0x0082,0x0000);
    LCD_WriteReg(0x0083,0x0000);
    LCD_WriteReg(0x0084,0x0000);
    LCD_WriteReg(0x0085,0x0000);
      
    LCD_WriteReg(0x0090,0x0010);     
    LCD_WriteReg(0x0092,0x0600);  
    //������ʾ����    
    LCD_WriteReg(0x0007,0x0133); 
  }
  else if(s_structLCDCB.id == 0x9320)//9320��ʼ��
  {
    LCD_WriteReg(0x00,0x0000);
    LCD_WriteReg(0x01,0x0100);  //Driver Output Contral.
    LCD_WriteReg(0x02,0x0700);  //LCD Driver Waveform Contral.
    LCD_WriteReg(0x03,0x1030);//Entry Mode Set.
    //LCD_WriteReg(0x03,0x1018);  //Entry Mode Set.
  
    LCD_WriteReg(0x04,0x0000);  //Scalling Contral.
    LCD_WriteReg(0x08,0x0202);  //Display Contral 2.(0x0207)
    LCD_WriteReg(0x09,0x0000);  //Display Contral 3.(0x0000)
    LCD_WriteReg(0x0a,0x0000);  //Frame Cycle Contal.(0x0000)
    LCD_WriteReg(0x0c,(1<<0));  //Extern Display Interface Contral 1.(0x0000)
    LCD_WriteReg(0x0d,0x0000);  //Frame Maker Position.
    LCD_WriteReg(0x0f,0x0000);  //Extern Display Interface Contral 2.      
    DelayNms(50); 
    LCD_WriteReg(0x07,0x0101);  //Display Contral.
    DelayNms(50);                   
    LCD_WriteReg(0x10,(1<<12) | (0<<8) | (1<<7) | (1<<6) | (0<<4));  //Power Control 1.(0x16b0)
    LCD_WriteReg(0x11,0x0007);                //Power Control 2.(0x0001)
    LCD_WriteReg(0x12,(1<<8) | (1<<4) | (0<<0));        //Power Control 3.(0x0138)
    LCD_WriteReg(0x13,0x0b00);                //Power Control 4.
    LCD_WriteReg(0x29,0x0000);                //Power Control 7.
  
    LCD_WriteReg(0x2b,(1<<14) | (1<<4));      
    LCD_WriteReg(0x50,0);  //Set X Star
    //ˮƽGRAM��ֹλ��Set X End.
    LCD_WriteReg(0x51,239);  //Set Y Star
    LCD_WriteReg(0x52,0);  //Set Y End.t.
    LCD_WriteReg(0x53,319);  //
  
    LCD_WriteReg(0x60,0x2700);  //Driver Output Control.
    LCD_WriteReg(0x61,0x0001);  //Driver Output Control.
    LCD_WriteReg(0x6a,0x0000);  //Vertical Srcoll Control.
  
    LCD_WriteReg(0x80,0x0000);  //Display Position? Partial Display 1.
    LCD_WriteReg(0x81,0x0000);  //RAM Address Start? Partial Display 1.
    LCD_WriteReg(0x82,0x0000);  //RAM Address End-Partial Display 1.
    LCD_WriteReg(0x83,0x0000);  //Displsy Position? Partial Display 2.
    LCD_WriteReg(0x84,0x0000);  //RAM Address Start? Partial Display 2.
    LCD_WriteReg(0x85,0x0000);  //RAM Address End? Partial Display 2.
  
    LCD_WriteReg(0x90,(0<<7) | (16<<0));  //Frame Cycle Contral.(0x0013)
    LCD_WriteReg(0x92,0x0000);  //Panel Interface Contral 2.(0x0000)
    LCD_WriteReg(0x93,0x0001);  //Panel Interface Contral 3.
    LCD_WriteReg(0x95,0x0110);  //Frame Cycle Contral.(0x0110)
    LCD_WriteReg(0x97,(0<<8));  //
    LCD_WriteReg(0x98,0x0000);  //Frame Cycle Contral.     
    LCD_WriteReg(0x07,0x0173);  //(0x0173)
  }
  else if(s_structLCDCB.id == 0X9331)//9331��ʼ��    
  {
    LCD_WriteReg(0x00E7, 0x1014);
    LCD_WriteReg(0x0001, 0x0100); // set SS and SM bit
    LCD_WriteReg(0x0002, 0x0200); // set 1 line inversion
    LCD_WriteReg(0x0003,(1<<12) | (3<<4) | (1<<3));//65K    
    //LCD_WriteReg(0x0003, 0x1030); // set GRAM write direction and BGR=1.
    LCD_WriteReg(0x0008, 0x0202); // set the back porch and front porch
    LCD_WriteReg(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
    LCD_WriteReg(0x000A, 0x0000); // FMARK function
    LCD_WriteReg(0x000C, 0x0000); // RGB interface setting
    LCD_WriteReg(0x000D, 0x0000); // Frame marker Position
    LCD_WriteReg(0x000F, 0x0000); // RGB interface polarity
    //*************Power On sequence ****************//
    LCD_WriteReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WriteReg(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
    LCD_WriteReg(0x0012, 0x0000); // VREG1OUT voltage
    LCD_WriteReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
    DelayNms(200); // Dis-charge capacitor power voltage
    LCD_WriteReg(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WriteReg(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
    DelayNms(50); // Delay 50ms
    LCD_WriteReg(0x0012, 0x000C); // Internal reference voltage= Vci;
    DelayNms(50); // Delay 50ms
    LCD_WriteReg(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
    LCD_WriteReg(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
    LCD_WriteReg(0x002B, 0x000B); // Set Frame Rate
    DelayNms(50); // Delay 50ms
    LCD_WriteReg(0x0020, 0x0000); // GRAM horizontal Address
    LCD_WriteReg(0x0021, 0x013f); // GRAM Vertical Address
    // ----------- Adjust the Gamma Curve ----------//
    LCD_WriteReg(0x0030, 0x0000);
    LCD_WriteReg(0x0031, 0x0106);
    LCD_WriteReg(0x0032, 0x0000);
    LCD_WriteReg(0x0035, 0x0204);
    LCD_WriteReg(0x0036, 0x160A);
    LCD_WriteReg(0x0037, 0x0707);
    LCD_WriteReg(0x0038, 0x0106);
    LCD_WriteReg(0x0039, 0x0707);
    LCD_WriteReg(0x003C, 0x0402);
    LCD_WriteReg(0x003D, 0x0C0F);
    //------------------ Set GRAM area ---------------//
    LCD_WriteReg(0x0050, 0x0000); // Horizontal GRAM Start Address
    LCD_WriteReg(0x0051, 0x00EF); // Horizontal GRAM End Address
    LCD_WriteReg(0x0052, 0x0000); // Vertical GRAM Start Address
    LCD_WriteReg(0x0053, 0x013F); // Vertical GRAM Start Address
    LCD_WriteReg(0x0060, 0x2700); // Gate Scan Line
    LCD_WriteReg(0x0061, 0x0001); // NDL,VLE, REV 
    LCD_WriteReg(0x006A, 0x0000); // set scrolling line
    //-------------- Partial Display Control ---------//
    LCD_WriteReg(0x0080, 0x0000);
    LCD_WriteReg(0x0081, 0x0000);
    LCD_WriteReg(0x0082, 0x0000);
    LCD_WriteReg(0x0083, 0x0000);
    LCD_WriteReg(0x0084, 0x0000);
    LCD_WriteReg(0x0085, 0x0000);
    //-------------- Panel Control -------------------//
    LCD_WriteReg(0x0090, 0x0010);
    LCD_WriteReg(0x0092, 0x0600);
    LCD_WriteReg(0x0007, 0x0133); // 262K color and display ON
  }
  else if(s_structLCDCB.id == 0x5408)//5408��ʼ��
  {
    LCD_WriteReg(0x01,0x0100);                  
    LCD_WriteReg(0x02,0x0700);//LCD Driving Waveform Contral 
    LCD_WriteReg(0x03,0x1030);//Entry Mode����      
    //ָ������������϶��µ��Զ���ģʽ
    //Normal Mode(Window Mode disable)
    //RGB��ʽ
    //16λ����2�δ����8��������
    LCD_WriteReg(0x04,0x0000); //Scalling Control register     
    LCD_WriteReg(0x08,0x0207); //Display Control 2 
    LCD_WriteReg(0x09,0x0000); //Display Control 3   
    LCD_WriteReg(0x0A,0x0000); //Frame Cycle Control   
    LCD_WriteReg(0x0C,0x0000); //External Display Interface Control 1 
    LCD_WriteReg(0x0D,0x0000); //Frame Maker Position     
    LCD_WriteReg(0x0F,0x0000); //External Display Interface Control 2 
    DelayNms(20);
    //TFT Һ����ɫͼ����ʾ����14
    LCD_WriteReg(0x10,0x16B0); //0x14B0 //Power Control 1
    LCD_WriteReg(0x11,0x0001); //0x0007 //Power Control 2
    LCD_WriteReg(0x17,0x0001); //0x0000 //Power Control 3
    LCD_WriteReg(0x12,0x0138); //0x013B //Power Control 4
    LCD_WriteReg(0x13,0x0800); //0x0800 //Power Control 5
    LCD_WriteReg(0x29,0x0009); //NVM read data 2
    LCD_WriteReg(0x2a,0x0009); //NVM read data 3
    LCD_WriteReg(0xa4,0x0000);   
    LCD_WriteReg(0x50,0x0000); //���ò������ڵ�X�Ὺʼ��
    LCD_WriteReg(0x51,0x00EF); //���ò������ڵ�X�������
    LCD_WriteReg(0x52,0x0000); //���ò������ڵ�Y�Ὺʼ��
    LCD_WriteReg(0x53,0x013F); //���ò������ڵ�Y�������
    LCD_WriteReg(0x60,0x2700); //Driver Output Control
    //������Ļ�ĵ����Լ�ɨ�����ʼ��
    LCD_WriteReg(0x61,0x0001); //Driver Output Control
    LCD_WriteReg(0x6A,0x0000); //Vertical Scroll Control
    LCD_WriteReg(0x80,0x0000); //Display Position �C Partial Display 1
    LCD_WriteReg(0x81,0x0000); //RAM Address Start �C Partial Display 1
    LCD_WriteReg(0x82,0x0000); //RAM address End - Partial Display 1
    LCD_WriteReg(0x83,0x0000); //Display Position �C Partial Display 2
    LCD_WriteReg(0x84,0x0000); //RAM Address Start �C Partial Display 2
    LCD_WriteReg(0x85,0x0000); //RAM address End �C Partail Display2
    LCD_WriteReg(0x90,0x0013); //Frame Cycle Control
    LCD_WriteReg(0x92,0x0000);  //Panel Interface Control 2
    LCD_WriteReg(0x93,0x0003); //Panel Interface control 3
    LCD_WriteReg(0x95,0x0110);  //Frame Cycle Control
    LCD_WriteReg(0x07,0x0173);     
    DelayNms(50);
  }  
  else if(s_structLCDCB.id == 0x1505)//1505��ʼ��
  {
    // second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
    LCD_WriteReg(0x0007,0x0000);
    DelayNms(50); 
    LCD_WriteReg(0x0012,0x011C);//0x011A   why need to set several times?
    LCD_WriteReg(0x00A4,0x0001);//NVM   
    LCD_WriteReg(0x0008,0x000F);
    LCD_WriteReg(0x000A,0x0008);
    LCD_WriteReg(0x000D,0x0008);      
    //٤��У��
    LCD_WriteReg(0x0030,0x0707);
    LCD_WriteReg(0x0031,0x0007); //0x0707
    LCD_WriteReg(0x0032,0x0603); 
    LCD_WriteReg(0x0033,0x0700); 
    LCD_WriteReg(0x0034,0x0202); 
    LCD_WriteReg(0x0035,0x0002); //?0x0606
    LCD_WriteReg(0x0036,0x1F0F);
    LCD_WriteReg(0x0037,0x0707); //0x0f0f  0x0105
    LCD_WriteReg(0x0038,0x0000); 
    LCD_WriteReg(0x0039,0x0000); 
    LCD_WriteReg(0x003A,0x0707); 
    LCD_WriteReg(0x003B,0x0000); //0x0303
    LCD_WriteReg(0x003C,0x0007); //?0x0707
    LCD_WriteReg(0x003D,0x0000); //0x1313//0x1f08
    DelayNms(50); 
    LCD_WriteReg(0x0007,0x0001);
    LCD_WriteReg(0x0017,0x0001);//������Դ
    DelayNms(50); 
    //��Դ����
    LCD_WriteReg(0x0010,0x17A0); 
    LCD_WriteReg(0x0011,0x0217);//reference voltage VC[2:0]   Vciout = 1.00*Vcivl
    LCD_WriteReg(0x0012,0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
    LCD_WriteReg(0x0013,0x0F00);//VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
    LCD_WriteReg(0x002A,0x0000);  
    LCD_WriteReg(0x0029,0x000A);//0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
    LCD_WriteReg(0x0012,0x013E);// 0x013C  power supply on
    //Coordinates Control//
    LCD_WriteReg(0x0050,0x0000);//0x0e00
    LCD_WriteReg(0x0051,0x00EF); 
    LCD_WriteReg(0x0052,0x0000); 
    LCD_WriteReg(0x0053,0x013F); 
    //Pannel Image Control//
    LCD_WriteReg(0x0060,0x2700); 
    LCD_WriteReg(0x0061,0x0001); 
    LCD_WriteReg(0x006A,0x0000); 
    LCD_WriteReg(0x0080,0x0000); 
    //Partial Image Control//
    LCD_WriteReg(0x0081,0x0000); 
    LCD_WriteReg(0x0082,0x0000); 
    LCD_WriteReg(0x0083,0x0000); 
    LCD_WriteReg(0x0084,0x0000); 
    LCD_WriteReg(0x0085,0x0000); 
    //Panel Interface Control//
    LCD_WriteReg(0x0090,0x0013);//0x0010 frenqucy
    LCD_WriteReg(0x0092,0x0300); 
    LCD_WriteReg(0x0093,0x0005); 
    LCD_WriteReg(0x0095,0x0000); 
    LCD_WriteReg(0x0097,0x0000); 
    LCD_WriteReg(0x0098,0x0000); 
  
    LCD_WriteReg(0x0001,0x0100); 
    LCD_WriteReg(0x0002,0x0700); 
    LCD_WriteReg(0x0003,0x1038);//ɨ�跽�� ��->��  ��->�� 
    LCD_WriteReg(0x0004,0x0000); 
    LCD_WriteReg(0x000C,0x0000); 
    LCD_WriteReg(0x000F,0x0000); 
    LCD_WriteReg(0x0020,0x0000); 
    LCD_WriteReg(0x0021,0x0000); 
    LCD_WriteReg(0x0007,0x0021); 
    DelayNms(20);
    LCD_WriteReg(0x0007,0x0061); 
    DelayNms(20);
    LCD_WriteReg(0x0007,0x0173); 
    DelayNms(20);
  }
  else if(s_structLCDCB.id == 0xB505)//B505��ʼ��
  {
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    
    LCD_WriteReg(0x00a4,0x0001);
    DelayNms(20);      
    LCD_WriteReg(0x0060,0x2700);
    LCD_WriteReg(0x0008,0x0202);
    
    LCD_WriteReg(0x0030,0x0214);
    LCD_WriteReg(0x0031,0x3715);
    LCD_WriteReg(0x0032,0x0604);
    LCD_WriteReg(0x0033,0x0e16);
    LCD_WriteReg(0x0034,0x2211);
    LCD_WriteReg(0x0035,0x1500);
    LCD_WriteReg(0x0036,0x8507);
    LCD_WriteReg(0x0037,0x1407);
    LCD_WriteReg(0x0038,0x1403);
    LCD_WriteReg(0x0039,0x0020);
    
    LCD_WriteReg(0x0090,0x001a);
    LCD_WriteReg(0x0010,0x0000);
    LCD_WriteReg(0x0011,0x0007);
    LCD_WriteReg(0x0012,0x0000);
    LCD_WriteReg(0x0013,0x0000);
    DelayNms(20);
    
    LCD_WriteReg(0x0010,0x0730);
    LCD_WriteReg(0x0011,0x0137);
    DelayNms(20);
    
    LCD_WriteReg(0x0012,0x01b8);
    DelayNms(20);
    
    LCD_WriteReg(0x0013,0x0f00);
    LCD_WriteReg(0x002a,0x0080);
    LCD_WriteReg(0x0029,0x0048);
    DelayNms(20);
    
    LCD_WriteReg(0x0001,0x0100);
    LCD_WriteReg(0x0002,0x0700);
    LCD_WriteReg(0x0003,0x1038);//ɨ�跽�� ��->��  ��->�� 
    LCD_WriteReg(0x0008,0x0202);
    LCD_WriteReg(0x000a,0x0000);
    LCD_WriteReg(0x000c,0x0000);
    LCD_WriteReg(0x000d,0x0000);
    LCD_WriteReg(0x000e,0x0030);
    LCD_WriteReg(0x0050,0x0000);
    LCD_WriteReg(0x0051,0x00ef);
    LCD_WriteReg(0x0052,0x0000);
    LCD_WriteReg(0x0053,0x013f);
    LCD_WriteReg(0x0060,0x2700);
    LCD_WriteReg(0x0061,0x0001);
    LCD_WriteReg(0x006a,0x0000);
    //LCD_WriteReg(0x0080,0x0000);
    //LCD_WriteReg(0x0081,0x0000);
    LCD_WriteReg(0x0090,0X0011);
    LCD_WriteReg(0x0092,0x0600);
    LCD_WriteReg(0x0093,0x0402);
    LCD_WriteReg(0x0094,0x0002);
    DelayNms(20);
    
    LCD_WriteReg(0x0007,0x0001);
    DelayNms(20);
    LCD_WriteReg(0x0007,0x0061);
    LCD_WriteReg(0x0007,0x0173);
    
    LCD_WriteReg(0x0020,0x0000);
    LCD_WriteReg(0x0021,0x0000);    
    LCD_WriteReg(0x00,0x22);  
  }
  else if(s_structLCDCB.id == 0xC505)//C505��ʼ��
  {
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    DelayNms(20);      
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x0000,0x0000);
    LCD_WriteReg(0x00a4,0x0001);
    DelayNms(20);      
    LCD_WriteReg(0x0060,0x2700);
    LCD_WriteReg(0x0008,0x0806);
    
    LCD_WriteReg(0x0030,0x0703);//gamma setting
    LCD_WriteReg(0x0031,0x0001);
    LCD_WriteReg(0x0032,0x0004);
    LCD_WriteReg(0x0033,0x0102);
    LCD_WriteReg(0x0034,0x0300);
    LCD_WriteReg(0x0035,0x0103);
    LCD_WriteReg(0x0036,0x001F);
    LCD_WriteReg(0x0037,0x0703);
    LCD_WriteReg(0x0038,0x0001);
    LCD_WriteReg(0x0039,0x0004);   
    
    LCD_WriteReg(0x0090, 0x0015);  //80Hz
    LCD_WriteReg(0x0010, 0X0410);  //BT,AP
    LCD_WriteReg(0x0011,0x0247);  //DC1,DC0,VC
    LCD_WriteReg(0x0012, 0x01BC);
    LCD_WriteReg(0x0013, 0x0e00);
    DelayNms(120);
    LCD_WriteReg(0x0001, 0x0100);
    LCD_WriteReg(0x0002, 0x0200);
    LCD_WriteReg(0x0003, 0x1030);
    
    LCD_WriteReg(0x000A, 0x0008);
    LCD_WriteReg(0x000C, 0x0000);
    
    LCD_WriteReg(0x000E, 0x0020);
    LCD_WriteReg(0x000F, 0x0000);
    LCD_WriteReg(0x0020, 0x0000);  //H Start
    LCD_WriteReg(0x0021, 0x0000);  //V Start
    LCD_WriteReg(0x002A,0x003D);  //vcom2
    DelayNms(20);
    LCD_WriteReg(0x0029, 0x002d);
    LCD_WriteReg(0x0050, 0x0000);
    LCD_WriteReg(0x0051, 0xD0EF);
    LCD_WriteReg(0x0052, 0x0000);
    LCD_WriteReg(0x0053, 0x013F);
    LCD_WriteReg(0x0061, 0x0000);
    LCD_WriteReg(0x006A, 0x0000);
    LCD_WriteReg(0x0092,0x0300); 
 
    LCD_WriteReg(0x0093, 0x0005);
    LCD_WriteReg(0x0007, 0x0100);
  }
  else if(s_structLCDCB.id == 0x4531)//4531��ʼ��
  {
    LCD_WriteReg(0X00,0X0001);   
    DelayNms(10);   
    LCD_WriteReg(0X10,0X1628);   
    LCD_WriteReg(0X12,0X000e);//0x0006    
    LCD_WriteReg(0X13,0X0A39);   
    DelayNms(10);   
    LCD_WriteReg(0X11,0X0040);   
    LCD_WriteReg(0X15,0X0050);   
    DelayNms(10);   
    LCD_WriteReg(0X12,0X001e);//16    
    DelayNms(10);   
    LCD_WriteReg(0X10,0X1620);   
    LCD_WriteReg(0X13,0X2A39);   
    DelayNms(10);   
    LCD_WriteReg(0X01,0X0100);   
    LCD_WriteReg(0X02,0X0300);   
    LCD_WriteReg(0X03,0X1038);//�ı䷽���   
    LCD_WriteReg(0X08,0X0202);   
    LCD_WriteReg(0X0A,0X0008);   
    LCD_WriteReg(0X30,0X0000);   
    LCD_WriteReg(0X31,0X0402);   
    LCD_WriteReg(0X32,0X0106);   
    LCD_WriteReg(0X33,0X0503);   
    LCD_WriteReg(0X34,0X0104);   
    LCD_WriteReg(0X35,0X0301);   
    LCD_WriteReg(0X36,0X0707);   
    LCD_WriteReg(0X37,0X0305);   
    LCD_WriteReg(0X38,0X0208);   
    LCD_WriteReg(0X39,0X0F0B);   
    LCD_WriteReg(0X41,0X0002);   
    LCD_WriteReg(0X60,0X2700);   
    LCD_WriteReg(0X61,0X0001);   
    LCD_WriteReg(0X90,0X0210);   
    LCD_WriteReg(0X92,0X010A);   
    LCD_WriteReg(0X93,0X0004);   
    LCD_WriteReg(0XA0,0X0100);   
    LCD_WriteReg(0X07,0X0001);   
    LCD_WriteReg(0X07,0X0021);   
    LCD_WriteReg(0X07,0X0023);   
    LCD_WriteReg(0X07,0X0033);   
    LCD_WriteReg(0X07,0X0133);   
    LCD_WriteReg(0XA0,0X0000); 
  }
  else if(s_structLCDCB.id == 0x4535)//4535��ʼ��
  {            
    LCD_WriteReg(0X15,0X0030);   
    LCD_WriteReg(0X9A,0X0010);   
    LCD_WriteReg(0X11,0X0020);   
    LCD_WriteReg(0X10,0X3428);   
    LCD_WriteReg(0X12,0X0002);//16    
    LCD_WriteReg(0X13,0X1038);   
    DelayNms(40);   
    LCD_WriteReg(0X12,0X0012);//16    
    DelayNms(40);   
    LCD_WriteReg(0X10,0X3420);   
    LCD_WriteReg(0X13,0X3038);   
    DelayNms(70);   
    LCD_WriteReg(0X30,0X0000);   
    LCD_WriteReg(0X31,0X0402);   
    LCD_WriteReg(0X32,0X0307);   
    LCD_WriteReg(0X33,0X0304);   
    LCD_WriteReg(0X34,0X0004);   
    LCD_WriteReg(0X35,0X0401);   
    LCD_WriteReg(0X36,0X0707);   
    LCD_WriteReg(0X37,0X0305);   
    LCD_WriteReg(0X38,0X0610);   
    LCD_WriteReg(0X39,0X0610); 
      
    LCD_WriteReg(0X01,0X0100);   
    LCD_WriteReg(0X02,0X0300);   
    LCD_WriteReg(0X03,0X1030);//�ı䷽���   
    LCD_WriteReg(0X08,0X0808);   
    LCD_WriteReg(0X0A,0X0008);   
    LCD_WriteReg(0X60,0X2700);   
    LCD_WriteReg(0X61,0X0001);   
    LCD_WriteReg(0X90,0X013E);   
    LCD_WriteReg(0X92,0X0100);   
    LCD_WriteReg(0X93,0X0100);   
    LCD_WriteReg(0XA0,0X3000);   
    LCD_WriteReg(0XA3,0X0010);   
    LCD_WriteReg(0X07,0X0001);   
    LCD_WriteReg(0X07,0X0021);   
    LCD_WriteReg(0X07,0X0023);   
    LCD_WriteReg(0X07,0X0033);   
    LCD_WriteReg(0X07,0X0133);   
  }else if(s_structLCDCB.id == 0X1963)//1963��ʼ��
  {
    //Set PLL with OSC = 10MHz (hardware)
    //Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
    LCD_WR_REG(0xE2);    
    LCD_WR_DATA(0x1D);                     //����1 
    LCD_WR_DATA(0x02);                     //����2 Divider M = 2, PLL = 300/(M+1) = 100MHz
    LCD_WR_DATA(0x04);                     //����3 Validate M and N values   
    DelayNus(100);                  
    LCD_WR_REG(0xE0);                      // Start PLL command
    LCD_WR_DATA(0x01);                     // enable PLL
    DelayNms(10);                 
    LCD_WR_REG(0xE0);                      // Start PLL command again
    LCD_WR_DATA(0x03);                     // now, use PLL output as system clock  
    DelayNms(12);                   
    LCD_WR_REG(0x01);                      //��λ
    DelayNms(10);                 
                      
    LCD_WR_REG(0xE6);                      //��������Ƶ��,33Mhz
    LCD_WR_DATA(0x2F);                  
    LCD_WR_DATA(0xFF);                  
    LCD_WR_DATA(0xFF);                  
                      
    LCD_WR_REG(0xB0);                      //����LCDģʽ
    LCD_WR_DATA(0x20);                     //24λģʽ
    LCD_WR_DATA(0x00);                     //TFT ģʽ 
  
    LCD_WR_DATA((SSD_HOR_RESOLUTION-1)>>8);//����LCDˮƽ����
    LCD_WR_DATA(SSD_HOR_RESOLUTION-1);     
    LCD_WR_DATA((SSD_VER_RESOLUTION-1)>>8);//����LCD��ֱ����
    LCD_WR_DATA(SSD_VER_RESOLUTION-1);     
    LCD_WR_DATA(0x00);                     //RGB���� 
    
    LCD_WR_REG(0xB4);                      //Set horizontal period
    LCD_WR_DATA((SSD_HT-1)>>8);
    LCD_WR_DATA(SSD_HT-1);
    LCD_WR_DATA(SSD_HPS>>8);
    LCD_WR_DATA(SSD_HPS);
    LCD_WR_DATA(SSD_HOR_PULSE_WIDTH-1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xB6);                      //Set vertical period
    LCD_WR_DATA((SSD_VT-1)>>8);
    LCD_WR_DATA(SSD_VT-1);
    LCD_WR_DATA(SSD_VPS>>8);
    LCD_WR_DATA(SSD_VPS);
    LCD_WR_DATA(SSD_VER_FRONT_PORCH-1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    
    LCD_WR_REG(0xF0);                     //����SSD1963��CPU�ӿ�Ϊ16bit  
    LCD_WR_DATA(0x03);                    //16-bit(565 format) data for 16bpp 
                     
    LCD_WR_REG(0x29);                     //������ʾ
    //����PWM���  ����ͨ��ռ�ձȿɵ� 
    LCD_WR_REG(0xD0);                     //�����Զ���ƽ��DBC
    LCD_WR_DATA(0x00);                    //disable
                      
    LCD_WR_REG(0xBE);                     //����PWM���
    LCD_WR_DATA(0x05);                    //1����PWMƵ��
    LCD_WR_DATA(0xFE);                    //2����PWMռ�ձ�
    LCD_WR_DATA(0x01);                    //3����C
    LCD_WR_DATA(0x00);                    //4����D
    LCD_WR_DATA(0x00);                    //5����E 
    LCD_WR_DATA(0x00);                    //6����F 
                        
    LCD_WR_REG(0xB8);                     //����GPIO����
    LCD_WR_DATA(0x03);                    //2��IO�����ó����
    LCD_WR_DATA(0x01);                    //GPIOʹ��������IO���� 
    LCD_WR_REG(0xBA);                   
    LCD_WR_DATA(0X01);                    //GPIO[1:0]=01,����LCD����
    
    LCDSSDSetBackLight(100);              //��������Ϊ����
  }                   
  LCDSetDispDir(0);                       //����LCDΪ������ʾ
  BACK_LIGHT_ON();                        //���������
  LCDClear(WHITE);                        //����Ļ����ɫˢ��Ϊ��ɫ
} 

/*********************************************************************************************************
* ��������: GetLCDID
* ��������: ��ȡLCD��ID
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
u16  GetLCDID(void)
{
  return s_structLCDCB.id;                 //����LCD��ID
}

/*********************************************************************************************************
* ��������: LCDDispOn
* ��������: LCD������ʾ
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDDispOn(void)
{             
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X6804 || 
     s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X1963)    //���LCD��IDΪ9341|6804|5310|1963
  {
    LCD_WR_REG(0X29);                                             //������ʾ  
  }  
  else if(s_structLCDCB.id == 0X5510)                             //���IDΪ5510
  {
    LCD_WR_REG(0X2900);                                           //������ʾ
  }
  else                                                            //���Ϊ����
  {
    LCD_WriteReg(0X07, 0x0173);                                   //������ʾ
  }
}   

/*********************************************************************************************************
* ��������: LCDDispOff
* ��������: LCD�ر���ʾ
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDDispOff(void)
{     
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X6804 ||  
     s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X1963)    //���LCD��IDΪ9341|6804|5310|1963
  {                                                               
    LCD_WR_REG(0X28);                                             //�ر���ʾ                                 
  }                                                               
  else if(s_structLCDCB.id == 0X5510)                             //���IDΪ5510
  {                                                               
    LCD_WR_REG(0X2800);                                           //�ر���ʾ                                 
  }                                                               
  else                                                            //���Ϊ����
  {
    LCD_WriteReg(0X07,0x0);                                       //�ر���ʾ
  }    
} 

/*********************************************************************************************************
* ��������: SetPointColor
* ��������: ���û��ʵ���ɫ
* �������: color-���ʵ���ɫ
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void SetPointColor(u16 color)
{
  s_iPointColor = color;          //���û���Ϊ���뺯������ɫ
}

/*********************************************************************************************************
* ��������: SetBackColor
* ��������: ���ñ���ɫ
* �������: color-Ҫ��䱳������ɫ
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void SetBackColor(u16 color)
{
  s_iBackColor = color;           //���ñ���ɫΪ���뺯������ɫ
}

/*********************************************************************************************************
* ��������: LCDClear
* ��������: ����
* �������: color:Ҫ���������ɫ
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDClear(u16 color)
{
  u32 index = 0;                                                //����ɨ��LCD����ÿһ����Ĺ��  
  u32 totalPoint;                                               //�������LCD���������ص���
  
  totalPoint = s_structLCDCB.width * s_structLCDCB.height;      //LCD���Ŀ��*�߶ȣ��õ��ܵ���
  
  if((s_structLCDCB.id == 0X6804) && (s_structLCDCB.dir == 1))  //6804������ʱ�����⴦��  
  {                
    s_structLCDCB.dir = 0;                                      //����������־λ����Ϊ����
    s_structLCDCB.setXCmd = 0X2A;                               //����X����ָ��
    s_structLCDCB.setYCmd = 0X2B;                               //����Y����ָ�� 
    
    LCDSetCursor(0x00, 0x0000);                                 //���ù��λ�� 
    
    s_structLCDCB.dir = 1;                                      //����������־λ����Ϊ����   
    s_structLCDCB.setXCmd = 0X2B;                               //����X����ָ��
    s_structLCDCB.setYCmd = 0X2A;                               //����Y����ָ��      
  }
  else
  {
    LCDSetCursor(0x00,0x0000);                                  //���ù��λ�� 
  }
  
  LCD_WriteRAM_Prepare();                                       //��ʼд��GRAM     
  
  for(index = 0; index < totalPoint; index++)                   //ѭ����ֱ�����index�ﵽLCD���������ص���
  {
    LCD->LCD_RAM = color;                                       //д�����ݣ���ʾ��ɫ
  }
} 

/*********************************************************************************************************
* ��������: LCDSetCursor
* ��������: ���ù��λ��
* �������: xPos:�����꣬yPos:������
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDSetCursor(u16 xPos, u16 yPos)
{   
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310)//���LCD��IDΪ9341��5310
  {        
    LCD_WR_REG(s_structLCDCB.setXCmd);        //�������λ�üĴ���д����
    LCD_WR_DATA(xPos >> 8);                   //������λ�����ư�λ
    LCD_WR_DATA(xPos & 0XFF);                 //�����갴λ��0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);        //��������λ�üĴ���д����
    LCD_WR_DATA(yPos >> 8);                   //������λ�����ư�λ
    LCD_WR_DATA(yPos & 0XFF);                 //�����갴λ��0XFF
  }   
  else if(s_structLCDCB.id == 0X6804)         //���LCD��IDΪ6804
  {
    if(s_structLCDCB.dir == 1)                //���������Ʊ�־λΪ����
    {
      xPos = s_structLCDCB.width - 1 - xPos;  //����ʱ����  
    }  
    LCD_WR_REG(s_structLCDCB.setXCmd);        //�������λ�üĴ���д����
    LCD_WR_DATA(xPos >> 8);                   //������λ�����ư�λ
    LCD_WR_DATA(xPos & 0XFF);                 //�����갴λ��0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);        //��������λ�üĴ���д����
    LCD_WR_DATA(yPos >> 8);                   //������λ�����ư�λ
    LCD_WR_DATA(yPos & 0XFF);                 //�����갴λ��0XFF
  }
  else if(s_structLCDCB.id == 0X1963)         //���LCD��IDΪ1963
  {             
    if(s_structLCDCB.dir == 0)                //���������Ʊ�־λΪ����
    {
      xPos = s_structLCDCB.width - 1 - xPos;  //����ʱ����
      
      LCD_WR_REG(s_structLCDCB.setXCmd);      //�������λ�üĴ���д����
      LCD_WR_DATA(0);                         //д������0
      LCD_WR_DATA(0);                         //д������0
      LCD_WR_DATA(xPos >> 8);                 //������λ�����ư�λ
      LCD_WR_DATA(xPos&0XFF);                 //�����갴λ��0XFF
    }
    else
    {
      LCD_WR_REG(s_structLCDCB.setXCmd);            //�������λ�üĴ���д����
      LCD_WR_DATA(xPos >> 8);                       //������λ�����ư�λ
      LCD_WR_DATA(xPos & 0XFF);                     //�����갴λ��0XFF
      LCD_WR_DATA((s_structLCDCB.width - 1) >> 8);  //��Ļ�Ŀ�ȼ�1�������ư�λ
      LCD_WR_DATA((s_structLCDCB.width - 1) & 0XFF);//��Ļ�Ŀ�ȼ�1���ڰ�λ��0XFF
    }  
    LCD_WR_REG(s_structLCDCB.setYCmd);              //��������λ�üĴ���д����
    LCD_WR_DATA(yPos >> 8);                         //������λ�����ư�λ
    LCD_WR_DATA(yPos & 0XFF);                       //�����갴λ��0XFF
    LCD_WR_DATA((s_structLCDCB.height - 1) >> 8);   //��Ļ�Ŀ�ȼ�1�������ư�λ
    LCD_WR_DATA((s_structLCDCB.height - 1) & 0XFF); //��Ļ�Ŀ�ȼ�1���ڰ�λ��0XFF           
  }
  else if(s_structLCDCB.id == 0X5510)         //���LCD��IDΪ5510
  {
    LCD_WR_REG(s_structLCDCB.setXCmd);        //�������λ�üĴ���д����
    LCD_WR_DATA(xPos >> 8);                   //������λ�����ư�λ
    LCD_WR_REG(s_structLCDCB.setXCmd + 1);    //���������һ��λ�üĴ���д����
    LCD_WR_DATA(xPos & 0XFF);                 //�����갴λ��0XFF   
    LCD_WR_REG(s_structLCDCB.setYCmd);        //��������λ�üĴ���д����
    LCD_WR_DATA(yPos >> 8);                   //������λ�����ư�λ
    LCD_WR_REG(s_structLCDCB.setYCmd + 1);    //����������һ��λ�üĴ���д����
    LCD_WR_DATA(yPos & 0XFF);                 //�����갴λ��0XFF
  }
  else
  {
    if(s_structLCDCB.dir == 1)                //���������Ʊ�־λΪ����
    {
      xPos = s_structLCDCB.width - 1 - xPos;  //����ʱ����
    }
    
    LCD_WriteReg(s_structLCDCB.setXCmd, xPos);//�������λ�üĴ���д����
    LCD_WriteReg(s_structLCDCB.setYCmd, yPos);//��������λ�üĴ���д����
  }
}

/*********************************************************************************************************
* ��������: LCDDrawPoint
* ��������: ����
* �������: x,y:����
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: s_iPointColor:�˵����ɫ
*********************************************************************************************************/
void LCDDrawPoint(u16 x,u16 y)
{
  LCDSetCursor(x,y);            //���ù��λ�� 
  LCD_WriteRAM_Prepare();       //��ʼд��GRAM
  
  LCD->LCD_RAM = s_iPointColor; //д�����ݣ���ʾ��ɫ
}

/*********************************************************************************************************
* ��������: LCDFastDrawPoint
* ��������: ���ٻ���
* �������: x,y:���꣬color:��ɫ
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: s_iPointColor:�˵����ɫ,�ú���ʹ�ú궨����л��㣬�����ٶ���ԽϿ�
*********************************************************************************************************/
void LCDFastDrawPoint(u16 x, u16 y, u16 color)
{     
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310) //���LCD��IDΪ9341��5310
  {
    LCD_WR_REG(s_structLCDCB.setXCmd);      //�������λ�üĴ���д����
    LCD_WR_DATA(x >> 8);                    //������λ�����ư�λ
    LCD_WR_DATA(x & 0XFF);                  //�����갴λ��0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);      //��������λ�üĴ���д����
    LCD_WR_DATA(y >> 8);                    //������λ�����ư�λ
    LCD_WR_DATA(y & 0XFF);                  //�����갴λ��0XFF
  } 
  else if(s_structLCDCB.id == 0X5510)       //���LCD��IDΪ5510
  { 
    LCD_WR_REG(s_structLCDCB.setXCmd);      //�������λ�üĴ���д����
    LCD_WR_DATA(x >> 8);                    //������λ�����ư�λ
    LCD_WR_REG(s_structLCDCB.setXCmd + 1);  //���������һ��λ�üĴ���д����
    LCD_WR_DATA(x & 0XFF);                  //�����갴λ��0XFF   
    LCD_WR_REG(s_structLCDCB.setYCmd);      //��������λ�üĴ���д����
    LCD_WR_DATA(y >> 8);                    //������λ�����ư�λ
    LCD_WR_REG(s_structLCDCB.setYCmd + 1);  //����������һ��λ�üĴ���д����
    LCD_WR_DATA(y & 0XFF);                  //�����갴λ��0XFF
  } 
  else if(s_structLCDCB.id == 0X1963)       //���LCD��IDΪ1963
  { 
    if(s_structLCDCB.dir == 0)              //���������Ʊ�־λΪ����
    { 
      x = s_structLCDCB.width - 1 - x;      //����ʱ����
    } 
    
    LCD_WR_REG(s_structLCDCB.setXCmd);      //�������λ�üĴ���д����
    LCD_WR_DATA(x >> 8);                    //������λ�����ư�λ
    LCD_WR_DATA(x & 0XFF);                  //�����갴λ��0XFF
    LCD_WR_DATA(x >> 8);                    //������λ�����ư�λ
    LCD_WR_DATA(x & 0XFF);                  //�����갴λ��0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);      //��������λ�üĴ���д����
    LCD_WR_DATA(y >> 8);                    //������λ�����ư�λ
    LCD_WR_DATA(y & 0XFF);                  //�����갴λ��0XFF
    LCD_WR_DATA(y >> 8);                    //������λ�����ư�λ
    LCD_WR_DATA(y & 0XFF);                  //�����갴λ��0XFF
  } 
  else if(s_structLCDCB.id == 0X6804)       //���LCD��IDΪ6804
  {         
    if(s_structLCDCB.dir == 1)              //���������Ʊ�־λΪ����
    { 
      x = s_structLCDCB.width - 1 - x;      //����ʱ����
    } 
    
    LCD_WR_REG(s_structLCDCB.setXCmd);      //�������λ�üĴ���д����
    LCD_WR_DATA(x >> 8);                    //������λ�����ư�λ
    LCD_WR_DATA(x & 0XFF);                  //�����갴λ��0XFF
    LCD_WR_REG(s_structLCDCB.setYCmd);      //��������λ�üĴ���д����
    LCD_WR_DATA(y >> 8);                    //������λ�����ư�λ
    LCD_WR_DATA(y & 0XFF);                  //�����갴λ��0XFF
  }
  else
  {
    if(s_structLCDCB.dir == 1)              //���������Ʊ�־λΪ����
    {
      x = s_structLCDCB.width - 1 - x;      //������ʵ���ǵ�תx,y����
    }
    
    LCD_WriteReg(s_structLCDCB.setXCmd, x); //����x��ָ��
    LCD_WriteReg(s_structLCDCB.setYCmd, y); //����y��ָ��
  }       
  
  LCD->LCD_REG = s_structLCDCB.wramcmd;     //��ʼдGRAMָ��
  LCD->LCD_RAM = color;                     //����ɫд��RAM
}

/*********************************************************************************************************
* ��������: LCDReadPoint
* ��������: ��ȡ��ĳ�����ɫֵ
* �������: x,y:����
* �������: void
* �� �� ֵ: �˵����ɫ
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
u16 LCDReadPoint(u16 x, u16 y)
{
  u16 r = 0;  //
  u16 g = 0;  //
  u16 b = 0;  //
  
  if(x >= s_structLCDCB.width || y >= s_structLCDCB.height)
  {
    return 0;                                                     //�����˷�Χ,ֱ�ӷ���
  }    
  
  LCDSetCursor(x,y);                                              //������Ƶ�ָ��λ��
  
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X6804 || 
     s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X1963)
  {
    LCD_WR_REG(0X2E);                                             //9341/6804/3510/1963 ���Ͷ�GRAMָ��
  }
  else if(s_structLCDCB.id == 0X5510)                             //���LCD��IDΪ5510
  {
    LCD_WR_REG(0X2E00);                                           //5510 ���Ͷ�GRAMָ��
  }
  else
  { 
    LCD_WR_REG(0X22);                                             //����IC���Ͷ�GRAMָ��
  }
  
  if(s_structLCDCB.id == 0X9320)                                  //���LCD��IDΪ9320
  {
    opt_delay(2);                                                 //��ʱ2us      
  }
  
  r = LCD_RD_DATA();                                              //dummy Read����Ч�Ķ�ȡֵ
  
  if(s_structLCDCB.id == 0X1963)
  {
    return r;                                                     //1963ֱ�Ӷ��Ϳ��� 
  }
  
  opt_delay(2);                                                   //��ʱ2us    
  
  r = LCD_RD_DATA();                                              //ʵ��������ɫ  
  
  //9341/NT35310/NT35510Ҫ��2�ζ���
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X5510)    
  {
    opt_delay(2);                                                 //��ʱ2us    
    
    b = LCD_RD_DATA();                          
    
    //����9341/5310/5510,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
    g = r & 0XFF;     
    g <<= 8;
  } 
  
  if(s_structLCDCB.id == 0X9325 || s_structLCDCB.id == 0X4535 || s_structLCDCB.id == 0X4531 ||  
     s_structLCDCB.id == 0XB505 || s_structLCDCB.id == 0XC505)
  {
    return r; //�⼸��ICֱ�ӷ�����ɫֵ
  }
  else if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X5510)
  {
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  //ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
  }
  else 
  {
    return LCD_BGR2RGB(r);  //����IC
  }
}       

/*********************************************************************************************************
* ��������: LCDDrawCircle
* ��������: ��ָ��λ�û�һ��ָ����С��Բ
* �������: (x,y):���ĵ㣬r:�뾶
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDDrawCircle(u16 x0,u16 y0,u8 r)
{
  int a;                        
  int b;                        //������Ű뾶
  int di;
  
  a = 0;
  b = r;    
  di = 3 - (r << 1);             //�ж��¸���λ�õı�־
  
  while(a <= b)
  {
    LCDDrawPoint(x0 + a, y0 - b); //5
    LCDDrawPoint(x0 + b, y0 - a); //0           
    LCDDrawPoint(x0 + b, y0 + a); //4   
    LCDDrawPoint(x0 + a, y0 + b); //6 
    LCDDrawPoint(x0 - a, y0 + b); //1       
    LCDDrawPoint(x0 - b, y0 + a); 
    LCDDrawPoint(x0 - a, y0 - b); //2 
    LCDDrawPoint(x0 - b, y0 - a); //7    
    
    a++;
    
    //ʹ��Bresenham�㷨��Բ     
    if(di < 0)
    {
      di += 4 * a + 6;
    }      
    else
    {
      di += 10 + 4 * (a - b);   
      b--;
    }     
  }
} 

/*********************************************************************************************************
* ��������: LCDDrawLine
* ��������: ����
* �������: (x1,y1):������ꣻ(x2,y2):�յ�����
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDDrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
  u16 t; 
  int xerr = 0, yerr = 0, delta_x, delta_y, distance; 
  int incx, incy, uRow, uCol; 
  
  delta_x = x2 - x1;                  //������������ 
  delta_y = y2 - y1;              
  uRow = x1;              
  uCol = y1;              
                
  if(delta_x > 0)                     //��x����������0
  {             
    incx = 1;                         //����x�ᵥ������Ϊ������
  }             
  else if(delta_x == 0)               //��x����������0
  {             
    incx = 0;                         //��ֱ�� 
  }             
  else                                //��x������С��0
  {             
    incx = -1;                        //����x�ᵥ������Ϊ������
    delta_x = -delta_x;               //ȡ����ֵ
  }               
                
  if(delta_y > 0)                     //��y����������0
  {             
    incy = 1;                         //����y�ᵥ������Ϊ������
  }                 
  else if(delta_y == 0)               //��y����������0
  {             
    incy = 0;                         //ˮƽ�� 
  }             
  else                                //��y������С��0
  {             
    incy = -1;                        //����y�ᵥ������Ϊ������
    delta_y = -delta_y;               //ȡ����ֵ
  } 
  
  if(delta_x > delta_y)               //�Ƚ�x��y��������С��ѡȡ�ϴ������Ϊ��������������
  {             
    distance = delta_x;               //x�������ϴ���ѡȡx������Ϊ��������������
  }    
  else
  {
    distance = delta_y;               //y�������ϴ���ѡȡy������Ϊ��������������
  }
  
  for(t = 0; t <= distance + 1; t++)  //������� 
  {  
    LCDDrawPoint(uRow, uCol);         //���� 
    xerr += delta_x ; 
    yerr += delta_y ;
    
    if(xerr > distance) 
    { 
      xerr -= distance; 
      uRow += incx; 
    } 
    
    if(yerr > distance) 
    { 
      yerr -= distance; 
      uCol += incy; 
    } 
  }  
} 

/*********************************************************************************************************
* ��������: LCDDrawRect
* ��������: ������
* �������: (x1,y1),(x2,y2):���εĶԽ�����
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDDrawRect(u16 x1, u16 y1, u16 x2, u16 y2)
{
  //���û��ߺ����������ε�������
  LCDDrawLine(x1,y1,x2,y1);         //���ϱߵĳ�
  LCDDrawLine(x1,y1,x1,y2);         //����ߵĿ�
  LCDDrawLine(x1,y2,x2,y2);         //���±ߵĳ�
  LCDDrawLine(x2,y1,x2,y2);         //���ұߵĿ�
}

/*********************************************************************************************************
* ��������: LCDFill
* ��������: ��ָ����������䵥����ɫ
* �������: (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)��color:Ҫ������ɫ
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDFill(u16 startX, u16 startY, u16 endX, u16 endY, u16 color)
{      
  u16 i, j;
  u16 xlen = 0;
  u16 temp;
  
  if((s_structLCDCB.id == 0X6804) && (s_structLCDCB.dir == 1))  //6804������ʱ�����⴦��  
  {
    temp = startX;
    startX = startY;
    startY = s_structLCDCB.width - endX - 1;    
    endX = endY;
    endY = s_structLCDCB.width - temp - 1;
    s_structLCDCB.dir = 0;   
    s_structLCDCB.setXCmd = 0X2A;
    s_structLCDCB.setYCmd = 0X2B;           
    LCDFill(startX, startY, endX, endY, color);  
    s_structLCDCB.dir = 1;   
    s_structLCDCB.setXCmd = 0X2B;
    s_structLCDCB.setYCmd = 0X2A;     
  }
  else
  {
    xlen = endX - startX + 1;   
    for(i = startY; i <= endY; i++)
    {
      LCDSetCursor(startX, i);  //���ù��λ�� 
      LCD_WriteRAM_Prepare();   //��ʼд��GRAM    
      
      for(j = 0; j < xlen; j++)
      {
        LCD->LCD_RAM = color;  //д�����ݣ���ʾ��ɫ
      }         
    }
  }
}  

/*********************************************************************************************************
* ��������: LCDColorFill
* ��������: ��ָ�����������ָ����ɫ��
* �������: (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)��color:Ҫ������ɫ
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDColorFill(u16 startX, u16 startY, u16 endX, u16 endY, u16* pColor)
{  
  u16 height, width;                        //�������ָ������ĸ߶ȺͿ��
  u16 i, j;                                 //ѭ��������
            
  width = endX - startX + 1;                //�õ����Ŀ��
  height = endY - startY + 1;               //�õ����ĸ߶�
  
   for(i = 0; i < height; i++)
  {
    LCDSetCursor(startX, startY + i);       //���ù��λ�� 
    LCD_WriteRAM_Prepare();                 //��ʼд��GRAM
    
    for(j = 0; j < width; j++)
    {
      LCD->LCD_RAM = pColor[i * width + j]; //д������
    }      
  }      
}  

/*********************************************************************************************************
* ��������: LCDShowChar
* ��������: ��ָ��λ����ʾһ���ַ�
* �������: x,y:��ʼ���꣬num:Ҫ��ʾ���ַ�:" "��"~"��size:�����С 12/16/24��mode:���ӷ�ʽ(1)�ǵ��ӷ�ʽ(0)
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{                  
  u8 temp;                                                      //�������
  u8 t1, t;                                                     //ѭ��������
  u16 y0 = y;                                                   //����������������ʼλ��
  u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);    //�õ�����һ���ַ���Ӧ������ռ���ֽ���  
  
  num = num - ' ';          //�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ�ƫ������
  
  for(t = 0; t < csize; t++)
  {   
    if(size == 12)
    {
      temp = asc2_1206[num][t];               //����1206����
    }             
    else if(size== 16)              
    {             
      temp=asc2_1608[num][t];                 //����1608����
    }             
    else if(size == 24)             
    {             
      temp=asc2_2412[num][t];                 //����2412����
    }             
    else              
    {             
      return;                                 //û���ֿ�
    }
    for(t1 = 0; t1 < 8; t1++)   
    {          
      if(temp & 0x80)
      {
        LCDFastDrawPoint(x, y, s_iPointColor);//�Ի��ʵ���ɫ���ٻ���
      }
      else if(mode == 0)
      {
        LCDFastDrawPoint(x, y, s_iBackColor); //�Ա���Ƶ���ɫ���ٻ���
      }
      
      temp <<= 1;                             //����һλ
      y++;                                    //�������1
                
      if(y >= s_structLCDCB.height)           //�������ߴ���LCD���ĸ߶�
      {         
        return;                               //�������ˣ���������
      }         
      if((y - y0) == size)          
      {         
        y = y0;                               //���������»ص����
        x++;                                  //�������1
                  
        if(x >= s_structLCDCB.width)          //������������LCD���Ŀ��
        {         
          return;                             //�������ˣ���������
        }         
        break;                                //����ѭ��
      }
    }     
  }                    
} 

/*********************************************************************************************************
* ��������: LCDShowNum
* ��������: ��ʾ����,��λΪ0,����ʾ
* �������: x,y:��ʼ���꣬len :���ֵ�λ����size:�����С 12/16/24��color:��ɫ��num:��ֵ(0~4294967295)
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{           
  u8 t;                                                       //ѭ��������
  u8 temp;                                                    //���ڴ�����ֵĸ���λ
  u8 enshow = 0;                                              //�����ж��Ƿ�Ϊ��λ0�ı�־λ
  
  for(t = 0; t < len; t++)                                    //ѭ��ֱ����ʾ���ֵ�����λ
  {
    temp = (num / LCDPower(10, len - t - 1)) % 10;            //�Ӹߵ�������ȡ�����ֵĸ���λ
    
    if(enshow == 0 && t < (len - 1))                          //û�г������ֵĳ����������˸�λ0��־
    {
      if(temp == 0)                                           //ȡ��������Ϊ0
      {
        LCDShowChar(x + (size / 2) * t, y, ' ', size, 0);     //��λ0�ÿո����
        continue;                                             //��������ѭ����������һ��ѭ��
      }
      else
      {
        enshow = 1;                                           //�����λ0��־
      }               
    }   
    
    LCDShowChar(x + (size / 2) * t, y, temp + '0', size, 0);  //������ʾ�ַ�������ʾ����
  }
} 

/*********************************************************************************************************
* ��������: LCDShowxNum
* ��������: ��ʾ����,��λΪ0,������ʾ
* �������: x,y:��ʼ���꣬num:��ֵ(0~999999999)��len:����(��Ҫ��ʾ��λ��)��size:�����С 12/16/24
*           mode��[7]:0,�����;1,���0��[6:1]:������[0]:0,�ǵ�����ʾ;1,������ʾ
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{  
  u8 t;                                                                //ѭ��������
  u8 temp;                                                             //���ڴ�����ֵĸ���λ
  u8 enshow = 0;                                                       //�����ж��Ƿ�Ϊ��λ0�ı�־λ
                                                                       
  for(t = 0; t < len; t++)                                             //ѭ��ֱ����ʾ���ֵ�����λ
  {                                                                    
    temp = (num / LCDPower(10, len - t - 1)) % 10;                     //�Ӹߵ�������ȡ�����ֵĸ���λ
                                                                       
    if(enshow == 0 && t < (len - 1))                                   //û�г������ֵĳ����������˸�λ0��־
    {                                                                  
      if(temp == 0)                                                    //ȡ��������Ϊ0
      {                                                                
        if(mode & 0X80)                                                
        {                                                              
          LCDShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);  //��ʾ0
        }                                                              
        else                                                           
        {                                                              
          LCDShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);  //��λ0�ÿո����
        }                                                              
        continue;                                                      //��������ѭ����������һ��ѭ��                         
      }                                                                
      else
      {
        enshow = 1;                                                    //�����λ0��־
      }
    }   
    LCDShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01); //������ʾ�ַ�������ʾ����
  }
} 

/*********************************************************************************************************
* ��������: LCDShowString
* ��������: ��ʾ�ַ���
* �������: x,y:������꣬width,height:�����С��size:�����С 12/16/24��*p:�ַ�����ʼ��ַ
* �������:
* �� �� ֵ: 
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8* p)
{         
  u8 x0   = x;                        //��ź������
  width  += x;                        //�����ת��Ϊ����ĩ��ַ
  height += y;                        //���߶�ת��Ϊ����ĩ��ַ
  
  while((*p <= '~') && (*p >= ' '))   //�ж��ǲ��ǷǷ��ַ�!
  {       
    if(x >= width)
    {
      x = x0;                         //������ָ��������
      y += size;                      //������ָ����һ��
    }
    if(y >= height)
    {
      break;                          //�����곬�������˳�  
    }  
    
    LCDShowChar(x, y, *p, size, 1);   //������ʾ�ַ�������ʾһ���ַ�
    
    x += size / 2;                    //x���������һλ
    p++;                              //�ַ���ָ���1
  }  
}

/*********************************************************************************************************
* ��������: LCDSSDSetBackLight
* ��������: SSD1963 ��������
* �������: pwm:����ȼ�,0~100.Խ��Խ��
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/
void LCDSSDSetBackLight(u8 pwm)
{  
  LCD_WR_REG(0xBE);       //����PWM���
  LCD_WR_DATA(0x05);      //����PWMƵ��
  LCD_WR_DATA(pwm * 2.55);//����PWMռ�ձ�
  LCD_WR_DATA(0x01);      
  LCD_WR_DATA(0xFF);      
  LCD_WR_DATA(0x00);      
  LCD_WR_DATA(0x00);      
}

/*********************************************************************************************************
* ��������: LCDSetScanDir
* ��������: ����LCD���Զ�ɨ�跽��
* �������: pwm:����ȼ�,0~100.Խ��Խ��
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: �����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
*           ����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������
*           dir:0~7,����8������(���嶨���lcd.h)
*           9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963��IC�Ѿ�ʵ�ʲ���
*********************************************************************************************************/       
void LCDSetScanDir(u8 dir)
{
  u16 regval = 0; //���ڴ�żĴ�����ֵ
  u16 dirreg = 0; //���ڴ����Ļ�ķ���
  u16 temp;  
  
  //����ʱ����6804��1963���ı�ɨ�跽������ʱ1963�ı䷽��
  if((s_structLCDCB.dir == 1 && s_structLCDCB.id != 0X6804 && s_structLCDCB.id != 0X1963) || 
     (s_structLCDCB.dir == 0 && s_structLCDCB.id == 0X1963))
  {         
    switch(dir)//����ת��
    {
      case 0:
        dir = 6;
        break;
      case 1:
        dir = 7;
        break;
      case 2:
        dir = 4;
        break;
      case 3:
        dir = 5;
        break;
      case 4:
        dir = 1;
        break;
      case 5:
        dir = 0;
        break;
      case 6:
        dir = 3;
        break;
      case 7:
        dir = 2;
        break;       
    }
  } 
  //9341/6804/5310/5510/1963,���⴦��
  if(s_structLCDCB.id == 0x9341 || s_structLCDCB.id == 0X6804 || s_structLCDCB.id == 0X5310 || 
     s_structLCDCB.id == 0X5510 || s_structLCDCB.id == 0X1963)
  {
    switch(dir)
    {
      case L2R_U2D://������,���ϵ���
        regval |= (0 << 7) | (0 << 6) | (0 << 5); 
        break;
      case L2R_D2U://������,���µ���
        regval |= (1 << 7) | (0 << 6) | (0 << 5); 
        break;
      case R2L_U2D://���ҵ���,���ϵ���
        regval |= (0 << 7) | (1 << 6) | (0 << 5); 
        break;
      case R2L_D2U://���ҵ���,���µ���
        regval |= (1 << 7) | (1 << 6) | (0 << 5); 
        break;   
      case U2D_L2R://���ϵ���,������
        regval |= (0 << 7) | (0 << 6) | (1 << 5); 
        break;
      case U2D_R2L://���ϵ���,���ҵ���
        regval |= (0 << 7) | (1 << 6) | (1 << 5); 
        break;
      case D2U_L2R://���µ���,������
        regval |= (1 << 7) | (0 << 6) | (1 << 5); 
        break;
      case D2U_R2L://���µ���,���ҵ���
        regval |= (1 << 7) | (1 << 6) | (1 << 5); 
        break;   
    }
    
    if(s_structLCDCB.id == 0X5510)
    {
      dirreg = 0X3600;  
    }  
    else 
    {
      dirreg = 0X36;
    }
    
    if((s_structLCDCB.id != 0X5310) && (s_structLCDCB.id != 0X5510) && (s_structLCDCB.id != 0X1963))
    {
      regval |= 0X08;                                   //5310/5510/1963����ҪBGR
    }
       
    if(s_structLCDCB.id == 0X6804)
    {
      regval |= 0x02;                                   //6804��BIT6��9341�ķ���
    }
    
    LCD_WriteReg(dirreg, regval);
    
    if(s_structLCDCB.id != 0X1963)                      //1963�������괦��
    {
      if(regval & 0X20)
      {
        if(s_structLCDCB.width < s_structLCDCB.height)  //����X,Y
        {
          temp = s_structLCDCB.width;
          s_structLCDCB.width = s_structLCDCB.height;
          s_structLCDCB.height = temp;
        }
      }
      else  
      {
        if(s_structLCDCB.width>s_structLCDCB.height)    //����X,Y
        {
          temp = s_structLCDCB.width;
          s_structLCDCB.width = s_structLCDCB.height;
          s_structLCDCB.height = temp;
        }
      }  
    }
    
    if(s_structLCDCB.id == 0X5510)
    {
      LCD_WR_REG(s_structLCDCB.setXCmd);
      LCD_WR_DATA(0); 
      LCD_WR_REG(s_structLCDCB.setXCmd + 1);
      LCD_WR_DATA(0); 
      LCD_WR_REG(s_structLCDCB.setXCmd + 2);
      LCD_WR_DATA((s_structLCDCB.width - 1) >> 8); 
      LCD_WR_REG(s_structLCDCB.setXCmd + 3);
      LCD_WR_DATA((s_structLCDCB.width - 1) & 0XFF); 
      LCD_WR_REG(s_structLCDCB.setYCmd);
      LCD_WR_DATA(0); 
      LCD_WR_REG(s_structLCDCB.setYCmd + 1);
      LCD_WR_DATA(0); 
      LCD_WR_REG(s_structLCDCB.setYCmd + 2);
      LCD_WR_DATA((s_structLCDCB.height - 1) >> 8); 
      LCD_WR_REG(s_structLCDCB.setYCmd + 3);
      LCD_WR_DATA((s_structLCDCB.height - 1) & 0XFF);
    }
    else
    {
      LCD_WR_REG(s_structLCDCB.setXCmd); 
      LCD_WR_DATA(0);
      LCD_WR_DATA(0);
      LCD_WR_DATA((s_structLCDCB.width - 1) >> 8);
      LCD_WR_DATA((s_structLCDCB.width - 1) & 0XFF);
      LCD_WR_REG(s_structLCDCB.setYCmd); 
      LCD_WR_DATA(0);
      LCD_WR_DATA(0);
      LCD_WR_DATA((s_structLCDCB.height - 1) >> 8);
      LCD_WR_DATA((s_structLCDCB.height - 1) & 0XFF);  
    }
  }
  else 
  {
    switch(dir)
    {
      case L2R_U2D://������,���ϵ���
        regval |= (1 << 5) | (1 << 4) | (0 << 3); 
        break;
      case L2R_D2U://������,���µ���
        regval |= (0 << 5) | (1 << 4) | (0 << 3); 
        break;
      case R2L_U2D://���ҵ���,���ϵ���
        regval |= (1 << 5) | (0 << 4) | (0 << 3);
        break;
      case R2L_D2U://���ҵ���,���µ���
        regval |= (0 << 5) | (0 << 4) | (0 << 3); 
        break;   
      case U2D_L2R://���ϵ���,������
        regval |= (1 << 5) | (1 << 4) | (1 << 3); 
        break;
      case U2D_R2L://���ϵ���,���ҵ���
        regval |= (1 << 5) | (0 << 4) | (1 << 3); 
        break;
      case D2U_L2R://���µ���,������
        regval |= (0 << 5) | (1 << 4) | (1 << 3); 
        break;
      case D2U_R2L://���µ���,���ҵ���
        regval |= (0 << 5) | (0 << 4) | (1 << 3); 
        break;   
    } 
    dirreg = 0X03;
    regval |= 1 << 12; 
    LCD_WriteReg(dirreg, regval);
  }
} 

/*********************************************************************************************************
* ��������: LCDSetDispDir
* ��������: ����LCD��ʾ����
* �������: dir:0,������1,����
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/   
void LCDSetDispDir(u8 dir)
{
  if(dir == 0)  //����
  {
    s_structLCDCB.dir = 0;                  //����������־λ����Ϊ����
    s_structLCDCB.width = 240;              //����LCD���Ŀ��Ϊ240
    s_structLCDCB.height = 320;             //����LCD���ĸ߶�Ϊ320
    
    if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X6804 || s_structLCDCB.id == 0X5310)
    {
      s_structLCDCB.wramcmd = 0X2C;         //����д��GRAM��ָ�� 
      s_structLCDCB.setXCmd = 0X2A;         //����дX����ָ��
      s_structLCDCB.setYCmd = 0X2B;         //����дY����ָ��     
      if(s_structLCDCB.id == 0X6804 || s_structLCDCB.id == 0X5310)
      {
        s_structLCDCB.width = 320;          //����LCD���Ŀ��320
        s_structLCDCB.height = 480;         //����LCD���ĸ߶�480 
      }
    }
    else if(s_structLCDCB.id == 0x5510)
    {
      s_structLCDCB.wramcmd = 0X2C00;       //����д��GRAM��ָ�� 
      s_structLCDCB.setXCmd = 0X2A00;       //����дX����ָ��
      s_structLCDCB.setYCmd = 0X2B00;       //����дY����ָ�� 
      s_structLCDCB.width = 480;            //����LCD���Ŀ��480
      s_structLCDCB.height = 800;           //����LCD���ĸ߶�800 ;
    }
    else if(s_structLCDCB.id == 0X1963)
    {
      s_structLCDCB.wramcmd = 0X2C;         //����д��GRAM��ָ�� 
      s_structLCDCB.setXCmd = 0X2B;         //����дX����ָ��
      s_structLCDCB.setYCmd = 0X2A;         //����дY����ָ��
      s_structLCDCB.width = 480;            //����LCD���Ŀ��480
      s_structLCDCB.height = 800;           //����LCD���ĸ߶�800  
    }
    else
    {
      s_structLCDCB.wramcmd = 0X22;         //����д��GRAM��ָ�� 
      s_structLCDCB.setXCmd = 0X20;         //����дX����ָ��
      s_structLCDCB.setYCmd = 0X21;         //����дY����ָ��   
    }
  }else                                     //����������־λ����Ϊ����
  {            
    s_structLCDCB.dir = 1;                  //����������־λ����Ϊ����
    s_structLCDCB.width = 320;              //����LCD���Ŀ��Ϊ320
    s_structLCDCB.height = 240;             //����LCD���ĸ߶�Ϊ240
    
    if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310)
    {
      s_structLCDCB.wramcmd = 0X2C;         //����д��GRAM��ָ�� 
      s_structLCDCB.setXCmd = 0X2A;         //����дX����ָ��
      s_structLCDCB.setYCmd = 0X2B;         //����дY����ָ��      
    }
    else if(s_structLCDCB.id == 0X6804)   
    {
      s_structLCDCB.wramcmd = 0X2C;         //����д��GRAM��ָ�� 
      s_structLCDCB.setXCmd = 0X2B;         //����дX����ָ��
      s_structLCDCB.setYCmd = 0X2A;         //����дY����ָ��  
    }
    else if(s_structLCDCB.id == 0x5510)
    {
      s_structLCDCB.wramcmd = 0X2C00;       //����д��GRAM��ָ�� 
      s_structLCDCB.setXCmd = 0X2A00;       //����дX����ָ��
      s_structLCDCB.setYCmd = 0X2B00;       //����дY����ָ��  
      s_structLCDCB.width = 800;
      s_structLCDCB.height = 480;
    }
    else if(s_structLCDCB.id == 0X1963)
    {
      s_structLCDCB.wramcmd = 0X2C;         //����д��GRAM��ָ�� 
      s_structLCDCB.setXCmd = 0X2A;         //����дX����ָ��
      s_structLCDCB.setYCmd = 0X2B;         //����дY����ָ��
      s_structLCDCB.width = 800;            //����LCD���Ŀ��800
      s_structLCDCB.height = 480;           //����LCD���ĸ߶�480  
    }
    else
    {
      s_structLCDCB.wramcmd = 0X22;         //����д��GRAM��ָ�� 
      s_structLCDCB.setXCmd = 0X21;         //����дX����ָ��
      s_structLCDCB.setYCmd = 0X20;         //����дY����ָ��   
    }
    
    if(s_structLCDCB.id == 0X6804 || s_structLCDCB.id == 0X5310)
    {    
      s_structLCDCB.width = 480;            //����LCD���Ŀ��Ϊ480
      s_structLCDCB.height = 320;           //����LCD���ĸ߶�Ϊ320
    }
  } 
  
  LCDSetScanDir(DFT_SCAN_DIR);              //Ĭ��ɨ�跽��
}   

/*********************************************************************************************************
* ��������: LCDSetWindow
* ��������: ���ô���,���Զ����û������굽�������Ͻ�(sx,sy)
* �������: sx, sy:������ʼ����(���Ͻ�)��width, height:���ڿ�Ⱥ͸߶�,�������0�������С:width * height
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01�� 
* ע    ��: 
*********************************************************************************************************/   
void LCDSetWindow(u16 sx, u16 sy, u16 width, u16 height)
{    
  u8  hsareg, heareg, vsareg, veareg;
  u16 hsaval, heaval, vsaval, veaval; 
  u16 twidth, theight;
  
  twidth  = sx + width - 1;
  theight = sy + height - 1;
  
  if(s_structLCDCB.id == 0X9341 || s_structLCDCB.id == 0X5310 || s_structLCDCB.id == 0X6804 || 
     (s_structLCDCB.dir == 1 && s_structLCDCB.id == 0X1963))
  {
    LCD_WR_REG(s_structLCDCB.setXCmd); 
    LCD_WR_DATA(sx >> 8); 
    LCD_WR_DATA(sx & 0XFF);   
    LCD_WR_DATA(twidth >> 8); 
    LCD_WR_DATA(twidth & 0XFF);  
    LCD_WR_REG(s_structLCDCB.setYCmd); 
    LCD_WR_DATA(sy >> 8); 
    LCD_WR_DATA(sy & 0XFF); 
    LCD_WR_DATA(theight >> 8); 
    LCD_WR_DATA(theight & 0XFF); 
  }
  else if(s_structLCDCB.id == 0X1963)//1963�������⴦��
  {
    sx = s_structLCDCB.width - width - sx; 
    height = sy + height - 1; 
    LCD_WR_REG(s_structLCDCB.setXCmd); 
    LCD_WR_DATA(sx >> 8); 
    LCD_WR_DATA(sx & 0XFF);   
    LCD_WR_DATA((sx + width - 1) >> 8); 
    LCD_WR_DATA((sx + width - 1) & 0XFF);  
    LCD_WR_REG(s_structLCDCB.setYCmd); 
    LCD_WR_DATA(sy >> 8); 
    LCD_WR_DATA(sy & 0XFF); 
    LCD_WR_DATA(height >> 8); 
    LCD_WR_DATA(height & 0XFF);     
  }
  else if(s_structLCDCB.id == 0X5510)
  {
    LCD_WR_REG(s_structLCDCB.setXCmd);
    LCD_WR_DATA(sx >> 8);  
    LCD_WR_REG(s_structLCDCB.setXCmd + 1);
    LCD_WR_DATA(sx & 0XFF);    
    LCD_WR_REG(s_structLCDCB.setXCmd + 2);
    LCD_WR_DATA(twidth >> 8);   
    LCD_WR_REG(s_structLCDCB.setXCmd + 3);
    LCD_WR_DATA(twidth & 0XFF);   
    LCD_WR_REG(s_structLCDCB.setYCmd);
    LCD_WR_DATA(sy >> 8);   
    LCD_WR_REG(s_structLCDCB.setYCmd + 1);
    LCD_WR_DATA(sy & 0XFF);  
    LCD_WR_REG(s_structLCDCB.setYCmd + 2);
    LCD_WR_DATA(theight >> 8);   
    LCD_WR_REG(s_structLCDCB.setYCmd + 3);
    LCD_WR_DATA(theight & 0XFF);  
  }
  else  //��������IC
  {
    if(s_structLCDCB.dir == 1)//�����������־λΪ����
    {
      //����ֵ
      hsaval = sy;        
      heaval = theight;
      vsaval = s_structLCDCB.width - twidth - 1;
      veaval = s_structLCDCB.width - sx - 1;        
    }else
    { 
      hsaval = sx;        
      heaval = twidth;
      vsaval = sy;
      veaval = theight;
    } 
    hsareg = 0X50;
    heareg = 0X51;                  //ˮƽ���򴰿ڼĴ���    
    vsareg = 0X52;                  
    veareg = 0X53;                  //��ֱ���򴰿ڼĴ���
    
    //���üĴ���ֵ
    LCD_WriteReg(hsareg, hsaval);
    LCD_WriteReg(heareg, heaval);
    LCD_WriteReg(vsareg, vsaval);
    LCD_WriteReg(veareg, veaval);    
    LCDSetCursor(sx, sy);           //���ù��λ��
  }
}
