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

