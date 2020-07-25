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

#include "mainwindow.h"
#include "ui_mainwindow.h"
/*
online update for QRCode at donate tab
*/
int MainWindow::Donate_DownloadOnlineQRCode()
{
    bool isGiteeBanned = ui->checkBox_BanGitee->isChecked();
    //============================
    QString Github_OnlineQRCode_online = "https://raw.githubusercontent.com/AaronFeng753/Waifu2x-Extension-GUI/master/.github/Online_Donate_QRCode.jpg";
    QString Gitee_OnlineQRCode_online = "https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/raw/master/.github/Online_Donate_QRCode.jpg";
    //=
    QString Github_OnlineQRCode_local = Current_Path+"/Online_Donate_QRCode_Github.jpg";
    QString Gitee_OnlineQRCode_local = Current_Path+"/Online_Donate_QRCode_Gitee.jpg";
    //=
    QFile::remove(Github_OnlineQRCode_local);
    QFile::remove(Gitee_OnlineQRCode_local);
    //==================== 从Github下载文件 ========================
    emit Send_TextBrowser_NewMessage(tr("Starting to download QR Code image(for [Donate] tab) from Github."));
    DownloadTo(Github_OnlineQRCode_online,Github_OnlineQRCode_local);
    //========= 检查github的文件是否下载成功 =================
    QFileInfo *Github_OnlineQRCode_QFileInfo = new QFileInfo(Github_OnlineQRCode_local);
    if(QFile::exists(Github_OnlineQRCode_local)&&(Github_OnlineQRCode_QFileInfo->size()>100000))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully downloaded QR Code image from Github."));
        //==
        emit Send_Donate_ReplaceQRCode(Github_OnlineQRCode_local);
        return 1;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to download QR Code image from Github."));
    }
    //==================== 从码云下载文件 ========================
    if(isGiteeBanned==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Starting to download QR Code image(for [Donate] tab) from Gitee."));
        DownloadTo(Gitee_OnlineQRCode_online,Gitee_OnlineQRCode_local);
        //========= 检查gitee的文件是否下载成功 =================
        QFileInfo *Gitee_OnlineQRCode_QFileInfo = new QFileInfo(Gitee_OnlineQRCode_local);
        if(QFile::exists(Gitee_OnlineQRCode_local)&&(Gitee_OnlineQRCode_QFileInfo->size()>100000))
        {
            emit Send_TextBrowser_NewMessage(tr("Successfully downloaded QR Code image from Gitee."));
            //==
            emit Send_Donate_ReplaceQRCode(Gitee_OnlineQRCode_local);
            return 1;
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Unable to download QR Code image from Gitee."));
        }
    }
    emit Send_Donate_ReplaceQRCode("");//下载失败,直接跳转
    return 0;
}

void MainWindow::Donate_ReplaceQRCode(QString QRCodePath)
{
    if(QRCodePath=="")
    {
        Donate_Count();
        return;
    }
    else
    {
        ui->label_DonateQRCode->setStyleSheet("image: url("+QRCodePath+");");
        Donate_Count();
        return;
    }
}

int MainWindow::Donate_Count()
{
    QString donate_ini = Current_Path+"/StartupStatistics_Waifu2xEX";
    if(!QFile::exists(donate_ini))
    {
        QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
        configIniWrite->setValue("/Description/.", "This file will store how many times you start this software, it's not for statistic, just for software to function normally, and won't be upload to the internet in any cases.");
        configIniWrite->setValue("/Donate/VERSION", VERSION);
        configIniWrite->setValue("/Donate/OpenCount_Current", 4);
        return 0;
    }
    else
    {
        QSettings *configIniRead_ver = new QSettings(donate_ini, QSettings::IniFormat);
        configIniRead_ver->setIniCodec(QTextCodec::codecForName("UTF-8"));
        QString DonateINI_VERSION = configIniRead_ver->value("/Donate/VERSION").toString();
        if(DonateINI_VERSION!=VERSION)
        {
            QFile::remove(donate_ini);
            QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
            configIniWrite->setValue("/Description/.", "This file will store how many times you start this software, it's not for statistic, just for software to function normally, and won't be upload to the internet in any cases.");
            configIniWrite->setValue("/Donate/VERSION", VERSION);
            configIniWrite->setValue("/Donate/OpenCount_Current", 4);
            return 0;
        }
    }
    QSettings *configIniRead = new QSettings(donate_ini, QSettings::IniFormat);
    //=======  读取打开次数  ======
    int OpenCount_Current = configIniRead->value("/Donate/OpenCount_Current").toInt();
    int OpenCount_Max = QRandomGenerator::global()->bounded(3,6);
    //===
    if(OpenCount_Current<=0)OpenCount_Current=OpenCount_Max;
    //===
    int DonateTestNum = QRandomGenerator::global()->bounded(1,10000);
    QSettings *configIniWrite_Test = new QSettings(donate_ini, QSettings::IniFormat);
    configIniWrite_Test->setValue("/Donate/Test", DonateTestNum);
    if(configIniRead->value("/Donate/Test").toInt()!=DonateTestNum)
    {
        OpenCount_Current=OpenCount_Max;
    }
    //===
    OpenCount_Current++;
    if(OpenCount_Current<OpenCount_Max)
    {
        QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
        configIniWrite->setValue("/Donate/OpenCount_Current", OpenCount_Current);
        return 0;
    }
    else
    {
        QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
        configIniWrite->setValue("/Donate/OpenCount_Current", 1);
        ui->tabWidget->setCurrentIndex(0);
        emit Send_SystemTray_NewMessage(tr("Please donate to support developers, so we can bring further update for this software, thank you! (｡･∀･)ﾉﾞ"));
        return 0;
    }
}
