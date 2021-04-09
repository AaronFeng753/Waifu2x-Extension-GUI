#include "topsupporterslist.h"
#include "ui_topsupporterslist.h"

TopSupportersList::TopSupportersList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TopSupportersList)
{
    ui->setupUi(this);
    //=================
    this->setWindowTitle("Top Supporters @Waifu2x-Extension-GUI");
    //=================
    QString Current_Path = qApp->applicationDirPath();//当前路径
    QString TopSupportersList_ini_path = Current_Path+"/TopSupportersList_W2xEX.ini";
    if(QFile::exists(TopSupportersList_ini_path) == true)
    {
        QSettings *configIniRead = new QSettings(TopSupportersList_ini_path, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        QString Change_log = configIniRead->value("/TopSupportersList/List").toString();
        ui->textBrowser_SupportersNameList->setText(Change_log);
    }
    else
    {
        ui->textBrowser_SupportersNameList->setText(tr("Failed to update \"Top Supporters List\", following list might be out dated.\n\n")+
                "DiabloTVHD | Teh Pron | PorcelainShrine");
    }
}

TopSupportersList::~TopSupportersList()
{
    delete ui;
}

void TopSupportersList::on_pushButton_BecomePatron_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.patreon.com/aaronfeng"));
}
