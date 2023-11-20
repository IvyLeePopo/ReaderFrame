#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QTextCodec>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QLibrary>
#include "CardReadWrite.h"

MainWindow* MainWindow::instance = nullptr;
MainWindow* MainWindow::getIntance()
{
    if (instance == nullptr) {
        instance = new MainWindow();
    }
    return instance;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->le_path->setText("COM1");

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//或者"GBK",不分大小写
    QTextCodec::setCodecForLocale(codec);

    QString appPath = QCoreApplication::applicationDirPath();
    CardReadWrite::getInstance(0)->loadLibrary(appPath);
    CardReadWrite::getInstance(1)->loadLibrary(appPath);

    qInstallMessageHandler(logOutput);

    connect(this, &MainWindow::sigAppendText, this, &MainWindow::slotAppendText);
    connect(CardReadWrite::getInstance(0), &CardReadWrite::sigAppendText, this, &MainWindow::slotAppendText);
    connect(CardReadWrite::getInstance(1), &CardReadWrite::sigAppendText, this, &MainWindow::slotAppendText);

    m_nReaderNo = 1;
    m_pTimerLoop = new QTimer();
    m_pTimerLoop->setSingleShot(false);
    connect(m_pTimerLoop, &QTimer::timeout, this, &MainWindow::slotTimerLoop);

    for (int i = 0; i < 4; i++)
        m_arrCnt[i] = 0;
    connect(CardReadWrite::getInstance(0), &CardReadWrite::sigEtcTradeResult, this, &MainWindow::slotEtcTradeResult);
    connect(CardReadWrite::getInstance(1), &CardReadWrite::sigEtcTradeResult, this, &MainWindow::slotEtcTradeResult);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);
    QString text;
    text.append(QDateTime::currentDateTime().toString("MM-dd hh:mm:ss.zzz") + " ");
    text.append(msg);
    text.append("\r\n");
    emit instance->sigAppendText(text);
}

void MainWindow::on_btn_openReader_clicked()
{
    CardReadWrite::getInstance(0)->openReader(ui->le_path->text());
}


void MainWindow::on_btn_readCard_clicked()
{
    switch (CardReadWrite::getInstance(0)->openCard())
    {
    case 0:
        CardReadWrite::getInstance(0)->readCpuCard();
        break;
    case 4:
        CardReadWrite::getInstance(0)->readCpcCard();
        break;
    }
}

void MainWindow::slotAppendText(QString strMsg)
{
    QString text;
    text.append(QDateTime::currentDateTime().toString("MM-dd hh:mm:ss.zzz") + " ");
    text.append(strMsg);
    text.append("\r\n");

    ui->textEdit->append(text);

    QFile file(tr("TesterLog%1.txt").arg(QDate::currentDate().toString("yyyyMMdd")));
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << strMsg << "\r\n";
    file.flush();
    file.close();

    qDebug() << strMsg;

    QEventLoop().processEvents(QEventLoop::AllEvents);
}

void MainWindow::on_btn_clear_clicked()
{
    ui->textEdit->clear();
    ui->lbl_com1Succ->setNum(0);
    ui->lbl_com1Fail->setNum(0);
    ui->lbl_com2Succ->setNum(0);
    ui->lbl_com2Succ->setNum(0);
    QEventLoop().processEvents(QEventLoop::AllEvents);
}

void MainWindow::on_btn_etcTrade_clicked()
{
    switch (CardReadWrite::getInstance(0)->openCard())
    {
    case 0:
        CardReadWrite::getInstance(0)->readCpuCard();
        break;
    }
}

void MainWindow::on_btn_openTwoReader_clicked()
{
    CardReadWrite::getInstance(0)->openReader("COM1");
    CardReadWrite::getInstance(1)->openReader("COM2");
}

void MainWindow::on_btn_etcTradeLoop_clicked()
{
    slotTimerLoop();
    m_pTimerLoop->start(5000);
}

void MainWindow::on_btn_stopTradeLoop_clicked()
{
    m_pTimerLoop->stop();
}

void MainWindow::slotTimerLoop()
{
    if (m_nReaderNo == 1) {
        m_nReaderNo = 0;
        emit sigAppendText("上工位开始寻卡");
    }
    else {
        m_nReaderNo = 1;
        emit sigAppendText("下工位开始寻卡");
    }

    switch (CardReadWrite::getInstance(m_nReaderNo)->openCard())
    {
    case 0:
        CardReadWrite::getInstance(m_nReaderNo)->readCpuCard();
        break;
    }
}

void MainWindow::slotEtcTradeResult(int nRet)
{
    switch (nRet) {
    case 0:
        ui->lbl_com1Succ->setNum(++m_arrCnt[nRet]);
        break;
    case 1:
        ui->lbl_com1Fail->setNum(++m_arrCnt[nRet]);
        break;
    case 2:
        ui->lbl_com2Succ->setNum(++m_arrCnt[nRet]);
        break;
    case 3:
        ui->lbl_com2Fail->setNum(++m_arrCnt[nRet]);
        break;
    }
}

void MainWindow::on_btn_openReaderSingle_clicked()
{
    CardReadWrite::getInstance(0)->openReaderSingle(ui->le_path->text());
}

void MainWindow::on_btn_samCommand_clicked()
{
    CardReadWrite::getInstance(0)->samCommand();
}

void MainWindow::on_btn_samReset_clicked()
{
    CardReadWrite::getInstance(0)->samReset();
}
