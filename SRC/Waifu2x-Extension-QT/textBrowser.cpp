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

void MainWindow::TextBrowser_NewMessage(QString message)
{
    QDateTime time = QDateTime::currentDateTime();
    QString Current_Time = time.toString("[yyyy-MM-dd hh:mm:ss]");
    QString OutPutStr = Current_Time+" "+message;
    ui->textBrowser->append(OutPutStr);
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::TextBrowser_StartMes()
{
    ui->textBrowser->append("Waifu2x-Extension-GUI by Aaron Feng");
    ui->textBrowser->append(VERSION);
    ui->textBrowser->append("Github: https://github.com/AaronFeng753/Waifu2x-Extension-GUI");
    ui->textBrowser->moveCursor(QTextCursor::End);
}
