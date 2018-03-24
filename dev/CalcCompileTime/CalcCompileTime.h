/*********************************************************************************************************
* 模块名称: CalcCompileTime.h
* 摘    要: CalcCompileTime驱动
* 当前版本: 1.0.0
* 作    者: 666immortal
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
#ifndef _CALC_COMPILE_TIME_H_
#define _CALC_COMPILE_TIME_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void  InitCalcCompileTime(void);            //初始化并计算编译时间
u16*  GetCompileTime(void);                 //获取编译时间
void  PrintCompileTime(void);               //打印编译时间

#endif
