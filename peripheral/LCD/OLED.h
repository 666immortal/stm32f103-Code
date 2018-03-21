/*********************************************************************************************************
* ģ������: OLED.h
* ժ    Ҫ: OLED���� 
* ��ǰ�汾: 1.0.0
* ��    ��: 
* �������: 2018��03��01�� 
* ��    ��:
* ע    ��: OLEDȡģʹ�õ���PCtoLCD2002���������                                                                  
**********************************************************************************************************
* ȡ���汾:
* ��    ��:
* �������: 
* �޸�����:
* �޸��ļ�: 
*********************************************************************************************************/
#ifndef _OLED_H_
#define _OLED_H_
 
/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include  "DataType.h" 

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/ 
#define USER_DEFINE_OLED_CS_GPIO_PORT    GPIOB                     //�û�����OLED.CS�Ķ˿�
#define USER_DEFINE_OLED_CS_GPIO_PIN     GPIO_Pin_12               //�û�����OLED.CS������
#define USER_DEFINE_OLED_CS_GPIO_CLK     RCC_APB2Periph_GPIOB      //�û�����OLED.CS��ʱ��

#define USER_DEFINE_OLED_RES_GPIO_PORT   GPIOB                     //�û�����OLED.RES�Ķ˿�
#define USER_DEFINE_OLED_RES_GPIO_PIN    GPIO_Pin_14               //�û�����OLED.RES������
#define USER_DEFINE_OLED_RES_GPIO_CLK    RCC_APB2Periph_GPIOB      //�û�����OLED.RES��ʱ��

#define USER_DEFINE_OLED_DC_GPIO_PORT    GPIOC                     //�û�����OLED.DC�Ķ˿�
#define USER_DEFINE_OLED_DC_GPIO_PIN     GPIO_Pin_3                //�û�����OLED.DC������
#define USER_DEFINE_OLED_DC_GPIO_CLK     RCC_APB2Periph_GPIOC      //�û�����OLED.DC��ʱ��
                                         
#define USER_DEFINE_OLED_SCLK_GPIO_PORT  GPIOB                     //�û�����OLED.SCLK�Ķ˿�
#define USER_DEFINE_OLED_SCLK_GPIO_PIN   GPIO_Pin_13               //�û�����OLED.SCLK������
#define USER_DEFINE_OLED_SCLK_GPIO_CLK   RCC_APB2Periph_GPIOB      //�û�����OLED.SCLK��ʱ��

#define USER_DEFINE_OLED_SDIN_GPIO_PORT  GPIOB                     //�û�����OLED.SDIN�Ķ˿�
#define USER_DEFINE_OLED_SDIN_GPIO_PIN   GPIO_Pin_15               //�û�����OLED.SDIN������
#define USER_DEFINE_OLED_SDIN_GPIO_CLK   RCC_APB2Periph_GPIOB      //�û�����OLED.SDIN��ʱ��

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/	    
void  InitOLED(void);           //��ʼ��OLED
void  OLEDDisplayOn(void);      //����OLED��ʾ
void  OLEDDisplayOff(void);     //�ر�OLED��ʾ
void  OLEDRefreshGRAM(void);    //����OLED GRAM

void  OLEDClear(void);                                       //���OLED������
void  OLEDShowNum(u8 x, u8 y, u32 num, u8 len, u8 size);     //��OLED����ָ��λ����ʾ����
void  OLEDShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode);    //��OLED����ָ��λ����ʾ�ַ�
void  OLEDShowString(u8 x, u8 y, const u8* p);               //��OLED����ָ��λ����ʾ�ַ���

#endif  
