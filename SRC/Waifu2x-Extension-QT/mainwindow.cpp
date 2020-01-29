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
    //===========================================
    translator = new QTranslator(this);
    TextBrowser_StartMes();
    this->setAcceptDrops(true);
    Init_Table();
    ui->checkBox_CompressJPG->setEnabled(0);
    //=================== 初始隐藏所有table和按钮 ======================
    ui->tableView_image->setVisible(0);
    ui->tableView_gif->setVisible(0);
    ui->tableView_video->setVisible(0);
    ui->pushButton_ClearList->setVisible(0);
    ui->pushButton_RemoveItem->setVisible(0);
    Table_FileCount_reload();
    //===============================================================
    ui->pushButton_Stop->setEnabled(0);
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
    connect(this, SIGNAL(Send_Table_Read_Current_Table_Filelist_Finished()), this, SLOT(Table_Read_Current_Table_Filelist_Finished()));
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
    //======
    TimeCostTimer = new QTimer();
    connect(TimeCostTimer, SIGNAL(timeout()), this, SLOT(TimeSlot()));
    //==================================================
    on_comboBox_Engine_GIF_currentIndexChanged(0);
    on_comboBox_Engine_Image_currentIndexChanged(0);
    on_comboBox_Engine_Video_currentIndexChanged(0);
    on_spinBox_textbrowser_fontsize_valueChanged(0);
    //=====================================
    Settings_Read_Apply();
    //=====================================
    QtConcurrent::run(this, &MainWindow::CheckUpadte_Auto);
    SystemShutDown_isAutoShutDown();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TimeSlot()
{
    TimeCost++;
    QString TimeCostStr = "Time cost:["+Seconds2hms(TimeCost)+"]";
    ui->label_TimeCost->setText(TimeCostStr);
    int TaskNumFinished_tmp = TaskNumFinished;
    int TimeCost_tmp = TimeCost;
    int TaskNumTotal_tmp = TaskNumTotal;
    if(TaskNumFinished_tmp>0&&TimeCost_tmp>0&&TaskNumTotal_tmp>0)
    {
        if(NewTaskFinished)
        {
            NewTaskFinished=false;
            int avgTimeCost = TimeCost/TaskNumFinished_tmp;
            ETA = int(avgTimeCost*(TaskNumTotal_tmp-TaskNumFinished_tmp));
        }
        else
        {
            if(ETA>1)
            {
                ETA--;
            }
        }
        QString TimeRemainingStr = "Time remaining:["+Seconds2hms(ETA)+"]";
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
QString MainWindow::Seconds2hms(int seconds)
{
    if(seconds<=0)return "0:0:0";
    int mm = 60;
    int hh = mm * 60;
    int hour = seconds / hh;
    int min = (seconds-hour*hh)/mm;
    int sec = seconds - hour*hh - min*mm;
    return QString::number(hour,10)+":"+QString::number(min,10)+":"+QString::number(sec,10);
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
}

void MainWindow::on_pushButton_Start_clicked()
{
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
        progressbar_clear();
        TimeCostTimer->start(1000);
        TimeCost=0;
        emit Send_TextBrowser_NewMessage(tr("Start processing files."));
        QtConcurrent::run(this, &MainWindow::Waifu2xMainThread);//启动waifu2x 主线程
    }
}

void MainWindow::on_pushButton_Stop_clicked()
{
    TimeCostTimer->stop();
    ui->pushButton_Stop->setEnabled(0);//禁用stop button
    ui->pushButton_Start->setEnabled(1);//启用start button
    waifu2x_STOP = true;
    emit TextBrowser_NewMessage(tr("Trying to stop, please wait..."));
    QFuture<void> f1 = QtConcurrent::run(this, &MainWindow::Wait_waifu2x_stop);//启动waifu2x 主线程
}

void MainWindow::Wait_waifu2x_stop()
{
    while(true)
    {
        if(waifu2x_STOP_confirm||ThreadNumRunning==0)
        {
            waifu2x_STOP_confirm = false;
            waifu2x_STOP = false;
            emit TextBrowser_NewMessage(tr("Processing of files has stopped."));
            break;
        }
        Delay_msec_sleep(500);
    }
    emit Send_Waifu2x_Finished_manual();
}

int MainWindow::on_pushButton_RemoveItem_clicked()
{
    if(curRow_image==-1&&curRow_video==-1&&curRow_gif==-1)
    {
        QMessageBox::information(this,tr("Error"),tr("No items are currently selected."));
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
        ui->label_FileCount->setVisible(0);
    }
    Table_FileCount_reload();
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
int MainWindow::SystemShutDown_Countdown()
{
    Delay_sec_sleep(60);
    emit Send_SystemShutDown();
}

bool MainWindow::SystemShutDown()
{
    on_pushButton_SaveFileList_clicked();
    QString Current_Path = qApp->applicationDirPath();
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

int MainWindow::SystemShutDown_isAutoShutDown()
{
    QString Current_Path = qApp->applicationDirPath();
    QString AutoShutDown = Current_Path+"/AutoShutDown";
    if(file_isFileExist(AutoShutDown))
    {
        QFile::remove(AutoShutDown);
        QMessageBox::information(this,tr("Notification"),tr("It was detected that the program executed an automatic shutdown of the computer when it was last run. The last File List was automatically saved before the shutdown. You can manually load the File List to view the file processing status."));
    }
    return 0;
}

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

void MainWindow::Delay_sec_sleep(int time)
{
    QThread::sleep(time);
}

void MainWindow::Delay_msec_sleep(int time)
{
    QThread::msleep(time);
}

void MainWindow::Play_NFSound()
{
    QString Current_Path = qApp->applicationDirPath();
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
                break;
            }
        case 1:
            {
                QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/README_CN.md"));
                break;
            }
    }
}

void MainWindow::on_pushButton_AddPath_clicked()
{
    QString Input_path = ui->lineEdit_inputPath->text();
    Input_path = Input_path.replace("\\","/");
    Input_path = Input_path.trimmed();
    if(QFile::exists(Input_path))
    {
        AddNew_gif=false;
        AddNew_image=false;
        AddNew_video=false;
        Add_File_Folder(Input_path);
    }
    if(AddNew_gif==false&&AddNew_image==false&&AddNew_video==false)
    {
        QMessageBox::information(this,tr("Error"),tr("The file format is not supported, please enter supported file format, or add more file extensions yourself."));
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
                break;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_image->setRange(0,2);
                ui->spinBox_DenoiseLevel_image->setValue(2);
                ui->spinBox_DenoiseLevel_image->setEnabled(1);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Range:0(No noise reduction)~2"));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Range:0(No noise reduction)~2"));
                break;
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
                break;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_gif->setRange(0,2);
                ui->spinBox_DenoiseLevel_gif->setValue(2);
                ui->spinBox_DenoiseLevel_gif->setEnabled(1);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Range:0(No noise reduction)~2"));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Range:0(No noise reduction)~2"));
                break;
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
                break;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_video->setRange(0,2);
                ui->spinBox_DenoiseLevel_video->setValue(2);
                ui->spinBox_DenoiseLevel_video->setEnabled(1);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Range:0(No noise reduction)~2"));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Range:0(No noise reduction)~2"));
                break;
            }
        case 2:
            {
                ui->spinBox_DenoiseLevel_video->setRange(-1,0);
                ui->spinBox_DenoiseLevel_video->setValue(-1);
                ui->spinBox_DenoiseLevel_video->setEnabled(0);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Anime4K engine does not support noise reduction."));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Anime4K engine does not support noise reduction."));
                break;
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
    QtConcurrent::run(this, &MainWindow::Waifu2x_Compatibility_Test);
}

void MainWindow::on_tableView_image_clicked(const QModelIndex &index)
{
    curRow_image = ui->tableView_image->currentIndex().row();
    curRow_gif = -1;
    curRow_video = -1;
    ui->tableView_gif->clearSelection();
    ui->tableView_video->clearSelection();
}

void MainWindow::on_tableView_gif_clicked(const QModelIndex &index)
{
    curRow_image = -1;
    curRow_gif = ui->tableView_gif->currentIndex().row();
    curRow_video = -1;
    ui->tableView_image->clearSelection();
    ui->tableView_video->clearSelection();
}

void MainWindow::on_tableView_video_clicked(const QModelIndex &index)
{
    curRow_image = -1;
    curRow_gif = -1;
    curRow_video = ui->tableView_video->currentIndex().row();
    ui->tableView_image->clearSelection();
    ui->tableView_gif->clearSelection();
}

void MainWindow::on_pushButton_CustRes_apply_clicked()
{
    CustRes_SetCustRes();
}

void MainWindow::on_pushButton_CustRes_cancel_clicked()
{
    CustRes_CancelCustRes();
}

void MainWindow::on_pushButton_HideSettings_clicked()
{
    if(ui->groupBox_Setting->isVisible())
    {
        ui->groupBox_Setting->setVisible(0);
        ui->pushButton_HideSettings->setText(tr("Show settings"));
        ui->pushButton_HideSettings->setToolTip(tr("Show all setting options."));
    }
    else
    {
        ui->groupBox_Setting->setVisible(1);
        ui->pushButton_HideSettings->setText(tr("Hide settings"));
        ui->pushButton_HideSettings->setToolTip(tr("Hide all setting options."));
    }
}

void MainWindow::on_pushButton_HideInput_clicked()
{
    if(ui->groupBox_Input->isVisible())
    {
        ui->groupBox_Input->setVisible(0);
        ui->pushButton_HideInput->setText(tr("Show input path"));
    }
    else
    {
        ui->groupBox_Input->setVisible(1);
        ui->pushButton_HideInput->setText(tr("Hide input path"));
    }
}


void MainWindow::on_pushButton_DetectGPU_clicked()
{
    ui->pushButton_Start->setEnabled(0);
    ui->pushButton_DetectGPU->setEnabled(0);
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
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error: Language files cannot be loaded properly."));
    }
}
void MainWindow::on_pushButton_SaveFileList_clicked()
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
    emit Send_TextBrowser_NewMessage(tr("Write to the file, please wait."));
    Table_Save_Current_Table_Filelist();
    QtConcurrent::run(this, &MainWindow::Table_Save_Current_Table_Filelist_Watchdog);
}

void MainWindow::on_pushButton_ReadFileList_clicked()
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
    QString Current_Path = qApp->applicationDirPath();
    QString Table_FileList_ini = Current_Path+"/Table_FileList.ini";
    if(file_isFileExist(Table_FileList_ini))
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
        Send_TextBrowser_NewMessage(tr("Please wait while reading the file."));
        QtConcurrent::run(this, &MainWindow::Table_Read_Current_Table_Filelist);
    }
    else
    {
        Send_TextBrowser_NewMessage(tr("Cannot find the saved Files List!"));
        Send_Table_Read_Current_Table_Filelist_Finished();
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
