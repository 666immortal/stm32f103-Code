/*********************************************************************************************************
* ģ������: OLED.c   
* ժ    Ҫ: 4�ߴ��нӿڣ�CS��DC��SCLK��SDIN��RES
* ��ǰ�汾: 1.0.0
* ��    ��: 
* �������: 2018��03��01��
* ��    ��:
* ע    ��: OLED���Դ�
            ��Ÿ�ʽ����.
            [0]0 1 2 3 ... 127
            [1]0 1 2 3 ... 127
            [2]0 1 2 3 ... 127
            [3]0 1 2 3 ... 127
            [4]0 1 2 3 ... 127
            [5]0 1 2 3 ... 127
            [6]0 1 2 3 ... 127
            [7]0 1 2 3 ... 127                                                                   
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
#include  <stm32f10x_conf.h>
#include  "OLED.h"
#include  "stdlib.h"
#include  "SysTick.h"
#include  "OLEDFont.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/  		   
#define OLED_CMD    0    //д����
#define OLED_DATA   1    //д����

//Ƭѡ��CS����ߵ�ƽ
#define SET_OLED_CS()   GPIO_SetBits(USER_DEFINE_OLED_CS_GPIO_PORT, USER_DEFINE_OLED_CS_GPIO_PIN) 
//Ƭѡ��CS����͵�ƽ
#define CLR_OLED_CS()   GPIO_ResetBits(USER_DEFINE_OLED_CS_GPIO_PORT, USER_DEFINE_OLED_CS_GPIO_PIN)  

//Ӳ��λ��RES����ߵ�ƽ
#define SET_OLED_RES()  GPIO_SetBits(USER_DEFINE_OLED_RES_GPIO_PORT, USER_DEFINE_OLED_RES_GPIO_PIN)  
//Ӳ��λ��RES����͵�ƽ
#define CLR_OLED_RES()  GPIO_ResetBits(USER_DEFINE_OLED_RES_GPIO_PORT, USER_DEFINE_OLED_RES_GPIO_PIN)

//DC����ߵ�ƽ������д����  
#define SET_OLED_DC()   GPIO_SetBits(USER_DEFINE_OLED_DC_GPIO_PORT, USER_DEFINE_OLED_DC_GPIO_PIN)
//DC����͵�ƽ������д����  
#define CLR_OLED_DC()   GPIO_ResetBits(USER_DEFINE_OLED_DC_GPIO_PORT, USER_DEFINE_OLED_DC_GPIO_PIN)
   
//ʱ����SCLK������ߵ�ƽ
#define SET_OLED_SCLK() GPIO_SetBits(USER_DEFINE_OLED_SCLK_GPIO_PORT, USER_DEFINE_OLED_SCLK_GPIO_PIN)  
//ʱ��������SCLK������͵�ƽ
#define CLR_OLED_SCLK() GPIO_ResetBits(USER_DEFINE_OLED_SCLK_GPIO_PORT, USER_DEFINE_OLED_SCLK_GPIO_PIN)

//������SDIN������ߵ�ƽ
#define SET_OLED_SDIN() GPIO_SetBits(USER_DEFINE_OLED_SDIN_GPIO_PORT, USER_DEFINE_OLED_SDIN_GPIO_PIN)
//������SDIN������͵�ƽ
#define CLR_OLED_SDIN() GPIO_ResetBits(USER_DEFINE_OLED_SDIN_GPIO_PORT, USER_DEFINE_OLED_SDIN_GPIO_PIN)
  
/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/	 

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static  u8  s_iOLEDGRAM[128][8];                //OLED�Դ���»�����
 
/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/ 
static  void  ConfigOLEDGPIO(void);             //����OLED�Ķ˿�
static  void  ConfigOLEDReg(void);              //����OLED�ļĴ���

static  void  OLEDWriteByte(u8 dat, u8 cmd);    //��OLEDд��һ���ֽ�
static  void  OLEDDrawPoint(u8 x, u8 y, u8 t);  //��OLED��ָ��λ�û���

static  u32   CalcPow(u8 m, u8 n);              //����ָ��λ�õ���

//static  void  OLEDFill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot);       //���ָ������

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: ConfigOLEDGPIO 
* ��������: ����OLED��GPIO 
* �������: void 
* �������: void
* �� �� ֵ: void 
* ��������: 2018-03-01
* ע    ��: CEPARK��OLED��˵����VCC��3.3V����CS��RES��DC��SCK��DIN��GND������RES��Ҫ��Ӳ��RESET������       
*********************************************************************************************************/
static  void  ConfigOLEDGPIO(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;//����ṹ��GPIO_InitStructure����������OLED��GPIO
  
  RCC_APB2PeriphClockCmd(USER_DEFINE_OLED_CS_GPIO_CLK, ENABLE);        //ʹ��OLED.CS��ʱ��
  RCC_APB2PeriphClockCmd(USER_DEFINE_OLED_RES_GPIO_CLK, ENABLE);       //ʹ��OLED.RES��ʱ��
  RCC_APB2PeriphClockCmd(USER_DEFINE_OLED_DC_GPIO_CLK, ENABLE);        //ʹ��OLED.DC��ʱ��
  RCC_APB2PeriphClockCmd(USER_DEFINE_OLED_SCLK_GPIO_CLK, ENABLE);      //ʹ��OLED.SCLK��ʱ��
  RCC_APB2PeriphClockCmd(USER_DEFINE_OLED_SDIN_GPIO_CLK, ENABLE);      //ʹ��OLED.SDIN��ʱ��
 
  //����CS����
  GPIO_InitStructure.GPIO_Pin   = USER_DEFINE_OLED_CS_GPIO_PIN;        //����OLED.CS������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;                    //�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                    //����I/O������50MHz
  GPIO_Init(USER_DEFINE_OLED_CS_GPIO_PORT, &GPIO_InitStructure);       //���ݲ�����ʼ��OLED.CS��GPIO�˿�
  GPIO_SetBits(USER_DEFINE_OLED_CS_GPIO_PORT, USER_DEFINE_OLED_CS_GPIO_PIN);//OLED.CS��ʼ��Ϊ�ߵ�ƽ
  
  //����RSE����
  GPIO_InitStructure.GPIO_Pin   = USER_DEFINE_OLED_RES_GPIO_PIN;       //����OLED.RES������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;                    //�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                    //����I/O������50MHz
  GPIO_Init(USER_DEFINE_OLED_RES_GPIO_PORT, &GPIO_InitStructure);      //���ݲ�����ʼ��OLED.RES��GPIO�˿�
  GPIO_SetBits(USER_DEFINE_OLED_RES_GPIO_PORT, USER_DEFINE_OLED_RES_GPIO_PIN);//OLED.RES��ʼ��Ϊ�ߵ�ƽ
  
  //����DC����
  GPIO_InitStructure.GPIO_Pin   = USER_DEFINE_OLED_DC_GPIO_PIN;        //����OLED.DC������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;                    //�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                    //����I/O������50MHz
  GPIO_Init(USER_DEFINE_OLED_DC_GPIO_PORT, &GPIO_InitStructure);       //���ݲ�����ʼ��OLED.DC��GPIO�˿�
  GPIO_SetBits(USER_DEFINE_OLED_DC_GPIO_PORT, USER_DEFINE_OLED_DC_GPIO_PIN);//OLED.DC��ʼ��Ϊ�ߵ�ƽ

  //����SCLK����                                                                 
  GPIO_InitStructure.GPIO_Pin   = USER_DEFINE_OLED_SCLK_GPIO_PIN;      //����OLED.SCLK������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;                    //�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                    //����I/O������50MHz
  GPIO_Init(USER_DEFINE_OLED_SCLK_GPIO_PORT, &GPIO_InitStructure);     //���ݲ�����ʼ��OLED.SCLK��GPIO�˿�
  GPIO_SetBits(USER_DEFINE_OLED_SCLK_GPIO_PORT, USER_DEFINE_OLED_SCLK_GPIO_PIN);//OLED.SCLK��ʼ��Ϊ�ߵ�ƽ
  
  //����SDIN����
  GPIO_InitStructure.GPIO_Pin   = USER_DEFINE_OLED_SDIN_GPIO_PIN;      //����OLED.SDIN������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;                    //�������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                    //����I/O������50MHz
  GPIO_Init(USER_DEFINE_OLED_SDIN_GPIO_PORT, &GPIO_InitStructure);     //���ݲ�����ʼ��OLED.SDIN��GPIO�˿�
  GPIO_SetBits(USER_DEFINE_OLED_SDIN_GPIO_PORT, USER_DEFINE_OLED_SDIN_GPIO_PIN);//OLED.SDIN��ʼ��Ϊ�ߵ�ƽ
}

/*********************************************************************************************************
* ��������: ConfigOLEDReg 
* ��������: ����OLED�ļĴ��� 
* �������: void 
* �������: void
* �� �� ֵ: void 
* ��������: 2018-03-01
* ע    ��: 
*********************************************************************************************************/
static  void  ConfigOLEDReg(void)
{
  OLEDWriteByte(0xAE, OLED_CMD); //�ر���ʾ
  OLEDWriteByte(0xD5, OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
  OLEDWriteByte(80,   OLED_CMD); //[3:0],��Ƶ����;[7:4],��Ƶ��
  OLEDWriteByte(0xA8, OLED_CMD); //��������·��
  OLEDWriteByte(0X3F, OLED_CMD); //Ĭ��0X3F(1/64) 
  OLEDWriteByte(0xD3, OLED_CMD); //������ʾƫ��
  OLEDWriteByte(0X00, OLED_CMD); //Ĭ��Ϊ0

  OLEDWriteByte(0x40, OLED_CMD); //������ʾ��ʼ��[5:0],����.
  
  OLEDWriteByte(0x8D, OLED_CMD); //��ɱ�����
  OLEDWriteByte(0x14, OLED_CMD); //bit2������/�ر�
  OLEDWriteByte(0x20, OLED_CMD); //�����ڴ��ַģʽ
  OLEDWriteByte(0x02, OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
  OLEDWriteByte(0xA1, OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
  OLEDWriteByte(0xC0, OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
  OLEDWriteByte(0xDA, OLED_CMD); //����COMӲ����������
  OLEDWriteByte(0x12, OLED_CMD); //[5:4]����
  
  OLEDWriteByte(0x81, OLED_CMD); //�Աȶ�����
  OLEDWriteByte(0xEF, OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
  OLEDWriteByte(0xD9, OLED_CMD); //����Ԥ�������
  OLEDWriteByte(0xf1, OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
  OLEDWriteByte(0xDB, OLED_CMD); //����VCOMH ��ѹ����
  OLEDWriteByte(0x30, OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

  OLEDWriteByte(0xA4, OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
  OLEDWriteByte(0xA6, OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
  OLEDWriteByte(0xAF, OLED_CMD); //������ʾ	
}

/*********************************************************************************************************
* ��������: OLEDWriteByte
* ��������: ��OLEDд��һ���ֽ�
* �������: datΪҪд������ݻ�����;cmd��������/�����־��0-���1-����
* �������: void
* �� �� ֵ: void 
* ��������: 2018-03-01
* ע    ��: 
*********************************************************************************************************/
static  void  OLEDWriteByte(u8 dat, u8 cmd)
{
  u8 i;                       //ѭ��������
     
  //�ж�Ҫд�����ݻ���д������
  if(OLED_CMD == cmd)         //��־cmdΪ��������ʱ
  {                          
    CLR_OLED_DC();            //DC����͵�ƽ������д����  
  }                          
  else if(OLED_DATA == cmd)   //��־cmdΪ��������ʱ
  {
    SET_OLED_DC();            //DC����ߵ�ƽ������д����  
  }                          
                             
  CLR_OLED_CS();              //CS����͵�ƽΪд�����ݻ�������׼��
                             
  for(i = 0; i < 8; i++)      //ѭ���˴Σ��Ӹߵ���ȡ��Ҫд������ݻ������8��λ
  {                          
    CLR_OLED_SCLK();          //SCLK����͵�ƽΪд��������׼��
                             
    if(dat & 0x80)            //�ж�Ҫд������ݻ���������λ�ǡ�1�����ǡ�0��
    {                        
      SET_OLED_SDIN();        //Ҫд������ݻ���������λ�ǡ�1����SDIN����ߵ�ƽ��ʾ��1��
    }                        
    else                     
    {                        
      CLR_OLED_SDIN();        //Ҫд������ݻ���������λ�ǡ�0����SDIN����͵�ƽ��ʾ��0��
    }                        
    SET_OLED_SCLK();          //SCLK����ߵ�ƽ��SDIN��״̬���ٱ仯����ʱд�������ߵ�����
                             
    dat <<= 1;                //����һλ���θ�λ�Ƶ����λ
  }                          
                             
  SET_OLED_CS();              //OLED��CS����ߵ�ƽ������д�����ݻ�����  
  SET_OLED_DC();              //OLED��DC����ߵ�ƽ
} 

/*********************************************************************************************************
* ��������: OLEDDrawPoint
* ��������: ��ָ��λ�û���
* �������: x:0~127; y:0~63; fill:1-��䣬0-���
* �������: void 
* �� �� ֵ: void
* ��������: 2018-03-01
* ע    ��: ��1��x-y����ϵ��ԭ����������Ļ���Ͻǣ�
*           ��2��������ʾ��y��SSD1306�Դ��COMn��n���Ϊ63����Ȼ����Ļ���кţ����ֽڣ���SSD1306���кţ���
*                �ֽڣ����Ϊ7��
*********************************************************************************************************/			   
static  void  OLEDDrawPoint(u8 x, u8 y, u8 fill)
{
  u8 pos;                           //��ŵ����ڵ�ҳ��
  u8 bx;                            //��ŵ����ڵ���Ļ���к�
  u8 temp = 0;                      //������Ż���λ��������ֽڵ�λ
  
  if(x > 127 || y > 63)             //���ָ��λ�ó������Χ
  {
    return;                         //���ؿգ���������
  }
  
  pos  = 7 - y / 8;                 //��ָ��λ������ҳ��
  bx   = y % 8;                     //��ָ��λ�����������ҳ���е��к�
  temp = 1 << (7 - bx);             //��7 - bx�������ӦSSD1306���кţ������ֽ�����Ӧ��λ��Ϊ��1��

  if(fill)                          //�ж�����־Ϊ��1�����ǡ�0��
  {
    s_iOLEDGRAM[x][pos] |= temp;    //�������־Ϊ��1����ָ�������
  }
  else
  {
    s_iOLEDGRAM[x][pos] &= ~temp;	  //�������־Ϊ��0����ָ�������
  }
}

/*********************************************************************************************************
* ��������: OLEDFill
* ��������: ���ָ������
* �������: x1,y1,x2,y2 �������ĶԽ����ꣻdot:0,���;1,���	
* �������: void
* �� �� ֵ: void
* ��������: 2018-03-01
* ע    ��: ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
*********************************************************************************************************/
//static  void  OLEDFill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot)  
//{  
//  u8 x;                               //��ǰ������
//  u8 y;                               //��ǰ������
//  
//  for(x = x1; x <= x2; x++)           //��������x�᷶Χ
//  {
//    for(y = y1; y <= y2; y++)         //��������y�᷶Χ
//      { 
//        OLEDDrawPoint(x, y, dot);     //ָ��λ�����
//      }        
//  }
//  
//  OLEDRefreshGRAM();                  //������ʾ
//}

/*********************************************************************************************************
* ��������: CalcPow
* ��������: m^n��������m��n����
* �������: m��n 
* �������: void 
* �� �� ֵ: void 
* ��������: 2018-03-01
* ע    ��: 
*********************************************************************************************************/
static  u32 CalcPow(u8 m, u8 n)
{
  u32 result = 1;       //����������Ž���ı���             
   
  while(n--)            //����ÿ��ѭ����n�ݼ���ֱ��Ϊ0
  {
    result *= m;        //ѭ��n�Σ��൱��n��m���
  }
  
  return result;        //����m��n���ݵ�ֵ
}
 
/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/	
/*********************************************************************************************************
* ��������: InitOLED
* ��������: ��ʼ��OLED
* �������: void 
* �������: void
* �� �� ֵ: void 
* ��������: 2018-03-01
* ע    ��:
*********************************************************************************************************/   
void  InitOLED(void)
{ 
  ConfigOLEDGPIO();     //����OLED��GPIO
  SET_OLED_RES();       //RES����ߵ�ƽ
  ConfigOLEDReg();      //����OLED�ļĴ���
  
  OLEDClear();          //���OLED������
}  

/*********************************************************************************************************
* ��������: OLEDDisplayOn
* ��������: ����OLED��ʾ
* �������: void 
* �������: void
* �� �� ֵ: void
* ��������: 2018-03-01
* ע    ��: 
*********************************************************************************************************/   
void  OLEDDisplayOn(void)
{
  OLEDWriteByte(0X8D, OLED_CMD);  //��ɱ�����
  OLEDWriteByte(0X14, OLED_CMD);  //bit2����
  OLEDWriteByte(0XAF, OLED_CMD);  //������ʾ
}

/*********************************************************************************************************
* ��������: OLEDDisplayOff
* ��������: �ر�OLED��ʾ
* �������: void 
* �������: void
* �� �� ֵ: void
* ��������: 2018-03-01
* ע    ��: 
*********************************************************************************************************/ 
void  OLEDDisplayOff(void)
{
  OLEDWriteByte(0X8D, OLED_CMD);  //��ɱ�����
  OLEDWriteByte(0X10, OLED_CMD);  //��OLED�Ĵ���д����
  OLEDWriteByte(0XAE, OLED_CMD);  //�ر���ʾ
}

/*********************************************************************************************************
* ��������: OLEDRefreshGRAM
* ��������: ����OLED GRAM 
* �������: void 
* �������: void
* �� �� ֵ: void
* ��������: 2018-03-01
* ע    ��: 
*********************************************************************************************************/
void  OLEDRefreshGRAM(void)
{
  u8  i;                                          //ѭ��������1
  u8  n;                                          //ѭ��������2
            
  for(i = 0; i < 8; i++)                          //����ÿһҳ
  {           
    OLEDWriteByte(0xb0 + i, OLED_CMD);            //����ҳ��ַ��0~7��
    OLEDWriteByte(0x00, OLED_CMD);                //������ʾλ�á��е͵�ַ
    OLEDWriteByte(0x10, OLED_CMD);                //������ʾλ�á��иߵ�ַ   
    for(n = 0; n < 128; n++)                      //����ÿһ��
    {
      //ͨ��ѭ����OLED�Դ���»����������ݸ���OLED�����Դ���
      OLEDWriteByte(s_iOLEDGRAM[n][i], OLED_DATA);
    }    
  }   
}

/*********************************************************************************************************
* ��������: OLEDClear
* ��������: ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018-03-01
* ע    ��: 
*********************************************************************************************************/  
void  OLEDClear(void)  
{  
  u8  i;                            //ѭ��������1
  u8  n;                            //ѭ��������2
  
  for(i = 0; i < 8; i++)            //����ÿһҳ
  {
    for(n = 0; n < 128; n++)        //����ÿһ��
    {
      s_iOLEDGRAM[n][i] = 0X00;   //��ָ��������
    }
  }

  OLEDRefreshGRAM();                //����OLED GRAM
}

/*********************************************************************************************************
* ��������: OLEDShowNum
* ��������: ��OLED����ָ��λ����ʾ����
* �������: x,y :�������;num:�������ֵ;len :���ֵ�λ��;size:�����С;
* �������: void
* �� �� ֵ: void
* ��������: 2018-03-01
* ע    ��: num:��ֵ(0~4294967295)
*********************************************************************************************************/	 		  
void  OLEDShowNum(u8 x, u8 y, u32 num, u8 len, u8 size)
{
  u8  t;                                                     //ѭ��������
  u8  temp;                                                  //�������Ҫ��ʾ���ֵĸ���λ
  u8  enshow = 0;                                            //���֡�0���Ƿ�Ϊ��λ0��־λ
                                                          
  for( t = 0; t < len; t++)                               
  {                                                       
    temp = (num / CalcPow(10, len - t - 1)) % 10;            //���Ӹߵ���ȡ��Ҫ��ʾ���ֵĸ���λ���浽temp��      
    if(enshow == 0 && t < (len - 1))                         //������enshowΪ��0�����һ�δȡ�����һλ
    {                                                     
      if(temp == 0 )                                         //���temp���ڡ�0��
      {
        OLEDShowChar(x + (size / 2) * t, y, ' ', size, 1);   //��ʱ�ġ�0���ڸ�λ���ÿո����
        continue;                                            //��ǰ��������ѭ����������һ��ѭ��
      }
      else
      {
        enshow = 1;                                          //���򽫱��enshow��Ϊ��1��
      }     
    }
    OLEDShowChar(x + (size / 2) * t, y, temp + '0', size, 1);//��ָ��λ����ʾ�õ�������
  }
}

/*********************************************************************************************************
* ��������: OLEDShowChar
* ��������: ��OLED����ָ��λ����ʾ�ַ�
* �������: x:0~127;y:0~63;size:ѡ������ 16/12;mode:0,������ʾ;1,������ʾ
            ptn:ѡ��ȡģ��ʽ��chrΪָ����ʾ���ַ�
* �������: void
* �� �� ֵ: void
* ��������: 2018-03-01
* ע    ��: 
*********************************************************************************************************/
void  OLEDShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{ 
  u8  temp;                           //��������ַ�˳������ʽ�����λ��
  u8  t;                              //ѭ��������1
  u8  t1;                             //ѭ��������2
  u8  y0 = y;                         //��ǰ����������

  chr = chr - ' ';                    //�õ�����ڡ� ��ƫ�Ƶ�ֵ���Ӷ����Ҫ��ӡ�ַ���asc1�е���

  for(t = 0; t < size; t++)           //ѭ��������ʾ
  {   
    if(size == 12)                    //�ж��ֺŴ�С��ѡ����Ե�˳������ʽ
    {
      temp = asc1_1206[chr][t];       //ȡ���ַ���asc1_1206�еĵ�t��
    }
    else
    {
      temp = asc1_1608[chr][t];       //ȡ���ַ���asc1_1608�еĵ�t��
    }      
  
    for(t1 = 0; t1 < 8; t1++)         //#��һ���ַ��ĵ�t�еĺ���Χ��8�����أ�����ʾ��
    {
      if(temp & 0x80)                 //ȡ��temp�����λ�����ж�Ϊ��0�����ǡ�1��
      {
        OLEDDrawPoint(x, y, mode);    //���temp�����λΪ��1�����ָ��λ�õĵ�
      }
      else
      {
        OLEDDrawPoint(x, y, !mode);   //���temp�����λΪ��0�����ָ��λ�õĵ�
      }

      temp <<= 1;                     //����һλ���θ�λ�Ƶ����λ
      y++;                            //������һ��

      if((y - y0) == size)            //�����ʾ��һ��
      {
        y = y0;                       //�кŻص�ԭ����λ��
        x++;                          //������һ��
        break;                        //���������#��ѭ��
      }
    } 
	}          
}

/*********************************************************************************************************
* ��������: OLEDShowString
* ��������: ��OLED����ָ��λ����ʾ�ַ���
* �������: x,y:�������;*p:�ַ�����ʼ��ַ
* �������: void
* �� �� ֵ: void
* ��������: 2018-03-01
* ע    ��: ��16��������ʾ
*********************************************************************************************************/
void  OLEDShowString(u8 x, u8 y, const u8* p)
{
#define MAX_CHAR_POSX 122         //OLED��Ļ��������Χ
#define MAX_CHAR_POSY 58          //OLED��Ļ��������Χ
          
  while(*p != '\0')               //ָ�벻���ڽ�����ʱ��ѭ������
  {       
    if(x > MAX_CHAR_POSX)         //���x����ָ�����Χ��x��ֵΪ0
    {
      x  = 0;                     
      y += 16;                    //��ʾ����һ�����
    }

    if(y > MAX_CHAR_POSY)         //���y����ָ�����Χ��x��y����ֵΪ0
    {
      OLEDClear();                //���OLED��Ļ����
      y = x = 0;                  //��ʾ��OLED��Ļ���Ͻ�
    }

    OLEDShowChar(x, y, *p, 16, 1);//ָ��λ����ʾһ���ַ�
        
    x += 8;                       //һ���ַ�����ռ8�����ص�
    p++;                          //ָ��ָ����һ���ַ�
  }  
}
