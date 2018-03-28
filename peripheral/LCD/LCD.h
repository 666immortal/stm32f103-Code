/*********************************************************************************************************
* ģ������: LCD.h
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
#ifndef _LCD_H_
#define _LCD_H_	

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "stdlib.h"
#include "DataType.h"
#include "stm32f10x.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
//LCD���������PB0
#define BACK_LIGHT_ON()   GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);    

//ʹ��NOR/SRAM�� Bank1.sector4,��ַλHADDR[27,26]=11 A10��Ϊ�������������� 
//ע������ʱSTM32�ڲ�������һλ����!  
#define LCD_BASE        ((u32)(0x6C000000 | 0x000007FE))    //LCD�Ļ���ַ
#define LCD             ((LCD_TypeDef *) LCD_BASE)          //��LCD�Ļ���ַǿ��ת��ΪLCD_TypeDef���͵�ָ��

#define DFT_SCAN_DIR  L2R_U2D                               //Ĭ�ϵ�ɨ�跽��

//������ɫ
#define WHITE   0xFFFF    //��ɫ
#define BLACK   0x0000    //��ɫ 
#define BLUE    0x001F    //��ɫ 
#define BRED    0XF81F    //�����ɫ
#define GRED    0XFFE0    //�̺��ɫ
#define GBLUE   0X07FF    //������ɫ
#define RED     0xF800    //��ɫ
#define MAGENTA 0xF81F    //Ʒ��ɫ
#define GREEN   0x07E0    //��ɫ
#define CYAN    0x7FFF    //����ɫ
#define YELLOW  0xFFE0    //��ɫ
#define BROWN   0XBC40    //��ɫ
#define BRRED   0XFC07    //�غ�ɫ
#define GRAY    0X8430    //��ɫ

//GUI��ɫ
#define DARKBLUE  0X01CF  //����ɫ
#define LIGHTBLUE 0X7D7C  //ǳ��ɫ  
#define GRAYBLUE  0X5458  //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN  0X841F //ǳ��ɫ
//#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY       0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE   0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE      0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

//LCD�ֱ�������
#define SSD_HOR_RESOLUTION  800 //LCDˮƽ�ֱ���
#define SSD_VER_RESOLUTION  480 //LCD��ֱ�ֱ���

//LCD������������
#define SSD_HOR_PULSE_WIDTH 1   //ˮƽ����
#define SSD_HOR_BACK_PORCH  46  //ˮƽǰ��
#define SSD_HOR_FRONT_PORCH 210 //ˮƽ����

#define SSD_VER_PULSE_WIDTH 1   //��ֱ����
#define SSD_VER_BACK_PORCH  23  //��ֱǰ��
#define SSD_VER_FRONT_PORCH 22  //��ֱǰ��

//���¼����������Զ�����
#define SSD_HT  (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_BACK_PORCH)
#define SSD_VT  (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
//LCD��ַ�ṹ��,������Ա�ֱ�����д�����д����
typedef struct
{
  vu16 LCD_REG;   //��LCDд����
  vu16 LCD_RAM;   //��LCDд����
}LCD_TypeDef;

typedef enum
{
  L2R_U2D = 0,    //������,���ϵ���
  L2R_D2U,        //������,���µ���
  R2L_U2D,        //���ҵ���,���ϵ���
  R2L_D2U,        //���ҵ���,���µ���
  U2D_L2R,        //���ϵ���,������
  U2D_R2L,        //���ϵ���,���ҵ���
  D2U_L2R,        //���µ���,������
  D2U_R2L         //���µ���,���ҵ���
}EnumDirection; 

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/    															  
void InitTFTLCD(void);                                                      //��ʼ��LCD
u16  GetLCDID(void);                                                        //��ȡLCD��ID
void LCDDispOn(void);                                                       //LCD������ʾ
void LCDDispOff(void);                                                      //LCD�ر���ʾ
void SetPointColor(u16 color);                                              //���û�����ɫ
void SetBackColor(u16 color);                                               //���ñ�����ɫ
                   
void LCDClear(u16 color);                                                   //����
void LCDSetCursor(u16 xPos, u16 yPos);                                      //���ù��
void LCDDrawPoint(u16 x, u16 y);                                            //����
void LCDFastDrawPoint(u16 x, u16 y, u16 color);                             //���ٻ���
u16  LCDReadPoint(u16 x, u16 y);                                            //���� 
void LCDDrawCircle(u16 x0, u16 y0, u8 r);                                   //��Բ
void LCDDrawLine(u16 x1, u16 y1, u16 x2, u16 y2);                           //����
void LCDDrawRect(u16 x1, u16 y1, u16 x2, u16 y2);                           //������
void LCDFill(u16 startX, u16 startY, u16 endX, u16 endY, u16 color);        //��䵥ɫ
void LCDColorFill(u16 startX, u16 startY, u16 endX, u16 endY, u16* pColor); //���ָ����ɫ
void LCDShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode);                   //��ʾһ���ַ�
void LCDShowNum(u16 x, u16 y, u32 num, u8 len, u8 size);                    //��ʾһ������
void LCDShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode);          //��ʾ����
void LCDShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8* p);    //��ʾһ���ַ���,12/16����

void LCDSSDSetBackLight(u8 pwm);                                            //SSD1963 �������
void LCDSetScanDir(u8 dir);                                                 //������ɨ�跽��
void LCDSetDispDir(u8 dir);                                                 //������Ļ��ʾ����
void LCDSetWindow(u16 sx, u16 sy, u16 width, u16 height);                   //���ô���

#endif  
