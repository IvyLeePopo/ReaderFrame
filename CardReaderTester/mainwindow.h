#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow *getIntance();
    ~MainWindow();
    static void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

public slots:
    void slotAppendText(QString str);
    void slotTimerLoop();
    void slotEtcTradeResult(int nRet);

signals:
    void sigAppendText(QString str);

private slots:
    void on_btn_openReader_clicked();

    void on_btn_readCard_clicked();

    void on_btn_clear_clicked();

    void on_btn_etcTrade_clicked();

    void on_btn_openTwoReader_clicked();

    void on_btn_etcTradeLoop_clicked();

    void on_btn_stopTradeLoop_clicked();

    void on_btn_openReaderSingle_clicked();

    void on_btn_samCommand_clicked();

    void on_btn_samReset_clicked();

private:
    static MainWindow *instance;
    MainWindow(QWidget *parent = nullptr);
    bool loadLibrary();

private:
    Ui::MainWindow *ui;

private:
    QTimer *m_pTimerLoop;
    int m_nReaderNo;
    int m_arrCnt[4];
};
#endif // MAINWINDOW_H
