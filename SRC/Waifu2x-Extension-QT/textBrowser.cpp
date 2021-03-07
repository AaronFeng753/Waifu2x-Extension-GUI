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
在文本框内输出一条新消息
*/
void MainWindow::TextBrowser_NewMessage(QString message)
{
    QString Current_Time = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]");
    QString OutPutStr = Current_Time+" "+message;
    ui->textBrowser->append(OutPutStr);
    ui->textBrowser->moveCursor(QTextCursor::End);
}
/*
输出文本框的欢迎消息
启动时显示
*/
void MainWindow::TextBrowser_StartMes()
{
    QString CurrentVerState="";
    if(isBetaVer)
    {
        CurrentVerState=tr("[Beta]");
    }
    else
    {
        CurrentVerState=tr("[Stable]");
    }
    //====
    ui->textBrowser->append("Waifu2x-Extension-GUI by Aaron Feng");
    ui->textBrowser->append(tr("Version:")+" "+VERSION+" "+CurrentVerState);
    ui->textBrowser->append("Github: https://github.com/AaronFeng753/Waifu2x-Extension-GUI");
    if(ui->comboBox_language->currentIndex()==1)ui->textBrowser->append("码云: https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI");
    ui->textBrowser->append(tr("Please donate to support the developers, so we can bring further updates for this software, thank you! (｡･∀･)ﾉﾞ"));
    ui->textBrowser->moveCursor(QTextCursor::End);
}
