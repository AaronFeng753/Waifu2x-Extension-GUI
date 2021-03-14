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
初始化tableview
*/
void MainWindow::Init_Table()
{
    Table_model_image->setColumnCount(4);
    Table_model_image->setHeaderData(0, Qt::Horizontal, tr("Image File Name"));
    Table_model_image->setHeaderData(1, Qt::Horizontal, tr("Status"));
    Table_model_image->setHeaderData(2, Qt::Horizontal, tr("Full Path"));
    Table_model_image->setHeaderData(3, Qt::Horizontal, tr("Custom resolution(Width x Height)"));
    ui->tableView_image->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_image->setModel(Table_model_image);
    //=================================
    Table_model_gif->setColumnCount(4);
    Table_model_gif->setHeaderData(0, Qt::Horizontal, tr("GIF File Name"));
    Table_model_gif->setHeaderData(1, Qt::Horizontal, tr("Status"));
    Table_model_gif->setHeaderData(2, Qt::Horizontal, tr("Full Path"));
    Table_model_gif->setHeaderData(3, Qt::Horizontal, tr("Custom resolution(Width x Height)"));
    ui->tableView_gif->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_gif->setModel(Table_model_gif);
    //=================================
    Table_model_video->setColumnCount(4);
    Table_model_video->setHeaderData(0, Qt::Horizontal, tr("Video File Name"));
    Table_model_video->setHeaderData(1, Qt::Horizontal, tr("Status"));
    Table_model_video->setHeaderData(2, Qt::Horizontal, tr("Full Path"));
    Table_model_video->setHeaderData(3, Qt::Horizontal, tr("Custom resolution(Width x Height)"));
    ui->tableView_video->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_video->setModel(Table_model_video);
    //将横向表头设置为可见
    ui->tableView_video->horizontalHeader()->setVisible(1);
    ui->tableView_gif->horizontalHeader()->setVisible(1);
    ui->tableView_image->horizontalHeader()->setVisible(1);
    //=============================================
    emit Send_Table_EnableSorting(1);//启用文件列表的排序功能
}
/*
重载Tableview下的文件数量统计
*/
int MainWindow::Table_FileCount_reload()
{
    long int filecount_image=Table_model_image->rowCount();
    long int filecount_gif=Table_model_gif->rowCount();
    long int filecount_video=Table_model_video->rowCount();
    long int filecount_total=filecount_image+filecount_gif+filecount_video;
    //====================
    // 列表内有文件
    //====================
    if(filecount_total>0)
    {
        //===========
        //显示文件数量
        //===========
        ui->label_FileCount->setVisible(1);
        ui->label_FileCount->setText(QString(tr("File count: %1")).arg(filecount_total));
        ui->label_FileCount->setToolTip(QString(tr("Image: %1\nGIF: %2\nVideo: %3")).arg(filecount_image).arg(filecount_gif).arg(filecount_video));
        //=================
        //启用控制按钮
        //=================
        ui->pushButton_ClearList->setEnabled(1);
        ui->pushButton_RemoveItem->setEnabled(1);
        //===============================================
        //根据每个列表内文件数量逐个判断是否显示这个列表
        //===============================================
        int TableView_VisibleCount = 0;
        ui->label_DropFile->setVisible(0);//隐藏文件投放label
        if(filecount_image>0)
        {
            ui->tableView_image->setVisible(1);
            TableView_VisibleCount++;
        }
        else
        {
            curRow_image = -1;
            ui->tableView_image->clearSelection();
            ui->tableView_image->setVisible(0);
        }
        //===
        if(filecount_gif>0)
        {
            ui->tableView_gif->setVisible(1);
            TableView_VisibleCount++;
        }
        else
        {
            curRow_gif = -1;
            ui->tableView_gif->clearSelection();
            ui->tableView_gif->setVisible(0);
        }
        //===
        if(filecount_video>0)
        {
            ui->tableView_video->setVisible(1);
            TableView_VisibleCount++;
        }
        else
        {
            curRow_video = -1;
            ui->tableView_video->clearSelection();
            ui->tableView_video->setVisible(0);
        }
        //========================
        ui->pushButton_ResizeFilesListSplitter->setEnabled(TableView_VisibleCount>1);//当可见的文件列表数量大于1时启用重置文件列表比例按钮
        //========================
        ui->pushButton_SaveFileList->setEnabled(1);//列表内有文件时自动启用保存文件列表的按钮
    }
    //====================
    // 列表内无文件
    //====================
    else
    {
        ui->label_FileCount->setVisible(0);//隐藏文件数量label
        //===================
        // 禁用文件列表控制按钮
        //===================
        ui->pushButton_ClearList->setEnabled(0);
        ui->pushButton_RemoveItem->setEnabled(0);
        ui->pushButton_ResizeFilesListSplitter->setEnabled(0);
        //====================
        //隐藏文件列表,取消选中
        //====================
        ui->label_DropFile->setVisible(1);
        curRow_image = -1;
        ui->tableView_image->clearSelection();
        ui->tableView_image->setVisible(0);
        curRow_gif = -1;
        ui->tableView_gif->clearSelection();
        ui->tableView_gif->setVisible(0);
        curRow_video = -1;
        ui->tableView_video->clearSelection();
        ui->tableView_video->setVisible(0);
        //========================
        ui->pushButton_SaveFileList->setEnabled(0);//列表内没有文件时自动禁用保存文件列表的按钮
    }
    return 0;
}
//============================
// 插入文件
//============================
void MainWindow::Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath)
{
    mutex_Table_insert.lock();
    //=========
    int rowNum = Table_model_image->rowCount();
    Table_model_image->setItem(rowNum, 0, new QStandardItem(fileName));
    Table_model_image->setItem(rowNum, 1, new QStandardItem(""));
    Table_model_image->setItem(rowNum, 2, new QStandardItem(SourceFile_fullPath));
    Table_model_image->setItem(rowNum, 3, new QStandardItem(""));
    mutex_Table_insert_finished.lock();
    Table_insert_finished=true;
    mutex_Table_insert_finished.unlock();
    //========
    mutex_Table_insert.unlock();
}

void MainWindow::Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath)
{
    mutex_Table_insert.lock();
    //=========
    int rowNum = Table_model_gif->rowCount();
    Table_model_gif->setItem(rowNum, 0, new QStandardItem(fileName));
    Table_model_gif->setItem(rowNum, 1, new QStandardItem(""));
    Table_model_gif->setItem(rowNum, 2, new QStandardItem(SourceFile_fullPath));
    Table_model_gif->setItem(rowNum, 3, new QStandardItem(""));
    mutex_Table_insert_finished.lock();
    Table_insert_finished=true;
    mutex_Table_insert_finished.unlock();
    //========
    mutex_Table_insert.unlock();
}

void MainWindow::Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath)
{
    mutex_Table_insert.lock();
    //=========
    int rowNum = Table_model_video->rowCount();
    Table_model_video->setItem(rowNum, 0, new QStandardItem(fileName));
    Table_model_video->setItem(rowNum, 1, new QStandardItem(""));
    Table_model_video->setItem(rowNum, 2, new QStandardItem(SourceFile_fullPath));
    Table_model_video->setItem(rowNum, 3, new QStandardItem(""));
    mutex_Table_insert_finished.lock();
    Table_insert_finished=true;
    mutex_Table_insert_finished.unlock();
    //========
    mutex_Table_insert.unlock();
}

void MainWindow::Table_image_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status)
{
    ui->tableView_image->setUpdatesEnabled(false);
    Table_model_image->setItem(rowNum, 1, new QStandardItem(status));
    if(ui->checkBox_FileListAutoSlide->isChecked())
    {
        QAbstractItemModel *modessl = Table_model_image;
        QModelIndex indextemp = modessl->index(rowNum, 1);
        ui->tableView_image->scrollTo(indextemp,QAbstractItemView::PositionAtCenter);
    }
    ui->tableView_image->setUpdatesEnabled(true);
}

void MainWindow::Table_gif_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status)
{
    ui->tableView_gif->setUpdatesEnabled(false);
    Table_model_gif->setItem(rowNum, 1, new QStandardItem(status));
    if(ui->checkBox_FileListAutoSlide->isChecked())
    {
        QAbstractItemModel *modessl = Table_model_gif;
        QModelIndex indextemp = modessl->index(rowNum, 1);
        ui->tableView_gif->scrollTo(indextemp,QAbstractItemView::PositionAtCenter);
    }
    ui->tableView_gif->setUpdatesEnabled(true);
}

void MainWindow::Table_video_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status)
{
    ui->tableView_video->setUpdatesEnabled(false);
    Table_model_video->setItem(rowNum, 1, new QStandardItem(status));
    if(ui->checkBox_FileListAutoSlide->isChecked())
    {
        QAbstractItemModel *modessl = Table_model_video;
        QModelIndex indextemp = modessl->index(rowNum, 1);
        ui->tableView_video->scrollTo(indextemp,QAbstractItemView::PositionAtCenter);
    }
    ui->tableView_video->setUpdatesEnabled(true);
}

void MainWindow::Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width)
{
    QString ResStr = width+"x"+height;
    ui->tableView_image->setUpdatesEnabled(false);
    Table_model_image->setItem(rowNum, 3, new QStandardItem(ResStr));
    ui->tableView_image->setUpdatesEnabled(true);
}

void MainWindow::Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width)
{
    QString ResStr = width+"x"+height;
    ui->tableView_gif->setUpdatesEnabled(false);
    Table_model_gif->setItem(rowNum, 3, new QStandardItem(ResStr));
    ui->tableView_gif->setUpdatesEnabled(true);
}

void MainWindow::Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width)
{
    QString ResStr = width+"x"+height;
    ui->tableView_video->setUpdatesEnabled(false);
    Table_model_video->setItem(rowNum, 3, new QStandardItem(ResStr));
    ui->tableView_video->setUpdatesEnabled(true);
}

void MainWindow::Table_image_CustRes_Cancel_rowNumInt(int rowNum)
{
    ui->tableView_image->setUpdatesEnabled(false);
    Table_model_image->setItem(rowNum, 3, new QStandardItem(""));
    ui->tableView_image->setUpdatesEnabled(true);
}

void MainWindow::Table_gif_CustRes_Cancel_rowNumInt(int rowNum)
{
    ui->tableView_gif->setUpdatesEnabled(false);
    Table_model_gif->setItem(rowNum, 3, new QStandardItem(""));
    ui->tableView_gif->setUpdatesEnabled(true);
}

void MainWindow::Table_video_CustRes_Cancel_rowNumInt(int rowNum)
{
    ui->tableView_video->setUpdatesEnabled(false);
    Table_model_video->setItem(rowNum, 3, new QStandardItem(""));
    ui->tableView_video->setUpdatesEnabled(true);
}


void MainWindow::Table_ChangeAllStatusToWaiting()
{
    QMutex_Table_ChangeAllStatusToWaiting.lock();
    ui_tableViews_setUpdatesEnabled(false);
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->isChecked();
    int rowNum = Table_model_image->rowCount();
    for (int i = 0; i < rowNum; i++)
    {
        QAbstractItemModel *modessl = Table_model_image;
        QModelIndex indextemp = modessl->index(i, 1);
        QVariant datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        if(ReProcFinFiles)
        {
            if(status != "Finished, original file deleted")
            {
                Table_model_image->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
        else
        {
            if(status != "Finished, original file deleted" && status != "Finished")
            {
                Table_model_image->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
    }
    rowNum = Table_model_gif->rowCount();
    for (int i = 0; i < rowNum; i++)
    {
        QAbstractItemModel *modessl = Table_model_gif;
        QModelIndex indextemp = modessl->index(i, 1);
        QVariant datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        if(ReProcFinFiles)
        {
            if(status != "Finished, original file deleted")
            {
                Table_model_gif->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
        else
        {
            if(status != "Finished, original file deleted" && status != "Finished")
            {
                Table_model_gif->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
    }
    rowNum = Table_model_video->rowCount();
    for (int i = 0; i < rowNum; i++)
    {
        QAbstractItemModel *modessl = Table_model_video;
        QModelIndex indextemp = modessl->index(i, 1);
        QVariant datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        if(ReProcFinFiles)
        {
            if(status != "Finished, original file deleted")
            {
                Table_model_video->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
        else
        {
            if(status != "Finished, original file deleted" && status != "Finished")
            {
                Table_model_video->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
    }
    ui_tableViews_setUpdatesEnabled(true);
    QMutex_Table_ChangeAllStatusToWaiting.unlock();
}

void MainWindow::Table_Clear()
{
    ui_tableViews_setUpdatesEnabled(false);
    //=====
    Table_model_image->clear();
    Table_model_gif->clear();
    Table_model_video->clear();
    Init_Table();
    curRow_image = -1;
    curRow_gif = -1;
    curRow_video = -1;
    //=====
    ui_tableViews_setUpdatesEnabled(true);
}

int MainWindow::Table_image_get_rowNum()
{
    int rowNum = Table_model_image->rowCount();
    return rowNum;
}

int MainWindow::Table_gif_get_rowNum()
{
    int rowNum = Table_model_gif->rowCount();
    return rowNum;
}

int MainWindow::Table_video_get_rowNum()
{
    int rowNum = Table_model_video->rowCount();
    return rowNum;
}
/*
从table读取状态和完整路径
[fullpath]=status;
*/
QMap<QString, QString> MainWindow::Table_Read_status_fullpath(QStandardItemModel *Table_model)
{
    int rowNum = Table_model->rowCount();
    QMap<QString, QString> Map_fullPath_status;
    for (int i = 0; i < rowNum; i++)
    {
        QAbstractItemModel *modessl = Table_model;
        QModelIndex indextemp_status = modessl->index(i, 1);
        QVariant datatemp_status = modessl->data(indextemp_status);
        QString status = datatemp_status.toString();
        QModelIndex indextemp_fullpath = modessl->index(i, 2);
        QVariant datatemp_fullpath = modessl->data(indextemp_fullpath);
        QString fullpath = datatemp_fullpath.toString();
        Map_fullPath_status[fullpath]=status;
    }
    return Map_fullPath_status;
}

//保存当前文件列表
int MainWindow::Table_Save_Current_Table_Filelist(QString Table_FileList_ini)
{
    QFile::remove(Table_FileList_ini);
    //=================
    QSettings *configIniWrite = new QSettings(Table_FileList_ini, QSettings::IniFormat);
    //================= 添加警告 =========================
    configIniWrite->setValue("/Warning/EN", "Do not modify this file! It may cause the program to crash! If problems occur after the modification, delete this file and restart the program.");
    //================= 存储table_image =========================
    configIniWrite->setValue("/table_image/rowCount", Table_model_image->rowCount());
    for(int i=0; i<Table_model_image->rowCount(); i++)
    {
        QAbstractItemModel *modessl = Table_model_image;
        //===
        QModelIndex indextemp = modessl->index(i, 0);
        QVariant datatemp = modessl->data(indextemp);
        QString FileName = datatemp.toString();
        configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_FileName", FileName);
        //===
        indextemp = modessl->index(i, 1);
        datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_status", status);
        //===
        indextemp = modessl->index(i, 2);
        datatemp = modessl->data(indextemp);
        QString fullPath = datatemp.toString();
        configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_fullPath", fullPath);
        //===
        indextemp = modessl->index(i, 3);
        datatemp = modessl->data(indextemp);
        QString CustRes_str = datatemp.toString();
        configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_CustRes_str", CustRes_str);
        if(CustRes_str!="")
        {
            QMap<QString, QString> ResMap = CustRes_getResMap(fullPath);
            if(!ResMap.isEmpty())
            {
                configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_CustRes_height", ResMap["height"]);
                configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_CustRes_width", ResMap["width"]);
            }
        }
    }
    //================= 存储table_gif =========================
    configIniWrite->setValue("/table_gif/rowCount", Table_model_gif->rowCount());
    for(int i=0; i<Table_model_gif->rowCount(); i++)
    {
        QAbstractItemModel *modessl = Table_model_gif;
        //===
        QModelIndex indextemp = modessl->index(i, 0);
        QVariant datatemp = modessl->data(indextemp);
        QString FileName = datatemp.toString();
        configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_FileName", FileName);
        //===
        indextemp = modessl->index(i, 1);
        datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_status", status);
        //===
        indextemp = modessl->index(i, 2);
        datatemp = modessl->data(indextemp);
        QString fullPath = datatemp.toString();
        configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_fullPath", fullPath);
        //===
        indextemp = modessl->index(i, 3);
        datatemp = modessl->data(indextemp);
        QString CustRes_str = datatemp.toString();
        configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_CustRes_str", CustRes_str);
        if(CustRes_str!="")
        {
            QMap<QString, QString> ResMap = CustRes_getResMap(fullPath);
            if(!ResMap.isEmpty())
            {
                configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_CustRes_height", ResMap["height"]);
                configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_CustRes_width", ResMap["width"]);
            }
        }
    }
    //================= 存储table_video =========================
    configIniWrite->setValue("/table_video/rowCount", Table_model_video->rowCount());
    for(int i=0; i<Table_model_video->rowCount(); i++)
    {
        QAbstractItemModel *modessl = Table_model_video;
        //===
        QModelIndex indextemp = modessl->index(i, 0);
        QVariant datatemp = modessl->data(indextemp);
        QString FileName = datatemp.toString();
        configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_FileName", FileName);
        //===
        indextemp = modessl->index(i, 1);
        datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_status", status);
        //===
        indextemp = modessl->index(i, 2);
        datatemp = modessl->data(indextemp);
        QString fullPath = datatemp.toString();
        configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_fullPath", fullPath);
        //===
        indextemp = modessl->index(i, 3);
        datatemp = modessl->data(indextemp);
        QString CustRes_str = datatemp.toString();
        configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_CustRes_str", CustRes_str);
        if(CustRes_str!="")
        {
            QMap<QString, QString> ResMap = CustRes_getResMap(fullPath);
            if(!ResMap.isEmpty())
            {
                configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_CustRes_height", ResMap["height"]);
                configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_CustRes_width", ResMap["width"]);
            }
        }
    }
    return 0;
}
void MainWindow::on_pushButton_SaveFileList_clicked()
{
    if(Table_model_video->rowCount()<=0&&Table_model_image->rowCount()<=0&&Table_model_gif->rowCount()<=0)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Error"));
        MSG->setText(tr("File list is empty!"));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
        return;
    }
    //====================== 选择保存位置 ==========================
    //生成默认保存文件的文件夹
    file_mkDir(Current_Path+"/FilesList_W2xEX");
    //保存文件弹窗
    QString FilesListFullPath = QFileDialog::getSaveFileName(this, tr("Save files list @Waifu2x-Extension-GUI"),
                                Current_Path+"/FilesList_W2xEX/FilesList_W2xEX_"+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss")+".ini",
                                "*.ini");
    if(FilesListFullPath.trimmed()=="")return;
    //判断文件夹是否存在以及是否可写入
    QFileInfo FilesListFullPath_fileinfo(FilesListFullPath);
    QString FilesListFullPath_FolderPath = file_getFolderPath(FilesListFullPath_fileinfo);
    if(file_isDirExist(FilesListFullPath_FolderPath)==false || file_isDirWritable(FilesListFullPath_FolderPath)==false)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Error"));
        MSG->setText(tr("Target folder doesn't exist or unable to write to the target folder."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
        return;
    }
    //======================= 开始保存 ========================
    this->setAcceptDrops(0);//禁止drop file
    pushButton_Start_setEnabled_self(0);//禁用start button
    ui->pushButton_ClearList->setEnabled(0);
    ui->pushButton_RemoveItem->setEnabled(0);
    ui->pushButton_CustRes_cancel->setEnabled(0);
    ui->pushButton_CustRes_apply->setEnabled(0);
    ui->pushButton_ReadFileList->setEnabled(0);
    ui->pushButton_SaveFileList->setEnabled(0);
    ui->pushButton_BrowserFile->setEnabled(0);
    emit Send_TextBrowser_NewMessage(tr("Write to the file, please wait."));
    Table_Save_Current_Table_Filelist(FilesListFullPath);
    QtConcurrent::run(this, &MainWindow::Table_Save_Current_Table_Filelist_Watchdog,FilesListFullPath);
}
int MainWindow::Table_Save_Current_Table_Filelist_Watchdog(QString Table_FileList_ini)
{
    while(!QFile::exists(Table_FileList_ini))
    {
        Delay_msec_sleep(100);
    }
    emit Send_Table_Save_Current_Table_Filelist_Finished();
    return 0;
}
int MainWindow::Table_Save_Current_Table_Filelist_Finished()
{
    if(Waifu2xMain.isRunning()==false)
    {
        this->setAcceptDrops(1);//启用drop file
        ui->pushButton_ClearList->setEnabled(1);
        ui->pushButton_RemoveItem->setEnabled(1);
        ui->pushButton_ReadFileList->setEnabled(1);
        ui->pushButton_BrowserFile->setEnabled(1);
    }
    ui->pushButton_CustRes_cancel->setEnabled(1);
    ui->pushButton_CustRes_apply->setEnabled(1);
    pushButton_Start_setEnabled_self(1);//启用start button
    ui->pushButton_SaveFileList->setEnabled(1);
    emit Send_TextBrowser_NewMessage(tr("File list saved successfully!"));
    //===
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Notification"));
    MSG->setText(tr("File list saved successfully!"));
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(true);
    MSG->show();
    //===
    return 0;
}
int MainWindow::Table_Read_Saved_Table_Filelist(QString Table_FileList_ini)
{
    if(!QFile::exists(Table_FileList_ini))
    {
        emit Send_TextBrowser_NewMessage(tr("Cannot find the saved Files List!"));
        emit Send_Table_Read_Saved_Table_Filelist_Finished(Table_FileList_ini);
        return 0;
    }
    //=================
    QSettings *configIniRead = new QSettings(Table_FileList_ini, QSettings::IniFormat);
    //====================
    int rowCount_image = configIniRead->value("/table_image/rowCount").toInt();
    int rowCount_gif = configIniRead->value("/table_gif/rowCount").toInt();
    int rowCount_video = configIniRead->value("/table_video/rowCount").toInt();
    //==========
    Progressbar_MaxVal = rowCount_image + rowCount_gif + rowCount_video;
    Progressbar_CurrentVal = 0;
    emit Send_PrograssBar_Range_min_max(0, Progressbar_MaxVal);
    //========= 加载image ========
    for(int i=0; i<rowCount_image; i++)
    {
        //===========
        QString FileName =configIniRead->value("/table_image/"+QString::number(i,10)+"_FileName").toString();
        QString status =configIniRead->value("/table_image/"+QString::number(i,10)+"_status").toString();
        QString fullPath =configIniRead->value("/table_image/"+QString::number(i,10)+"_fullPath").toString();
        QString CustRes_str =configIniRead->value("/table_image/"+QString::number(i,10)+"_CustRes_str").toString();
        //================
        mutex_Table_insert_finished.lock();
        Table_insert_finished=false;
        mutex_Table_insert_finished.unlock();
        emit Send_Table_image_insert_fileName_fullPath(FileName,fullPath);
        while(!Table_insert_finished)
        {
            Delay_msec_sleep(10);
        }
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(i,status);
        if(status=="Failed")
        {
            emit Send_TextBrowser_NewMessage(tr("[Failed]--[")+fullPath+"]");
        }
        //===============
        if(CustRes_str!="")
        {
            QString CustRes_height =configIniRead->value("/table_image/"+QString::number(i,10)+"_CustRes_height").toString();
            QString CustRes_width =configIniRead->value("/table_image/"+QString::number(i,10)+"_CustRes_width").toString();
            emit Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(i,CustRes_height,CustRes_width);
            QMap<QString, QString> res_map;
            res_map["fullpath"] = fullPath;
            res_map["height"] = CustRes_height;
            res_map["width"] = CustRes_width;
            Custom_resolution_list.append(res_map);
        }
        emit Send_progressbar_Add();
        //Delay_msec_sleep(100);
    }
    //========= 加载gif ========
    for(int i=0; i<rowCount_gif; i++)
    {
        //===========
        QString FileName =configIniRead->value("/table_gif/"+QString::number(i,10)+"_FileName").toString();
        QString status =configIniRead->value("/table_gif/"+QString::number(i,10)+"_status").toString();
        QString fullPath =configIniRead->value("/table_gif/"+QString::number(i,10)+"_fullPath").toString();
        QString CustRes_str =configIniRead->value("/table_gif/"+QString::number(i,10)+"_CustRes_str").toString();
        //================
        mutex_Table_insert_finished.lock();
        Table_insert_finished=false;
        mutex_Table_insert_finished.unlock();
        emit Send_Table_gif_insert_fileName_fullPath(FileName,fullPath);
        while(!Table_insert_finished)
        {
            Delay_msec_sleep(10);
        }
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(i,status);
        if(status=="Failed")
        {
            emit Send_TextBrowser_NewMessage(tr("[Failed]--[")+fullPath+"]");
        }
        //===============
        if(CustRes_str!="")
        {
            QString CustRes_height =configIniRead->value("/table_gif/"+QString::number(i,10)+"_CustRes_height").toString();
            QString CustRes_width =configIniRead->value("/table_gif/"+QString::number(i,10)+"_CustRes_width").toString();
            emit Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(i,CustRes_height,CustRes_width);
            QMap<QString, QString> res_map;
            res_map["fullpath"] = fullPath;
            res_map["height"] = CustRes_height;
            res_map["width"] = CustRes_width;
            Custom_resolution_list.append(res_map);
        }
        emit Send_progressbar_Add();
        //Delay_msec_sleep(100);
    }
    //========= 加载video ========
    for(int i=0; i<rowCount_video; i++)
    {
        //===========
        QString FileName =configIniRead->value("/table_video/"+QString::number(i,10)+"_FileName").toString();
        QString status =configIniRead->value("/table_video/"+QString::number(i,10)+"_status").toString();
        QString fullPath =configIniRead->value("/table_video/"+QString::number(i,10)+"_fullPath").toString();
        QString CustRes_str =configIniRead->value("/table_video/"+QString::number(i,10)+"_CustRes_str").toString();
        //================
        mutex_Table_insert_finished.lock();
        Table_insert_finished=false;
        mutex_Table_insert_finished.unlock();
        emit Send_Table_video_insert_fileName_fullPath(FileName,fullPath);
        while(!Table_insert_finished)
        {
            Delay_msec_sleep(10);
        }
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(i,status);
        if(status=="Failed")
        {
            emit Send_TextBrowser_NewMessage(tr("[Failed]--[")+fullPath+"]");
        }
        //===============
        if(CustRes_str!="")
        {
            QString CustRes_height =configIniRead->value("/table_video/"+QString::number(i,10)+"_CustRes_height").toString();
            QString CustRes_width =configIniRead->value("/table_video/"+QString::number(i,10)+"_CustRes_width").toString();
            emit Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(i,CustRes_height,CustRes_width);
            QMap<QString, QString> res_map;
            res_map["fullpath"] = fullPath;
            res_map["height"] = CustRes_height;
            res_map["width"] = CustRes_width;
            Custom_resolution_list.append(res_map);
        }
        emit Send_progressbar_Add();
        //Delay_msec_sleep(100);
    }
    //====================
    emit Send_Table_Read_Saved_Table_Filelist_Finished(Table_FileList_ini);
    return 0;
}
int MainWindow::Table_Read_Saved_Table_Filelist_Finished(QString Table_FileList_ini)
{
    Progressbar_MaxVal = 0;
    Progressbar_CurrentVal = 0;
    progressbar_clear();
    //====
    ui_tableViews_setUpdatesEnabled(true);
    this->setAcceptDrops(1);
    pushButton_Start_setEnabled_self(1);
    ui->pushButton_CustRes_cancel->setEnabled(1);
    ui->pushButton_CustRes_apply->setEnabled(1);
    ui->pushButton_ReadFileList->setEnabled(1);
    ui->pushButton_SaveFileList->setEnabled(1);
    ui->pushButton_BrowserFile->setEnabled(1);
    if(!QFile::exists(Table_FileList_ini))
    {
        return 0;
    }
    //=================
    QSettings *configIniRead = new QSettings(Table_FileList_ini, QSettings::IniFormat);
    //====================
    //========= image ========
    int rowCount_image = configIniRead->value("/table_image/rowCount").toInt();
    if(rowCount_image>0)
    {
        ui->label_DropFile->setVisible(0);//隐藏文件投放label
        ui->tableView_image->setVisible(1);
    }
    //========= gif ========
    int rowCount_gif = configIniRead->value("/table_gif/rowCount").toInt();
    if(rowCount_gif>0)
    {
        ui->label_DropFile->setVisible(0);//隐藏文件投放label
        ui->tableView_gif->setVisible(1);
    }
    //========= video ========
    int rowCount_video = configIniRead->value("/table_video/rowCount").toInt();
    if(rowCount_video>0)
    {
        ui->label_DropFile->setVisible(0);//隐藏文件投放label
        ui->tableView_video->setVisible(1);
    }
    //====================
    ui->tableView_gif->scrollToBottom();
    ui->tableView_image->scrollToBottom();
    ui->tableView_video->scrollToBottom();
    QScrollBar *image_ScrBar = ui->tableView_image->horizontalScrollBar();
    image_ScrBar->setValue(0);
    QScrollBar *gif_ScrBar = ui->tableView_gif->horizontalScrollBar();
    gif_ScrBar->setValue(0);
    QScrollBar *video_ScrBar = ui->tableView_video->horizontalScrollBar();
    video_ScrBar->setValue(0);
    //============
    Send_TextBrowser_NewMessage(tr("File list update is complete!"));
    //====
    progressbar_SetToMax(Progressbar_MaxVal);
    Progressbar_MaxVal = 0;
    Progressbar_CurrentVal = 0;
    //====
    if(rowCount_image<=0&&rowCount_video<=0&&rowCount_gif<=0)
    {
        Send_TextBrowser_NewMessage(tr("The file list saved last time is empty."));
        progressbar_clear();
    }
    //==
    Table_FileCount_reload();
    //===
    ui->label_DropFile->setText(tr("Drag and drop files or folders here\n(Image, GIF and Video)"));
    //===
    return 0;
}
void MainWindow::on_tableView_image_doubleClicked(const QModelIndex &index)
{
    if(curRow_image==-1)return;
    QModelIndex a;
    on_tableView_image_pressed(a);
}
void MainWindow::on_tableView_gif_doubleClicked(const QModelIndex &index)
{
    if(curRow_gif==-1)return;
    QModelIndex a;
    on_tableView_gif_pressed(a);
}
void MainWindow::on_tableView_video_doubleClicked(const QModelIndex &index)
{
    if(curRow_video==-1)return;
    QModelIndex a;
    on_tableView_video_pressed(a);
}
void MainWindow::on_tableView_image_pressed(const QModelIndex &index)
{
    curRow_image = ui->tableView_image->currentIndex().row();
    curRow_gif = -1;
    curRow_video = -1;
    ui->tableView_gif->clearSelection();
    ui->tableView_video->clearSelection();
}
void MainWindow::on_tableView_gif_pressed(const QModelIndex &index)
{
    curRow_gif = ui->tableView_gif->currentIndex().row();
    curRow_image = -1;
    curRow_video = -1;
    ui->tableView_image->clearSelection();
    ui->tableView_video->clearSelection();
}
void MainWindow::on_tableView_video_pressed(const QModelIndex &index)
{
    curRow_video = ui->tableView_video->currentIndex().row();
    curRow_image = -1;
    curRow_gif = -1;
    ui->tableView_image->clearSelection();
    ui->tableView_gif->clearSelection();
}
/*
启用 or 禁用 三个文件列表的排序功能
*/
void MainWindow::Table_EnableSorting(bool EnableSorting)
{
    ui->tableView_video->horizontalHeader()->setSortIndicatorShown(EnableSorting);
    ui->tableView_gif->horizontalHeader()->setSortIndicatorShown(EnableSorting);
    ui->tableView_image->horizontalHeader()->setSortIndicatorShown(EnableSorting);
    //=====
    ui->tableView_image->setSortingEnabled(EnableSorting);
    ui->tableView_gif->setSortingEnabled(EnableSorting);
    ui->tableView_video->setSortingEnabled(EnableSorting);
}
/*
启用/禁用 文件列表table的UI更新
*/
void MainWindow::ui_tableViews_setUpdatesEnabled(bool isEnabled)
{
    ui->tableView_video->setUpdatesEnabled(isEnabled);
    ui->tableView_gif->setUpdatesEnabled(isEnabled);
    ui->tableView_image->setUpdatesEnabled(isEnabled);
}
