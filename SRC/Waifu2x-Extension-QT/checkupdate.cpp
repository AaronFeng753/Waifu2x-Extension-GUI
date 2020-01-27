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
python_ext.exe:
get video fps : [python_ext.exe videoFilePath fps]
get video frame number : [python_ext.exe videoFilePath countframe]
get gif duration : [python_ext.exe videoFilePath countframedigits]
check update :[python_ext.exe null checkupdate]
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::on_pushButton_CheckUpdate_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest"));
}

int MainWindow::CheckUpadte_Auto()
{
    Delay_sec_sleep(5);
    QString Current_Path = qApp->applicationDirPath();
    QString program = Current_Path+"/python_ext.exe";
    QProcess checkupdate;
    checkupdate.start("\""+program+"\" null checkupdate");
    checkupdate.waitForStarted();
    checkupdate.waitForFinished();
    QString update_str=checkupdate.readAllStandardOutput().data();
    update_str = update_str.trimmed();
    if(update_str!=VERSION&&update_str!="failed")
    {
        emit Send_CheckUpadte_NewUpdate(update_str);
    }
    return 0;
}

int MainWindow::CheckUpadte_NewUpdate(QString update_str)
{
    bool MsgBox = false;
    QMessageBox Msg(QMessageBox::Question, QString("Notification"), QString("New version: %1 \nDo you wanna update now???").arg(update_str));
    QAbstractButton *pYesBtn = (QAbstractButton *)Msg.addButton(QString("YES"), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString("NO"), QMessageBox::NoRole);
    Msg.exec();
    if (Msg.clickedButton() == pNoBtn)MsgBox = false;
    if(MsgBox)
    {
        QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest"));
    }
}

