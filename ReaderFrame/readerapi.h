#ifndef READERAPI_H
#define READERAPI_H

using namespace std;

//定义宏
#if defined(__cplusplus)
#define D_EXTERN_C extern "C"
#else
#define D_EXTERN_C
#endif

#define D_CALLTYPE
#define D_DECL_EXPORT   __attribute__((visibility("default")))
#define D_DECL_IMPORT   __attribute__((visibility("default")))

/*****************************************************************
 函数名称：JT_OpenReader
 函数描述：1 打开读卡器
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_OpenReader(int nMode, char *sParas);

/*****************************************************************
 函数名称：JT_CloseReader
 函数描述：2 关闭读卡器
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_CloseReader(int nHandle);

/*****************************************************************
 函数名称：JT_SetInitPPS
 函数描述：3 设置 IC 卡读写器通讯速率（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_SetInitPPS(int nHandle, char* sPPS);

/*****************************************************************
 函数名称：JT_SetInitTimeOut
 函数描述：4 设置 IC 卡读写器超时时间（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_SetInitTimeOut(int nHandle, char* sTimeOut);

/*****************************************************************
 函数名称：JT_LEDDisplay
 函数描述：5 设置读写器发光二极管
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_LEDDisplay(int nHandle, int nRed, int nGreen, int nBlue);

/*****************************************************************
 函数名称：PJT_AudioControl
 函数描述：6 设置读写器蜂鸣器
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_AudioControl(int nHandle, int nTimes, int nVoice);

/*****************************************************************
 函数名称：JT_GetStatus
 函数描述：7 获取设备状态
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_GetStatus(int nHandle, int* pStatusCode);

/*****************************************************************
 函数名称：JT_ReaderVersion
 函数描述：8 获取设备版本信息
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_ReaderVersion(int nHandle, char* sRDVersion, int nRDVerMaxLen, char*sAPIVersion, int nAPIVerMaxLen);

/*****************************************************************
 函数名称：JT_GetVersion
 函数描述：9 获取设备动态库版本
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_GetVersion(char* sVersion, int nVerLen);

/*****************************************************************
 函数名称：JT_GetStatusMsg
 函数描述：10 获取错误码详细描述
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_GetStatusMsg(int nStatusCode, char* sStatusMsg, int nStatusMsgLen);

/*****************************************************************
 函数名称：JT_OpenCard
 函数描述：11 打开卡片
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_OpenCard(int nHandle, int*pCardPlace, char* sPhysicsCardno);

/*****************************************************************
 函数名称：JT_CloseCard
 函数描述：12 关闭卡片
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_CloseCard(int nHandle);

/*****************************************************************
 函数名称：JT_CPUCommand
 函数描述：13 CPU 卡通用指令函数
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_CPUCommand(int nHandle, char* sCommand, int nLenCom, char* sReply, int* pLenRep);

/*****************************************************************
 函数名称：JT_SamReset
 函数描述：14 SAM 卡复位
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_SamReset(int nHandle, int nSockID, int nProtocolType);

/*****************************************************************
 函数名称：JT_SamCommand
 函数描述：15 SAM 卡通用指令函数
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_SamCommand(int nHandle, int nSockID, char* sCommand, int nLenCom, char* sReply, int* pLenRep);

/*****************************************************************
 函数名称：JT_LoadKey
 函数描述：16 M1 卡载入密钥（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_LoadKey(int nHandle, int nKeyType, int nSec, char* sKeyValue);

/*****************************************************************
 函数名称：JT_ReadBlock
 函数描述：17 M1 卡读块函数（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_ReadBlock(int nHandle, int nKeyType, int nBlock, char* sReply);

/*****************************************************************
 函数名称：JT_WriteBlock
 函数描述：18 M1 卡写块函数（可选）
 输入参数：
 输出参数：
 返回说明：
 其它说明：
 *****************************************************************/
D_EXTERN_C int D_CALLTYPE JT_WriteBlock(int nHandle, int nKeyType, int nBlock, char* sData);

//动态库接口声明
typedef int (*TYPE_JT_OpenReader  )(int nMode, char *sParas);
typedef int (*TYPE_JT_CloseReader )(int nHandle);
typedef int (*TYPE_JT_OpenCard    )(int nHandle, int* pCardPlace,char* sPhysicsCardno);
typedef int (*TYPE_JT_CloseCard   )(int nHandle);
typedef int (*TYPE_JT_LEDDisplay  )(int nHandle, int nRed,  int nGreen,int nBlue);
typedef int (*TYPE_JT_AudioControl)(int nHandle, int nTimes, int nVoice );
typedef int (*TYPE_JT_CPUCommand  )(int nHandle, char* sCommand, int nLenCom, char* sReply, int* pLenRep);
typedef int (*TYPE_JT_SamReset    )(int nHandle, int nSockID, int nProtocolType );
typedef int (*TYPE_JT_SamCommand  )(int nHandle, int nSockID, char* sCommand, int nLenCom, char* sReply,  int* pLenRep);
typedef int (*TYPE_JT_GetStatus   )(int nHandle, int* pStatusCode);
typedef int (*TYPE_JT_GetStatusMsg)(int nStatusCode, char* sStatusMsg, int nStatusMsgLen);
typedef int (*TYPE_JT_ReaderVersion)(int nHandle,char* sRDVersion,int nRDVerMaxLen,char* sAPIVersion,int nAPIVerMaxLen);
typedef int (*TYPE_JT_LoadKey     )(int nHandle, int nKeyType, int nSec, char* sKeyValue);
typedef int (*TYPE_JT_ReadBlock   )(int nHandle, int nKeyType, int nBlock, char* sReply);
typedef int (*TYPE_JT_WriteBlock  )(int nHandle, int nKeyType, int nBlock, char* sData);


#endif // READERAPI_H
