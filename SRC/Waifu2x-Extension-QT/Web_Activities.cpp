/*
    Copyright (C) 2021  Aaron Feng

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    My Github homepage: https://github.com/AaronFeng753
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
从网络下载文件到本地
*/
bool MainWindow::DownloadTo(QString OnlineLink,QString LocalPath)
{
    QFile::remove(LocalPath);
    QString user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.96 Safari/537.36";
    QString program = Current_Path+"/wget_waifu2xEX.exe";
    QProcess Downlad2;
    Downlad2.start("\""+program+"\" --user-agent=\""+user_agent+"\" -O \""+LocalPath+"\" \""+OnlineLink+"\" --timeout=15");
    while(!Downlad2.waitForStarted(500)&&!QProcess_stop) {}
    while(!Downlad2.waitForFinished(500)&&!QProcess_stop) {}
    QString Downlad2_OutPutStr = Downlad2.readAllStandardError().toLower();
    QFileInfo *LocalPath_QFileInfo = new QFileInfo(LocalPath);
    if(LocalPath_QFileInfo->size()<1 || Downlad2_OutPutStr.contains("saved")==false)QFile::remove(LocalPath);
    return QFile::exists(LocalPath);
}
/*
判断与github的链接状态以告知用户是否可以禁用gitee
*/
void MainWindow::on_checkBox_BanGitee_clicked()
{
    if(ui->checkBox_BanGitee->isChecked() && isConnectivityTest_RawGithubusercontentCom_Running==false)
    {
        QtConcurrent::run(this, &MainWindow::ConnectivityTest_RawGithubusercontentCom);//后台运行网络测试,判断是否可以链接raw.githubusercontent.com
    }
}
/*
Github连通性测试
*/
void MainWindow::ConnectivityTest_RawGithubusercontentCom()
{
    QMutex_ConnectivityTest_RawGithubusercontentCom.lock();
    isConnectivityTest_RawGithubusercontentCom_Running=true;
    //===
    QString OnlineAddress="https://raw.githubusercontent.com/AaronFeng753/Waifu2x-Extension-GUI/master/.github/ConnectivityTest_githubusercontent.txt";
    QString LocalAddress=Current_Path+"/ConnectivityTest_Waifu2xEX.txt";
    QFile::remove(LocalAddress);
    //===
    emit Send_TextBrowser_NewMessage(tr("Testing if your PC can connect to raw.githubusercontent.com..."));
    if(DownloadTo(OnlineAddress,LocalAddress)==true)
    {
        emit Send_TextBrowser_NewMessage(tr("Detection complete, your PC can connect to raw.githubusercontent.com."));
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Detection complete, your PC cannot connect to raw.githubusercontent.com."));
        emit Send_Unable2Connect_RawGithubusercontentCom();
    }
    QFile::remove(LocalAddress);
    //===
    isConnectivityTest_RawGithubusercontentCom_Running=false;
    QMutex_ConnectivityTest_RawGithubusercontentCom.unlock();
}
/*
Github连通性测试--提示无法连接
*/
void MainWindow::Unable2Connect_RawGithubusercontentCom()
{
    QMessageBox *MSG_2 = new QMessageBox();
    MSG_2->setWindowTitle(tr("Notification"));
    MSG_2->setText(tr("Detected that you are currently unable to connect to raw.githubusercontent.com, so enabling [Ban Gitee] won't allow the software to automatically check for updates. It is recommended that you disable [Ban Gitee]."));
    MSG_2->setIcon(QMessageBox::Warning);
    MSG_2->setModal(true);
    MSG_2->show();
}
