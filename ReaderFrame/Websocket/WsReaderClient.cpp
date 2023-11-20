#include "WsReaderClient.h"

#include <QWebSocket>
#include <QTimer>
#include <QThread>
#include <QByteArray>
#include <cstring>
#include <QSettings>
#include <QDir>
#include <dlfcn.h>

#include "WebSocketBase.h"

WsReaderClient::WsReaderClient(QObject *parent)
    : QObject(parent)
{
    readConfig();

    m_thdReaderClient = new QThread();
    m_wsReaderClient = new WebSocketBase();
    m_wsReaderClient->setWebSocketUrl(m_URL);

    m_wsReaderClient->moveToThread(m_thdReaderClient);
    connect(m_thdReaderClient, &QThread::started, m_wsReaderClient, &WebSocketBase::slotCreateDataRecWS);
    connect(this, &WsReaderClient::sigSendToServer, m_wsReaderClient, &WebSocketBase::slotSendBinaryMessageMessage);
    //connect(this, &WsReaderClient::sigReconnectServer, m_wsReaderClient, &WebSocketBase::slotActiveReconnect);

    connect(m_thdReaderClient, &QThread::finished, m_wsReaderClient, &WebSocketBase::deleteLater);
    connect(m_thdReaderClient, &QThread::finished, m_thdReaderClient, &QThread::deleteLater);
    m_thdReaderClient->start();

    m_pTimerReader = new QTimer(this);
    connect(m_pTimerReader, &QTimer::timeout, this, &WsReaderClient::slotHeartBeatToServer);
    //m_nHeartBeatTimeOutKeyBoard = 0;
    m_pTimerReader->start(10*1000);
}

WsReaderClient::~WsReaderClient()
{
    m_pTimerReader->stop();
    m_pTimerReader->deleteLater();

    if(m_wsReaderClient)
    {
       delete m_wsReaderClient;
        m_wsReaderClient = nullptr;
    }

    if(m_pTimerReader)
    {
        delete m_pTimerReader;
        m_pTimerReader = nullptr;
    }
}

void WsReaderClient::slotHeartBeatToServer()
{
    //todo
}

void getCurrentDllPath()
{
    return;
}

void WsReaderClient::readConfig()
{
    Dl_info info;
    //dladdr获取某个地址的符号信息
    int rc = dladdr((void*)getCurrentDllPath, &info);
    if (!rc)
    {
        QString strError = QString("Problem retrieving program information for %1").arg(dlerror());
        qDebug() << __FUNCTION__ << strError;
    }

    QString strPath = "";
    strPath = info.dli_fname;
    strPath = strPath.mid(0, strPath.lastIndexOf("/"));
    qDebug() << "Current path : " << strPath;

    QString pathIni = strPath + "/ReaderConfig.ini";
    QSettings settings(pathIni, QSettings::IniFormat);
    m_URL = settings.value("Communication/ipAddr").toString();
    qDebug() << "m_URL=" << m_URL;
}

void WsReaderClient::SendTo(const QByteArray &data)
{
    emit sigSendToServer(data);
}

int WsReaderClient::RecvFrom(QByteArray &byteArray)
{
    return m_wsReaderClient->RecvFrom(byteArray);
}

bool WsReaderClient::getConnectStatus()
{
    return m_wsReaderClient->getConnectStatus();
}
