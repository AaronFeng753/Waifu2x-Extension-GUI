#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);
    connect(this, SIGNAL(Send_RUN()), this, SLOT(RUN_SLOT()));
    connect(this, SIGNAL(Send_Duplicate()), this, SLOT(Duplicate_SLOT()));
    QtConcurrent::run(this, &MainWindow::RUN_Concurrent);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Duplicate_SLOT()
{
    ui->label_status->setText("It has been detected that the program is already\n"
                              "running,do not start the program repeatedly.");
    ui->label_status->setStyleSheet("color: rgb(255, 69, 69);background-color: rgb(255, 255, 255);");
}

void MainWindow::RUN_Concurrent()
{
    QThread::sleep(3);
    //========
    QProcess Get_tasklist;
    Get_tasklist.start("tasklist");
    while(!Get_tasklist.waitForStarted(200)) {}
    while(!Get_tasklist.waitForFinished(200)) {}
    if(Get_tasklist.readAllStandardOutput().contains("Waifu2x-Extension-GUI.exe")==true)
    {
        emit Send_Duplicate();
        QThread::sleep(5);
        this->close();
        return;
    }
    //========
    emit Send_RUN();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    qApp->setQuitOnLastWindowClosed(true);//無窗口時不再保持運行
    qApp->closeAllWindows();
}

/*
检测文件夹是否可写入
*/
bool MainWindow::file_isDirWritable(QString DirPath)
{
    if(DirPath.right(1)=="/")
    {
        DirPath = DirPath.left(DirPath.length() - 1);
    }
    QString TestTemp = DirPath+"/RWTest_W2xEXLauncher.tmp";
    QFile file_TestTemp(TestTemp);
    file_TestTemp.remove();
    if (file_TestTemp.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite支持读写
    {
        QTextStream stream(&file_TestTemp);
        stream << "W2xEX";
    }
    if(QFile::exists(TestTemp))
    {
        file_TestTemp.remove();
        return true;
    }
    else
    {
        file_TestTemp.remove();
        return false;
    }
}

void MainWindow::RUN_SLOT()
{
    //Current_Path = "D:/workspace/Waifu2x-Extension-QT/Full_package/waifu2x-extension-gui";
    if(file_isDirWritable(Current_Path)==false)
    {
        ui->label_status->setText("Please grant administrator rights to\n"
                                  "ensure the normal operation of the software.");
        ShellExecuteW(NULL, QString("runas").toStdWString().c_str(), QString(Current_Path+"/Waifu2x-Extension-GUI.exe").toStdWString().c_str(), QString(Current_Path+"/Waifu2x-Extension-GUI.exe").toStdWString().c_str(), NULL, 1);
        this->close();
        return;
    }
    ShellExecuteW(NULL, QString("open").toStdWString().c_str(), QString(Current_Path+"/Waifu2x-Extension-GUI.exe").toStdWString().c_str(), NULL, NULL, 1);
    this->close();
    return;
}
