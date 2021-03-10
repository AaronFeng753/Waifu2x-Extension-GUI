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

void MainWindow::Init_ActionsMenu_checkBox_DelOriginal()
{
    QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setText(tr("Move to Recycle Bin."));
    QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setCheckable(1);
    //===
    ui->checkBox_DelOriginal->addAction(QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal);
}

void MainWindow::Init_ActionsMenu_checkBox_ReplaceOriginalFile()
{
    QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile->setText(tr("Move to Recycle Bin."));
    QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile->setCheckable(1);
    //===
    ui->checkBox_ReplaceOriginalFile->addAction(QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile);
}

void MainWindow::Init_ActionsMenu_pushButton_RemoveItem()
{
    RemoveALL_image->setText(tr("Remove all image files."));
    RemoveALL_gif->setText(tr("Remove all gif files."));
    RemoveALL_video->setText(tr("Remove all video files."));
    //==============
    RemoveALL_image->setIcon(QIcon(":/new/prefix1/icon/picture_Rmenu.png"));
    RemoveALL_gif->setIcon(QIcon(":/new/prefix1/icon/gif_Rmenu.png"));
    RemoveALL_video->setIcon(QIcon(":/new/prefix1/icon/video_Rmenu.png"));
    //==============
    connect(RemoveALL_image, SIGNAL(triggered()), this, SLOT(RemoveALL_image_slot()),Qt::UniqueConnection);
    connect(RemoveALL_gif, SIGNAL(triggered()), this, SLOT(RemoveALL_gif_slot()),Qt::UniqueConnection);
    connect(RemoveALL_video, SIGNAL(triggered()), this, SLOT(RemoveALL_video_slot()),Qt::UniqueConnection);
    //==============
    ui->pushButton_RemoveItem->addAction(RemoveALL_image);
    ui->pushButton_RemoveItem->addAction(RemoveALL_gif);
    ui->pushButton_RemoveItem->addAction(RemoveALL_video);
}

void MainWindow::RemoveALL_image_slot()
{
    ui->pushButton_RemoveItem->setEnabled(0);
    int rowCount_image = Table_model_image->rowCount();
    if(rowCount_image>0)
    {
        for( int i = 0; i < rowCount_image; i++ )
        {
            CustRes_remove(Table_model_image->item(i,2)->text());
        }
        Table_model_image->clear();
        Init_Table();
        curRow_image = -1;
        ui->tableView_image->clearSelection();
        ui->tableView_image->setVisible(0);
    }
    Table_FileCount_reload();
    if(Table_model_gif->rowCount()==0&&Table_model_image->rowCount()==0&&Table_model_video->rowCount()==0)
    {
        on_pushButton_ClearList_clicked();
    }
}

void MainWindow::RemoveALL_gif_slot()
{
    ui->pushButton_RemoveItem->setEnabled(0);
    int rowCount_gif = Table_model_gif->rowCount();
    if(rowCount_gif>0)
    {
        for( int i = 0; i < rowCount_gif; i++ )
        {
            CustRes_remove(Table_model_gif->item(i,2)->text());
        }
        Table_model_gif->clear();
        Init_Table();
        curRow_gif = -1;
        ui->tableView_gif->clearSelection();
        ui->tableView_gif->setVisible(0);
    }
    Table_FileCount_reload();
    if(Table_model_gif->rowCount()==0&&Table_model_image->rowCount()==0&&Table_model_video->rowCount()==0)
    {
        on_pushButton_ClearList_clicked();
    }
}

void MainWindow::RemoveALL_video_slot()
{
    ui->pushButton_RemoveItem->setEnabled(0);
    int rowCount_video = Table_model_video->rowCount();
    if(rowCount_video>0)
    {
        for( int i = 0; i < rowCount_video; i++ )
        {
            CustRes_remove(Table_model_video->item(i,2)->text());
        }
        Table_model_video->clear();
        Init_Table();
        curRow_video = -1;
        ui->tableView_video->clearSelection();
        ui->tableView_video->setVisible(0);
    }
    Table_FileCount_reload();
    if(Table_model_gif->rowCount()==0&&Table_model_image->rowCount()==0&&Table_model_video->rowCount()==0)
    {
        on_pushButton_ClearList_clicked();
    }
}

void MainWindow::Init_ActionsMenu_lineEdit_outputPath()
{
    OpenFolder_lineEdit_outputPath->setText(tr("Open output path"));
    OpenFolder_lineEdit_outputPath->setIcon(QIcon(":/new/prefix1/icon/opne_folder.png"));
    connect(OpenFolder_lineEdit_outputPath, SIGNAL(triggered()), this, SLOT(OpenOutputFolder()),Qt::UniqueConnection);
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
    OpenFile_QAction_FileList->setText(tr("Open file"));
    OpenFile_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/view_file.png"));
    connect(OpenFile_QAction_FileList, SIGNAL(triggered()), this, SLOT(OpenSelectedFile_FilesList()),Qt::UniqueConnection);
    ui->tableView_image->addAction(OpenFile_QAction_FileList);
    ui->tableView_gif->addAction(OpenFile_QAction_FileList);
    ui->tableView_video->addAction(OpenFile_QAction_FileList);
    //===
    OpenFilesFolder_QAction_FileList->setText(tr("Open folder"));
    OpenFilesFolder_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/opne_folder.png"));
    connect(OpenFilesFolder_QAction_FileList, SIGNAL(triggered()), this, SLOT(OpenSelectedFilesFolder_FilesList()),Qt::UniqueConnection);
    ui->tableView_image->addAction(OpenFilesFolder_QAction_FileList);
    ui->tableView_gif->addAction(OpenFilesFolder_QAction_FileList);
    ui->tableView_video->addAction(OpenFilesFolder_QAction_FileList);
    //===
    RemoveFile_FilesList_QAction_FileList->setText(tr("Remove from list [Delete]"));
    RemoveFile_FilesList_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/RemoveFile.png"));
    connect(RemoveFile_FilesList_QAction_FileList, SIGNAL(triggered()), this, SLOT(on_pushButton_RemoveItem_clicked()),Qt::UniqueConnection);
    ui->tableView_image->addAction(RemoveFile_FilesList_QAction_FileList);
    ui->tableView_gif->addAction(RemoveFile_FilesList_QAction_FileList);
    ui->tableView_video->addAction(RemoveFile_FilesList_QAction_FileList);
    //===
    Apply_CustRes_QAction_FileList->setText(tr("Apply custom resolution [Ctrl+A]"));
    Apply_CustRes_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/Apply.png"));
    connect(Apply_CustRes_QAction_FileList, SIGNAL(triggered()), this, SLOT(Apply_CustRes_QAction_FileList_slot()),Qt::UniqueConnection);
    ui->tableView_image->addAction(Apply_CustRes_QAction_FileList);
    ui->tableView_gif->addAction(Apply_CustRes_QAction_FileList);
    ui->tableView_video->addAction(Apply_CustRes_QAction_FileList);
    //===
    Cancel_CustRes_QAction_FileList->setText(tr("Cancel custom resolution [Ctrl+C]"));
    Cancel_CustRes_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/cancel.png"));
    connect(Cancel_CustRes_QAction_FileList, SIGNAL(triggered()), this, SLOT(Cancel_CustRes_QAction_FileList_slot()),Qt::UniqueConnection);
    ui->tableView_image->addAction(Cancel_CustRes_QAction_FileList);
    ui->tableView_gif->addAction(Cancel_CustRes_QAction_FileList);
    ui->tableView_video->addAction(Cancel_CustRes_QAction_FileList);
}
void MainWindow::Apply_CustRes_QAction_FileList_slot()
{
    EnableApply2All_CustRes=false;
    on_pushButton_CustRes_apply_clicked();
    EnableApply2All_CustRes=true;
}
void MainWindow::Cancel_CustRes_QAction_FileList_slot()
{
    EnableApply2All_CustRes=false;
    on_pushButton_CustRes_cancel_clicked();
    EnableApply2All_CustRes=true;
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
