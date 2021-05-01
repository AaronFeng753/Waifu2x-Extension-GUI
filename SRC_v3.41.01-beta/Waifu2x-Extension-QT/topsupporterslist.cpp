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

#include "topsupporterslist.h"
#include "ui_topsupporterslist.h"

TopSupportersList::TopSupportersList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TopSupportersList)
{
    ui->setupUi(this);
    //=================
    this->setWindowTitle(tr("Top Supporters @Waifu2x-Extension-GUI"));
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    ui->textBrowser_SupportersNameList->setText(tr("Failed to update \"Top Supporters List\", following list might be out dated.\n\n")+
            "DiabloTVHD | Teh Pron | PorcelainShrine");
    //=================
    QString Current_Path = qApp->applicationDirPath();//当前路径
    QString TopSupportersList_ini_path = Current_Path+"/TopSupportersList_W2xEX.ini";
    if(QFile::exists(TopSupportersList_ini_path) == true)
    {
        QSettings *configIniRead = new QSettings(TopSupportersList_ini_path, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        QString Change_log = configIniRead->value("/TopSupportersList/List").toString();
        if(configIniRead->value("/TopSupportersList/List") != QVariant() && Change_log.trimmed()!="")
        {
            ui->textBrowser_SupportersNameList->setText(Change_log);
        }
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
