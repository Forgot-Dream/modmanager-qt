#include "curseforgemodinfodialog.h"
#include "ui_curseforgemodinfodialog.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "curseforge/curseforgemod.h"
#include "ui/curseforgefileitemwidget.h"
#include "util/datetimesortitem.h"

CurseforgeModInfoDialog::CurseforgeModInfoDialog(QWidget *parent, CurseforgeMod *mod) :
    QDialog(parent),
    ui(new Ui::CurseforgeModInfoDialog),
    curseforgeMod(mod)
{
    ui->setupUi(this);

    //update basic info
    auto updateBasicInfo = [=]{
        setWindowTitle(mod->getModInfo().getName() + tr(" - Curseforge"));
        ui->modName->setText(mod->getModInfo().getName());
        ui->modSummary->setText(mod->getModInfo().getSummary());
        ui->modUrl->setText(QString("<a href= \"%1\">%1</a>").arg(mod->getModInfo().getWebsiteUrl().toString()));
        ui->modAuthors->setText(mod->getModInfo().getAuthors().join(", ").prepend(tr("by ")));

        //update thumbnail
        //included by basic info
        auto updateThumbnail = [=]{
            QPixmap pixelmap;
            pixelmap.loadFromData(curseforgeMod->getModInfo().getThumbnailBytes());
            ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
            ui->modIcon->setCursor(Qt::ArrowCursor);
        };

        if(!curseforgeMod->getModInfo().getThumbnailBytes().isEmpty())
            updateThumbnail();
        else {
            mod->acquireThumbnail();
            ui->modIcon->setCursor(Qt::BusyCursor);
            connect(curseforgeMod, &CurseforgeMod::thumbnailReady, this, updateThumbnail);
        }
    };

    if(curseforgeMod->getModInfo().hasBasicInfo())
        updateBasicInfo();
    else {
        mod->acquireBasicInfo();
        connect(curseforgeMod, &CurseforgeMod::basicInfoReady, this, updateBasicInfo);
    }

    //update description
    auto updateDescription = [=]{
        ui->modDescription->setText(curseforgeMod->getModInfo().getDescription());
        ui->modDescription->setCursor(Qt::ArrowCursor);
    };

    if(!curseforgeMod->getModInfo().getDescription().isEmpty())
        updateDescription();
    else{
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod->acquireDescription();
        connect(mod, &CurseforgeMod::descriptionReady, this, updateDescription);
    }

    //update file list
    auto updateFileList = [=]{
        ui->fileListWidget->clear();
        auto files = curseforgeMod->getModInfo().getAllFiles();
        for(const auto &fileInfo : files){
            auto *listItem = new DateTimeSortItem();
            listItem->setData(DateTimeSortItem::Role, fileInfo.getFileDate());
            listItem->setSizeHint(QSize(500, 90));
            auto itemWidget = new CurseforgeFileItemWidget(this, fileInfo);
            ui->fileListWidget->addItem(listItem);
            ui->fileListWidget->setItemWidget(listItem, itemWidget);
        }
        ui->fileListWidget->sortItems(Qt::DescendingOrder);
        ui->fileListWidget->setCursor(Qt::ArrowCursor);
    };

    if(!curseforgeMod->getModInfo().getAllFiles().isEmpty())
        updateFileList();
    else {
        ui->fileListWidget->setCursor(Qt::BusyCursor);
        mod->acquireAllFileList();
        connect(mod, &CurseforgeMod::allFileListReady, this, updateFileList);
    }
}

CurseforgeModInfoDialog::~CurseforgeModInfoDialog()
{
    delete ui;
}
