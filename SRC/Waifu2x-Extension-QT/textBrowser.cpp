#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::TextBrowser_NewMessage(QString message)
{
    QDateTime time = QDateTime::currentDateTime();
    QString Current_Time = time.toString("[yyyy-MM-dd hh:mm:ss]");
    QString OutPutStr = Current_Time+" "+message;
    ui->textBrowser->append(OutPutStr);
}

void MainWindow::TextBrowser_StartMes()
{
    ui->textBrowser->append("Waifu2x-Extension-GUI by Aaron Feng");
    ui->textBrowser->append("Github: https://github.com/AaronFeng753/Waifu2x-Extension-GUI");
    ui->textBrowser->append("---------------------------------------------------------------------------------------------------");
}
