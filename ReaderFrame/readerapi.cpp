#include "readerapi.h"
#include "readerframe.h"

/*****************************************************************
 函数名称：JT_OpenReader
 函数描述：1 打开读卡器
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_OpenReader(int nMode, char *sParas)
{
    return ReaderFrame::getInstance()->OpenReader(nMode, sParas);
}

/*****************************************************************
 函数名称：JT_CloseReader
 函数描述：2 关闭读卡器
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_CloseReader(int nHandle)
{
    return ReaderFrame::getInstance()->CloseReader(nHandle);
}

/*****************************************************************
 函数名称：JT_SetInitPPS
 函数描述：3 设置 IC 卡读写器通讯速率（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_SetInitPPS(int nHandle, char* sPPS)
{
    return ReaderFrame::getInstance()->SetInitPPS(nHandle, sPPS);
}

/*****************************************************************
 函数名称：JT_SetInitTimeOut
 函数描述：4 设置 IC 卡读写器超时时间（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_SetInitTimeOut(int nHandle, char* sTimeOut)
{
    return ReaderFrame::getInstance()->SetInitTimeOut(nHandle, sTimeOut);
}

/*****************************************************************
 函数名称：JT_LEDDisplay
 函数描述：5 设置读写器发光二极管
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_LEDDisplay(int nHandle, int nRed, int nGreen, int nBlue)
{
    return ReaderFrame::getInstance()->LEDDisplay(nHandle, nRed, nGreen, nBlue);
}

/*****************************************************************
 函数名称：PJT_AudioControl
 函数描述：6 设置读写器蜂鸣器
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_AudioControl(int nHandle, int nTimes, int nVoice)
{
    return ReaderFrame::getInstance()->AudioControl(nHandle, nTimes, nVoice);
}

/*****************************************************************
 函数名称：JT_GetStatus
 函数描述：7 获取设备状态
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_GetStatus(int nHandle, int* pStatusCode)
{
    return ReaderFrame::getInstance()->GetStatus(nHandle, pStatusCode);
}

/*****************************************************************
 函数名称：JT_ReaderVersion
 函数描述：8 获取设备版本信息
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_ReaderVersion(int nHandle, char* sRDVersion, int nRDVerMaxLen, char*sAPIVersion, int nAPIVerMaxLen)
{
    return ReaderFrame::getInstance()->ReaderVersion(nHandle, sRDVersion, nRDVerMaxLen, sAPIVersion, nAPIVerMaxLen);
}

/*****************************************************************
 函数名称：JT_GetVersion
 函数描述：9 获取设备动态库版本
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_GetVersion(char* sVersion, int nVerLen)
{
    return ReaderFrame::getInstance()->GetVersion(sVersion, nVerLen);
}

/*****************************************************************
 函数名称：JT_GetStatusMsg
 函数描述：10 获取错误码详细描述
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_GetStatusMsg(int nStatusCode, char* sStatusMsg, int nStatusMsgLen)
{
    return ReaderFrame::getInstance()->GetStatusMsg(nStatusCode, sStatusMsg, nStatusMsgLen);
}

/*****************************************************************
 函数名称：JT_OpenCard
 函数描述：11 打开卡片
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_OpenCard(int nHandle, int*pCardPlace, char* sPhysicsCardno)
{
    return ReaderFrame::getInstance()->OpenCard(nHandle, pCardPlace, sPhysicsCardno);
}

/*****************************************************************
 函数名称：JT_CloseCard
 函数描述：12 关闭卡片
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_CloseCard(int nHandle)
{
    return ReaderFrame::getInstance()->CloseCard(nHandle);
}

/*****************************************************************
 函数名称：JT_CPUCommand
 函数描述：13 CPU 卡通用指令函数
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_CPUCommand(int nHandle, char* sCommand, int nLenCom, char* sReply, int* pLenRep)
{
    return ReaderFrame::getInstance()->CPUCommand(nHandle, sCommand, nLenCom, sReply, pLenRep);
}

/*****************************************************************
 函数名称：JT_SamReset
 函数描述：14 SAM 卡复位
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_SamReset(int nHandle, int nSockID, int nProtocolType)
{
    return ReaderFrame::getInstance()->SamReset(nHandle, nSockID, nProtocolType);
}

/*****************************************************************
 函数名称：JT_SamCommand
 函数描述：15 SAM 卡通用指令函数
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_SamCommand(int nHandle, int nSockID, char* sCommand, int nLenCom, char* sReply, int* pLenRep)
{
    return ReaderFrame::getInstance()->SamCommand(nHandle, nSockID, sCommand, nLenCom, sReply, pLenRep);
}

/*****************************************************************
 函数名称：JT_LoadKey
 函数描述：16 M1 卡载入密钥（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_LoadKey(int nHandle, int nKeyType, int nSec, char* sKeyValue)
{
    return ReaderFrame::getInstance()->LoadKey(nHandle, nKeyType, nSec, sKeyValue);
}

/*****************************************************************
 函数名称：JT_ReadBlock
 函数描述：17 M1 卡读块函数（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_ReadBlock(int nHandle, int nKeyType, int nBlock, char* sReply)
{
    return ReaderFrame::getInstance()->ReadBlock(nHandle, nKeyType, nBlock, sReply);
}

/*****************************************************************
 函数名称：JT_WriteBlock
 函数描述：18 M1 卡写块函数（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
int JT_WriteBlock(int nHandle, int nKeyType, int nBlock, char* sData)
{
    return ReaderFrame::getInstance()->WriteBlock(nHandle, nKeyType, nBlock, sData);
}
