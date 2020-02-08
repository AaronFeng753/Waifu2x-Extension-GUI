#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDragEnterEvent>
#include <QMimeData>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QDir>
#include <QList>
#include <QMap>
#include <QtConcurrent>
#include <QProcess>
#include <QFutureWatcher>
#include <QImage>
#include <QImageWriter>
#include <QFile>
#include <QVariant>
#include <windows.h>
#include <QTime>
#include <QMediaPlayer>
#include <QDesktopServices>
#include <QSize>
#include <QMovie>
#include <QTimer>
#include <QTextCursor>
#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QScreen>
#include <QCloseEvent>
#include <windows.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);

    //=======================
    QString VERSION="v0.32-beta";//软件版本号
    //=======================
    QTranslator * translator;//界面翻译
    //=======
    void Set_Font_fixed();
    //=================================  File 文件=================================
    void dragEnterEvent(QDragEnterEvent *event);//拖放文件event
    void dropEvent(QDropEvent *event);
    bool AddNew_gif=false;//判断是否有新增文件-gif
    bool AddNew_image=false;//判断是否有新增文件-图片
    bool AddNew_video=false;//判断是否有新增文件-视频
    void Add_File_Folder(QString Full_Path);//添加文件or文件夹(判断一个路径是文件还是文件夹,然后处理判断类型添加到table和file list)
    QStringList getFileNames(QString path);//当拖入的路径是文件夹时,读取文件夹内指定扩展名的文件并返回一个qstringlist
    int FileList_Add(QString fileName, QString SourceFile_fullPath);//直接向file list和tableview添加文件

    //待处理的filelist
    QList<QMap<QString, QString>> FileList_image;//map["SourceFile_fullPath"],map["rowNum"]
    QList<QMap<QString, QString>> FileList_gif;
    QList<QMap<QString, QString>> FileList_video;
    //处理完成的filelist(当重复处理已完成没启用时,将会把处理完的条目缓存在这里)
    QList<QMap<QString, QString>> FileList_image_finished;//map["SourceFile_fullPath"],map["rowNum"]
    QList<QMap<QString, QString>> FileList_gif_finished;
    QList<QMap<QString, QString>> FileList_video_finished;
    //判断一个文件是否已存在于一个文件列表中(输入list和完整路径,然后判断返回bool)
    bool Deduplicate_filelist(QList<QMap<QString, QString>> FileList, QString SourceFile_fullPath);
    //输入一个filemap,然后查询在哪个list内,找到后移除
    int FileList_remove(QMap<QString, QString> File_map);
    //根据行号在filelist内查找条目(map)
    QMap<QString, QString> FileList_find_rowNum(QList<QMap<QString, QString>> FileList, int rowNum);

    void RecFinedFiles();//将[已完成列表]接回对应的File list

    bool file_isDirExist(QString SourceFile_fullPath);//判断文件夹是否存在
    void file_mkDir(QString SourceFile_fullPath);//创建文件夹
    bool file_isFileExist(QString fullFilePath);//判断文件是否存在
    void file_copyFile(QString sourceDir, QString toDir, bool coverFileIfExist);//复制文件
    QStringList file_getFileNames_in_Folder_nofilter(QString path);//读取文件夹内文件列表并返回(无过滤器)
    bool file_DelDir(const QString &path);//删除文件夹(无论是否为空,强制删除)

    //=================================  Table =================================
    void Init_Table();
    QStandardItemModel *Table_model_image = new QStandardItemModel();
    QStandardItemModel *Table_model_video = new QStandardItemModel();
    QStandardItemModel *Table_model_gif = new QStandardItemModel();

    void Table_image_CustRes_Cancel_rowNumInt(int rowNum);
    void Table_gif_CustRes_Cancel_rowNumInt(int rowNum);
    void Table_video_CustRes_Cancel_rowNumInt(int rowNum);

    void Table_ChangeAllStatusToWaiting();
    void Table_Clear();

    int Table_image_get_rowNum();
    int Table_gif_get_rowNum();
    int Table_video_get_rowNum();

    void Table_FileList_reload();

    QMap<QString, QString> Table_Read_status_fullpath(QStandardItemModel *Table_model);

    int curRow_image = -1;
    int curRow_gif = -1;
    int curRow_video = -1;

    int Table_Save_Current_Table_Filelist();

    int Table_Read_Current_Table_Filelist();

    int Table_Save_Current_Table_Filelist_Watchdog();



    //================================= Waifu2x ====================================
    int Waifu2xMainThread();
    int Waifu2x_NCNN_Vulkan_Image(QMap<QString, QString> File_map);

    int Waifu2x_NCNN_Vulkan_GIF(QMap<QString, QString> File_map);
    int Waifu2x_NCNN_Vulkan_GIF_scale(QString Frame_fileName,QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning);

    int Waifu2x_NCNN_Vulkan_Video(QMap<QString, QString> File_map);
    int Waifu2x_NCNN_Vulkan_Video_scale(QString Frame_fileName,QMap<QString, QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning);

    int Anime4k_Video(QMap<QString, QString> File_map);
    int Anime4k_Video_scale(QString Frame_fileName,QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning);

    int Waifu2x_Converter_Image(QMap<QString, QString> File_map);

    int Waifu2x_Converter_GIF(QMap<QString, QString> File_map);
    int Waifu2x_Converter_GIF_scale(QString Frame_fileName,QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning);

    int Waifu2x_Converter_Video(QMap<QString, QString> File_map);
    int Waifu2x_Converter_Video_scale(QString Frame_fileName,QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning);


    void Wait_waifu2x_stop();

    bool waifu2x_STOP = false;
    bool waifu2x_STOP_confirm = false;

    int ThreadNumMax = 0;
    int ThreadNumRunning = 0;

    int TaskNumTotal=0;
    int TaskNumFinished=0;
    bool NewTaskFinished=false;
    int ETA=0;

    int Waifu2x_Compatibility_Test();

    int Waifu2x_DetectGPU();
    QStringList Available_GPUID;
    QString GPU_ID_STR="";



    //================================ progressbar ===================================
    int Progressbar_MaxVal = 0;
    int Progressbar_CurrentVal = 0;
    void progressbar_clear();
    void progressbar_SetToMax(int maxval);

    //=============================== textbrowser===============================
    void TextBrowser_StartMes();

    //================================ gif ====================================
    int Gif_getDuration(QString gifPath);
    int Gif_getFrameDigits(QString gifPath);
    void Gif_splitGif(QString gifPath,QString SplitFramesFolderPath);
    void Gif_assembleGif(QString ResGifPath,QString ScaledFramesPath,int Duration);
    void Gif_compressGif(QString gifPath,QString gifPath_compressd);

    //================================= video ===============================
    int video_get_fps(QString videoPath);
    int video_get_frameNumDigits(QString videoPath);
    void video_video2images(QString VideoPath,QString FrameFolderPath,QString AudioPath);
    int video_images2video(QString VideoPath,QString video_mp4_scaled_fullpath,QString ScaledFrameFolderPath,QString AudioPath);

    //============================   custom res  ====================================
    QList<QMap<QString, QString>> Custom_resolution_list;//res_map["fullpath"],["height"],["width"]
    void CustRes_remove(QString fullpath);
    bool CustRes_isContained(QString fullpath);
    QMap<QString, QString> CustRes_getResMap(QString fullpath);
    int CustRes_CalNewScaleRatio(QString fullpath,int Height_new,int width_new);
    int CustRes_SetCustRes();
    int CustRes_CancelCustRes();
    int CustRes_SetToScreenRes();

    Qt::AspectRatioMode CustRes_AspectRatioMode = Qt::IgnoreAspectRatio;

    //======================== 设置 ===========================================
    int Settings_Save();
    int Settings_Read_Apply();
    bool Settings_isReseted = false;


    //================================ Other =======================================
    int SystemShutDown_Countdown();

    void Delay_sec(int time);
    void Delay_msec(int time);

    void Delay_sec_sleep(int time);
    void Delay_msec_sleep(int time);

    void Play_NFSound();

    QTimer *TimeCostTimer;
    int TimeCost = 0;
    QString Seconds2hms(int seconds);

    int CheckUpadte_Auto();
    int SystemShutDown_isAutoShutDown();

    int Donate_Count();
    int Donate_watchdog();

    ~MainWindow();

    //=========== 关闭窗口时执行的代码 ===============
    void closeEvent(QCloseEvent* event);
    bool QProcess_stop=false;
    int Auto_Save_Settings_Watchdog();
    QFuture<int> AutoUpdate;
    QFuture<int> Waifu2xMain;
    int Force_close();
    //=============================================

public slots:
    void progressbar_setRange_min_max(int min, int max);
    void progressbar_Add();

    void Table_image_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
    void Table_gif_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
    void Table_video_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);

    void Waifu2x_Finished();
    void Waifu2x_Finished_manual();

    void TextBrowser_NewMessage(QString message);

    void TimeSlot();

    int Waifu2x_Compatibility_Test_finished();
    int Waifu2x_DetectGPU_finished();

    int CheckUpadte_NewUpdate(QString update_str);

    int Table_FileCount_reload();

    void MovToFinedList();

    void Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);

    void Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);

    int Table_Read_Current_Table_Filelist_Finished();
    int Table_Save_Current_Table_Filelist_Finished();

    void on_pushButton_ClearList_clicked();

    bool SystemShutDown();

    int Donate_Notification();

    int Auto_Save_Settings_Finished();




private slots:

    void on_pushButton_Start_clicked();


    void on_pushButton_Stop_clicked();

    int on_pushButton_RemoveItem_clicked();

    void on_checkBox_ReProcFinFiles_stateChanged(int arg1);

    void on_checkBox_SaveAsJPG_stateChanged(int arg1);

    void on_pushButton_donate_clicked();

    void on_pushButton_CheckUpdate_clicked();

    void on_pushButton_Report_clicked();

    void on_pushButton_ReadMe_clicked();

    void on_pushButton_AddPath_clicked();

    void on_comboBox_Engine_Image_currentIndexChanged(int index);

    void on_comboBox_Engine_GIF_currentIndexChanged(int index);

    void on_comboBox_Engine_Video_currentIndexChanged(int index);

    void on_pushButton_clear_textbrowser_clicked();

    void on_spinBox_textbrowser_fontsize_valueChanged(int arg1);

    void on_pushButton_compatibilityTest_clicked();


    void on_tableView_image_clicked(const QModelIndex &index);

    void on_tableView_gif_clicked(const QModelIndex &index);

    void on_tableView_video_clicked(const QModelIndex &index);

    void on_pushButton_CustRes_apply_clicked();

    void on_pushButton_CustRes_cancel_clicked();

    void on_pushButton_HideSettings_clicked();

    void on_pushButton_HideInput_clicked();


    void on_pushButton_DetectGPU_clicked();

    void on_comboBox_GPUID_currentIndexChanged(int index);

    void on_pushButton_SaveSettings_clicked();

    void on_pushButton_ResetSettings_clicked();

    void on_comboBox_language_currentIndexChanged(int index);

    void on_pushButton_SaveFileList_clicked();

    void on_pushButton_ReadFileList_clicked();

    void on_Ext_image_editingFinished();

    void on_Ext_video_editingFinished();

    void on_checkBox_autoCheckUpdate_clicked();

    void on_checkBox_AutoSaveSettings_clicked();

    void on_pushButton_about_clicked();

    void on_checkBox_AlwaysHideInput_stateChanged(int arg1);

    void on_comboBox_AspectRatio_custRes_currentIndexChanged(int index);

    void on_checkBox_AlwaysHideSettings_stateChanged(int arg1);

    void on_spinBox_ThreadNum_gif_internal_valueChanged(int arg1);

    void on_spinBox_ThreadNum_gif_valueChanged(int arg1);

    void on_spinBox_ThreadNum_video_valueChanged(int arg1);

    void on_spinBox_ThreadNum_video_internal_valueChanged(int arg1);

    void on_pushButton_Save_GlobalFontSize_clicked();

signals:
    void Send_PrograssBar_Range_min_max(int, int);
    void Send_progressbar_Add();

    void Send_Table_image_ChangeStatus_rowNumInt_statusQString(int, QString);
    void Send_Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString);
    void Send_Table_video_ChangeStatus_rowNumInt_statusQString(int, QString);

    void Send_Waifu2x_Finished();
    void Send_Waifu2x_Finished_manual();

    void Send_TextBrowser_NewMessage(QString);

    void Send_Waifu2x_Compatibility_Test_finished();

    void Send_Waifu2x_DetectGPU_finished();

    void Send_CheckUpadte_NewUpdate(QString update_str);

    void Send_Table_FileCount_reload();

    void Send_MovToFinedList();

    void Send_Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Send_Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Send_Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);

    void Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);

    void Send_Table_Read_Current_Table_Filelist_Finished();
    void Send_Table_Save_Current_Table_Filelist_Finished();

    void Send_SystemShutDown();

    void Send_Donate_Notification();

    void Send_Auto_Save_Settings_Finished();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

