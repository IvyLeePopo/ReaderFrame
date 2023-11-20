//头文件

//动态库接口声明
typedef QString (*TYPE_printfAppPath)();
typedef QString (*TYPE_printfCurrentPath)();

TYPE_printfAppPath          printfAppPath;
TYPE_printfCurrentPath      printfCurrentPath;


bool loadLibrary(const QString& appPath);


//源文件
bool MainWindow::loadLibrary(const QString& appPath)
{
    QLibrary *m_pLibTest;
    QString strLibFile = appPath + "/dll/libtestDll.so";

    if (QFile::exists(strLibFile))
        m_pLibTest = new QLibrary(strLibFile);
    else {
        slotAppendText(strLibFile + " don't exists");
        return false;
    }

    if(!m_pLibTest->load())
    {
        QString strErrMsg = m_pLibTest->errorString();
        slotAppendText(strLibFile + " load failed: " + m_pLibTest->errorString());
        return false;
    }
    else
    {
        slotAppendText(strLibFile + " load success");
    }


    printfAppPath    = reinterpret_cast<TYPE_printfAppPath>(m_pLibTest->resolve("printfAppPath"));
    printfCurrentPath    = reinterpret_cast<TYPE_printfCurrentPath>(m_pLibTest->resolve("printfCurrentPath"));


    if(!printfAppPath)
    {
        slotAppendText("printfAppPath not resolve");
        return false;
    }

    if(!printfCurrentPath)
    {
        slotAppendText("printfCurrentPath not resolve");
        return false;
    }

    QString strAppPath = printfAppPath();
    QString strCurrentPath = printfCurrentPath();

    slotAppendText("strAppPath : " + strAppPath);
    slotAppendText("strCurrentPath : " + strCurrentPath);

    return true;

}