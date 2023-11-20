#include "readerframe.h"

#include <QMutex>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QTextCodec>
#include <QTimer>
#include <QThread>

#include "WsReaderClient.h"

#define READER_HW_VERSION   "TWKJ_ICCRDRSTM_V20220509"
#define READER_API_VERSION  "TWKJ_ICCRDRAPI_V20220509"
#define READER_DLL_VERSION  "DLL_TWKJ_V20220509"


#define TOTAL_WAIT_MS  (3000)

static QMutex g_qmutex;
int level = 4;
bool checkLogThreadFlag = true;
int logTimeInterval = 600;
long long fileOffSet=0;

void outputMessage(QtMsgType type, const QMessageLogContext& Context, const QString &data)
{
    Q_UNUSED(Context);

    if(g_qmutex.tryLock(10)==false)
    {
        return;
    }

    QString msg(data);
    QString text;
    int outputLevel = 0;
    switch(type)
    {
    case QtDebugMsg:
    {
        text = QString("Debug:");
        outputLevel = 4;
        break;
    }
    case QtWarningMsg:
    {
        text = QString("Warning:");
        outputLevel = 3;
        break;
    }
    case QtCriticalMsg:
    {
        text = QString("Critical:");
        outputLevel = 2;
        break;
    }
    case QtFatalMsg:
    {
        text = QString("Fatal:");
        outputLevel = 1;
        break;
    }
    case QtInfoMsg:
    {
        text = QString("Info");
        outputLevel = 0;
        break;
    }
    }

    if (level < outputLevel)
    {
        return;
    }

    QString current_date_time = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss.zzz");
    QString current_date = QString("[%1]").arg(current_date_time);
    QString message = QString("%1 %2").arg(current_date, msg);

    QString file_time = QDate::currentDate().toString("yyyyMMdd");

    QString appPath = QCoreApplication::applicationDirPath();

    QString file_name1  = appPath + QString("/log/LogReaderApi%1.log").arg(file_time);

    QFile file(file_name1);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
    g_qmutex.unlock();
}

ReaderFrame::ReaderFrame()
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    qInstallMessageHandler(outputMessage);

    //开启读卡器websocket client
    WsReaderClient::getInstance();
    QTimer::singleShot(500, this, [this](){
        m_bTimeoutConnect = true;
    });

}

ReaderFrame::~ReaderFrame()
{
    //释放读卡器client
    delete WsReaderClient::getInstance();
}

//1 打开读卡器
int ReaderFrame::OpenReader(int nMode, char *sParas)
{
    qDebug() << __FUNCTION__ << "Connectinbg server start... ";
    while(1)
    {
        if(WsReaderClient::getInstance()->getConnectStatus() || m_bTimeoutConnect)
            break;
    }
    qDebug() << __FUNCTION__ << "Connectinbg server end... ";

    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    if ((sParas == NULL) || (sParas[0] == '\0'))
    {
        qDebug() << "nMode=" << nMode << " sParas = " << "NULL";
        return READER_API_ERROR_OTHER;
    }

    //区分上下工位
    int nHandle = 1;
    std::string strParas(sParas);
    if (strParas == "COM1")
        nHandle = 1;
    else if (strParas == "COM2")
        nHandle = 2;


    string strReqJson, strRspJson;
    TW_Protocol::generateOpenReaderJson(strReqJson, 0, sParas);//默认为0
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    //send data
    int ret = ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_OpenReader, strReqJson, strRspJson, TOTAL_WAIT_MS);

    if (ret < 0)
    {
        qDebug() << __FUNCTION__ << " ReaderTrx ret fail:" << ret;
        return READER_API_ERROR_OPEN_DEV_FAIL;
    }

    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string strHandleCode;
    TW_Protocol::parseOpenReaderRespJson(strRspJson, strHandleCode);

    if (strHandleCode.empty() == false)
    {
        int num = std::atoi(strHandleCode.c_str());
        m_hReaderHandle = (HANDLE)(num);
    }

    qDebug() << __FUNCTION__ << "Connect To Dev Successfull!";
    qDebug() << __FUNCTION__ << "Handle = " << (int)(m_hReaderHandle);

    return (int)(m_hReaderHandle);
}

//2 关闭读卡器
int ReaderFrame::CloseReader(int nHandle)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    //发送数据
    string strReqJson, strRspJson;
    TW_Protocol::generateCloseReaderJson(strReqJson, nHandle);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_CloseReader, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string strReturnCode;
    TW_Protocol::parseCloseReaderRespJson(strRspJson, strReturnCode);

    //再断开
//    if (0 == std::atoi(strReturnCode.c_str()))
//	{
//		if (m_pcReaderInterface != NULL)
//		{
//			m_pcReaderInterface->Disconnect();
//		}
//		m_hReaderHandle = 0;
//		memset(m_cDevName, 0, sizeof(m_cDevName));
//		return READER_API_NO_ERROR;
//	}

    return std::atoi(strReturnCode.c_str());

}

//3 设置 IC 卡读写器通讯速率（可选）
int ReaderFrame::SetInitPPS(int nHandle, char* sPPS)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateSetInitPPSJson(strReqJson, nHandle, sPPS);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_SetInitPPS, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string strReturnCode;
    TW_Protocol::parseSetInitPPSRespJson(strRspJson, strReturnCode);

    return std::atoi(strReturnCode.c_str());
}

//4 设置 IC 卡读写器超时时间（可选）
int ReaderFrame::SetInitTimeOut(int nHandle, char* sTimeOut)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateSetInitTimeOutJson(strReqJson, nHandle, sTimeOut);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_SetInitTimeOut, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string strReturnCode;
    TW_Protocol::parseGeneralRespJsonData(strRspJson, strReturnCode);

    return std::atoi(strReturnCode.c_str());
}

//5 设置读写器发光二极管
int ReaderFrame::LEDDisplay(int nHandle, int nRed, int nGreen, int nBlue)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateLEDDisplayJson(strReqJson, nHandle, nRed, nGreen, nBlue);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_LEDDisplay, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string strReturnCode;
    TW_Protocol::parseLEDDisplayRespJson(strRspJson, strReturnCode);
    return std::atoi(strReturnCode.c_str());
}

//6 设置读写器蜂鸣器
int ReaderFrame::AudioControl(int nHandle, int nTimes, int nVoice)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateAudioControlJson(strReqJson, nHandle, nTimes, nVoice);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_AudioControl, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string strReturnCode;
    TW_Protocol::parseGetStatusRespJson(strRspJson, strReturnCode);

    return std::atoi(strReturnCode.c_str());
}

//7 获取设备状态
int ReaderFrame::GetStatus(int nHandle, int* pStatusCode)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateGetStatusJson(strReqJson, nHandle);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_GetStatus, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string StatusCode;
    TW_Protocol::parseGetStatusRespJson(strRspJson, StatusCode);
    *pStatusCode = std::atoi(StatusCode.c_str());

    return READER_API_NO_ERROR;
}

//8 获取设备版本信息
int ReaderFrame::ReaderVersion(int nHandle, char* sRDVersion, int nRDVerMaxLen, char*sAPIVersion, int nAPIVerMaxLen)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateReaderVersionJson(strReqJson, nHandle);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    int ret = ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_ReaderVersion, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    if (ret == 0)
    {
        string strRDVersion;
        string strAPIVersion;
        TW_Protocol::parseReaderVersionRespJson(strRspJson, strRDVersion, strAPIVersion);

        snprintf(sRDVersion, nRDVerMaxLen, "%s", strRDVersion.c_str());
        snprintf(sAPIVersion, nAPIVerMaxLen, "%s", strAPIVersion.c_str());
    }

    return READER_API_NO_ERROR;
}

//9 获取设备动态库版本
int ReaderFrame::GetVersion(char* sVersion, int nVerLen)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    if ((sVersion != NULL) && (nVerLen > 0))
    {
        snprintf(sVersion, nVerLen, "%s", READER_DLL_VERSION);
        return READER_API_NO_ERROR;
    }
    return READER_API_ERROR_INPUT_PARAMETER;
}

//10 获取错误码详细描述
int ReaderFrame::GetStatusMsg(int nStatusCode, char* sStatusMsg, int nStatusMsgLen)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    if ((sStatusMsg != NULL) && (nStatusMsgLen > 0))
    {
        if (nStatusCode == READER_API_NO_ERROR)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "操作成功");
        }
        else if (nStatusCode == READER_API_ERROR_NO_CARD)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "无卡");
        }
        else if (nStatusCode == READER_API_ERROR_OPEN_CARD_FAIL)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "开卡失败");
        }
        else if (nStatusCode == READER_API_ERROR_CLOSE_CARD_FAIL)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "关卡失败");
        }
        else if (nStatusCode == READER_API_ERROR_NO_RESPONSE)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "超时无响应");
        }
        else if (nStatusCode == READER_API_ERROR_INPUT_PARAMETER)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "参数错误");
        }
        else if (nStatusCode == READER_API_ERROR_DEV_IN_BUSY)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "设备忙");
        }
        else if (nStatusCode == READER_API_ERROR_OPEN_DEV_FAIL)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "打开设备失败");
        }
        else if (nStatusCode == READER_API_ERROR_SET_RED_LED_FAIL)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "设置红灯失败");
        }
        else if (nStatusCode == READER_API_ERROR_SET_GRN_LED_FAIL)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "设置绿灯失败");
        }
        else if (nStatusCode == READER_API_ERROR_SET_BLE_LED_FAIL)
        {
            snprintf(sStatusMsg, nStatusMsgLen, "打开蓝灯失败");
        }
        else
        {
            snprintf(sStatusMsg, nStatusMsgLen, "其他失败/错误");
        }
        return READER_API_NO_ERROR;
    }
    return READER_API_ERROR_INPUT_PARAMETER;
}

//11 打开卡片
int ReaderFrame::OpenCard(int nHandle, int*pCardPlace, char* sPhysicsCardno)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateOpenCardJson(strReqJson, nHandle);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_OpenCard, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string ReturnCode;
    string strCardPlace;
    string strPhysicsCardno;
    TW_Protocol::parseOpenCardRespJson(strRspJson, ReturnCode, strCardPlace, strPhysicsCardno);

    *pCardPlace = std::atoi(strCardPlace.c_str());

    int size = strPhysicsCardno.size();
    memcpy(sPhysicsCardno, strPhysicsCardno.c_str(), size);
    //snprintf(sPhysicsCardno, 8, "%s", strPhysicsCardno.c_str());
    return std::atoi(ReturnCode.c_str());
}

//12 关闭卡片
int ReaderFrame::CloseCard(int nHandle)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateCloseCardJson(strReqJson, nHandle);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_CloseCard, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string ReturnCode;
    TW_Protocol::parseCloseCardRespJson(strRspJson, ReturnCode);

    return std::atoi(ReturnCode.c_str());
}

//13 CPU 卡通用指令函数
int ReaderFrame::CPUCommand(int nHandle, char* sCommand, int nLenCom, char* sReply, int* pLenRep)
{
    Q_UNUSED(nLenCom);
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateCPUCommandJson(strReqJson, nHandle, sCommand);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_CPUCommand, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string ReturnCode;
    string strReply;
    TW_Protocol::parseCPUCommandRespJson(strRspJson, ReturnCode, strReply);
    int ReplyLen = strReply.length();

    *pLenRep = ReplyLen;
    snprintf(sReply, ReplyLen + 1, "%s", strReply.c_str());

    return std::atoi(ReturnCode.c_str());
}

//14 SAM 卡复位
int ReaderFrame::SamReset(int nHandle, int nSockID, int nProtocolType)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateSamResetJson(strReqJson, nHandle, nSockID, nProtocolType);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_SamReset, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string ReturnCode;
    TW_Protocol::parseSamResetRespJson(strRspJson, ReturnCode);

    return std::atoi(ReturnCode.c_str());
}

//15 SAM 卡通用指令函数
int ReaderFrame::SamCommand(int nHandle, int nSockID, char* sCommand, int nLenCom, char* sReply, int* pLenRep)
{
    Q_UNUSED(nLenCom);
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateSamCommandJson(strReqJson, nHandle, nSockID, sCommand);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_SamCommand, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string ReturnCode;
    string strReply;
    TW_Protocol::parseSamCommandRespJson(strRspJson, ReturnCode, strReply);

    *pLenRep = strReply.length();
    snprintf(sReply, strReply.length() + 1, "%s", strReply.c_str());\
    return std::atoi(ReturnCode.c_str());
}

//16 M1 卡载入密钥（可选）
int ReaderFrame::LoadKey(int nHandle, int nKeyType, int nSec, char* sKeyValue)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateLoadKeyJson(strReqJson, nHandle, nKeyType, nSec, sKeyValue);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_LoadKey, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string ReturnCode;
    TW_Protocol::parseLoadKeyRespJson(strRspJson, ReturnCode);

    return std::atoi(ReturnCode.c_str());
}

//17 M1 卡读块函数（可选）
int ReaderFrame::ReadBlock(int nHandle, int nKeyType, int nBlock, char* sReply)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateReadBlockJson(strReqJson, nHandle, nKeyType, nBlock);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_ReadBlock, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string ReturnCode;
    string strReply;
    TW_Protocol::parseReadBlockRespJson(strRspJson, ReturnCode, strReply);

    int ReplyLen = strReply.length();
    snprintf(sReply, ReplyLen + 1, "%s", strReply.c_str());

    return std::atoi(ReturnCode.c_str());
}

//18 M1 卡写块函数（可选）
int ReaderFrame::WriteBlock(int nHandle, int nKeyType, int nBlock, char* sData)
{
    if(!WsReaderClient::getInstance()->getConnectStatus())
        return READER_API_ERROR_DISCONNECT;

    string strReqJson, strRspJson;
    TW_Protocol::generateWriteBlockJson(strReqJson, nHandle, nKeyType, nBlock, sData);
    qDebug() << __FUNCTION__ << " Request Json:" << strReqJson.c_str();

    ReaderTrx(nHandle, eSubCmdType_t::SUB_CMD_TYPE_WriteBlock, strReqJson, strRspJson, TOTAL_WAIT_MS);
    qDebug() << __FUNCTION__ << " Response Json:" << strRspJson.c_str();

    string ReturnCode;
    TW_Protocol::parseWriteBlockRespJson(strRspJson, ReturnCode);

    return std::atoi(ReturnCode.c_str());
}

//子命令类型，发送的json串，发送的json串的长度，应答的json串，应答的json串的长度，时间
int ReaderFrame::ReaderTrx(int nHandle, eSubCmdType_t subCmdType, const string &pReqDataJson, string &pRspDataJson, int iTotalWaitMs)
{
    Q_UNUSED(iTotalWaitMs);
    int iRet = 0;
    bool bParseResult = false;
    unsigned int nRspLen = 0;

    std::vector<unsigned char> escapePackage;
    TW_Protocol::ConstructFrame(subCmdType, nHandle, pReqDataJson, escapePackage);

    const QByteArray byteArray(reinterpret_cast<const char*>(escapePackage.data()), escapePackage.size());

//    qDebug() << "escapePackage.size() = " + escapePackage.size();

//    QString strTmp;
//    for(int n = 0; n < escapePackage.size(); n++)
//    {
//        strTmp +=  QString().sprintf("%x", (unsigned char)escapePackage[n]);
//    }
//    qDebug() << "escapePackage = " + strTmp;

    // 将数据发送过去

    qDebug() << __FUNCTION__ << "SendTo~";
    QString hex_data = byteArray.toHex();
    qDebug() << "hex_data=" + hex_data;
    //qDebug() << "byteArray = " + byteArray;

    WsReaderClient::getInstance()->SendTo(byteArray);

    QDateTime startTime=  QDateTime::currentDateTime();

    QByteArray recvArray;
    while (WsReaderClient::getInstance()->getConnectStatus()) //pRspData 应答数据
    {
        QDateTime endTime=  QDateTime::currentDateTime();
        qint64 intervalTimeMS = startTime.msecsTo(endTime);

        if (intervalTimeMS > TOTAL_WAIT_MS)
        {
            qDebug() << "Timeout, Break Recv Wait!";
            break;
        }

        QThread::msleep(20);

        iRet = WsReaderClient::getInstance()->RecvFrom(recvArray);
        nRspLen = iRet;

        if(nRspLen > 10)
        {
            //解析收到的数据
            bParseResult = TW_Protocol::ParseProtocolData(nHandle, recvArray, subCmdType, pRspDataJson);

            if (bParseResult == true)
            {
                qDebug() << __FUNCTION__ << "ParseProtocolData Successfully, Break Recv Wait.";
                break;
            }
            else
            {
                qDebug() << __FUNCTION__ << "ParseProtocolData Fail, Continue to Wait!";
            }
        }
    }

    return bParseResult ? 0 : READER_API_ERROR_NO_RESPONSE;
}
