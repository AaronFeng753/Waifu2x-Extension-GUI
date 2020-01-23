#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    //===============================================================
    ui->pushButton_Stop->setEnabled(0);
    connect(this, SIGNAL(Send_PrograssBar_Range_min_max(int, int)), this, SLOT(progressbar_setRange_min_max(int, int)));
    connect(this, SIGNAL(Send_progressbar_Add()), this, SLOT(progressbar_Add()));
    connect(this, SIGNAL(Send_Table_image_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_image_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_video_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_video_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Waifu2x_Finished()), this, SLOT(Waifu2x_Finished()));
    connect(this, SIGNAL(Send_Waifu2x_Finished_manual()), this, SLOT(Waifu2x_Finished_manual()));
    connect(this, SIGNAL(Send_TextBrowser_NewMessage(QString)), this, SLOT(TextBrowser_NewMessage(QString)));
    connect(this, SIGNAL(Send_Waifu2x_Compatibility_Test_finished()), this, SLOT(Waifu2x_Compatibility_Test_finished()));
    TimeCostTimer = new QTimer();
    connect(TimeCostTimer, SIGNAL(timeout()), this, SLOT(TimeSlot()));
    //==================================================
    on_comboBox_Engine_GIF_currentIndexChanged(0);
    on_comboBox_Engine_Image_currentIndexChanged(0);
    on_comboBox_Engine_Video_currentIndexChanged(0);
    on_spinBox_textbrowser_fontsize_valueChanged(0);
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
    Table_Clear();
    FileList_image.clear();
    FileList_gif.clear();
    FileList_video.clear();
    FileList_image_finished.clear();
    FileList_gif_finished.clear();
    FileList_video_finished.clear();
    ui->label_DropFile->setVisible(1);
    ui->tableView_gif->setVisible(0);
    ui->tableView_image->setVisible(0);
    ui->tableView_video->setVisible(0);
    ui->pushButton_ClearList->setVisible(0);
    ui->pushButton_RemoveItem->setVisible(0);
}

void MainWindow::on_pushButton_Start_clicked()
{
    if(FileList_image.isEmpty()&&FileList_gif.isEmpty()&&FileList_video.isEmpty())
    {
        emit Send_TextBrowser_NewMessage("Unable to start processing files: The file list is empty or there are no available files to process.");
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
        progressbar_clear();
        TimeCostTimer->start(1000);
        TimeCost=0;
        QtConcurrent::run(this, &MainWindow::Waifu2xMainThread);//启动waifu2x 主线程
    }
}

void MainWindow::on_pushButton_Stop_clicked()
{
    TimeCostTimer->stop();
    ui->pushButton_Stop->setEnabled(0);//禁用stop button
    ui->pushButton_Start->setEnabled(1);//启用start button
    waifu2x_STOP = true;
    emit TextBrowser_NewMessage("Trying to stop, please wait...");
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
            emit TextBrowser_NewMessage("The process has stopped.");
            break;
        }
        Delay_msec_sleep(500);
    }
    emit Send_Waifu2x_Finished_manual();
}

void MainWindow::on_pushButton_RemoveItem_clicked()
{
    int curRow_image = ui->tableView_image->currentIndex().row();
    if(curRow_image >= 0)
    {
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_image, curRow_image);
        if(!fileMap.isEmpty())
        {
            FileList_remove(fileMap);
            Table_FileList_reload();
        }
    }
    ui->tableView_image->clearSelection();
    //============================================================
    int curRow_video = ui->tableView_video->currentIndex().row();
    if(curRow_video >= 0)
    {
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_video, curRow_video);
        if(!fileMap.isEmpty())
        {
            FileList_remove(fileMap);
            Table_FileList_reload();
        }
    }
    ui->tableView_video->clearSelection();
    //============================================================
    int curRow_gif = ui->tableView_gif->currentIndex().row();
    if(curRow_gif >= 0)
    {
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_gif, curRow_gif);
        if(!fileMap.isEmpty())
        {
            FileList_remove(fileMap);
            Table_FileList_reload();
        }
    }
    ui->tableView_gif->clearSelection();
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

bool MainWindow::SystemShutDown()
{
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
    emit Send_TextBrowser_NewMessage("Thank you! :)");
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI#donate"));
}

void MainWindow::on_pushButton_CheckUpdate_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest"));
}

void MainWindow::on_pushButton_Report_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/issues/new"));
}

void MainWindow::on_pushButton_ReadMe_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/README.md"));
}

void MainWindow::on_pushButton_AddPath_clicked()
{
    ui->label_DropFile->setVisible(0);//隐藏文件投放label
    ui->tableView_gif->setVisible(1);
    ui->tableView_image->setVisible(1);
    ui->tableView_video->setVisible(1);
    ui->pushButton_ClearList->setVisible(1);
    ui->pushButton_RemoveItem->setVisible(1);
    QString Input_path = ui->lineEdit_inputPath->text();
    Input_path = Input_path.replace("\\","/");
    Input_path = Input_path.trimmed();
    if(QFile::exists(Input_path))
    {
        Add_File_Folder(Input_path);
    }
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
                ui->spinBox_DenoiseLevel_image->setToolTip("Range:-1(No noise reduction)~3");
                ui->label_ImageDenoiseLevel->setToolTip("Range:-1(No noise reduction)~3");
                break;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_image->setRange(0,2);
                ui->spinBox_DenoiseLevel_image->setValue(2);
                ui->spinBox_DenoiseLevel_image->setEnabled(1);
                ui->spinBox_DenoiseLevel_image->setToolTip("Range:0(No noise reduction)~2");
                ui->label_ImageDenoiseLevel->setToolTip("Range:0(No noise reduction)~2");
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
                ui->spinBox_DenoiseLevel_gif->setToolTip("Range:-1(No noise reduction)~3");
                ui->label_GIFDenoiseLevel->setToolTip("Range:-1(No noise reduction)~3");
                break;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_gif->setRange(0,2);
                ui->spinBox_DenoiseLevel_gif->setValue(2);
                ui->spinBox_DenoiseLevel_gif->setEnabled(1);
                ui->spinBox_DenoiseLevel_gif->setToolTip("Range:0(No noise reduction)~2");
                ui->label_GIFDenoiseLevel->setToolTip("Range:0(No noise reduction)~2");
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
                ui->spinBox_DenoiseLevel_video->setToolTip("Range:-1(No noise reduction)~3");
                ui->label_VideoDenoiseLevel->setToolTip("Range:-1(No noise reduction)~3");
                break;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_video->setRange(0,2);
                ui->spinBox_DenoiseLevel_video->setValue(2);
                ui->spinBox_DenoiseLevel_video->setEnabled(1);
                ui->spinBox_DenoiseLevel_video->setToolTip("Range:0(No noise reduction)~2");
                ui->label_VideoDenoiseLevel->setToolTip("Range:0(No noise reduction)~2");
                break;
            }
        case 2:
            {
                ui->spinBox_DenoiseLevel_video->setRange(-1,0);
                ui->spinBox_DenoiseLevel_video->setValue(-1);
                ui->spinBox_DenoiseLevel_video->setEnabled(0);
                ui->spinBox_DenoiseLevel_video->setToolTip("Anime4K engine does not support noise reduction.");
                ui->label_VideoDenoiseLevel->setToolTip("Anime4K engine does not support noise reduction.");
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
}

void MainWindow::on_pushButton_compatibilityTest_clicked()
{
    ui->pushButton_Start->setEnabled(0);
    QtConcurrent::run(this, &MainWindow::Waifu2x_Compatibility_Test);
}
