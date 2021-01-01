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
设定进度条最小值与最大值
*/
void MainWindow::progressbar_setRange_min_max(int min, int max)
{
    ui->progressBar->setRange(min, max);
    TaskNumTotal=max;
    ui->label_progressBar_filenum->setText("0/"+QString::number(max,10));
}
/*
将进度条直接充满
*/
void MainWindow::progressbar_SetToMax(int maxval)
{
    ui->progressBar->setValue(maxval);
    ui->label_progressBar_filenum->setText(QString::number(maxval,10)+"/"+QString::number(maxval,10));
}
/*
进度条 进度+1
*/
void MainWindow::progressbar_Add()
{
    Progressbar_CurrentVal++;
    TaskNumFinished++;
    NewTaskFinished=true;
    ui->progressBar->setValue(Progressbar_CurrentVal);
    ui->label_progressBar_filenum->setText(QString::number(Progressbar_CurrentVal,10)+"/"+QString::number(Progressbar_MaxVal,10));
}
/*
清空进度条
*/
void MainWindow::progressbar_clear()
{
    ui->progressBar->setRange(0, 1);
    ui->progressBar->setValue(0);
    ui->label_progressBar_filenum->setText("0/0");
}

