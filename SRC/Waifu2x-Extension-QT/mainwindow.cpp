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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //==============
    translator = new QTranslator(this);
    //==============
    ui->tabWidget->setCurrentIndex(0);//显示home tab
    ui->tabWidget_waifu2xSettings->setCurrentIndex(0);
    ui->tabWidget_videoSettings->setCurrentIndex(0);
    TextBrowser_StartMes();//显示启动msg
    this->setAcceptDrops(true);//mainwindow接收drop
    Init_Table();//初始化table
    //=================== 初始隐藏所有table和按钮 ======================
    ui->tableView_image->setVisible(0);
    ui->tableView_gif->setVisible(0);
    ui->tableView_video->setVisible(0);
    ui->pushButton_ClearList->setVisible(0);
    ui->pushButton_RemoveItem->setVisible(0);
    Table_FileCount_reload();//重载文件列表下的文件数量统计
    ui->pushButton_Stop->setEnabled(0);
    //===========================================
    connect(this, SIGNAL(Send_PrograssBar_Range_min_max(int, int)), this, SLOT(progressbar_setRange_min_max(int, int)));
    connect(this, SIGNAL(Send_progressbar_Add()), this, SLOT(progressbar_Add()));
    //===
    connect(this, SIGNAL(Send_Table_image_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_image_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_video_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_video_ChangeStatus_rowNumInt_statusQString(int, QString)));
    //===
    connect(this, SIGNAL(Send_Table_FileCount_reload()), this, SLOT(Table_FileCount_reload()));
    connect(this, SIGNAL(Send_MovToFinedList()), this, SLOT(MovToFinedList()));
    //===
    connect(this, SIGNAL(Send_Table_image_insert_fileName_fullPath(QString,QString)), this, SLOT(Table_image_insert_fileName_fullPath(QString,QString)));
    connect(this, SIGNAL(Send_Table_gif_insert_fileName_fullPath(QString,QString)), this, SLOT(Table_gif_insert_fileName_fullPath(QString,QString)));
    connect(this, SIGNAL(Send_Table_video_insert_fileName_fullPath(QString,QString)), this, SLOT(Table_video_insert_fileName_fullPath(QString,QString)));
    //===
    connect(this, SIGNAL(Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)), this, SLOT(Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)));
    connect(this, SIGNAL(Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)), this, SLOT(Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)));
    connect(this, SIGNAL(Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)), this, SLOT(Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)));
    //===
    connect(this, SIGNAL(Send_Table_Read_Saved_Table_Filelist_Finished()), this, SLOT(Table_Read_Saved_Table_Filelist_Finished()));
    connect(this, SIGNAL(Send_Table_Save_Current_Table_Filelist_Finished()), this, SLOT(Table_Save_Current_Table_Filelist_Finished()));
    //===
    connect(this, SIGNAL(Send_Waifu2x_Finished()), this, SLOT(Waifu2x_Finished()));
    connect(this, SIGNAL(Send_Waifu2x_Finished_manual()), this, SLOT(Waifu2x_Finished_manual()));
    //===
    connect(this, SIGNAL(Send_TextBrowser_NewMessage(QString)), this, SLOT(TextBrowser_NewMessage(QString)));
    connect(this, SIGNAL(Send_Waifu2x_Compatibility_Test_finished()), this, SLOT(Waifu2x_Compatibility_Test_finished()));
    connect(this, SIGNAL(Send_Waifu2x_DetectGPU_finished()), this, SLOT(Waifu2x_DetectGPU_finished()));
    connect(this, SIGNAL(Send_CheckUpadte_NewUpdate(QString)), this, SLOT(CheckUpadte_NewUpdate(QString)));
    connect(this, SIGNAL(Send_SystemShutDown()), this, SLOT(SystemShutDown()));
    connect(this, SIGNAL(Send_Donate_Notification()), this, SLOT(Donate_Notification()));
    //Send_Waifu2x_DumpProcessorList_converter_finished
    connect(this, SIGNAL(Send_Waifu2x_DumpProcessorList_converter_finished()), this, SLOT(Waifu2x_DumpProcessorList_converter_finished()));
    //======
    TimeCostTimer = new QTimer();
    connect(TimeCostTimer, SIGNAL(timeout()), this, SLOT(TimeSlot()));
    //==================================================
    Settings_Read_Apply();//读取与应用更新
    //=====================================
    Set_Font_fixed();//固定字体
    //=====================================
    AutoUpdate = QtConcurrent::run(this, &MainWindow::CheckUpadte_Auto);//自动检查更新线程
    SystemShutDown_isAutoShutDown();//上次是否自动关机
    Donate_Count();//捐赠统计
    //===================================
    Tip_FirstTimeStart();
    //===================================
    this->adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox Msg(QMessageBox::Question, QString(tr("Notification")), QString(tr("Do you really wanna exit Waifu2x-Extension-GUI ?")));
    Msg.setIcon(QMessageBox::Question);
    QAbstractButton *pYesBtn = (QAbstractButton *)Msg.addButton(QString(tr("YES")), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
    Msg.exec();
    if (Msg.clickedButton() == pNoBtn)
    {
        event->ignore();
        return;
    }
    bool AutoSaveSettings = ui->checkBox_AutoSaveSettings->checkState();
    if(AutoSaveSettings&&(!Settings_isReseted))
    {
        Settings_Save();
        QtConcurrent::run(this, &MainWindow::Auto_Save_Settings_Watchdog);
    }
    else
    {
        QProcess_stop=true;
        AutoUpdate.cancel();
        Waifu2xMain.cancel();
        Force_close();
    }
}

int MainWindow::Auto_Save_Settings_Watchdog()
{
    QString settings_ini = Current_Path+"/settings.ini";
    while(!file_isFileExist(settings_ini))
    {
        Delay_msec_sleep(100);
    }
    Delay_msec_sleep(500);
    //=====
    QProcess_stop=true;
    AutoUpdate.cancel();
    Waifu2xMain.cancel();
    Force_close();
    //====
    return 0;
}

int MainWindow::Force_close()
{
    QProcess Close;
    Close.start("taskkill /f /t /fi \"imagename eq Waifu2x-Extension-GUI.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    return 0;
}

void MainWindow::TimeSlot()
{
    TimeCost++;
    QString TimeCostStr = tr("Time cost:[")+Seconds2hms(TimeCost)+"]";
    ui->label_TimeCost->setText(TimeCostStr);
    int TaskNumFinished_tmp = TaskNumFinished;
    long unsigned int TimeCost_tmp = TimeCost;
    int TaskNumTotal_tmp = TaskNumTotal;
    if(TaskNumFinished_tmp>0&&TimeCost_tmp>0&&TaskNumTotal_tmp>0)
    {
        if(NewTaskFinished)
        {
            NewTaskFinished=false;
            long unsigned int avgTimeCost = TimeCost/TaskNumFinished_tmp;
            ETA = int(avgTimeCost*(TaskNumTotal_tmp-TaskNumFinished_tmp));
        }
        else
        {
            if(ETA>1)
            {
                ETA--;
            }
        }
        QString TimeRemainingStr = tr("Time remaining:[")+Seconds2hms(ETA)+"]";
        ui->label_TimeRemain->setText(TimeRemainingStr);
        QDateTime time = QDateTime::currentDateTime();
        long unsigned int Time_t = time.toTime_t();
        Time_t+=ETA;
        time = QDateTime::fromTime_t(Time_t);
        QString Current_Time = time.toString("hh:mm:ss");
        QString ETA_str = "ETA:["+Current_Time+"]";
        ui->label_ETA->setText(ETA_str);
    }
}
QString MainWindow::Seconds2hms(long unsigned int seconds)
{
    if(seconds<=0)return "0:0:0";
    long unsigned int mm = 60;
    long unsigned int hh = mm * 60;
    long unsigned int hour = seconds / hh;
    long unsigned int min = (seconds-hour*hh)/mm;
    long unsigned int sec = seconds - hour*hh - min*mm;
    return QString::number(hour,10)+":"+QString::number(min,10)+":"+QString::number(sec,10);
}

void MainWindow::Set_Font_fixed()
{
    QFont font;
    if(ui->checkBox_isCustFontEnable->checkState())
    {
        font = ui->fontComboBox_CustFont->currentFont();
    }
    else
    {
        font = qApp->font();
    }
    font.setPixelSize(ui->spinBox_GlobalFontSize->value());
    qApp->setFont(font);
}

void MainWindow::on_pushButton_ClearList_clicked()
{
    curRow_image = -1;
    curRow_gif = -1;
    curRow_video = -1;
    Table_Clear();
    FileList_image.clear();
    FileList_gif.clear();
    FileList_video.clear();
    FileList_image_finished.clear();
    FileList_gif_finished.clear();
    FileList_video_finished.clear();
    Custom_resolution_list.clear();
    ui->label_DropFile->setVisible(1);
    ui->tableView_gif->setVisible(0);
    ui->tableView_image->setVisible(0);
    ui->tableView_video->setVisible(0);
    ui->pushButton_ClearList->setVisible(0);
    ui->pushButton_RemoveItem->setVisible(0);
    Table_FileCount_reload();
    progressbar_clear();
}

void MainWindow::on_pushButton_Start_clicked()
{
    if(ui->checkBox_OutPath_isEnabled->checkState())
    {
        QString tmp = ui->lineEdit_outputPath->text();
        tmp = tmp.trimmed();
        if(tmp=="")
        {
            emit Send_TextBrowser_NewMessage(tr("Output path is empty."));
            return;
        }
        tmp = tmp.replace("\\","/");
        tmp = tmp.replace("\\\\","/");
        tmp = tmp.replace("//","/");
        if(tmp.right(1)=="/")
        {
            tmp = tmp.left(tmp.length() - 1);
        }
        QFileInfo fileinfo_tmp(tmp);
        if(file_isDirExist(tmp)&&fileinfo_tmp.isDir()&&fileinfo_tmp.isWritable())
        {
            OutPutFolder_main = tmp;
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Invalid output path."));
            return;
        }
    }
    if(FileList_image.isEmpty()&&FileList_gif.isEmpty()&&FileList_video.isEmpty())
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to start processing files: The file list is empty or there are no available files to process."));
    }
    else
    {
        //============== 数值初始化 ==================
        waifu2x_STOP = false;
        waifu2x_STOP_confirm = false;
        ThreadNumMax = 0;
        ThreadNumRunning = 0;
        Progressbar_MaxVal = 0;
        Progressbar_CurrentVal = 0;
        TaskNumTotal=0;
        TaskNumFinished=0;
        NewTaskFinished=false;
        ETA=0;
        //============== 界面初始化 ======================
        this->setAcceptDrops(0);//禁止drop file
        ui->pushButton_Stop->setEnabled(1);//启用stop button
        ui->pushButton_Start->setEnabled(0);//禁用start button
        ui->groupBox_Input->setEnabled(0);
        ui->groupBox_OutPut->setEnabled(0);
        ui->pushButton_ClearList->setEnabled(0);
        ui->pushButton_RemoveItem->setEnabled(0);
        ui->groupBox_Engine->setEnabled(0);
        ui->groupBox_ScaleRaton_DenoiseLevel->setEnabled(0);
        ui->checkBox_OptGIF->setEnabled(0);
        ui->checkBox_SaveAsJPG->setEnabled(0);
        ui->checkBox_CompressJPG->setEnabled(0);
        ui->checkBox_DelOriginal->setEnabled(0);
        ui->checkBox_ReProcFinFiles->setEnabled(0);
        ui->pushButton_compatibilityTest->setEnabled(0);
        ui->pushButton_CustRes_cancel->setEnabled(0);
        ui->pushButton_CustRes_apply->setEnabled(0);
        ui->pushButton_ReadFileList->setEnabled(0);
        ui->pushButton_SaveFileList->setEnabled(0);
        ui->comboBox_AspectRatio_custRes->setEnabled(0);
        ui->spinBox_JPGCompressedQuality->setEnabled(0);
        progressbar_clear();
        ui->label_TimeCost->setText(tr("Time cost:NULL"));
        ui->label_ETA->setText(tr("ETA:NULL"));
        ui->label_TimeRemain->setText(tr("Time remaining:NULL"));
        ui->groupBox_video_settings->setEnabled(0);
        ui->checkBox_Move2RecycleBin->setEnabled(0);
        ui->pushButton_ForceRetry->setEnabled(1);
        //==========
        TimeCostTimer->start(1000);
        TimeCost=0;
        emit Send_TextBrowser_NewMessage(tr("Start processing files."));
        Waifu2xMain = QtConcurrent::run(this, &MainWindow::Waifu2xMainThread);//启动waifu2x 主线程
    }
}
/*
停止处理键
*/
void MainWindow::on_pushButton_Stop_clicked()
{
    TimeCostTimer->stop();
    ui->pushButton_Stop->setEnabled(0);//禁用stop button
    waifu2x_STOP = true;
    emit TextBrowser_NewMessage(tr("Trying to stop, please wait..."));
    QFuture<void> f1 = QtConcurrent::run(this, &MainWindow::Wait_waifu2x_stop);
}
/*
等待处理线程完全停止
*/
void MainWindow::Wait_waifu2x_stop()
{
    while(true)
    {
        if(waifu2x_STOP_confirm||ThreadNumRunning==0)
        {
            waifu2x_STOP_confirm = false;
            emit TextBrowser_NewMessage(tr("Processing of files has stopped."));
            break;
        }
        Delay_msec_sleep(300);
    }
    Waifu2xMain.cancel();
    emit Send_Waifu2x_Finished_manual();
}
/*
从tableview移除item
*/
int MainWindow::on_pushButton_RemoveItem_clicked()
{
    ui->pushButton_RemoveItem->setEnabled(0);
    if(curRow_image==-1&&curRow_video==-1&&curRow_gif==-1)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("No items are currently selected."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
        ui->pushButton_RemoveItem->setEnabled(1);
        return 0;
    }
    RecFinedFiles();
    if(curRow_image >= 0)
    {
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_image, curRow_image);
        if(!fileMap.isEmpty())
        {
            CustRes_remove(fileMap["SourceFile_fullPath"]);
            FileList_remove(fileMap);
            Table_FileList_reload();
        }
        else
        {
            Table_FileList_reload();
        }
    }
    ui->tableView_image->clearSelection();
    //============================================================
    if(curRow_video >= 0)
    {
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_video, curRow_video);
        if(!fileMap.isEmpty())
        {
            CustRes_remove(fileMap["SourceFile_fullPath"]);
            FileList_remove(fileMap);
            Table_FileList_reload();
        }
        else
        {
            Table_FileList_reload();
        }
    }
    ui->tableView_video->clearSelection();
    //============================================================
    if(curRow_gif >= 0)
    {
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_gif, curRow_gif);
        if(!fileMap.isEmpty())
        {
            CustRes_remove(fileMap["SourceFile_fullPath"]);
            FileList_remove(fileMap);
            Table_FileList_reload();
        }
        else
        {
            Table_FileList_reload();
        }
    }
    ui->tableView_gif->clearSelection();
    //==================================================
    if(!ui->checkBox_ReProcFinFiles->checkState())
    {
        MovToFinedList();
    }
    //=====================
    curRow_image = -1;
    curRow_gif = -1;
    curRow_video = -1;
    //======================
    if(Table_model_gif->rowCount()==0)
    {
        FileList_gif.clear();
        FileList_gif_finished.clear();
        ui->tableView_gif->setVisible(0);
    }
    if(Table_model_image->rowCount()==0)
    {
        FileList_image.clear();
        FileList_image_finished.clear();
        ui->tableView_image->setVisible(0);
    }
    if(Table_model_video->rowCount()==0)
    {
        FileList_video.clear();
        FileList_video_finished.clear();
        ui->tableView_video->setVisible(0);
    }
    if(Table_model_gif->rowCount()==0&&Table_model_image->rowCount()==0&&Table_model_video->rowCount()==0)
    {
        Table_Clear();
        Custom_resolution_list.clear();
        ui->label_DropFile->setVisible(1);
        ui->pushButton_ClearList->setVisible(0);
        ui->pushButton_RemoveItem->setVisible(0);
        ui->label_FileCount->setVisible(0);
    }
    Table_FileCount_reload();
    ui->pushButton_RemoveItem->setEnabled(1);
    return 0;
}

void MainWindow::on_checkBox_ReProcFinFiles_stateChanged(int arg1)
{
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();
    if(ReProcFinFiles)
    {
        RecFinedFiles();
    }
    else
    {
        MovToFinedList();
    }
}
//====== 自动关机===================================================================================
/*
60s倒计时
*/
int MainWindow::SystemShutDown_Countdown()
{
    Delay_sec_sleep(60);
    emit Send_SystemShutDown();
    return 0;
}
/*
关机
保存列表,生成关机标志,关机
*/
bool MainWindow::SystemShutDown()
{
    on_pushButton_SaveFileList_clicked();
    QString AutoShutDown = Current_Path+"/AutoShutDown";
    QFile file(AutoShutDown);
    file.open(QIODevice::WriteOnly);
    file.close();
    Delay_msec_sleep(50);
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    //获取进程标志
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    //获取关机特权的LUID
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,    &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    //获取这个进程的关机特权
    AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS) return false;
    // 强制关闭计算机
    if ( !ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
        return false;
    return true;
}
/*
判断上次软件启动后是否执行了自动关机
*/
int MainWindow::SystemShutDown_isAutoShutDown()
{
    QString AutoShutDown = Current_Path+"/AutoShutDown";
    QString Table_FileList_ini = Current_Path+"/Table_FileList.ini";
    if(file_isFileExist(AutoShutDown)&&file_isFileExist(Table_FileList_ini))
    {
        QFile::remove(AutoShutDown);
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Notification"));
        MSG->setText(tr("It was detected that the program executed an automatic shutdown of the computer when it was last run. The last File List was automatically saved before the shutdown. You can manually load the File List to view the file processing status."));
        MSG->setIcon(QMessageBox::Information);
        MSG->setModal(true);
        MSG->show();
    }
    return 0;
}
//==========================================================
/*
============= 不安全的非阻塞延时 =====================
*/
void MainWindow::Delay_sec(int time)
{
    QTime dieTime = QTime::currentTime().addSecs(time);
    while( QTime::currentTime() < dieTime ) QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::Delay_msec(int time)
{
    QTime dieTime = QTime::currentTime().addMSecs(time);
    while( QTime::currentTime() < dieTime ) QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
//==========================================================
/*
============= 安全的阻塞延时 =====================
*/
void MainWindow::Delay_sec_sleep(int time)
{
    QThread::sleep(time);
}

void MainWindow::Delay_msec_sleep(int time)
{
    QThread::msleep(time);
}
//==========================================================

/*
播放提示音
*/
void MainWindow::Play_NFSound()
{
    QString NFSound = Current_Path+"/NotificationSound_waifu2xExtension.mp3";
    QMediaPlayer *player = new QMediaPlayer;
    player->setMedia(QUrl::fromLocalFile(NFSound));
    player->play();
}


void MainWindow::on_checkBox_SaveAsJPG_stateChanged(int arg1)
{
    if(ui->checkBox_SaveAsJPG->checkState())
    {
        ui->checkBox_CompressJPG->setEnabled(1);
    }
    else
    {
        ui->checkBox_CompressJPG->setEnabled(0);
    }
}

void MainWindow::on_pushButton_donate_clicked()
{
    emit Send_TextBrowser_NewMessage(tr("Thank you! :)"));
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/Donate_page.md"));
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_Report_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/issues/new"));
}

void MainWindow::on_pushButton_ReadMe_clicked()
{
    switch(ui->comboBox_language->currentIndex())
    {
        case 0:
            {
                QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/README.md"));
                return;
            }
        case 1:
            {
                QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/README_CN.md"));
                return;
            }
    }
}
/*
添加手动输入路径
*/
void MainWindow::on_pushButton_AddPath_clicked()
{
    QString Input_path = ui->lineEdit_inputPath->text();
    Input_path = Input_path.trimmed();
    if(Input_path=="")return;
    Input_path = Input_path.replace("\\","/");
    Input_path = Input_path.replace("\\\\","/");
    Input_path = Input_path.replace("//","/");
    if(Input_path.right(1)=="/")
    {
        Input_path = Input_path.left(Input_path.length() - 1);
    }
    if(QFile::exists(Input_path))
    {
        AddNew_gif=false;
        AddNew_image=false;
        AddNew_video=false;
        if(ui->checkBox_ScanSubFolders->checkState())
        {
            Add_File_Folder_IncludeSubFolder(Input_path);
        }
        else
        {
            Add_File_Folder(Input_path);
        }
    }
    else
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Error"));
        MSG->setText(tr("Input path does not exist."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
        return;
    }
    if(AddNew_gif==false&&AddNew_image==false&&AddNew_video==false)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("The file format is not supported, please enter supported file format, or add more file extensions yourself."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
    }
    else
    {
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
    }
    ui->tableView_gif->scrollToBottom();
    ui->tableView_image->scrollToBottom();
    ui->tableView_video->scrollToBottom();
    AddNew_image=false;
    AddNew_image=false;
    AddNew_video=false;
    Table_FileCount_reload();
}

void MainWindow::on_comboBox_Engine_Image_currentIndexChanged(int index)
{
    switch(ui->comboBox_Engine_Image->currentIndex())
    {
        case 0:
            {
                ui->spinBox_DenoiseLevel_image->setRange(-1,3);
                ui->spinBox_DenoiseLevel_image->setValue(2);
                ui->spinBox_DenoiseLevel_image->setEnabled(1);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Range:-1(No noise reduction)~3"));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~3"));
                return;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_image->setRange(0,3);
                ui->spinBox_DenoiseLevel_image->setValue(2);
                ui->spinBox_DenoiseLevel_image->setEnabled(1);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Range:0(No noise reduction)~3"));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Range:0(No noise reduction)~3"));
                return;
            }
    }
}

void MainWindow::on_comboBox_Engine_GIF_currentIndexChanged(int index)
{
    switch(ui->comboBox_Engine_GIF->currentIndex())
    {
        case 0:
            {
                ui->spinBox_DenoiseLevel_gif->setRange(-1,3);
                ui->spinBox_DenoiseLevel_gif->setValue(2);
                ui->spinBox_DenoiseLevel_gif->setEnabled(1);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Range:-1(No noise reduction)~3"));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~3"));
                return;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_gif->setRange(0,3);
                ui->spinBox_DenoiseLevel_gif->setValue(2);
                ui->spinBox_DenoiseLevel_gif->setEnabled(1);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Range:0(No noise reduction)~3"));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Range:0(No noise reduction)~3"));
                return;
            }
    }
}

void MainWindow::on_comboBox_Engine_Video_currentIndexChanged(int index)
{
    switch(ui->comboBox_Engine_Video->currentIndex())
    {
        case 0:
            {
                ui->spinBox_DenoiseLevel_video->setRange(-1,3);
                ui->spinBox_DenoiseLevel_video->setValue(2);
                ui->spinBox_DenoiseLevel_video->setEnabled(1);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Range:-1(No noise reduction)~3"));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~3"));
                return;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_video->setRange(0,3);
                ui->spinBox_DenoiseLevel_video->setValue(2);
                ui->spinBox_DenoiseLevel_video->setEnabled(1);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Range:0(No noise reduction)~3"));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Range:0(No noise reduction)~3"));
                return;
            }
        case 2:
            {
                ui->spinBox_DenoiseLevel_video->setRange(-1,0);
                ui->spinBox_DenoiseLevel_video->setValue(-1);
                ui->spinBox_DenoiseLevel_video->setEnabled(0);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Anime4K engine does not support noise reduction."));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Anime4K engine does not support noise reduction."));
                return;
            }
    }
}

void MainWindow::on_pushButton_clear_textbrowser_clicked()
{
    ui->textBrowser->clear();
    TextBrowser_StartMes();
}

void MainWindow::on_spinBox_textbrowser_fontsize_valueChanged(int arg1)
{
    int size = ui->spinBox_textbrowser_fontsize->value();
    ui->textBrowser->setStyleSheet("font: "+QString::number(size,10)+"pt \"Arial\";");
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::on_pushButton_compatibilityTest_clicked()
{
    ui->pushButton_Start->setEnabled(0);
    ui->pushButton_compatibilityTest->setEnabled(0);
    ui->pushButton_DetectGPU->setEnabled(0);
    ui->pushButton_DumpProcessorList_converter->setEnabled(0);
    if(!ui->textBrowser->isVisible())
    {
        on_pushButton_HideTextBro_clicked();
    }
    QtConcurrent::run(this, &MainWindow::Waifu2x_Compatibility_Test);
}

void MainWindow::on_pushButton_CustRes_apply_clicked()
{
    ui->pushButton_CustRes_apply->setEnabled(0);
    CustRes_SetCustRes();
    ui->pushButton_CustRes_apply->setEnabled(1);
}

void MainWindow::on_pushButton_CustRes_cancel_clicked()
{
    ui->pushButton_CustRes_cancel->setEnabled(0);
    CustRes_CancelCustRes();
    ui->pushButton_CustRes_cancel->setEnabled(1);
}

void MainWindow::on_pushButton_HideSettings_clicked()
{
    if(ui->groupBox_Setting->isVisible())
    {
        ui->groupBox_Setting->setVisible(0);
        ui->pushButton_HideSettings->setText(tr("Show settings"));
    }
    else
    {
        ui->groupBox_Setting->setVisible(1);
        ui->pushButton_HideSettings->setText(tr("Hide settings"));
    }
}

void MainWindow::on_pushButton_HideInput_clicked()
{
    if(ui->groupBox_Input->isVisible())
    {
        ui->groupBox_OutPut->setVisible(0);
        ui->groupBox_Input->setVisible(0);
        ui->pushButton_HideInput->setText(tr("Show Path Settings"));
    }
    else
    {
        ui->groupBox_OutPut->setVisible(1);
        ui->groupBox_Input->setVisible(1);
        ui->pushButton_HideInput->setText(tr("Hide Path Settings"));
    }
}


void MainWindow::on_pushButton_DetectGPU_clicked()
{
    ui->pushButton_Start->setEnabled(0);
    ui->pushButton_DetectGPU->setEnabled(0);
    ui->pushButton_DumpProcessorList_converter->setEnabled(0);
    ui->pushButton_compatibilityTest->setEnabled(0);
    Available_GPUID.clear();
    QtConcurrent::run(this, &MainWindow::Waifu2x_DetectGPU);
}

void MainWindow::on_comboBox_GPUID_currentIndexChanged(int index)
{
    if(ui->comboBox_GPUID->currentText()!="auto")
    {
        GPU_ID_STR = " -g "+ui->comboBox_GPUID->currentText()+" ";
    }
    else
    {
        GPU_ID_STR="";
    }
}
/*
改变语言设置
*/
void MainWindow::on_comboBox_language_currentIndexChanged(int index)
{
    QString qmFilename;
    QString runPath = qApp->applicationDirPath();
    switch(ui->comboBox_language->currentIndex())
    {
        case 0:
            {
                qmFilename = runPath + "/language_English.qm";
                break;
            }
        case 1:
            {
                qmFilename = runPath + "/language_Chinese.qm";
                break;
            }
    }
    if (translator->load(qmFilename))
    {
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        Table_FileCount_reload();
        Init_Table();
        Set_Font_fixed();
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error: Language files cannot be loaded properly."));
    }
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
    this->setAcceptDrops(0);//禁止drop file
    ui->pushButton_Start->setEnabled(0);//禁用start button
    ui->groupBox_Input->setEnabled(0);
    ui->pushButton_ClearList->setEnabled(0);
    ui->pushButton_RemoveItem->setEnabled(0);
    ui->checkBox_ReProcFinFiles->setEnabled(0);
    ui->pushButton_CustRes_cancel->setEnabled(0);
    ui->pushButton_CustRes_apply->setEnabled(0);
    ui->pushButton_ReadFileList->setEnabled(0);
    ui->pushButton_SaveFileList->setEnabled(0);
    emit Send_TextBrowser_NewMessage(tr("Write to the file, please wait."));
    Table_Save_Current_Table_Filelist();
    QtConcurrent::run(this, &MainWindow::Table_Save_Current_Table_Filelist_Watchdog);
}

void MainWindow::on_pushButton_ReadFileList_clicked()
{
    QString Table_FileList_ini = Current_Path+"/Table_FileList.ini";
    if(file_isFileExist(Table_FileList_ini))
    {
        this->setAcceptDrops(0);//禁止drop file
        ui->pushButton_Start->setEnabled(0);//禁用start button
        ui->groupBox_Input->setEnabled(0);
        ui->pushButton_ClearList->setEnabled(0);
        ui->pushButton_RemoveItem->setEnabled(0);
        ui->checkBox_ReProcFinFiles->setEnabled(0);
        ui->pushButton_CustRes_cancel->setEnabled(0);
        ui->pushButton_CustRes_apply->setEnabled(0);
        ui->pushButton_ReadFileList->setEnabled(0);
        ui->pushButton_SaveFileList->setEnabled(0);
        curRow_image = -1;
        curRow_gif = -1;
        curRow_video = -1;
        Table_Clear();
        FileList_image.clear();
        FileList_gif.clear();
        FileList_video.clear();
        FileList_image_finished.clear();
        FileList_gif_finished.clear();
        FileList_video_finished.clear();
        Custom_resolution_list.clear();
        ui->label_DropFile->setVisible(1);
        ui->tableView_gif->setVisible(0);
        ui->tableView_image->setVisible(0);
        ui->tableView_video->setVisible(0);
        ui->pushButton_ClearList->setVisible(0);
        ui->pushButton_RemoveItem->setVisible(0);
        Table_FileCount_reload();
        Send_TextBrowser_NewMessage(tr("Please wait while reading the file."));
        QtConcurrent::run(this, &MainWindow::Table_Read_Saved_Table_Filelist);
    }
    else
    {
        Send_TextBrowser_NewMessage(tr("Cannot find the saved Files List!"));
    }
}

void MainWindow::on_Ext_image_editingFinished()
{
    QString ext_image_str = ui->Ext_image->text();
    ext_image_str = ext_image_str.trimmed();
    ui->Ext_image->setText(ext_image_str);
}

void MainWindow::on_Ext_video_editingFinished()
{
    QString ext_video_str = ui->Ext_video->text();
    ext_video_str = ext_video_str.trimmed();
    ui->Ext_video->setText(ext_video_str);
}

int MainWindow::Donate_Count()
{
    QString donate_ini = Current_Path+"/donate.ini";
    if(!file_isFileExist(donate_ini))
    {
        QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
        configIniWrite->setValue("/Donate/OpenCount", 1);
        configIniWrite->setValue("/Donate/PopUp", 0);
        return 0;
    }
    QSettings *configIniRead = new QSettings(donate_ini, QSettings::IniFormat);
    if(configIniRead->value("/Donate/PopUp").toInt()==1)return 0;
    //=======  读取打开次数  ======
    int Open_count = configIniRead->value("/Donate/OpenCount").toInt();
    Open_count++;
    if(Open_count<5)
    {
        QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
        configIniWrite->setValue("/Donate/OpenCount", Open_count);
        return 0;
    }
    else
    {
        QtConcurrent::run(this, &MainWindow::Donate_watchdog);
        return 0;
    }
}

int MainWindow::Donate_watchdog()
{
    Delay_sec_sleep(5);
    emit Send_Donate_Notification();
    return 0;
}

int MainWindow::Donate_Notification()
{
    QMessageBox Msg(QMessageBox::Question, QString(tr("Notification")), QString(tr("Do you like using this software?\nIf you like the software, please donate to support the developers to ensure the software is continuously updated.\n(This popup will only pop up once after the software is installed.)")));
    Msg.setModal(false);
    QAbstractButton *pYesBtn = (QAbstractButton *)Msg.addButton(QString(tr("YES")), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
    Msg.exec();
    if (Msg.clickedButton() == pYesBtn)QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/Donate_page.md"));
    //=========
    QString donate_ini = Current_Path+"/donate.ini";
    QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
    configIniWrite->setValue("/Donate/PopUp", 1);
    //=========
    return 0;
}

void MainWindow::on_checkBox_autoCheckUpdate_clicked()
{
    if(ui->checkBox_autoCheckUpdate->checkState()==false)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("We do not recommend that you cancel the automatic check for updates as this may prevent you from receiving timely bug fixes."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
    }
}

void MainWindow::on_checkBox_AutoSaveSettings_clicked()
{
    QString settings_ini = Current_Path+"/settings.ini";
    if(file_isFileExist(settings_ini))
    {
        QSettings *configIniWrite = new QSettings(settings_ini, QSettings::IniFormat);
        configIniWrite->setValue("/settings/AutoSaveSettings", ui->checkBox_AutoSaveSettings->checkState());
    }
}

void MainWindow::on_pushButton_about_clicked()
{
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("About"));
    QString line1 = "Waifu2x-Extension-GUI\n\n";
    QString line2 = VERSION+"\n\n";
    QString line3 = "Github:https://github.com/AaronFeng753/Waifu2x-Extension-GUI\n\n";
    QString line4 = "Waifu2x-Extension-GUI is licensed under the\n";
    QString line5 = "GNU Affero General Public License v3.0\n\n";
    QString line6 = "Copyright (C) 2020  Aaron Feng";
    MSG->setText(line1+line2+line3+line4+line5+line6);
    QImage img(":/new/prefix1/icon/icon_main.png");
    QImage img_scaled = img.scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QPixmap pix(QPixmap::fromImage(img_scaled));
    MSG->setIconPixmap(pix);
    MSG->setModal(false);
    MSG->show();
}

void MainWindow::on_checkBox_AlwaysHideInput_stateChanged(int arg1)
{
    if(ui->checkBox_AlwaysHideInput->checkState())
    {
        ui->groupBox_Input->setVisible(0);
        ui->groupBox_OutPut->setVisible(0);
        ui->pushButton_HideInput->setText(tr("Show Path Settings"));
    }
}

void MainWindow::on_comboBox_AspectRatio_custRes_currentIndexChanged(int index)
{
    int CurrentIndex = ui->comboBox_AspectRatio_custRes->currentIndex();
    switch(CurrentIndex)
    {
        case 0:
            {
                CustRes_AspectRatioMode = Qt::IgnoreAspectRatio;
                break;
            }
        case 1:
            {
                CustRes_AspectRatioMode = Qt::KeepAspectRatio;
                break;
            }
        case 2:
            {
                CustRes_AspectRatioMode = Qt::KeepAspectRatioByExpanding;
                break;
            }
    }
}

void MainWindow::on_checkBox_AlwaysHideSettings_stateChanged(int arg1)
{
    if(ui->checkBox_AlwaysHideSettings->checkState())
    {
        ui->groupBox_Setting->setVisible(0);
        ui->pushButton_HideSettings->setText(tr("Show settings"));
    }
}

void MainWindow::on_spinBox_ThreadNum_gif_internal_valueChanged(int arg1)
{
    int Total=(ui->spinBox_ThreadNum_gif->value())*(ui->spinBox_ThreadNum_gif_internal->value());
    ui->label_TotalThreadNum_gif->setText(QString(tr("Total:%1")).arg(Total));
}

void MainWindow::on_spinBox_ThreadNum_gif_valueChanged(int arg1)
{
    int Total=(ui->spinBox_ThreadNum_gif->value())*(ui->spinBox_ThreadNum_gif_internal->value());
    ui->label_TotalThreadNum_gif->setText(QString(tr("Total:%1")).arg(Total));
}

void MainWindow::on_spinBox_ThreadNum_video_valueChanged(int arg1)
{
    int Total=(ui->spinBox_ThreadNum_video->value())*(ui->spinBox_ThreadNum_video_internal->value());
    ui->label_TotalThreadNum_video->setText(QString(tr("Total:%1")).arg(Total));
}

void MainWindow::on_spinBox_ThreadNum_video_internal_valueChanged(int arg1)
{
    int Total=(ui->spinBox_ThreadNum_video->value())*(ui->spinBox_ThreadNum_video_internal->value());
    ui->label_TotalThreadNum_video->setText(QString(tr("Total:%1")).arg(Total));
}

void MainWindow::on_pushButton_Save_GlobalFontSize_clicked()
{
    QString settings_ini = Current_Path+"/settings.ini";
    QSettings *configIniWrite = new QSettings(settings_ini, QSettings::IniFormat);
    configIniWrite->setValue("/settings/GlobalFontSize", ui->spinBox_GlobalFontSize->value());
}

void MainWindow::on_pushButton_BrowserFile_clicked()
{
    //========
    QStringList nameFilters;
    nameFilters.append("*.gif");
    QString Ext_image_str = ui->Ext_image->text();
    QStringList nameFilters_image = Ext_image_str.split(":");
    for(int i = 0; i < nameFilters_image.size(); ++i)
    {
        QString tmp = nameFilters_image.at(i);
        tmp = "*." + tmp;
        nameFilters.append(tmp);
    }
    QString Ext_video_str = ui->Ext_video->text();
    QStringList nameFilters_video = Ext_video_str.split(":");
    for(int i = 0; i < nameFilters_video.size(); ++i)
    {
        QString tmp = nameFilters_video.at(i);
        tmp = "*." + tmp;
        nameFilters.append(tmp);
    }
    QString nameFilters_QString = "";
    for(int i = 0; i < nameFilters.size(); ++i)
    {
        QString tmp = nameFilters.at(i);
        nameFilters_QString = nameFilters_QString +" "+ tmp;
    }
    //========
    QString Input_path = "";
    Input_path = QFileDialog::getOpenFileName(this, tr("Select file"), " ",  tr("All file(")+nameFilters_QString+")");
    Input_path = Input_path.trimmed();
    if(Input_path=="")return;
    //=====
    Input_path = Input_path.replace("\\","/");
    Input_path = Input_path.replace("\\\\","/");
    Input_path = Input_path.replace("//","/");
    if(Input_path.right(1)=="/")
    {
        Input_path = Input_path.left(Input_path.length() - 1);
    }
    //=======
    if(QFile::exists(Input_path))
    {
        AddNew_gif=false;
        AddNew_image=false;
        AddNew_video=false;
        Add_File_Folder(Input_path);
    }
    else
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Error"));
        MSG->setText(tr("Input path does not exist."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
        return;
    }
    if(AddNew_gif==false&&AddNew_image==false&&AddNew_video==false)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("The file format is not supported, please enter supported file format, or add more file extensions yourself."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
    }
    else
    {
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
打开wiki
*/
void MainWindow::on_pushButton_wiki_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/wiki"));
}
/*
最小化
*/
void MainWindow::on_pushButton_Minimize_clicked()
{
    if(ui->groupBox_Input->isVisible())on_pushButton_HideInput_clicked();
    if(ui->groupBox_Setting->isVisible())on_pushButton_HideSettings_clicked();
    if(ui->textBrowser->isVisible())on_pushButton_HideTextBro_clicked();
    this->adjustSize();
}

void MainWindow::on_pushButton_HideTextBro_clicked()
{
    if(ui->textBrowser->isVisible())
    {
        ui->textBrowser->setVisible(0);
        ui->pushButton_HideTextBro->setText(tr("Show Text Browser"));
    }
    else
    {
        ui->textBrowser->setVisible(1);
        ui->pushButton_HideTextBro->setText(tr("Hide Text Browser"));
    }
}

void MainWindow::on_checkBox_AlwaysHideTextBrowser_stateChanged(int arg1)
{
    if(ui->checkBox_AlwaysHideTextBrowser->checkState())
    {
        ui->textBrowser->setVisible(0);
        ui->pushButton_HideTextBro->setText(tr("Show Text Browser"));
    }
}

void MainWindow::on_pushButton_DumpProcessorList_converter_clicked()
{
    ui->pushButton_Start->setEnabled(0);
    ui->pushButton_DetectGPU->setEnabled(0);
    ui->pushButton_DumpProcessorList_converter->setEnabled(0);
    ui->pushButton_compatibilityTest->setEnabled(0);
    Available_ProcessorList_converter.clear();
    QtConcurrent::run(this, &MainWindow::Waifu2x_DumpProcessorList_converter);
}

int MainWindow::Waifu2x_DumpProcessorList_converter()
{
    emit Send_TextBrowser_NewMessage(tr("Detecting available Processor, please wait."));
    //===============
    QString InputPath = Current_Path + "/Compatibility_Test/Compatibility_Test.jpg";
    QString OutputPath = Current_Path + "/Compatibility_Test/res.jpg";
    QFile::remove(OutputPath);
    //==============
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter-cpp.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    //=========
    int Processor_ID=0;
    //=========
    while(true)
    {
        QFile::remove(OutputPath);
        QProcess *Waifu2x = new QProcess();
        QString Processor_str = " -p "+QString::number(Processor_ID,10)+" ";
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale-ratio 2 --noise-level 1 --model-dir " + "\"" + model_path + "\""+Processor_str;
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(100)&&!QProcess_stop) {}
        if(file_isFileExist(OutputPath))
        {
            Available_ProcessorList_converter.append(QString::number(Processor_ID,10));
            Processor_ID++;
            QFile::remove(OutputPath);
        }
        else
        {
            break;
        }
    }
    QFile::remove(OutputPath);
    //===============
    emit Send_TextBrowser_NewMessage(tr("Detection is complete!"));
    if(Available_ProcessorList_converter.isEmpty())
    {
        Send_TextBrowser_NewMessage(tr("No available Processor detected!"));
    }
    else
    {
        QProcess *Waifu2x = new QProcess();
        QString Processor_str = " -p "+QString::number(Processor_ID,10)+" ";
        QString cmd = "\"" + program + "\"" + " -l ";
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(100)&&!QProcess_stop) {}
        ui->textBrowser->append(Waifu2x->readAllStandardOutput());
    }
    emit Send_Waifu2x_DumpProcessorList_converter_finished();
    return 0;
}
int MainWindow::Waifu2x_DumpProcessorList_converter_finished()
{
    ui->pushButton_Start->setEnabled(1);
    ui->pushButton_DetectGPU->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_DumpProcessorList_converter->setEnabled(1);
    //====
    ui->comboBox_TargetProcessor_converter->clear();
    ui->comboBox_TargetProcessor_converter->addItem("auto");
    if(!Available_ProcessorList_converter.isEmpty())
    {
        for(int i=0; i<Available_ProcessorList_converter.size(); i++)
        {
            ui->comboBox_TargetProcessor_converter->addItem(Available_ProcessorList_converter.at(i));
        }
    }
    return 0;
}

void MainWindow::on_comboBox_TargetProcessor_converter_currentIndexChanged(int index)
{
    if(ui->comboBox_TargetProcessor_converter->currentText()!="auto")
    {
        Processor_converter_STR = " -p "+ui->comboBox_TargetProcessor_converter->currentText()+" ";
    }
    else
    {
        Processor_converter_STR="";
    }
}

void MainWindow::on_Ext_image_textChanged(const QString &arg1)
{
    QString lower = ui->Ext_image->text().toLower();
    ui->Ext_image->setText(lower);
}

void MainWindow::on_Ext_video_textChanged(const QString &arg1)
{
    QString lower = ui->Ext_video->text().toLower();
    ui->Ext_video->setText(lower);
}

void MainWindow::on_comboBox_model_vulkan_currentIndexChanged(int index)
{
    if(ui->comboBox_model_vulkan->currentIndex()==0)
    {
        ui->comboBox_ImageStyle->setEnabled(1);
    }
    if(ui->comboBox_model_vulkan->currentIndex()==1)
    {
        ui->comboBox_ImageStyle->setEnabled(0);
    }
}

void MainWindow::on_comboBox_ImageStyle_currentIndexChanged(int index)
{
    if(ui->comboBox_ImageStyle->currentIndex()==0)
    {
        ui->comboBox_model_vulkan->setEnabled(1);
    }
    if(ui->comboBox_ImageStyle->currentIndex()==1)
    {
        ui->comboBox_model_vulkan->setEnabled(0);
    }
}

void MainWindow::on_pushButton_ResetVideoSettings_clicked()
{
    ui->lineEdit_pixformat->setText("yuv420p");
    ui->lineEdit_encoder_vid->setText("libx264");
    ui->lineEdit_encoder_audio->setText("aac");
    ui->spinBox_bitrate_vid->setValue(6000);
    ui->spinBox_bitrate_audio->setValue(320);
    //====
    ui->spinBox_bitrate_vid_2mp4->setValue(2500);
    ui->spinBox_bitrate_audio_2mp4->setValue(320);
    ui->checkBox_acodec_copy_2mp4->setChecked(0);
    ui->checkBox_vcodec_copy_2mp4->setChecked(0);
    ui->spinBox_bitrate_vid_2mp4->setEnabled(1);
    ui->spinBox_bitrate_audio_2mp4->setEnabled(1);
}

void MainWindow::on_lineEdit_encoder_vid_textChanged(const QString &arg1)
{
    QString tmp = ui->lineEdit_encoder_vid->text().trimmed();
    ui->lineEdit_encoder_vid->setText(tmp);
}

void MainWindow::on_lineEdit_encoder_audio_textChanged(const QString &arg1)
{
    QString tmp = ui->lineEdit_encoder_audio->text().trimmed();
    ui->lineEdit_encoder_audio->setText(tmp);
}

void MainWindow::on_lineEdit_pixformat_textChanged(const QString &arg1)
{
    QString tmp = ui->lineEdit_pixformat->text().trimmed();
    ui->lineEdit_pixformat->setText(tmp);
}

void MainWindow::on_checkBox_vcodec_copy_2mp4_stateChanged(int arg1)
{
    if(ui->checkBox_vcodec_copy_2mp4->checkState())
    {
        ui->spinBox_bitrate_vid_2mp4->setEnabled(0);
    }
    else
    {
        ui->spinBox_bitrate_vid_2mp4->setEnabled(1);
    }
}

void MainWindow::on_checkBox_acodec_copy_2mp4_stateChanged(int arg1)
{
    if(ui->checkBox_acodec_copy_2mp4->checkState())
    {
        ui->spinBox_bitrate_audio_2mp4->setEnabled(0);
    }
    else
    {
        ui->spinBox_bitrate_audio_2mp4->setEnabled(1);
    }
}


void MainWindow::on_pushButton_encodersList_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/EncodersList.txt"));
}

void MainWindow::Tip_FirstTimeStart()
{
    QString FirstTimeStart = Current_Path+"/FirstTimeStart";
    if(file_isFileExist(FirstTimeStart))
    {
        return;
    }
    else
    {
        QString English_1 = "- Run a compatibility test to check which engines your PC is compatible with before starting to process files.\n";
        QString English_2 = "- If the [block size] or [Number of threads] or [Scale ratio] is too large, the software may crash because insufficient VRAM or RAM.\n";
        QString English_3 = "- Adjusting the number of threads and block size can increase processing speed. The number of threads and block size your computer can support depends on your computer's CPU, GPU, RAM and VRAM.\n";
        QString English_4 = "- If the generated GIF file is too large, you can enable \"Optimize .gif\" to reduce the file size.\n";
        QString English_5 = "- The software needs to split the video into pictures before process the video, so please make sure you have enough hard disk space.\n";
        QString English_6 = "- You can further adjust the picture quality and sound quality of the output video in additional settings.\n";
        QString English_7 = "- If there is a problem with the software font display, you can modify the font in the additional settings.\n";
        QString English_8 = "- This software is free software, if you find anyone selling this software, please report the seller.\n";
        QString English_9 = "- This software is free and open source, and is is licensed under the GNU Affero General Public License v3.0. All consequences of using this software are borne by the user, and the developer does not bear any responsibility.\n";
        QString English_10 = "- If you like this software, please donate to the developer, thank you.\n";
        //========
        QString Chinese_1 = "- 开始处理文件前先运行一次兼容性测试以检查您的PC与哪些引擎兼容.\n";
        QString Chinese_2 = "- 如果[块大小]或[线程数量]或[放大倍数]过大,软件可能因为RAM或VRAM不足而崩溃.\n";
        QString Chinese_3 = "- 调整线程数量和块大小可以提高处理速度. 你的电脑能支持的线程数量和块大小取决于你的电脑的CPU, GPU, RAM和VRAM.\n";
        QString Chinese_4 = "- 如果生成的GIF文件体积过大, 可以启用\"优化 .gif\"以降低文件体积.\n";
        QString Chinese_5 = "- 软件在放大视频前需要先将视频文件拆分成图片, 所以请确保您有足够的硬盘空间.\n";
        QString Chinese_6 = "- 您可以在附加设置进一步调整输出视频的画质和音质.\n";
        QString Chinese_7 = "- 如果软件字体显示有问题, 您可以在附加设置内修改字体.\n";
        QString Chinese_8 = "- 本软件为免费软件, 如果您发现任何人贩售本软件, 请举报贩售者.\n";
        QString Chinese_9 = "- 在附加设置内可以将语言调整为简体中文.\n";
        QString Chinese_10 = "- 本软件免费开源, 并基于 GNU Affero General Public License v3.0 协议发行, 使用本软件的一切后果均由用户自己承担, 开发者不承担任何责任.\n";
        QString Chinese_11 = "- 如果您喜欢本软件, 请向开发者捐赠, 谢谢.\n";
        //========
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle("!!! Tips 必读 !!!");
        MSG->setText(English_1+English_2+English_3+English_4+English_5+English_6+English_7+English_8+English_9+English_10+"----------------------\n"+Chinese_1+Chinese_2+Chinese_3+Chinese_4+Chinese_5+Chinese_6+Chinese_7+Chinese_8+Chinese_9+Chinese_10+Chinese_11);
        MSG->setIcon(QMessageBox::Information);
        MSG->setModal(true);
        MSG->show();
        //======
        QFile file(FirstTimeStart);
        file.open(QIODevice::WriteOnly);
        file.close();
    }
}

void MainWindow::on_pushButton_showTips_clicked()
{
    QString FirstTimeStart = Current_Path+"/FirstTimeStart";
    QFile::remove(FirstTimeStart);
    Tip_FirstTimeStart();
}

void MainWindow::on_checkBox_DelOriginal_stateChanged(int arg1)
{
    if(ui->checkBox_DelOriginal->checkState())
    {
        ui->checkBox_Move2RecycleBin->setEnabled(1);
    }
    else
    {
        ui->checkBox_Move2RecycleBin->setEnabled(0);
    }
}
/*
是否启用自定义视频设置
*/
void MainWindow::on_checkBox_videoSettings_isEnabled_stateChanged(int arg1)
{
    if(ui->checkBox_videoSettings_isEnabled->checkState())
    {
        ui->tabWidget_videoSettings->setEnabled(1);
    }
    else
    {
        ui->tabWidget_videoSettings->setEnabled(0);
    }
}

void MainWindow::on_checkBox_FileList_Interactive_stateChanged(int arg1)
{
    if(ui->checkBox_FileList_Interactive->checkState())
    {
        ui->tableView_image->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->tableView_gif->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->tableView_video->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    }
    else
    {
        ui->tableView_image->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_gif->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_video->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
}

void MainWindow::on_checkBox_OutPath_isEnabled_stateChanged(int arg1)
{
    if(ui->checkBox_OutPath_isEnabled->checkState())
    {
        ui->lineEdit_outputPath->setEnabled(1);
    }
    else
    {
        ui->lineEdit_outputPath->setEnabled(0);
    }
}

void MainWindow::on_checkBox_old_vulkan_stateChanged(int arg1)
{
    if(ui->checkBox_old_vulkan->checkState())
    {
        ui->checkBox_TTA_vulkan->setEnabled(0);
        ui->checkBox_TTA_vulkan->setChecked(0);
    }
    else
    {
        ui->checkBox_TTA_vulkan->setEnabled(1);
    }
}

void MainWindow::on_checkBox_TTA_vulkan_stateChanged(int arg1)
{
    if(ui->checkBox_TTA_vulkan->checkState())
    {
        ui->checkBox_old_vulkan->setEnabled(0);
        ui->checkBox_old_vulkan->setChecked(0);
    }
    else
    {
        ui->checkBox_old_vulkan->setEnabled(1);
    }
}

void MainWindow::on_pushButton_ForceRetry_clicked()
{
    int tmp = ui->spinBox_retry->value();
    tmp++;
    ui->spinBox_retry->setValue(tmp);
    //========
    QProcess Close;
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-ncnn-vulkan.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-converter-cpp.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    //========
    emit Send_TextBrowser_NewMessage(tr("Force retry."));
    return;
}
