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
#include <QFileDialog>
#include <QTextCodec>
#include <math.h>
#include <QMutex>

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
    QString VERSION = "v2.22.01-beta";//软件版本号
    //=======================
    QTranslator * translator;//界面翻译
    //=======
    QString Current_Path = qApp->applicationDirPath();//当前路径
    //=======
    void Set_Font_fixed();
    //=================================  File 文件=================================
    void dragEnterEvent(QDragEnterEvent *event);//拖放文件event
    void dropEvent(QDropEvent *event);
    void Read_urls(QList<QUrl> urls);

    bool AddNew_gif=false;//判断是否有新增文件-gif
    bool AddNew_image=false;//判断是否有新增文件-图片
    bool AddNew_video=false;//判断是否有新增文件-视频
    void Add_File_Folder(QString Full_Path);//添加文件or文件夹(判断一个路径是文件还是文件夹,然后处理判断类型添加到table和file list)
    void Add_File_Folder_IncludeSubFolder(QString Full_Path);//添加文件文件夹(扫描子文件夹
    QStringList getFileNames(QString path);//当拖入的路径是文件夹时,读取文件夹内指定扩展名的文件并返回一个qstringlist
    QStringList getFileNames_IncludeSubFolder(QString path);//读取文件列表, 包括文件夹
    int FileList_Add(QString fileName, QString SourceFile_fullPath);//直接向file list和tableview添加文件

    //判断一个文件是否已存在于一个文件列表中(输入list和完整路径,然后判断返回bool)
    bool Deduplicate_filelist(QString SourceFile_fullPath);

    bool file_isDirExist(QString SourceFile_fullPath);//判断文件夹是否存在
    void file_mkDir(QString SourceFile_fullPath);//创建文件夹
    bool file_isFileExist(QString fullFilePath);//判断文件是否存在
    //void file_copyFile(QString sourceDir, QString toDir, bool coverFileIfExist);//复制文件
    QStringList file_getFileNames_in_Folder_nofilter(QString path);//读取文件夹内文件列表并返回(无过滤器)
    bool file_DelDir(const QString &path);//删除文件夹(无论是否为空,强制删除)
    QString file_getBaseName(QString path);//获取basename

    void file_MoveToTrash( QString file );//移动到回收站

    void file_MoveFile(QString Orginal,QString Target);//移动文件

    QString file_getFolderPath(QFileInfo fileInfo);//获取文件夹路径(去除末尾的"/")

    bool file_isDirWritable(QString DirPath);//检查文件夹是否可写入

    //检查当前行的文件所在的文件夹是否可写入
    bool file_isFilesFolderWritable_row_image(int rowNum);
    bool file_isFilesFolderWritable_row_video(int rowNum);
    bool file_isFilesFolderWritable_row_gif(int rowNum);
    //=================================  Table =================================
    void Init_Table();//初始化三个tableview
    QStandardItemModel *Table_model_image = new QStandardItemModel();
    QStandardItemModel *Table_model_video = new QStandardItemModel();
    QStandardItemModel *Table_model_gif = new QStandardItemModel();
    //取消指定row的自定义分辨率
    void Table_image_CustRes_Cancel_rowNumInt(int rowNum);
    void Table_gif_CustRes_Cancel_rowNumInt(int rowNum);
    void Table_video_CustRes_Cancel_rowNumInt(int rowNum);

    void Table_ChangeAllStatusToWaiting();//将所有row的状态改为waiting
    void Table_Clear();//清空tableview
    //获取下一个row值(用于插入新数据
    int Table_image_get_rowNum();
    int Table_gif_get_rowNum();
    int Table_video_get_rowNum();

    //根据输入的table,返回成对的[完整路径]和[状态]:[fullpath]=status;
    QMap<QString, QString> Table_Read_status_fullpath(QStandardItemModel *Table_model);
    //当前选中的rowNum
    int curRow_image = -1;
    int curRow_gif = -1;
    int curRow_video = -1;

    int Table_Save_Current_Table_Filelist();//保存当前文件列表(包括table

    int Table_Read_Saved_Table_Filelist();//读取保存的文件列表

    int Table_Save_Current_Table_Filelist_Watchdog();//保存文件列表的看门狗线程,确保文件成功保存

    bool Table_insert_finished=false;
    QMutex mutex_Table_insert_finished;//监管是否完成表格插入的bool值的mutex

    QMutex mutex_Table_insert;//监管表格插入的mutex
    //================================= Waifu2x ====================================
    QString OutPutFolder_main="";//总输出文件夹
    int Waifu2xMainThread();//waifu2x总线程,负责读取文件列表,调度waifu2x放大线程
    //==========================
    QString Waifu2x_ncnn_vulkan_FolderPath = "";
    QString Waifu2x_ncnn_vulkan_ProgramPath = "";
    //===
    int Waifu2x_NCNN_Vulkan_Image(int rowNum);//vulkan放大图片线程
    //vulakn放大GIF线程:1.主线程,拆分,调度放大子线程,组装&压缩;2.放大子线程,负责放大所有帧以及调整大小
    int Waifu2x_NCNN_Vulkan_GIF(int rowNum);
    int Waifu2x_NCNN_Vulkan_GIF_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    //vulkan放大视频线程:1.主线程,拆分,调度放大子线程,组装;2.放大子线程,负责放大所有帧以及调整大小
    int Waifu2x_NCNN_Vulkan_Video(int rowNum);
    int Waifu2x_NCNN_Vulkan_Video_BySegment(int rowNum);
    int Waifu2x_NCNN_Vulkan_Video_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed);
    //=========================
    int Anime4k_Image(int rowNum);
    int Anime4k_GIF(int rowNum);
    int Anime4k_GIF_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    //Anime4k放大视频线程:1.主线程,拆分,调度放大子线程,组装;2.放大子线程,负责放大所有帧以及调整大小
    int Anime4k_Video(int rowNum);
    int Anime4k_Video_BySegment(int rowNum);
    int Anime4k_Video_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed);
    QString Anime4k_ReadSettings();
    //=================================
    int Waifu2x_Converter_Image(int rowNum);//Converter放大图片线程
    //Converter放大GIF线程:1.主线程,拆分,调度放大子线程,组装&压缩;2.放大子线程,负责放大所有帧以及调整大小
    int Waifu2x_Converter_GIF(int rowNum);
    int Waifu2x_Converter_GIF_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    //Converter放大视频线程:1.主线程,拆分,调度放大子线程,组装;2.放大子线程,负责放大所有帧以及调整大小
    int Waifu2x_Converter_Video(int rowNum);
    int Waifu2x_Converter_Video_BySegment(int rowNum);
    int Waifu2x_Converter_Video_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed);
    //===================================
    int SRMD_NCNN_Vulkan_Image(int rowNum);//SRMD放大图片线程
    //SRMD放大GIF线程:1.主线程,拆分,调度放大子线程,组装&压缩;2.放大子线程,负责放大所有帧以及调整大小
    int SRMD_NCNN_Vulkan_GIF(int rowNum);
    int SRMD_NCNN_Vulkan_GIF_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    //SRMD放大视频线程:1.主线程,拆分,调度放大子线程,组装;2.放大子线程,负责放大所有帧以及调整大小
    int SRMD_NCNN_Vulkan_Video(int rowNum);
    int SRMD_NCNN_Vulkan_Video_BySegment(int rowNum);
    int SRMD_NCNN_Vulkan_Video_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed);

    void Wait_waifu2x_stop();//等待waifu2x主线程完全停止所有子线程的看门狗线程
    bool waifu2x_STOP = false;//负责通知waifu2x主线程及其子线程的停止信号
    bool waifu2x_STOP_confirm = false;//返回给waifu2x停止看门狗的信号

    int ThreadNumMax = 0;//waifu2x放大线程最大值
    int ThreadNumRunning = 0;//正在运行的waifu2x线程数量

    QMutex mutex_ThreadNumRunning;//监管总线程数量的mutex
    QMutex mutex_SubThreadNumRunning;//监管内部线程数量的mutex

    int Waifu2x_Compatibility_Test();//引擎兼容性检测

    int Waifu2x_DetectGPU();//检测可用gpu(for vulkan)
    QStringList Available_GPUID;//可用GPU ID列表
    QString GPU_ID_STR="";//向vulkan命令行cmd插入的gpuid命令,如果auto则为空

    int Waifu2x_DumpProcessorList_converter();
    int Core_num = 0;
    QStringList Available_ProcessorList_converter;
    QString Processor_converter_STR="";

    int SRMD_DetectGPU();//检测可用gpu(for srmd)
    QStringList Available_GPUID_srmd;//可用GPU ID列表
    QString GPU_ID_STR_SRMD="";//向srmd命令行cmd插入的gpuid命令,如果auto则为空

    bool Imgae_hasAlphaChannel(int rowNum);

    //兼容性检测
    bool isCompatible_Waifu2x_NCNN_Vulkan_OLD=false;
    bool isCompatible_Waifu2x_NCNN_Vulkan_NEW=false;
    bool isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P=false;
    bool isCompatible_SRMD_NCNN_Vulkan=false;
    bool isCompatible_Waifu2x_Converter=false;
    bool isCompatible_Anime4k_CPU=false;
    bool isCompatible_Anime4k_GPU=false;

    bool isCompatible_PythonExtension=false;
    bool isCompatible_FFmpeg=false;
    bool isCompatible_FFprobe=false;
    bool isCompatible_ImageMagick=false;
    bool isCompatible_Gifsicle=false;
    bool isCompatible_SoX=false;
    //================================ progressbar ===================================
    int Progressbar_MaxVal = 0;//进度条最大值
    int Progressbar_CurrentVal = 0;//进度条当前值
    void progressbar_clear();//清空进度条
    void progressbar_SetToMax(int maxval);//将进度条设定到最大值

    //=============================== textbrowser===============================
    void TextBrowser_StartMes();//输出启动信息

    //================================ gif ====================================
    int Gif_getDuration(QString gifPath);//获取帧间隔时长
    int Gif_getFrameDigits(QString gifPath);//获取帧数量的位数
    void Gif_splitGif(QString gifPath,QString SplitFramesFolderPath);//拆分gif
    void Gif_assembleGif(QString ResGifPath,QString ScaledFramesPath,int Duration,bool CustRes_isEnabled,int CustRes_height,int CustRes_width);//组装gif
    void Gif_compressGif(QString gifPath,QString gifPath_compressd);//压缩gif
    //================================= video ===============================
    QString video_get_fps(QString videoPath);//获取视频fps
    int video_get_frameNumDigits(QString videoPath);//获取帧数量的位数
    //拆分视频
    void video_video2images(QString VideoPath,QString FrameFolderPath,QString AudioPath);
    //组装视频
    int video_images2video(QString VideoPath,QString video_mp4_scaled_fullpath,QString ScaledFrameFolderPath,QString AudioPath,bool CustRes_isEnabled,int CustRes_height,int CustRes_width);
    //获取视频比特率
    QString video_get_bitrate(QString videoPath);
    //获取视频比特率(根据分辨率计算)
    QString video_get_bitrate_AccordingToRes(QString ScaledFrameFolderPath);
    //音频降噪
    QString video_AudioDenoise(QString OriginalAudioPath);
    //获取时长(秒)
    int video_get_duration(QString videoPath);
    //转换为mp4
    void video_2mp4(QString VideoPath);
    //提取音频
    void video_get_audio(QString VideoPath,QString AudioPath);
    //拆分视频(分段)
    void video_video2images_ProcessBySegment(QString VideoPath,QString FrameFolderPath,int StartTime,int SegmentDuration);
    //组装视频(mp4片段到成片)
    void video_AssembleVideoClips(QString VideoClipsFolderPath,QString VideoClipsFolderName,QString video_mp4_scaled_fullpath,QString AudioPath);
    //生成视频片段文件夹编号
    QString video_getClipsFolderNo();
    QMutex getClipsFolderNo_mutex;
    //============================   custom res  ====================================
    //自定义分辨率列表
    QList<QMap<QString, QString>> Custom_resolution_list;//res_map["fullpath"],["height"],["width"]
    void CustRes_remove(QString fullpath);//根据完整路径,移除自定义分辨率内条目
    bool CustRes_isContained(QString fullpath);//检查是否包括某文件
    QMap<QString, QString> CustRes_getResMap(QString fullpath);//读取指定文件的自定义分辨率值
    int CustRes_CalNewScaleRatio(QString fullpath,int Height_new,int width_new);//计算新的放大倍数
    int CustRes_SetCustRes();//设定自定义分辨率
    int CustRes_CancelCustRes();//取消自定义分辨率
    int CustRes_SetToScreenRes();//将自定义分辨率值设定为屏幕分辨率

    Qt::AspectRatioMode CustRes_AspectRatioMode = Qt::IgnoreAspectRatio;//自定义分辨率的纵横比策略
    //======================== 设置 ===========================================
    int Settings_Read_Apply();//读取与apply设置
    void Settings_Apply();
    bool Settings_isReseted = false;//是否重置设置标记
    //================================ Other =======================================
    int SystemShutDown_Countdown();//自动关机倒计时
    int SystemShutDown_isAutoShutDown();//判断之前是否执行过自动关机
    //阻塞延时(安全
    void Delay_sec_sleep(int time);
    void Delay_msec_sleep(int time);

    void Play_NFSound();//播放提示音

    QTimer *TimeCostTimer;//计算耗时的timer
    long unsigned int TimeCost = 0;//已消耗时间
    QString Seconds2hms(long unsigned int seconds);//秒 转 时:分:秒
    int TaskNumTotal=0;//总任务数量(需要处理的文件)
    int TaskNumFinished=0;//处理完的文件数量
    bool NewTaskFinished=false;//新任务被完成之标记
    long unsigned int ETA=0;//ETA时间(s)

    int CheckUpadte_Auto();//自动检查更新

    int Donate_Count();//计算启动次数判断是否弹窗
    //=========== 关闭窗口时执行的代码 ===============
    void closeEvent(QCloseEvent* event);//关闭事件,包含所有关闭时执行的代码
    bool QProcess_stop=false;//所有QProcess停止标记
    int Auto_Save_Settings_Watchdog();//自动保存设置的看门狗
    QFuture<int> AutoUpdate;//监视自动检查更新线程
    QFuture<int> Waifu2xMain;//监视waifu2x主线程
    int Force_close();//调用cmd强制关闭自己
    //================== 处理当前文件的进度 =========================
    long unsigned int TimeCost_CurrentFile =0;
    int TaskNumTotal_CurrentFile=0;
    int TaskNumFinished_CurrentFile=0;
    bool NewTaskFinished_CurrentFile=false;
    long unsigned int ETA_CurrentFile=0;
    bool isStart_CurrentFile=false;
    //=============================================
    void Tip_FirstTimeStart();
    //=============
    ~MainWindow();



public slots:
    void progressbar_setRange_min_max(int min, int max);//进度条设定min和max
    void progressbar_Add();//进度条进度+1

    //根据row修改指定row的状态
    void Table_image_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
    void Table_gif_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
    void Table_video_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);

    void Waifu2x_Finished();//自动结束时调用的代码(会自动接着调用manual
    void Waifu2x_Finished_manual();//手动停止后调用的结束代码

    void TextBrowser_NewMessage(QString message);//Textbrowser发送新消息"[时间] 消息"

    void TimeSlot();//计时槽函数

    int Waifu2x_Compatibility_Test_finished();//兼容性检测结束后执行的槽函数

    int Waifu2x_DetectGPU_finished();//检测可用gpu结束后的执行的槽函数

    int CheckUpadte_NewUpdate(QString update_str,QString Change_log);//检测到更新的弹窗代码

    int Table_FileCount_reload();//重载table下的文件数量计数

    //向table插入文件名和fullpath
    void Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);

    //向table插入自定义分辨率值
    void Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);

    //读取&保存文件列表与table后执行的代码
    int Table_Read_Saved_Table_Filelist_Finished();
    int Table_Save_Current_Table_Filelist_Finished();

    void on_pushButton_ClearList_clicked();//清空列表

    bool SystemShutDown();//关机

    int Waifu2x_DumpProcessorList_converter_finished();

    void Read_urls_finfished();

    void SRMD_DetectGPU_finished();

    void AutoDetectAlphaChannel_setChecked(bool Checked_);

    void video_write_VideoConfiguration(QString VideoConfiguration_fullPath,int ScaleRatio,int DenoiseLevel,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,QString EngineName,bool isProcessBySegment,QString VideoClipsFolderPath,QString VideoClipsFolderName);

    int Settings_Save();//保存设置

    //存储进度
    void video_write_Progress_ProcessBySegment(QString VideoConfiguration_fullPath,int StartTime,bool isSplitComplete,bool isScaleComplete);

    //================== 处理当前文件的进度 =========================
    void CurrentFileProgress_Start(QString FileName,int FrameNum);
    void CurrentFileProgress_Stop();
    void CurrentFileProgress_progressbar_Add();
    void CurrentFileProgress_progressbar_Add_SegmentDuration(int SegmentDuration);

private slots:

    void on_pushButton_Start_clicked();


    void on_pushButton_Stop_clicked();

    int on_pushButton_RemoveItem_clicked();


    void on_checkBox_SaveAsJPG_stateChanged(int arg1);

    void on_pushButton_CheckUpdate_clicked();

    void on_pushButton_Report_clicked();

    void on_pushButton_ReadMe_clicked();

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

    void on_pushButton_DetectGPU_clicked();

    void on_comboBox_GPUID_currentIndexChanged(int index);

    void on_pushButton_SaveSettings_clicked();

    void on_pushButton_ResetSettings_clicked();

    void on_comboBox_language_currentIndexChanged(int index);

    void on_pushButton_SaveFileList_clicked();

    void on_pushButton_ReadFileList_clicked();

    void on_Ext_image_editingFinished();

    void on_Ext_video_editingFinished();

    void on_checkBox_AutoSaveSettings_clicked();

    void on_pushButton_about_clicked();

    void on_comboBox_AspectRatio_custRes_currentIndexChanged(int index);

    void on_checkBox_AlwaysHideSettings_stateChanged(int arg1);

    void on_pushButton_Save_GlobalFontSize_clicked();

    void on_pushButton_BrowserFile_clicked();

    void on_pushButton_wiki_clicked();

    void on_pushButton_HideTextBro_clicked();

    void on_checkBox_AlwaysHideTextBrowser_stateChanged(int arg1);

    void on_pushButton_DumpProcessorList_converter_clicked();

    void on_comboBox_TargetProcessor_converter_currentIndexChanged(int index);

    void on_Ext_image_textChanged(const QString &arg1);

    void on_Ext_video_textChanged(const QString &arg1);

    void on_comboBox_model_vulkan_currentIndexChanged(int index);

    void on_comboBox_ImageStyle_currentIndexChanged(int index);

    void on_pushButton_ResetVideoSettings_clicked();

    void on_lineEdit_encoder_vid_textChanged(const QString &arg1);

    void on_lineEdit_encoder_audio_textChanged(const QString &arg1);

    void on_lineEdit_pixformat_textChanged(const QString &arg1);

    void on_checkBox_vcodec_copy_2mp4_stateChanged(int arg1);

    void on_checkBox_acodec_copy_2mp4_stateChanged(int arg1);

    void on_pushButton_encodersList_clicked();

    void on_checkBox_DelOriginal_stateChanged(int arg1);

    void on_checkBox_videoSettings_isEnabled_stateChanged(int arg1);

    void on_checkBox_FileList_Interactive_stateChanged(int arg1);

    void on_checkBox_OutPath_isEnabled_stateChanged(int arg1);

    void on_pushButton_ForceRetry_clicked();

    void on_pushButton_PayPal_clicked();

    void on_pushButton_DetectGPUID_srmd_clicked();

    void on_comboBox_GPUID_srmd_currentIndexChanged(int index);

    void on_checkBox_AudioDenoise_stateChanged(int arg1);

    void on_tabWidget_currentChanged(int index);

    void on_checkBox_ProcessVideoBySegment_stateChanged(int arg1);

    void on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int index);

    void on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int arg1);

    void on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int arg1);

    void on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked();

    void on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked();

    void on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked();

    void on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked();

    void on_checkBox_isCompatible_Waifu2x_Converter_clicked();

    void on_checkBox_isCompatible_Anime4k_CPU_clicked();

    void on_checkBox_isCompatible_Anime4k_GPU_clicked();

    void on_checkBox_SpecifyGPU_Anime4k_stateChanged(int arg1);

    void on_pushButton_ListGPUs_Anime4k_clicked();

    void on_checkBox_isCompatible_PythonExtension_clicked();

    void on_checkBox_isCompatible_FFmpeg_clicked();

    void on_checkBox_isCompatible_FFprobe_clicked();

    void on_checkBox_isCompatible_ImageMagick_clicked();

    void on_checkBox_isCompatible_Gifsicle_clicked();

    void on_checkBox_isCompatible_SoX_clicked();

    void on_checkBox_GPUMode_Anime4K_stateChanged(int arg1);

    void on_checkBox_ShowInterPro_stateChanged(int arg1);

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

    void Send_CheckUpadte_NewUpdate(QString, QString);

    void Send_Table_FileCount_reload();

    void Send_MovToFinedList();

    void Send_Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Send_Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Send_Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);

    void Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);

    void Send_Table_Read_Saved_Table_Filelist_Finished();
    void Send_Table_Save_Current_Table_Filelist_Finished();

    void Send_SystemShutDown();

    void Send_Waifu2x_DumpProcessorList_converter_finished();

    void Send_Read_urls_finfished();

    void Send_SRMD_DetectGPU_finished();

    void Send_AutoDetectAlphaChannel_setChecked(bool);

    void Send_video_write_VideoConfiguration(QString VideoConfiguration_fullPath,int ScaleRatio,int DenoiseLevel,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,QString EngineName,bool isProcessBySegment,QString VideoClipsFolderPath,QString VideoClipsFolderName);

    void Send_Settings_Save();

    void Send_video_write_Progress_ProcessBySegment(QString VideoConfiguration_fullPath,int StartTime,bool isSplitComplete,bool isScaleComplete);

    //================== 处理当前文件的进度 =========================
    void Send_CurrentFileProgress_Start(QString FileName,int FrameNum);
    void Send_CurrentFileProgress_Stop();
    void Send_CurrentFileProgress_progressbar_Add();
    void Send_CurrentFileProgress_progressbar_Add_SegmentDuration(int SegmentDuration);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

