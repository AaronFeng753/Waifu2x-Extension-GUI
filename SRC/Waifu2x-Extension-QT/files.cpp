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

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}
/*
拖放文件event
*/
void MainWindow::dropEvent(QDropEvent *event)
{
    //重置 是否有某种类型的新文件加入 的判断标志
    AddNew_gif=false;
    AddNew_image=false;
    AddNew_video=false;
    //================
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;
    foreach(QUrl url, urls)
    {
        if(ui->checkBox_ScanSubFolders->checkState())
        {
            Add_File_Folder_IncludeSubFolder(url.toLocalFile());
        }
        else
        {
            Add_File_Folder(url.toLocalFile());
        }
    }
    //=================
    //如果没有增加任何文件
    if(AddNew_gif==false&&AddNew_image==false&&AddNew_video==false)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("The file format is not supported, please enter supported file format, or add more file extensions yourself."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
        return;
    }
    if(AddNew_image)
    {
        ui->label_DropFile->setVisible(0);//隐藏文件投放label
        ui->tableView_image->setVisible(1);
        ui->pushButton_ClearList->setVisible(1);
        ui->pushButton_RemoveItem->setVisible(1);
    }
    if(AddNew_gif)
    {
        ui->label_DropFile->setVisible(0);//隐藏文件投放label
        ui->tableView_gif->setVisible(1);
        ui->pushButton_ClearList->setVisible(1);
        ui->pushButton_RemoveItem->setVisible(1);
    }
    if(AddNew_video)
    {
        ui->label_DropFile->setVisible(0);//隐藏文件投放label
        ui->tableView_video->setVisible(1);
        ui->pushButton_ClearList->setVisible(1);
        ui->pushButton_RemoveItem->setVisible(1);
    }
    ui->tableView_gif->scrollToBottom();
    ui->tableView_image->scrollToBottom();
    ui->tableView_video->scrollToBottom();
    AddNew_image=false;
    AddNew_image=false;
    AddNew_video=false;
    Table_FileCount_reload();
}
/*
添加文件&文件夹
*/
void MainWindow::Add_File_Folder(QString Full_Path)
{
    QFileInfo fileinfo(Full_Path);
    if(fileinfo.isFile())
    {
        QString file_name = fileinfo.fileName();
        FileList_Add(file_name, Full_Path);
    }
    else
    {
        QStringList FileNameList = getFileNames(Full_Path);//读取合法的文件名
        QString Full_Path_File = "";
        if(!FileNameList.isEmpty())
        {
            for(int i = 0; i < FileNameList.size(); i++)
            {
                QString tmp = FileNameList.at(i);
                Full_Path_File = Full_Path + "/" + tmp;
                FileList_Add(tmp, Full_Path_File);
            }
        }
    }
}
/*
添加文件&文件夹
扫描子文件夹
*/
void MainWindow::Add_File_Folder_IncludeSubFolder(QString Full_Path)
{
    QFileInfo fileinfo(Full_Path);
    if(fileinfo.isFile())
    {
        QString file_name = fileinfo.fileName();
        FileList_Add(file_name, Full_Path);
    }
    else
    {
        QStringList FileNameList = getFileNames_IncludeSubFolder(Full_Path);//读取合法的文件名
        FileNameList.removeAll("..");
        FileNameList.removeAll(".");
        QString Full_Path_File = "";
        if(!FileNameList.isEmpty())
        {
            for(int i = 0; i < FileNameList.size(); i++)
            {
                QString tmp = FileNameList.at(i);
                Full_Path_File = Full_Path + "/" + tmp;
                QFileInfo fileinfo_tmp(Full_Path_File);
                if(fileinfo_tmp.isFile())
                {
                    if(QFile::exists(Full_Path_File))
                        FileList_Add(tmp, Full_Path_File);
                }
                else
                {
                    if(QFile::exists(Full_Path_File))
                        Add_File_Folder_IncludeSubFolder(Full_Path_File);
                }
            }
        }
    }
}
/*
读取文件夹下的文件名(包括子文件夹
*/
QStringList MainWindow::getFileNames_IncludeSubFolder(QString path)
{
    QDir dir(path);
    QStringList files = dir.entryList(QDir::Dirs | QDir::Files | QDir::Writable, QDir::Name);
    return files;
}
/*
读取文件夹下的文件名(不包括子文件夹
*/
QStringList MainWindow::getFileNames(QString path)
{
    QDir dir(path);
    QStringList files = dir.entryList(QDir::Files | QDir::Writable, QDir::Name);
    return files;
}
/*
向文件列表和table添加文件
*/
int MainWindow::FileList_Add(QString fileName, QString SourceFile_fullPath)
{
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix();
    //============================  判断是否为图片 ===============================
    QString Ext_image_str = ui->Ext_image->text();
    QStringList nameFilters_image = Ext_image_str.split(":");
    if (nameFilters_image.contains(file_ext.toLower()))
    {
        AddNew_image=true;
        int rowNum = Table_image_get_rowNum();
        QMap<QString, QString> map;
        map["SourceFile_fullPath"] = SourceFile_fullPath;
        map["rowNum"] = QString::number(rowNum, 10);
        if(!Deduplicate_filelist(FileList_image, SourceFile_fullPath)&&!Deduplicate_filelist(FileList_image_finished, SourceFile_fullPath))
        {
            FileList_image.append(map);
            Table_image_insert_fileName_fullPath(fileName, SourceFile_fullPath);
        }
        return 0;
    }
    //============================  判断是否为视频 ===============================
    QString Ext_video_str = ui->Ext_video->text();
    QStringList nameFilters_video = Ext_video_str.split(":");
    if (nameFilters_video.contains(file_ext.toLower()))
    {
        if(file_ext!="mp4"&&file_ext.toLower()=="mp4")
        {
            QString file_name = file_getBaseName(fileinfo.filePath());
            QString file_ext = fileinfo.suffix();
            QString file_path = fileinfo.path();
            if(file_path.right(1)=="/")
            {
                file_path = file_path.left(file_path.length() - 1);
            }
            QFile::rename(file_path+"/"+file_name+"."+file_ext,file_path+"/"+file_name+".mp4");
            SourceFile_fullPath = file_path+"/"+file_name+".mp4";
        }
        AddNew_video=true;
        int rowNum = Table_video_get_rowNum();
        QMap<QString, QString> map;
        map["SourceFile_fullPath"] = SourceFile_fullPath;
        map["rowNum"] = QString::number(rowNum, 10);
        if(!Deduplicate_filelist(FileList_video, SourceFile_fullPath)&&!Deduplicate_filelist(FileList_video_finished, SourceFile_fullPath))
        {
            FileList_video.append(map);
            Table_video_insert_fileName_fullPath(fileName, SourceFile_fullPath);
        }
        return 0;
    }
    //============================  最后只能是gif ===============================
    if(file_ext.toLower()=="gif")
    {
        int rowNum = Table_gif_get_rowNum();
        QMap<QString, QString> map;
        map["SourceFile_fullPath"] = SourceFile_fullPath;
        map["rowNum"] = QString::number(rowNum, 10);
        AddNew_gif=true;
        if(!Deduplicate_filelist(FileList_gif, SourceFile_fullPath)&&!Deduplicate_filelist(FileList_gif_finished, SourceFile_fullPath))
        {
            FileList_gif.append(map);
            Table_gif_insert_fileName_fullPath(fileName, SourceFile_fullPath);
        }
        return 0;
    }
    return 0;
}
/*
判断是否已经在文件列表内
*/
bool MainWindow::Deduplicate_filelist(QList<QMap<QString, QString>> FileList, QString SourceFile_fullPath)
{
    if(FileList.isEmpty())return false;
    for ( int i = 0; i != FileList.size(); ++i )
    {
        QMap<QString, QString> File_map = FileList.at(i);
        if(File_map["SourceFile_fullPath"] == SourceFile_fullPath)
        {
            return true;
        }
    }
    return false;
}
/*
从文件列表内移除条目
*/
int MainWindow::FileList_remove(QMap<QString, QString> File_map)
{
    if(FileList_image.removeAll(File_map)>0)return 0;
    if(FileList_gif.removeAll(File_map)>0)return 0;
    if(FileList_video.removeAll(File_map)>0)return 0;
    return 0;
}
/*
查询某个文件所在row
*/
QMap<QString, QString> MainWindow::FileList_find_rowNum(QList<QMap<QString, QString>> FileList, int rowNum)
{
    QString rowNum_QS = QString::number(rowNum, 10);
    for ( int i = 0; i != FileList.size(); ++i )
    {
        QMap<QString, QString> File_map = FileList.at(i);
        if(File_map["rowNum"] == rowNum_QS)
        {
            return File_map;
        }
    }
    QMap<QString, QString> emptymap;
    emptymap.clear();
    return emptymap;
}
/*
恢复已处理完成的文件到待处理文件列表
*/
void MainWindow::RecFinedFiles()
{
    if(!FileList_image_finished.isEmpty())
    {
        FileList_image.append(FileList_image_finished);
        FileList_image_finished.clear();
    }
    if(!FileList_gif_finished.isEmpty())
    {
        FileList_gif.append(FileList_gif_finished);
        FileList_gif_finished.clear();
    }
    if(!FileList_video_finished.isEmpty())
    {
        FileList_video.append(FileList_video_finished);
        FileList_video_finished.clear();
    }
}
/*
将处理完成的文件移出文件列表
*/
void MainWindow::MovToFinedList()
{
    if(!FileList_image.isEmpty())
    {
        int rowNum = Table_model_image->rowCount();
        for (int i = 0; i < rowNum; i++)
        {
            QAbstractItemModel *modessl = Table_model_image;
            QModelIndex indextemp = modessl->index(i, 1);
            QVariant datatemp = modessl->data(indextemp);
            QString status = datatemp.toString();
            if(status == "Finished")
            {
                QMap<QString, QString> File_map = MainWindow::FileList_find_rowNum(FileList_image, i);
                if(!File_map.isEmpty())
                {
                    FileList_remove(File_map);
                    FileList_image_finished.append(File_map);
                }
            }
        }
    }
    if(!FileList_gif.isEmpty())
    {
        int rowNum = Table_model_gif->rowCount();
        for (int i = 0; i < rowNum; i++)
        {
            QAbstractItemModel *modessl = Table_model_gif;
            QModelIndex indextemp = modessl->index(i, 1);
            QVariant datatemp = modessl->data(indextemp);
            QString status = datatemp.toString();
            if(status == "Finished")
            {
                QMap<QString, QString> File_map = MainWindow::FileList_find_rowNum(FileList_gif, i);
                if(!File_map.isEmpty())
                {
                    FileList_remove(File_map);
                    FileList_gif_finished.append(File_map);
                }
            }
        }
    }
    if(!FileList_video.isEmpty())
    {
        int rowNum = Table_model_video->rowCount();
        for (int i = 0; i < rowNum; i++)
        {
            QAbstractItemModel *modessl = Table_model_video;
            QModelIndex indextemp = modessl->index(i, 1);
            QVariant datatemp = modessl->data(indextemp);
            QString status = datatemp.toString();
            if(status == "Finished")
            {
                QMap<QString, QString> File_map = MainWindow::FileList_find_rowNum(FileList_video, i);
                if(!File_map.isEmpty())
                {
                    FileList_remove(File_map);
                    FileList_video_finished.append(File_map);
                }
            }
        }
    }
}
/*
文件夹是否存在
*/
bool MainWindow::file_isDirExist(QString SourceFile_fullPath)
{
    QDir dir(SourceFile_fullPath);
    if(dir.exists())
    {
        return true;
    }
    return false;
}
/*
创建文件夹
*/
void MainWindow::file_mkDir(QString SourceFile_fullPath)
{
    QDir dir(SourceFile_fullPath);
    if(dir.exists() == false)
    {
        dir.mkdir(SourceFile_fullPath);
    }
}
/*
判断文件是否存在
*/
bool MainWindow::file_isFileExist(QString fullFilePath)
{
    QFile file(fullFilePath);
    if(file.exists())
    {
        return true;
    }
    return false;
}
//===================================================================================
/*
复制文件
*/
/*
void MainWindow::file_copyFile(QString sourceDir, QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\", "/");
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist)
    {
        if(coverFileIfExist)
        {
            createfile->remove(toDir);
        }
    }//end if
    QFile::copy(sourceDir, toDir);
}*/
//=======================================================================================
/*
扫描文件夹下文件名列表(无过滤
*/
QStringList MainWindow::file_getFileNames_in_Folder_nofilter(QString path)
{
    QDir dir(path);
    QStringList files_old;
    QStringList files_new;
    while(true)
    {
        files_new = dir.entryList(QDir::Files | QDir::Writable, QDir::Name);
        if(files_new!=files_old)
        {
            files_old = files_new;
            Delay_sec_sleep(2);
        }
        else
        {
            break;
        }
    }
    return files_new;
}
/*
删除文件夹
*/
bool MainWindow::file_DelDir(const QString &path)
{
    if (path.isEmpty())
    {
        return false;
    }
    QDir dir(path);
    if(!dir.exists())
    {
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList)  //遍历文件信息
    {
        if (file.isFile())  // 是文件，删除
        {
            file.dir().remove(file.fileName());
        }
        else   // 递归删除
        {
            file_DelDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}
/*
重写的获取basename函数
*/
QString MainWindow::file_getBaseName(QString path)
{
    QFileInfo fileinfo(path);
    QString file_fullname = fileinfo.fileName();
    QStringList parts = file_fullname.split(".");
    QString file_basename="";
    for(int i=0; i<(parts.size()-1); i++)
    {
        file_basename+=parts.at(i);
        file_basename+=".";
    }
    if(file_basename.right(1)==".")
    {
        file_basename = file_basename.left(file_basename.length() - 1);
    }
    return file_basename;
}
/*
移动文件到回收站
*/
void MainWindow::file_MoveToTrash( QString file )
{
    QFileInfo fileinfo( file );
    if( !fileinfo.exists() )
        return;
    WCHAR from[ MAX_PATH ];
    memset( from, 0, sizeof( from ));
    int l = fileinfo.absoluteFilePath().toWCharArray( from );
    Q_ASSERT( 0 <= l && l < MAX_PATH );
    from[ l ] = '\0';
    SHFILEOPSTRUCT fileop;
    memset( &fileop, 0, sizeof( fileop ) );
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int rv = SHFileOperation( &fileop );
    if( 0 != rv )
    {
        return;
    }
}
/*
移动文件
*/
void MainWindow::file_MoveFile(QString Orginal,QString Target)
{
    if(file_isFileExist(Orginal))
    {
        if(file_isFileExist(Target))QFile::remove(Target);
        if(QFile::rename(Orginal,Target)==false)
        {
            emit Send_TextBrowser_NewMessage(tr("Error! Failed to move [")+Orginal+tr("] to [")+Target+"]");
        }
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Original file [")+Orginal+tr("] dose not exists."));
    }
}
