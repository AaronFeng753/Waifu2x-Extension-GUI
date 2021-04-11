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
手动检查更新:直接打开release页面
*/
void MainWindow::on_pushButton_CheckUpdate_clicked()
{
    if(ui->comboBox_language->currentIndex()==1)
    {
        QDesktopServices::openUrl(QUrl("https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/releases"));
    }
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases"));
}
/*
自动检查更新:
启动软件时以单独线程运行,检测更新,如有则弹窗
*/
int MainWindow::CheckUpadte_Auto()
{
    bool isGiteeBanned = ui->checkBox_BanGitee->isChecked();
    //============
    QString Latest_Ver="";
    QString Current_Ver="";
    QString Github_UpdateInfo_online = "";
    QString Gitee_UpdateInfo_online = "";
    QString UpdateType=ui->comboBox_UpdateChannel->currentText();
    switch(ui->comboBox_UpdateChannel->currentIndex())
    {
        case 0:
            Current_Ver=LastStableVer;
            Github_UpdateInfo_online = "https://raw.githubusercontent.com/AaronFeng753/Waifu2x-Extension-GUI/master/.github/Update_Info.ini";
            Gitee_UpdateInfo_online = "https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/raw/master/.github/Update_Info.ini";
            break;
        case 1:
            Current_Ver=LastBetaVer;
            Github_UpdateInfo_online = "https://raw.githubusercontent.com/AaronFeng753/Waifu2x-Extension-GUI/master/.github/Update_Info_Beta.ini";
            Gitee_UpdateInfo_online = "https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/raw/master/.github/Update_Info_Beta.ini";
            break;
        default:
            break;
    }
    //============================
    QString Github_UpdateInfo_local = Current_Path+"/Update_Info_Github.ini";
    QString Gitee_UpdateInfo_local = Current_Path+"/Update_Info_Gitee.ini";
    //============= 从Github下载更新信息 ==============
    emit Send_TextBrowser_NewMessage(tr("Starting to download update information(for auto-check update) from Github."));
    DownloadTo(Github_UpdateInfo_online,Github_UpdateInfo_local);
    //========= 检查github的文件是否下载成功 =================
    if(QFile::exists(Github_UpdateInfo_local))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully downloaded update information from Github."));
        //==
        QSettings *configIniRead = new QSettings(Github_UpdateInfo_local, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //=====
        if(configIniRead->value("/Latest_Version/Ver") == QVariant() || configIniRead->value("/Change_log/log") == QVariant())
        {
            emit Send_TextBrowser_NewMessage(tr("Unable to check for updates! Please check your network or check for updates manually."));
            QFile::remove(Github_UpdateInfo_local);
            QFile::remove(Gitee_UpdateInfo_local);
            return 0;
        }
        //=====
        Latest_Ver = configIniRead->value("/Latest_Version/Ver").toString();
        QString Change_log = configIniRead->value("/Change_log/log").toString();
        Latest_Ver = Latest_Ver.trimmed();
        if(Latest_Ver!=Current_Ver&&Latest_Ver!="")
        {
            emit Send_CheckUpadte_NewUpdate(Latest_Ver,Change_log);
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("No update found, you are using the latest ")+UpdateType+tr(" version."));
        }
        QFile::remove(Github_UpdateInfo_local);
        QFile::remove(Gitee_UpdateInfo_local);
        return 0;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to download update information from Github."));
    }
    //============= 从码云下载更新信息 ==============
    if(isGiteeBanned==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Starting to download update information(for auto-check update) from Gitee."));
        DownloadTo(Gitee_UpdateInfo_online,Gitee_UpdateInfo_local);
        //========= 检查gitee的文件是否下载成功 =================
        if(QFile::exists(Gitee_UpdateInfo_local))
        {
            emit Send_TextBrowser_NewMessage(tr("Successfully downloaded update information from Gitee."));
            //==
            QSettings *configIniRead = new QSettings(Gitee_UpdateInfo_local, QSettings::IniFormat);
            configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
            //=====
            if(configIniRead->value("/Latest_Version/Ver") == QVariant() || configIniRead->value("/Change_log/log") == QVariant())
            {
                emit Send_TextBrowser_NewMessage(tr("Unable to check for updates! Please check your network or check for updates manually."));
                QFile::remove(Github_UpdateInfo_local);
                QFile::remove(Gitee_UpdateInfo_local);
                return 0;
            }
            //=====
            Latest_Ver = configIniRead->value("/Latest_Version/Ver").toString();
            QString Change_log = configIniRead->value("/Change_log/log").toString();
            Latest_Ver = Latest_Ver.trimmed();
            if(Latest_Ver!=Current_Ver&&Latest_Ver!="")
            {
                emit Send_CheckUpadte_NewUpdate(Latest_Ver,Change_log);
            }
            else
            {
                emit Send_TextBrowser_NewMessage(tr("No update found, you are using the latest ")+UpdateType+tr(" version."));
            }
            QFile::remove(Github_UpdateInfo_local);
            QFile::remove(Gitee_UpdateInfo_local);
            return 0;
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Unable to download update information from Gitee."));
        }
    }
    if(Latest_Ver=="")
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to check for updates! Please check your network or check for updates manually."));
    }
    QFile::remove(Github_UpdateInfo_local);
    QFile::remove(Gitee_UpdateInfo_local);
    return 0;
}
/*
自动更新弹窗
*/
int MainWindow::CheckUpadte_NewUpdate(QString update_str,QString Change_log)
{
    QString UpdateType=ui->comboBox_UpdateChannel->currentText();
    //======
    if(ui->checkBox_UpdatePopup->isChecked())
    {
        QMessageBox Msg(QMessageBox::Question, QString(tr("New ")+UpdateType+tr(" update available!")), QString(tr("New version: %1\n\nBrief change log:\n%2\n\nDo you wanna update now???")).arg(update_str).arg(Change_log));
        Msg.setIcon(QMessageBox::Information);
        if(ui->comboBox_language->currentIndex()==1)
        {
            QAbstractButton *pYesBtn_Github = Msg.addButton(QString("前往Github下载"), QMessageBox::YesRole);
            QAbstractButton *pYesBtn_Gitee = Msg.addButton(QString("前往码云下载"), QMessageBox::YesRole);
            Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
            Msg.exec();
            if (Msg.clickedButton() == pYesBtn_Github)QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/"+update_str.trimmed()));
            if (Msg.clickedButton() == pYesBtn_Gitee)QDesktopServices::openUrl(QUrl("https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/releases/"+update_str.trimmed()));
            return 0;
        }
        else
        {
            QAbstractButton *pYesBtn = Msg.addButton(QString(tr("YES")), QMessageBox::YesRole);
            Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
            Msg.exec();
            if (Msg.clickedButton() == pYesBtn)QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/"+update_str.trimmed()));
            return 0;
        }
    }
    else
    {
        QString update_msg_str = QString(tr("New ")+UpdateType+tr(" update: %1 is available! Click [Check update] button to download the latest version!")).arg(update_str);
        emit Send_SystemTray_NewMessage(update_msg_str);
        emit Send_TextBrowser_NewMessage(update_msg_str);
    }
    return 0;
}

