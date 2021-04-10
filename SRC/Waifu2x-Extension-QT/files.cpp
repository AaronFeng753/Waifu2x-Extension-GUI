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
    //================== 界面管制 ========================
    ui_tableViews_setUpdatesEnabled(false);
    //================
    ui->groupBox_Setting->setEnabled(0);
    ui->groupBox_FileList->setEnabled(0);
    ui->groupBox_InputExt->setEnabled(0);
    pushButton_Start_setEnabled_self(0);
    ui->checkBox_ScanSubFolders->setEnabled(0);
    this->setAcceptDrops(0);
    ui->label_DropFile->setText(tr("Adding files, please wait."));
    emit Send_TextBrowser_NewMessage(tr("Adding files, please wait."));
    //===================================================
    QtConcurrent::run(this, &MainWindow::Read_urls, urls);
}
/*
读取urls
*/
void MainWindow::Read_urls(QList<QUrl> urls)
{
    Progressbar_MaxVal = urls.size();
    Progressbar_CurrentVal = 0;
    emit Send_PrograssBar_Range_min_max(0, Progressbar_MaxVal);
    if(ui->checkBox_ScanSubFolders->isChecked())
    {
        foreach(QUrl url, urls)
        {
            Add_File_Folder_IncludeSubFolder(url.toLocalFile());
            emit Send_progressbar_Add();
        }
    }
    else
    {
        foreach(QUrl url, urls)
        {
            Add_File_Folder(url.toLocalFile());
            emit Send_progressbar_Add();
        }
    }
    emit Send_Read_urls_finfished();
    return;
}
/*
读取urls
善后
*/
void MainWindow::Read_urls_finfished()
{
    //================== 解除界面管制 ========================
    ui_tableViews_setUpdatesEnabled(true);
    //===
    ui->groupBox_Setting->setEnabled(1);
    ui->groupBox_FileList->setEnabled(1);
    pushButton_Start_setEnabled_self(1);
    ui->groupBox_InputExt->setEnabled(1);
    ui->checkBox_ScanSubFolders->setEnabled(1);
    this->setAcceptDrops(1);
    ui->label_DropFile->setText(tr("Drag and drop files or folders here\n(Image, GIF and Video)"));
    emit Send_TextBrowser_NewMessage(tr("Add file complete."));
    //===================================================
    Progressbar_MaxVal = 0;
    Progressbar_CurrentVal = 0;
    progressbar_clear();
    //======================
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
    }
    if(AddNew_gif)
    {
        ui->label_DropFile->setVisible(0);//隐藏文件投放label
        ui->tableView_gif->setVisible(1);
    }
    if(AddNew_video)
    {
        ui->label_DropFile->setVisible(0);//隐藏文件投放label
        ui->tableView_video->setVisible(1);
    }
    //===================
    ui->tableView_gif->scrollToBottom();
    ui->tableView_image->scrollToBottom();
    ui->tableView_video->scrollToBottom();
    QScrollBar *image_ScrBar = ui->tableView_image->horizontalScrollBar();
    image_ScrBar->setValue(0);
    QScrollBar *gif_ScrBar = ui->tableView_gif->horizontalScrollBar();
    gif_ScrBar->setValue(0);
    QScrollBar *video_ScrBar = ui->tableView_video->horizontalScrollBar();
    video_ScrBar->setValue(0);
    //==========
    AddNew_image=false;
    AddNew_gif=false;
    AddNew_video=false;
    //============
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
        QStringList FileNameList = file_getFileNames_in_Folder_nofilter(Full_Path);//读取合法的文件名
        QString Full_Path_File = "";
        if(!FileNameList.isEmpty())
        {
            QString tmp="";
            for(int i = 0; i < FileNameList.size(); i++)
            {
                tmp = FileNameList.at(i);
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
        QString Full_Path_File = "";
        if(!FileNameList.isEmpty())
        {
            QString tmp="";
            for(int i = 0; i < FileNameList.size(); i++)
            {
                tmp = FileNameList.at(i);
                Full_Path_File = Full_Path + "/" + tmp;
                QFileInfo fileinfo_tmp(Full_Path_File);
                if(fileinfo_tmp.isFile())
                {
                    if(QFile::exists(Full_Path_File))FileList_Add(tmp, Full_Path_File);
                }
                else
                {
                    //if(QFile::exists(Full_Path_File))
                    if(file_isDirExist(Full_Path_File))Add_File_Folder_IncludeSubFolder(Full_Path_File);
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
    QStringList files_old;
    QStringList files_new;
    while(true)
    {
        files_new = dir.entryList(QDir::Dirs | QDir::Files | QDir::Writable, QDir::Name);
        if(files_new!=files_old)
        {
            files_old = files_new;
            Delay_msec_sleep(100);
        }
        else
        {
            break;
        }
    }
    files_new.removeAll("..");
    files_new.removeAll(".");
    return files_new;
}
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
            Delay_msec_sleep(100);
        }
        else
        {
            break;
        }
    }
    files_new.removeAll("..");
    files_new.removeAll(".");
    return files_new;
}

/*
向文件列表和table添加文件
*/
int MainWindow::FileList_Add(QString fileName, QString SourceFile_fullPath)
{
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix().toLower();
    //============================  判断是否为图片 ===============================
    QString Ext_image_str = ui->Ext_image->text().toLower();
    QStringList nameFilters_image = Ext_image_str.split(":");
    nameFilters_image.removeAll("gif");
    if (nameFilters_image.contains(file_ext))
    {
        AddNew_image=true;
        int rowNum = Table_image_get_rowNum();
        QMap<QString, QString> map;
        map["SourceFile_fullPath"] = SourceFile_fullPath;
        map["rowNum"] = QString::number(rowNum, 10);
        if(!Deduplicate_filelist(SourceFile_fullPath))
        {
            mutex_Table_insert_finished.lock();
            Table_insert_finished=false;
            mutex_Table_insert_finished.unlock();
            emit Send_Table_image_insert_fileName_fullPath(fileName, SourceFile_fullPath);
            while(!Table_insert_finished)
            {
                Delay_msec_sleep(10);
            }
        }
        return 0;
    }
    //============================  判断是否为视频 ===============================
    QString Ext_video_str = ui->Ext_video->text().toLower();
    QStringList nameFilters_video = Ext_video_str.split(":");
    nameFilters_video.removeAll("gif");
    if (nameFilters_video.contains(file_ext))
    {
        /*
        file_ext = fileinfo.suffix();
        if(file_ext!="mp4" && file_ext.toLower()=="mp4")
        {
            QString file_name = file_getBaseName(SourceFile_fullPath);
            QString file_path = file_getFolderPath(fileinfo);
            QFile::rename(file_path+"/"+file_name+"."+file_ext,file_path+"/"+file_name+".mp4");
            SourceFile_fullPath = file_path+"/"+file_name+".mp4";
        }
        */
        AddNew_video=true;
        int rowNum = Table_video_get_rowNum();
        QMap<QString, QString> map;
        map["SourceFile_fullPath"] = SourceFile_fullPath;
        map["rowNum"] = QString::number(rowNum, 10);
        if(!Deduplicate_filelist(SourceFile_fullPath))
        {
            mutex_Table_insert_finished.lock();
            Table_insert_finished=false;
            mutex_Table_insert_finished.unlock();
            emit Send_Table_video_insert_fileName_fullPath(fileName, SourceFile_fullPath);
            while(!Table_insert_finished)
            {
                Delay_msec_sleep(10);
            }
        }
        return 0;
    }
    //============================  最后只能是gif ===============================
    if(file_ext=="gif")
    {
        int rowNum = Table_gif_get_rowNum();
        QMap<QString, QString> map;
        map["SourceFile_fullPath"] = SourceFile_fullPath;
        map["rowNum"] = QString::number(rowNum, 10);
        AddNew_gif=true;
        if(!Deduplicate_filelist(SourceFile_fullPath))
        {
            mutex_Table_insert_finished.lock();
            Table_insert_finished=false;
            mutex_Table_insert_finished.unlock();
            emit Send_Table_gif_insert_fileName_fullPath(fileName, SourceFile_fullPath);
            while(!Table_insert_finished)
            {
                Delay_msec_sleep(10);
            }
        }
        return 0;
    }
    return 0;
}
/*
判断是否已经在文件列表内
*/
bool MainWindow::Deduplicate_filelist(QString SourceFile_fullPath)
{
    for ( int i = 0; i < Table_model_image->rowCount(); i++ )
    {
        QString fullpath_readRow =  Table_model_image->item(i,2)->text();
        if(fullpath_readRow == SourceFile_fullPath)
        {
            return true;
        }
    }
    for ( int i = 0; i < Table_model_gif->rowCount(); i++ )
    {
        QString fullpath_readRow =  Table_model_gif->item(i,2)->text();
        if(fullpath_readRow == SourceFile_fullPath)
        {
            return true;
        }
    }
    for ( int i = 0; i < Table_model_video->rowCount(); i++ )
    {
        QString fullpath_readRow =  Table_model_video->item(i,2)->text();
        if(fullpath_readRow == SourceFile_fullPath)
        {
            return true;
        }
    }
    return false;
}
/*
文件夹是否存在
*/
bool MainWindow::file_isDirExist(QString SourceFile_fullPath)
{
    QDir dir(SourceFile_fullPath.trimmed());
    return dir.exists();
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
文件夹是为空
*/
bool MainWindow::file_isDirEmpty(QString FolderPath)
{
    QDir dir(FolderPath);
    return dir.isEmpty();
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
获取文件夹路径(去除末尾的"/")
*/
QString MainWindow::file_getFolderPath(QFileInfo fileInfo)
{
    QString folder_path = fileInfo.path();
    if(folder_path.right(1)=="/")
    {
        folder_path = folder_path.left(folder_path.length() - 1);
    }
    return folder_path;
}
/*
检测文件夹是否可写入
*/
bool MainWindow::file_isDirWritable(QString DirPath)
{
    if(DirPath.right(1)=="/")
    {
        DirPath = DirPath.left(DirPath.length() - 1);
    }
    QString TestTemp = DirPath+"/RWTest_W2xEX.tmp";
    QFile file_TestTemp(TestTemp);
    file_TestTemp.remove();
    if (file_TestTemp.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite支持读写
    {
        QTextStream stream(&file_TestTemp);
        stream << "W2xEX";
    }
    if(QFile::exists(TestTemp))
    {
        file_TestTemp.remove();
        return true;
    }
    else
    {
        file_TestTemp.remove();
        return false;
    }
}
/*
判断当前处理的文件所在的文件夹是否可以写入
*/
bool MainWindow::file_isFilesFolderWritable_row_image(int rowNum)
{
    QString SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_FolderPath = file_getFolderPath(fileinfo);
    if(file_isDirWritable(file_FolderPath))
    {
        return true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Insufficient permissions, administrator permissions is needed.]"));
        QString status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        return false;
    }
}

bool MainWindow::file_isFilesFolderWritable_row_gif(int rowNum)
{
    QString SourceFile_fullPath = Table_model_gif->item(rowNum,2)->text();
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_FolderPath = file_getFolderPath(fileinfo);
    if(file_isDirWritable(file_FolderPath))
    {
        return true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Insufficient permissions, administrator permissions is needed.]"));
        QString status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        return false;
    }
}

bool MainWindow::file_isFilesFolderWritable_row_video(int rowNum)
{
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_FolderPath = file_getFolderPath(fileinfo);
    if(file_isDirWritable(file_FolderPath))
    {
        return true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Insufficient permissions, administrator permissions is needed.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        return false;
    }
}

bool MainWindow::file_OpenFolder(QString FolderPath)
{
    if(file_isDirExist(FolderPath))
    {
        FolderPath= FolderPath.replace("/","\\");
        QProcess::execute("explorer \""+FolderPath+"\"");
        return true;
    }
    else
    {
        return false;
    }
}

bool MainWindow::file_OpenFilesFolder(QString FilePath)
{
    QFileInfo finfo = QFileInfo(FilePath);
    return file_OpenFolder(file_getFolderPath(finfo));
}

bool MainWindow::file_OpenFile(QString FilePath)
{
    if(QFile::exists(FilePath))
    {
        if(QDesktopServices::openUrl(QUrl("file:"+QUrl::toPercentEncoding(FilePath),QUrl::TolerantMode))==false)
        {
            ExecuteCMD_batFile("start \"\" \""+FilePath.replace("%","%%")+"\"",false);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool MainWindow::file_generateMarkFile(QString FileFullPath,QString Msg)
{
    QFile file(FileFullPath);
    file.remove();
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite支持读写
    {
        QTextStream stream(&file);
        if(Msg.trimmed() == "")
        {
            stream << "Waifu2x-Extension-GUI\nDo NOT delete this file!!";
        }
        else
        {
            stream << Msg;
        }
    }
    return file.exists();
}


