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

void MainWindow::Init_ActionsMenu_lineEdit_outputPath()
{
    OpenFolder_lineEdit_outputPath->setText(tr("Open output path"));
    connect(OpenFolder_lineEdit_outputPath, SIGNAL(triggered()), this, SLOT(OpenOutputFolder()));
    ui->lineEdit_outputPath->addAction(OpenFolder_lineEdit_outputPath);
}

void MainWindow::OpenOutputFolder()
{
    QString OutPutPath=ui->lineEdit_outputPath->text();
    if(file_OpenFolder(OutPutPath)==false)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Error"));
        MSG->setText(tr("Output path doesn\'t exists!"));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
    }
}

void MainWindow::Init_ActionsMenu_FilesList()
{
    OpenFile->setText(tr("Open selected file"));
    connect(OpenFile, SIGNAL(triggered()), this, SLOT(OpenSelectedFile_FilesList()));
    ui->tableView_image->addAction(OpenFile);
    ui->tableView_gif->addAction(OpenFile);
    ui->tableView_video->addAction(OpenFile);
    //===
    OpenFilesFolder->setText(tr("Open the folder of the selected file"));
    connect(OpenFilesFolder, SIGNAL(triggered()), this, SLOT(OpenSelectedFilesFolder_FilesList()));
    ui->tableView_image->addAction(OpenFilesFolder);
    ui->tableView_gif->addAction(OpenFilesFolder);
    ui->tableView_video->addAction(OpenFilesFolder);
    //===
    RemoveFile_FilesList->setText(tr("Remove selected file from list"));
    connect(RemoveFile_FilesList, SIGNAL(triggered()), this, SLOT(on_pushButton_RemoveItem_clicked()));
    ui->tableView_image->addAction(RemoveFile_FilesList);
    ui->tableView_gif->addAction(RemoveFile_FilesList);
    ui->tableView_video->addAction(RemoveFile_FilesList);
}

void MainWindow::OpenSelectedFile_FilesList()
{
    if(curRow_image==-1&&curRow_video==-1&&curRow_gif==-1)
    {
        ui->tableView_image->clearSelection();
        ui->tableView_gif->clearSelection();
        ui->tableView_video->clearSelection();
        //=====
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("No items are currently selected."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
        //====
        return;
    }
    //==========================
    if(curRow_image >= 0)
    {
        if(file_OpenFile(Table_model_image->item(curRow_image,2)->text())==false)
        {
            OpenSelectedFile_FailedWarning_FilesList();
        }
        return;
    }
    //============================================================
    if(curRow_video >= 0)
    {
        if(file_OpenFile(Table_model_video->item(curRow_video,2)->text())==false)
        {
            OpenSelectedFile_FailedWarning_FilesList();
        }
        return;
    }
    //============================================================
    if(curRow_gif >= 0)
    {
        if(file_OpenFile(Table_model_gif->item(curRow_gif,2)->text())==false)
        {
            OpenSelectedFile_FailedWarning_FilesList();
        }
        return;
    }
}

void MainWindow::OpenSelectedFilesFolder_FilesList()
{
    if(curRow_image==-1&&curRow_video==-1&&curRow_gif==-1)
    {
        ui->tableView_image->clearSelection();
        ui->tableView_gif->clearSelection();
        ui->tableView_video->clearSelection();
        //=====
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("No items are currently selected."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
        //====
        return;
    }
    //==========================
    if(curRow_image >= 0)
    {
        if(file_OpenFilesFolder(Table_model_image->item(curRow_image,2)->text())==false)
        {
            OpenSelectedFile_FailedWarning_FilesList();
        }
        return;
    }
    //============================================================
    if(curRow_video >= 0)
    {
        if(file_OpenFilesFolder(Table_model_video->item(curRow_video,2)->text())==false)
        {
            OpenSelectedFile_FailedWarning_FilesList();
        }
        return;
    }
    //============================================================
    if(curRow_gif >= 0)
    {
        if(file_OpenFilesFolder(Table_model_gif->item(curRow_gif,2)->text())==false)
        {
            OpenSelectedFile_FailedWarning_FilesList();
        }
        return;
    }
}

void MainWindow::OpenSelectedFile_FailedWarning_FilesList()
{
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Error"));
    MSG->setText(tr("Target file(folder) doesn\'t exists!"));
    MSG->setIcon(QMessageBox::Warning);
    MSG->setModal(false);
    MSG->show();
}
