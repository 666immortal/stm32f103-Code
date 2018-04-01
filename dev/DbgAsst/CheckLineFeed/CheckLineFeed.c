/*********************************************************************************************************
* 模块名称: CheckLineFeed.c
* 摘    要: 
* 当前版本: 1.0.0
* 作    者: 
* 完成日期: 2018年03月01日
* 内    容:
* 注    意: none                                                                  
**********************************************************************************************************
* 取代版本: 
* 作    者:
* 完成日期: 
* 修改内容:
* 修改文件: 
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "UART.h"
#include "CheckLineFeed.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static  u8  s_arrRecData[MAX_REC_DATA_CNT]; //接收数据缓冲区
static  i16 s_iDataCnt;                     //接收数据计数
static  u8  s_iRecLineFeedFlag = 0;         //接收到换行符标志位，0-未接收到换行符，1-接收到换行符 
  
/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: InitCheckLineFeed
* 函数功能: 初始化CheckLineFeed模块
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年03月01日
* 注    意:
*********************************************************************************************************/
void InitCheckLineFeed(void)
{
  i16 i;                                //循环计数器
  
  for(i = 0; i < MAX_REC_DATA_CNT; i++)
  {
    s_arrRecData[i] = 0;                //将接收数据缓冲区清零
  }       
          
  s_iDataCnt = 0;                       //接收数据计数清零
  s_iRecLineFeedFlag = 0;               //清除接收到换行符标志
}

/*********************************************************************************************************
* 函数名称: CheckLineFeed
* 函数功能: 检查是否接收到换行符
* 输入参数: data，接收到的数据
* 输出参数: void
* 返 回 值: 接收到换行符的标志，1-接收到换行符标志，0-未接收到换行符标志
* 创建日期: 2018年03月01日
* 注    意: 0x0D即“\r”，0x0A即“\n” 
*********************************************************************************************************/
u8  CheckLineFeed(u8 data)
{  
  static u8 s_iRec0x0DFlag = 0;           //接收到0x0D标志，1-接收到了0x0D，0-未接收到0x0D 
        
  if(0 == s_iRecLineFeedFlag)             //未接收完成，即未接收到“0x0D 0x0A”组合
  {       
    if(1 == s_iRec0x0DFlag)               //接收到了0x0D
    {       
      if(data != 0x0A)                    //继续检查是否接收到了0x0A
      {       
        s_iRecLineFeedFlag = 0;           //未接收到0x0A，表示接收错误，需要重新开始
      }       
      else        
      {       
        s_iRecLineFeedFlag = 1;           //接收到了0x0A，表示收到换行符，将接收完成标志置1
        s_iRec0x0DFlag = 0;               //清除接收到0x0D标志，重新开始新一轮接收        
      }       
    }       
    else                                  //没接收到“0x0D”
    {       
      if(data == 0x0D)                   
      {       
        s_iRec0x0DFlag = 1;               //接收到0x0D，将接收到0x0D标志置1
      }       
      else                                //接收到数据
      {       
        s_arrRecData[s_iDataCnt] = data;  //将数据存入接收数据缓冲区
        s_iDataCnt++;                     //缓冲区指针地址递增
        
        if(s_iDataCnt >= MAX_REC_DATA_CNT)//如果缓冲区的数据量大于最大范围
        {
          s_iRec0x0DFlag = 0;             //清除接收到0x0D标志，重新开始新一轮接收 
        }
      }          
    }        
  }  
  
  return s_iRecLineFeedFlag;              //返回接收到换行符标志
}

/*********************************************************************************************************
* 函数名称: GetRecData
* 函数功能: 获取接收到的数据
* 输入参数: void
* 输出参数: pRecData，接收到数据的指针
* 返 回 值: 接收到数据的长度
* 创建日期: 2018年03月01日
* 注    意: 获取完数据后，此函数会自动将接收到换行符标志清除
*********************************************************************************************************/
i16 GetRecData(u8* pRecData)
{
  i16 i;                                //循环计数器
  i16 retCnt;                           //用于存放接收数据的长度
  
  retCnt = s_iDataCnt;                  //接收数据的长度等于接收数据缓冲区的长度
  
  for(i = 0; i < retCnt; i++)
  {
    pRecData[i] = s_arrRecData[i];      //读取接收数据缓冲区的数据并存入pRecData中
  }
  
  s_iDataCnt = 0;                       //接收数据计数清零
  s_iRecLineFeedFlag = 0;               //清除接收到换行符标志位
  
  return retCnt;                        //返回接收到数据的长度
}
