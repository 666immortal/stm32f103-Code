/*********************************************************************************************************
* ģ������: UART.c
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
#include "UART.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ṹ��ö�ٶ���
*********************************************************************************************************/
//����ģʽ 
typedef enum
{
  UART_STATE_OFF,         //USART.TX״̬Ϊ�ر�
	UART_STATE_ON,          //USART.TX״̬Ϊ��
	UART_STATE_MAX          
}EnumUARTState;           

//���ڻ���������
typedef struct
{
  u8  dataBuf[UART_BUF_SIZE];   //���ݻ�����
  u8  readPtr;                  //��ָ��
  u8  writePtr;                 //дָ��
  u8  dataLength;               //���ݳ���
}StructUartBuf;                 

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static  StructUartBuf s_structUARTSendBuf;  //���ʹ���buf
static  StructUartBuf s_structUARTRecBuf;   //���մ���buf
static  u8 s_iUARTTxSts;                    //USART.TX״̬��־

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static  void  InitUARTBuf(void);            //��ʼ�����ڻ��������������ͻ������ͽ��ջ����� 
static  u8    WriteReceiveBuf(u8 d);        //�����յ�������д�뵽���ջ�������
static  u8    ReadSendBuf(u8* p);           //��ȡ���ͻ������е�����

static  void  ConfigUART(void);             //����UART�Ĳ���������GPIO��RCC��USART���������NVIC 
static  void  EnableUARTTx(void);           //ʹ�ܴ��ڷ��ͣ���WriteUart�е��ã���ÿ�η�������֮����Ҫ����
                                            //��TXE���жϿ���

static  void  SendCharUsedByFputc(u16 ch);  //�����ַ�������ר��fputc��������
  
/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitUARTBuf
* ��������: ��ʼ�����ڻ��������������ͻ������ͽ��ջ�����  
* �������: void
* �������: void
* �� �� ֵ: void 
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  void  InitUARTBuf(void)
{
  s_structUARTSendBuf.dataLength = 0;  //��ʼ�����ڷ��ͻ��������ݳ���
  s_structUARTSendBuf.readPtr    = 0;  //��ʼ����ָ��
  s_structUARTSendBuf.writePtr   = 0;  //��ʼ��дָ��
  
  s_structUARTRecBuf.dataLength  = 0;  //��ʼ�����ڽ��ջ��������ݳ���
  s_structUARTRecBuf.readPtr     = 0;  //��ʼ����ָ��
  s_structUARTRecBuf.writePtr    = 0;  //��ʼ��дָ��
}

/*********************************************************************************************************
* ��������: WriteReceiveBuf
* ��������: д���ݵ����ڽ���BUF�����жϷ������У������յ�����ʱ�������յ�������д�뵽���ڽ���BUF�� 
* �������: d�������жϷ��������յ�������
* �������: void
* �� �� ֵ: д�����ݳɹ���־��0-���ɹ���1-�ɹ� 
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  u8  WriteReceiveBuf(u8 d)
{
  u8 ok = 0;                                                      //д�����ݳɹ���־��0-���ɹ���1-�ɹ�

  if(s_structUARTRecBuf.dataLength < UART_BUF_SIZE)               //ȷ�����ջ������пռ��д
  {
    ok = 1;                                                       //����־��Ϊ1����ʾд�����ݳɹ�
    s_structUARTRecBuf.dataBuf[s_structUARTRecBuf.writePtr] = d;  //д���ݵ����ջ�����
    s_structUARTRecBuf.writePtr++;                                //дָ���1        

    if(s_structUARTRecBuf.writePtr >= UART_BUF_SIZE)              //���дָ����ڻ���ڽ��ջ������ĳ���
    {
      s_structUARTRecBuf.writePtr = 0;                            //дָ������
    }
    
    s_structUARTRecBuf.dataLength++;                              //д��һ������֮�󣬽��ջ��������ݳ��ȼ�1
  }
  
  return ok;                                                      //����д�����ݳɹ���־��0-���ɹ���1-�ɹ� 
}

/*********************************************************************************************************
* ��������: ReadSendBuf
* ��������: ��ȡ����BUF���û�ͨ��WriteUart��Ҫд������ݴ������ͻ��壬�жϺ���ͨ�����ջ������ 
* �������: void
* �������: p�������������ݴ�ŵ��׵�ַ
* �� �� ֵ: ��ȡ���ݳɹ���־��0-���ɹ���1-�ɹ� 
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  u8  ReadSendBuf(u8* p)
{
  u8 ok = 0;                                                      //��ȡ���ݳɹ���־��0-���ɹ���1-�ɹ�

  if(s_structUARTSendBuf.dataLength > 0)                          //ȷ�����ͻ����������ݿɶ�
  {
    ok = 1;                                                       //����־��Ϊ1����ʾ��ȡ���ݳɹ�
    *p = s_structUARTSendBuf.dataBuf[s_structUARTSendBuf.readPtr];//��ȡ�����ͻ������е�Ԫ��
    s_structUARTSendBuf.readPtr++;                                //��ָ���1

    if(s_structUARTSendBuf.readPtr >= UART_BUF_SIZE)              //�����ָ����ڻ���ڷ��ͻ������ĳ���
    {
      s_structUARTSendBuf.readPtr = 0;                            //��ָ������
    }
    
    s_structUARTSendBuf.dataLength--;                             //��ȡ��һ������֮�󣬷��ͻ��������ݳ��ȼ�1
  }
  
  return ok;                                                      //���ض�ȡ���ݳɹ���־��0-���ɹ���1-�ɹ� 
}

/*********************************************************************************************************
* ��������: ConfigUART
* ��������: ����UART�Ĳ���������GPIO��RCC��������NVIC  
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  void  ConfigUART(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;      //����ṹ��GPIO_InitStructure����������USART��GPIO
  USART_InitTypeDef USART_InitStructure;     //����ṹ��USART_InitStructure����������USART�ĳ������
  NVIC_InitTypeDef  NVIC_InitStructure;      //����ṹ��NVIC_InitStructure����������USART��NVIC

  RCC_APB2PeriphClockCmd(USER_DEFINE_USART_TX_GPIO_CLK, ENABLE);     //����USART��GPIOʱ��
  if(USER_DEFINE_USART_TX_GPIO_CLK != USER_DEFINE_USART_RX_GPIO_CLK) //���USART.TX��USART.RX��GPIO�˿ڲ�ͬ
  { 
    RCC_APB2PeriphClockCmd(USER_DEFINE_USART_RX_GPIO_CLK, ENABLE);   //����USART.RX��GPIOʱ��
  }
  
  if(USER_DEFINE_USART == USART1)                                    //�������ΪUSART1
  { 
    RCC_APB2PeriphClockCmd(USER_DEFINE_USART_CLK, ENABLE);           //����USART1��ʱ��
  }
  else
  {
    RCC_APB1PeriphClockCmd(USER_DEFINE_USART_CLK, ENABLE);           //����USART2/3��ʱ��
  }
  
  //����USART.Tx��GPIO
  GPIO_InitStructure.GPIO_Pin   = USER_DEFINE_USART_TX_GPIO_PIN;     //����USART.TX������
  GPIO_InitStructure.GPIO_Mode  = USER_DEFINE_USART_TX_MODE;         //����USART.TX��ģʽ
  GPIO_InitStructure.GPIO_Speed = USER_DEFINE_USART_TX_SPEED;        //����USART.TX��I/O������
  GPIO_Init(USER_DEFINE_USART_TX_GPIO_PORT, &GPIO_InitStructure);    //���ݲ�����ʼ��USART.TX��GPIO�˿�
  
  //����USART.Rx��GPIO
  GPIO_InitStructure.GPIO_Pin   = USER_DEFINE_USART_RX_GPIO_PIN;     //����USART.RX������
  GPIO_InitStructure.GPIO_Mode  = USER_DEFINE_USART_RX_MODE;         //����USART.RX��ģʽ
  GPIO_InitStructure.GPIO_Speed = USER_DEFINE_USART_RX_SPEED;        //����USART.RX��I/O������
  GPIO_Init(USER_DEFINE_USART_RX_GPIO_PORT, &GPIO_InitStructure);    //���ݲ�����ʼ��USART.RX��GPIO�˿�
  
  //����USART�ĳ������
  USART_StructInit(&USART_InitStructure);                                //��ʼ�����ڵĸ�������
  USART_InitStructure.USART_BaudRate   = USER_DEFINE_USART_BAUDRATE;     //����USART�Ĳ�����
  USART_InitStructure.USART_WordLength = USER_DEFINE_USART_WORDLENGTH;   //����USART�Ĵ��ڴ�����ֳ�
  USART_InitStructure.USART_StopBits   = USER_DEFINE_USART_STOPBITS;     //����USART��ֹͣλ
  USART_InitStructure.USART_Parity     = USER_DEFINE_USART_PARITY;       //����USART����żУ��λ
  USART_InitStructure.USART_Mode       = USER_DEFINE_USART_MODE;         //����USART��ģʽ
  USART_InitStructure.USART_HardwareFlowControl = USER_DEFINE_USART_HRDWAREFLOWCONTROL;//����USART��Ӳ��������
  USART_Init(USER_DEFINE_USART, &USART_InitStructure);                   //���ݲ�����ʼ��USART

  //����USART��NVIC
  NVIC_InitStructure.NVIC_IRQChannel = USER_DEFINE_USART_IRQ;        //����USART���ж�
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //��ռ���ȼ������μ�Ĭ��ֵ
  //NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;        //�����ȼ������μ�Ĭ��ֵ
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);                                    //���ݲ�����ʼ��USART��NVIC

  //�򿪴���ʹ�ܿ���
  USART_ITConfig(USER_DEFINE_USART, USART_IT_RXNE, ENABLE);          //�����ж�ʹ��
  USART_ITConfig(USER_DEFINE_USART, USART_IT_TXE,  ENABLE);          //�����ж�ʹ��
  USART_Cmd(USER_DEFINE_USART, ENABLE);                              //ʹ��USART
                                                                     
  s_iUARTTxSts = UART_STATE_OFF;                                     //��ʼ��USART.TX״̬��־
}

/*********************************************************************************************************
* ��������: EnableUARTTx
* ��������: ʹ�ܴ��ڷ��ͣ���WriteUart�е��ã���ÿ�η�������֮����Ҫ���������������TXE���жϿ��� 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: s_iUARTTxSts = UART_STATE_ON;��仰�������USART_ITConfig֮ǰ������ᵼ���жϴ��޷�ִ��
*********************************************************************************************************/
static  void  EnableUARTTx(void)
{
  s_iUARTTxSts = UART_STATE_ON;                               //��USART.TX��״̬��־����Ϊ��

  USART_ITConfig(USER_DEFINE_USART, USART_IT_TXE, ENABLE);    //�����ж�ʹ��
}

/*********************************************************************************************************
* ��������: SendCharUsedByFputc
* ��������: �����ַ�������ר��fputc��������  
* �������: ch�������͵��ַ�
* �������: void
* �� �� ֵ: void 
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
static  void  SendCharUsedByFputc(u16 ch)
{
  USART_SendData(USER_DEFINE_USART, (u8)ch);//���ڷ���ch           

  //�����ڷ�����ɣ�����ѭ��
  while(USART_GetFlagStatus(USER_DEFINE_USART, USART_FLAG_TC) == RESET)  
  {
    
  }
}

/*********************************************************************************************************
* ��������: USER_DEFINE_USART_IRQHandler
* ��������: USART�жϷ����������û��Զ���
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void USER_DEFINE_USART_IRQHandler(void)                
{
  u8  uData = 0;

  if(USART_GetITStatus(USER_DEFINE_USART, USART_IT_RXNE) != RESET)   //�����ж�
  {
    NVIC_ClearPendingIRQ(USER_DEFINE_USART_IRQ);                     //��������ж�
    uData = USART_ReceiveData(USER_DEFINE_USART);                    //���մ��ڴ��������ݣ����浽uData��
    //�����յ�����ʱ�������յ�������д�뵽���ջ������� 
    WriteReceiveBuf(uData);
  }
  
  if(USART_GetFlagStatus(USER_DEFINE_USART, USART_FLAG_ORE) == SET) //���USART��������־λΪ��1��
  {
    USART_ClearFlag(USER_DEFINE_USART, USART_FLAG_ORE);             //���USART����������־
    USART_ReceiveData(USER_DEFINE_USART);                           //��USART�������� 
  }
  
  if(USART_GetITStatus(USER_DEFINE_USART, USART_IT_TXE) != RESET)   //�����ж�
  {
    USART_ClearITPendingBit(USER_DEFINE_USART, USART_IT_TXE);       //���USART�ķ����жϱ�־λ
    NVIC_ClearPendingIRQ(USER_DEFINE_USART_IRQ);                    //��������ж�
    
    ReadSendBuf(&uData);                                            //��ȡ���ͻ����������ݵ�uData
    
    USART_SendData(USER_DEFINE_USART, uData);                       //���ڷ�������uData
    
    if(0 == s_structUARTSendBuf.dataLength)                         //�����ͻ��������ݳ���Ϊ0
    {
      s_iUARTTxSts = UART_STATE_OFF;                                //USART.TX״̬��־����Ϊ�ر�                      
      USART_ITConfig(USER_DEFINE_USART, USART_IT_TXE, DISABLE);     //�رմ��ڷ����ж�
    }
  }
}  

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitUART
* ��������: ��ʼ��USART 
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
void InitUART(void)
{
  InitUARTBuf();  //��ʼ�����ڻ��������������ͻ������ͽ��ջ�����  

  ConfigUART();   //����UART�Ĳ���������GPIO��RCC�����������NVIC
}

/*********************************************************************************************************
* ��������: WriteUART
* ��������: д���ݵ����ͻ�����
* �������: pBuf��Ҫд�����ݵ��׵�ַ��len����д�����ݵĳ���
* �������: void
* �� �� ֵ: ʵ��д�����ݵĳ��ȣ���һ�����������len���
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
u8  WriteUART(u8* pBuf, u8 len)
{
  u8 i;                                                               //iΪѭ��������
  u8 wLen = 0;                                                        //ʵ��д�����ݵĳ���
                                                                      
  if(s_structUARTSendBuf.dataLength < UART_BUF_SIZE)                  //�����ͻ�����������С�ڻ�������С
  {                                                                                                                                      
    wLen = len;                                                       //ʵ��д�����ݳ��ȵ�����д�����ݳ���
                                                                      
    //�����ǰ���ͻ��������ݳ��ȼ�����д�����ݳ��ȴ��ڷ��ͻ�������С  
    if((s_structUARTSendBuf.dataLength + len) > UART_BUF_SIZE)         
    {                                                                 
      wLen = UART_BUF_SIZE - s_structUARTSendBuf.dataLength;          //ʵ��д�����ݳ��ȵ��ڷ��ͻ�����ʣ���С
    }                                                                 
                                                                      
    for(i = 0; i < wLen; i++)                                         //ѭ��ֱ������ȫ��д�뷢�ͻ�����
    {
      s_structUARTSendBuf.dataBuf[s_structUARTSendBuf.writePtr] = pBuf[i];//���ͻ�����д�����ݵĵ�iλ
      s_structUARTSendBuf.writePtr++;                                     //дָ���1
    
      if(s_structUARTSendBuf.writePtr >= UART_BUF_SIZE)                   //���дָ�볬����������С
      {  
        s_structUARTSendBuf.writePtr = 0;                                 //����дָ��Ϊ0
      }

      s_structUARTSendBuf.dataLength++;   //���ͻ��������ݳ��ȼ�1
    }

    if(s_iUARTTxSts == UART_STATE_OFF)    //�������TX��״̬Ϊ�ر�
    {
      EnableUARTTx();                     //ʹ�ܴ��ڷ��ͣ���TXE���жϿ���
    }
  }
  
  return wLen;                            //����ʵ��д�����ݵĳ���
}

/*********************************************************************************************************
* ��������: ReadUART
* ��������: ��ȡ��Ӧ���ջ������е�����  
* �������: len������ȡ���ݵĳ���
* �������: pBuf����ȡ�����ݴ�ŵ��׵�ַ
* �� �� ֵ: ʵ�ʶ�ȡ���ݵĳ���
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
u8  ReadUART(u8* pBuf, u8 len)
{
  u8 i;                                                 //iΪѭ��������
  u8 rLen = 0;                                          //ʵ�ʶ�ȡ���ݳ���
  
  if(s_structUARTRecBuf.dataLength > 0)                 //���ջ�������������
  {
    rLen = len;                                         //ʵ�ʶ�ȡ���ݳ��ȵ�������ȡ���ݳ���

    if(s_structUARTRecBuf.dataLength < len)             //����ȡ���ݵĳ��ȳ������ջ����������ݳ���
    {     
      rLen = s_structUARTRecBuf. dataLength;            //ʵ�ʶ�ȡ���ݳ��ȵ��ڽ��ջ����������ݳ���
    }

    for(i = 0; i < rLen; i++)                           //ѭ����ȡ���ջ������е�����
    {
      pBuf[i] = s_structUARTRecBuf.dataBuf[s_structUARTRecBuf.readPtr];//��ȡ���ջ����������ݲ��浽pBuf��iλ
      s_structUARTRecBuf.readPtr++;                                    //��ָ���1

      if(s_structUARTRecBuf.readPtr >= UART_BUF_SIZE)                  //�����ָ�볬�����ջ������Ĵ�С
      {  
        s_structUARTRecBuf.readPtr = 0;                                //���ö�ָ��Ϊ0
      }
      s_structUARTRecBuf.dataLength--;                                 //���ջ��������ݳ��ȼ�1
    }
  }

  return rLen;                                          //����ʵ�ʶ�ȡ���ݵĳ���
}

/*********************************************************************************************************
* ��������: fputc
* ��������: �ض�����  
* �������: ch��f
* �������: void
* �� �� ֵ: int 
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
int fputc(int ch, FILE* f)
{
  SendCharUsedByFputc((u8) ch);   //�����ַ�������ר��fputc��������
  
  return ch;                      //����ch
}

/*********************************************************************************************************
* ��������: fgetc
* ��������: �ض�����  
* �������: f
* �������: void
* �� �� ֵ: ���ڽ��յ������� 
* ��������: 2018��03��01��
* ע    ��: 
*********************************************************************************************************/
int fgetc(FILE* f)
{
  //�����ڽ��ղ�Ϊ��ʱ������ѭ��
  while(USART_GetFlagStatus(USER_DEFINE_USART, USART_FLAG_RXNE == RESET))
  {
    
  }
  
  return(USART_ReceiveData(USER_DEFINE_USART)); //���ش��ڽ��յ�������
}
