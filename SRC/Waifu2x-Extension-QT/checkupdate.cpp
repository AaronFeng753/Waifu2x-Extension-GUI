/*
    Copyright (C) 2020  Aaron Feng

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

/*
python_ext_Waifu2xEX.exe:
get video fps : [python_ext_Waifu2xEX.exe videoFilePath fps]
get video frame number : [python_ext_Waifu2xEX.exe videoFilePath countframe]
get gif duration : [python_ext_Waifu2xEX.exe videoFilePath countframedigits]
check update :[python_ext_Waifu2xEX.exe null checkupdate]
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
手动检查更新:直接打开latest release
*/
void MainWindow::on_pushButton_CheckUpdate_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest"));
}
/*
自动检查更新:
启动软件时以单独线程运行,调用python扩展检测更新,如有则弹窗
*/
int MainWindow::CheckUpadte_Auto()
{
    if(ui->checkBox_autoCheckUpdate->checkState()==false)
    {
        return 0;
    }
    //Delay_sec_sleep(5);
    QString program = Current_Path+"/python_ext_Waifu2xEX.exe";
    QProcess checkupdate;
    checkupdate.start("\""+program+"\" null checkupdate");
    while(!checkupdate.waitForStarted(100)&&!QProcess_stop) {}
    while(!checkupdate.waitForFinished(100)&&!QProcess_stop) {}
    QString update_str="";
    if(!QProcess_stop)update_str=checkupdate.readAllStandardOutput().data();
    update_str = update_str.trimmed();
    if(update_str!=VERSION&&update_str!="failed"&&update_str!="")
    {
        emit Send_CheckUpadte_NewUpdate(update_str);
    }
    return 0;
}
/*
自动更新弹窗
*/
int MainWindow::CheckUpadte_NewUpdate(QString update_str)
{
    switch(ui->comboBox_language->currentIndex())
    {
        case 0:
            {
                QMessageBox Msg(QMessageBox::Question, QString(tr("New version available!")), QString(tr("New version: %1 \n\nDo you wanna update now???")).arg(update_str));
                Msg.setIcon(QMessageBox::Information);
                QAbstractButton *pYesBtn = (QAbstractButton *)Msg.addButton(QString(tr("YES")), QMessageBox::YesRole);
                QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
                Msg.exec();
                if (Msg.clickedButton() == pYesBtn)QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest"));
                return 0;
            }
        case 1:
            {
                QMessageBox Msg(QMessageBox::Question, QString(tr("New version available!")), QString(tr("New version: %1 \n\nDo you wanna update now???")).arg(update_str));
                Msg.setIcon(QMessageBox::Information);
                QAbstractButton *pYesBtn_Github = (QAbstractButton *)Msg.addButton(QString("前往Github下载"), QMessageBox::YesRole);
                QAbstractButton *pYesBtn_Gitee = (QAbstractButton *)Msg.addButton(QString("前往码云Gitee下载(中国大陆境内)"), QMessageBox::YesRole);
                QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
                Msg.exec();
                if (Msg.clickedButton() == pYesBtn_Github)QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest"));
                if (Msg.clickedButton() == pYesBtn_Gitee)QDesktopServices::openUrl(QUrl("https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/releases"));
                return 0;
            }
    }
    return 0;
}

