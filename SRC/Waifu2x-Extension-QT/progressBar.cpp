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

void MainWindow::progressbar_setRange_min_max(int min, int max)
{
    ui->progressBar->setRange(min, max);
    TaskNumTotal=max;
}

void MainWindow::progressbar_Add()
{
    Progressbar_CurrentVal++;
    TaskNumFinished++;
    NewTaskFinished=true;
    ui->progressBar->setValue(Progressbar_CurrentVal);
}

void MainWindow::progressbar_clear()
{
    ui->progressBar->setRange(0, 1);
    ui->progressBar->setValue(0);
}

