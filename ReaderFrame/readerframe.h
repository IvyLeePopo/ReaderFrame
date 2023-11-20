#ifndef READERFRAME_H
#define READERFRAME_H

#include "readerframe_global.h"
#include <QObject>

#include "Singleton.h"
#include "TwProtocol.h"

class READERFRAMESHARED_EXPORT ReaderFrame: public QObject, public Singleton<ReaderFrame>
{
    Q_OBJECT
    friend class Singleton<ReaderFrame>;

public:
    explicit ReaderFrame();
    virtual ~ReaderFrame();

public:
    //1 打开读卡器
    int OpenReader(int nMode, char *sParas);
    //2 关闭读卡器
    int CloseReader(int nHandle);
    //3 设置 IC 卡读写器通讯速率（可选）
    int SetInitPPS(int nHandle, char* sPPS);
    //4 设置 IC 卡读写器超时时间（可选）
    int SetInitTimeOut(int nHandle, char* sTimeOut);
    //5 设置读写器发光二极管
    int LEDDisplay(int nHandle, int nRed, int nGreen, int nBlue);
    //6 设置读写器蜂鸣器
    int AudioControl(int nHandle, int nTimes, int nVoice);
    //7 获取设备状态
    int GetStatus(int nHandle, int* pStatusCode);
    //8 获取设备版本信息
    int ReaderVersion(int nHandle, char* sRDVersion, int nRDVerMaxLen, char*sAPIVersion, int nAPIVerMaxLen);
    //9 获取设备动态库版本
    int GetVersion(char* sVersion, int nVerLen);
    //10 获取错误码详细描述
    int GetStatusMsg(int nStatusCode, char* sStatusMsg, int nStatusMsgLen);
    //11 打开卡片
    int OpenCard(int nHandle, int*pCardPlace, char* sPhysicsCardno);
    //12 关闭卡片
    int CloseCard(int nHandle);
    //13 CPU 卡通用指令函数
    int CPUCommand(int nHandle, char* sCommand, int nLenCom, char* sReply, int* pLenRep);
    //14 SAM 卡复位
    int SamReset(int nHandle, int nSockID, int nProtocolType);
    //15 SAM 卡通用指令函数
    int SamCommand(int nHandle, int nSockID, char* sCommand, int nLenCom, char* sReply, int* pLenRep);
    //16 M1 卡载入密钥（可选）
    int LoadKey(int nHandle, int nKeyType, int nSec, char* sKeyValue);
    //17 M1 卡读块函数（可选）
    int ReadBlock(int nHandle, int nKeyType, int nBlock, char* sReply);
    //18 M1 卡写块函数（可选）
    int WriteBlock(int nHandle, int nKeyType, int nBlock, char* sData);

private:
    int ReaderTrx(int nHandle, eSubCmdType_t subCmdType, const string& pReqDataJson, string& pRspDataJson, int iTotalWaitMs);

private:
    HANDLE m_hReaderHandle = 0;
    bool m_bTimeoutConnect = false;
};

#endif // READERFRAME_H
