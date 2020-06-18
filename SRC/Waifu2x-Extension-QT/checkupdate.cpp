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
python_ext_waifu2xEX.exe:
get video fps : [python_ext_waifu2xEX.exe videoFilePath fps]
get video frame number : [python_ext_waifu2xEX.exe videoFilePath countframe]
get gif duration : [python_ext_waifu2xEX.exe videoFilePath countframedigits]
check update :[python_ext_waifu2xEX.exe Current_Path checkupdate]
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
手动检查更新:直接打开latest release
*/
void MainWindow::on_pushButton_CheckUpdate_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest"));
    if(ui->comboBox_language->currentIndex()==1)
    {
        QDesktopServices::openUrl(QUrl("https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/releases"));
    }
}
/*
自动检查更新:
启动软件时以单独线程运行,调用python扩展检测更新,如有则弹窗
*/
int MainWindow::CheckUpadte_Auto()
{
    //============
    QString Latest_Ver="";
    QString Current_Ver="";
    QString checkupdate_cmd="";
    switch(ui->comboBox_UpdateChannel->currentIndex())
    {
        case 0:
            Current_Ver=LastStableVer;
            checkupdate_cmd="checkupdate";
            break;
        case 1:
            Current_Ver=VERSION;
            checkupdate_cmd="checkupdate_beta";
            break;
        default:
            break;
    }
    //============================
    QString Github_UpdateInfo = Current_Path+"/Update_Info_Github.ini";
    QString Gitee_UpdateInfo = Current_Path+"/Update_Info_Gitee.ini";
    QFile::remove(Github_UpdateInfo);
    QFile::remove(Gitee_UpdateInfo);
    //===========================
    QString program = Current_Path+"/python_ext_waifu2xEX.exe";
    QProcess checkupdate;
    checkupdate.start("\""+program+"\" \""+Current_Path+"\" "+checkupdate_cmd);
    while(!checkupdate.waitForStarted(500)&&!QProcess_stop) {}
    while(!checkupdate.waitForFinished(500)&&!QProcess_stop) {}
    //========= 先检查github的文件是否下载成功 =================
    if(QFile::exists(Github_UpdateInfo))
    {
        QSettings *configIniRead = new QSettings(Github_UpdateInfo, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        Latest_Ver = configIniRead->value("/Latest_Version/Ver").toString();
        QString Change_log = configIniRead->value("/Change_log/log").toString();
        Latest_Ver = Latest_Ver.trimmed();
        if(Latest_Ver!=Current_Ver&&Latest_Ver!="")
        {
            emit Send_CheckUpadte_NewUpdate(Latest_Ver,Change_log);
            //=====
            QFile::remove(Github_UpdateInfo);
            QFile::remove(Gitee_UpdateInfo);
            return 0;
        }
    }
    //========= 再检查gitee的文件是否下载成功 =================
    if(QFile::exists(Gitee_UpdateInfo))
    {
        QSettings *configIniRead = new QSettings(Gitee_UpdateInfo, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        Latest_Ver = configIniRead->value("/Latest_Version/Ver").toString();
        QString Change_log = configIniRead->value("/Change_log/log").toString();
        Latest_Ver = Latest_Ver.trimmed();
        if(Latest_Ver!=Current_Ver&&Latest_Ver!="")
        {
            emit Send_CheckUpadte_NewUpdate(Latest_Ver,Change_log);
            //=====
            QFile::remove(Github_UpdateInfo);
            QFile::remove(Gitee_UpdateInfo);
            return 0;
        }
    }
    if(Latest_Ver=="")
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to check for updates automatically! Please check your network or check for updates manually."));
    }
    QFile::remove(Github_UpdateInfo);
    QFile::remove(Gitee_UpdateInfo);
    return 0;
}
/*
自动更新弹窗
*/
int MainWindow::CheckUpadte_NewUpdate(QString update_str,QString Change_log)
{
    if(ui->checkBox_UpdatePopup->isChecked())
    {
        QMessageBox Msg(QMessageBox::Question, QString(tr("New version available!")), QString(tr("New version: %1\n\nChange log:\n%2\n\nDo you wanna update now???")).arg(update_str).arg(Change_log));
        Msg.setIcon(QMessageBox::Information);
        if(ui->comboBox_language->currentIndex()==1)
        {
            QAbstractButton *pYesBtn_Github = (QAbstractButton *)Msg.addButton(QString("前往Github下载"), QMessageBox::YesRole);
            QAbstractButton *pYesBtn_Gitee = (QAbstractButton *)Msg.addButton(QString("前往码云Gitee下载(中国大陆境内)"), QMessageBox::YesRole);
            QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
            Msg.exec();
            if (Msg.clickedButton() == pYesBtn_Github)QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/"+update_str.trimmed()));
            if (Msg.clickedButton() == pYesBtn_Gitee)QDesktopServices::openUrl(QUrl("https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/releases/"+update_str.trimmed()));
            return 0;
        }
        else
        {
            QAbstractButton *pYesBtn = (QAbstractButton *)Msg.addButton(QString(tr("YES")), QMessageBox::YesRole);
            QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
            Msg.exec();
            if (Msg.clickedButton() == pYesBtn)QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/"+update_str.trimmed()));
            return 0;
        }
    }
    else
    {
        QString update_msg_str = QString(tr("New version: %1 is available! Click [Check update] button to download the latest version!")).arg(update_str);
        emit Send_SystemTray_NewMessage(update_msg_str);
        emit Send_TextBrowser_NewMessage(update_msg_str);
    }
    return 0;
}

