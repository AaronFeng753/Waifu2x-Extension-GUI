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

    //=================================  File =================================
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void Add_File_Folder(QString Full_Path);
    QStringList getFileNames(QString path);
    int FileList_Add(QString fileName, QString fullPath);

    QList<QMap<QString, QString>> FileList_image;//map["fullPath"],map["rowNum"]
    QList<QMap<QString, QString>> FileList_gif;
    QList<QMap<QString, QString>> FileList_video;

    QList<QMap<QString, QString>> FileList_image_finished;//map["fullPath"],map["rowNum"]
    QList<QMap<QString, QString>> FileList_gif_finished;
    QList<QMap<QString, QString>> FileList_video_finished;

    bool Deduplicate_filelist(QList<QMap<QString, QString>> FileList, QString fullPath);
    int FileList_remove(QMap<QString, QString> File_map);
    QMap<QString, QString> FileList_find_rowNum(QList<QMap<QString, QString>> FileList, int rowNum);

    void RecFinedFiles();
    void MovToFinedList();

    bool file_isDirExist(QString fullPath);
    void file_mkDir(QString fullPath);
    bool file_isFileExist(QString fullFilePath);
    void file_copyFile(QString sourceDir, QString toDir, bool coverFileIfExist);
    QStringList file_getFileNames_in_Folder_nofilter(QString path);
    bool file_DelDir(const QString &path);

    //=================================  Table =================================
    void Init_Table();
    QStandardItemModel *Table_model_image = new QStandardItemModel();
    QStandardItemModel *Table_model_video = new QStandardItemModel();
    QStandardItemModel *Table_model_gif = new QStandardItemModel();

    void Table_image_insert_fileName_fullPath(QString fileName, QString fullPath);
    void Table_gif_insert_fileName_fullPath(QString fileName, QString fullPath);
    void Table_video_insert_fileName_fullPath(QString fileName, QString fullPath);

    void Table_ChangeAllStatusToWaiting();
    void Table_Clear();

    int Table_image_get_rowNum();
    int Table_gif_get_rowNum();
    int Table_video_get_rowNum();

    void Table_FileList_reload();

    QMap<QString, QString> Table_Read_status_fullpath(QStandardItemModel *Table_model);

    //================================= Waifu2x ====================================
    int Waifu2xMainThread();
    int Waifu2x_NCNN_Vulkan_Image(QMap<QString, QString> File_map);

    int Waifu2x_NCNN_Vulkan_GIF(QMap<QString, QString> File_map);
    int Waifu2x_NCNN_Vulkan_GIF_scale(QString Frame_fileName,QString SplitFramesFolderPath,QString ScaledFramesFolderPath,int *Sub_gif_ThreadNumRunning);

    int Waifu2x_NCNN_Vulkan_Video(QMap<QString, QString> File_map);
    int Waifu2x_NCNN_Vulkan_Video_scale(QString Frame_fileName,QString SplitFramesFolderPath,QString ScaledFramesFolderPath,int *Sub_video_ThreadNumRunning);

    int Anime4k_Video(QMap<QString, QString> File_map);
    int Anime4k_Video_scale(QString Frame_fileName,QString SplitFramesFolderPath,QString ScaledFramesFolderPath,int *Sub_video_ThreadNumRunning);

    int Waifu2x_Converter_Image(QMap<QString, QString> File_map);

    int Waifu2x_Converter_GIF(QMap<QString, QString> File_map);
    int Waifu2x_Converter_GIF_scale(QString Frame_fileName,QString SplitFramesFolderPath,QString ScaledFramesFolderPath,int *Sub_gif_ThreadNumRunning);

    int Waifu2x_Converter_Video(QMap<QString, QString> File_map);
    int Waifu2x_Converter_Video_scale(QString Frame_fileName,QString SplitFramesFolderPath,QString ScaledFramesFolderPath,int *Sub_video_ThreadNumRunning);


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



    //================================ progressbar ===================================
    int Progressbar_MaxVal = 0;
    int Progressbar_CurrentVal = 0;
    void progressbar_clear();

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
    void video_video2images(QString VideoPath,QString FrameFolderPath);
    int video_images2video(QString VideoPath,QString ScaledFrameFolderPath);

    //================================ Other =======================================
    bool SystemShutDown();
    void Delay_sec(int time);
    void Delay_msec(int time);

    void Delay_sec_sleep(int time);
    void Delay_msec_sleep(int time);

    void Play_NFSound();

    QTimer *TimeCostTimer;
    int TimeCost = 0;
    QString Seconds2hms(int seconds);

    ~MainWindow();

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



private slots:

    void on_pushButton_Start_clicked();

    void on_pushButton_ClearList_clicked();

    void on_pushButton_Stop_clicked();

    void on_pushButton_RemoveItem_clicked();

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
    int Waifu2x_Compatibility_Test_finished();

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


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

