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
online update for QR Code at donate tab
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
    //==================== 从Github下载文件 ========================
    emit Send_TextBrowser_NewMessage(tr("Starting to download QR Code image(for [Donate] tab) from Github."));
    DownloadTo(Github_OnlineQRCode_online,Github_OnlineQRCode_local);
    //========= 检查github的文件是否下载成功 =================
    QFileInfo *Github_OnlineQRCode_QFileInfo = new QFileInfo(Github_OnlineQRCode_local);
    if(QFile::exists(Github_OnlineQRCode_local)&&(Github_OnlineQRCode_QFileInfo->size()>100000))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully downloaded QR Code image from Github."));
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
    if(QRCodePath!="")
    {
        ui->label_DonateQRCode->setStyleSheet("image: url(\""+QRCodePath+"\");");
    }
    if(isFirstTimeStart)return;
    int randomNum = QRandomGenerator::global()->bounded(0, 100);
    if(randomNum<40)
    {
        ui->tabWidget->setCurrentIndex(0);
        emit Send_SystemTray_NewMessage(tr("Please donate to support developers, so we can bring further update for this software, thank you! (｡･∀･)ﾉﾞ"));
    }
    return;
}
