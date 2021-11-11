#include "modrinthmodinfowidget.h"
#include "ui_modrinthmodinfowidget.h"

#include "modrinth/modrinthmod.h"
#include "util/flowlayout.h"
#include "util/funcutil.h"

ModrinthModInfoWidget::ModrinthModInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModrinthModInfoWidget)
{
    ui->setupUi(this);
    ui->tagsWidget->setLayout(new FlowLayout());
    ui->scrollArea->setVisible(false);
}

ModrinthModInfoWidget::~ModrinthModInfoWidget()
{
    delete ui;
}

void ModrinthModInfoWidget::setMod(ModrinthMod *mod)
{
    mod_ = mod;
    emit modChanged();

    ui->scrollArea->setVisible(mod_);
    if(!mod_) return;

//    auto action = new QAction(tr("Copy website link"), this);
//    connect(action, &QAction::triggered, this, [=]{
//        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
//    });
//    ui->websiteButton->addAction(action);

    updateBasicInfo();

    //update full info
    updateFullInfo();
    if(mod_->modInfo().description().isEmpty()){
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod_->acquireFullInfo();
    }

    connect(this, &ModrinthModInfoWidget::modChanged, this, disconnecter(
                connect(mod_, &ModrinthMod::fullInfoReady, this, &ModrinthModInfoWidget::updateFullInfo)));
}

void ModrinthModInfoWidget::updateBasicInfo()
{
    setWindowTitle(mod_->modInfo().name() + tr(" - Modrinth"));
    ui->modName->setText(mod_->modInfo().name());
//        ui->modSummary->setText(mod->modInfo().summary());
    if(!mod_->modInfo().author().isEmpty()){
//            ui->modAuthors->setText(mod->modInfo().author());
//            ui->modAuthors->setVisible(true);
//            ui->author_label->setVisible(true);
    } else{
//            ui->modAuthors->setVisible(false);
//            ui->author_label->setVisible(false);
    }

    //update icon
    //included by basic info
    updateIcon();
    if(mod_->modInfo().iconBytes().isEmpty()){
        mod_->acquireIcon();
        ui->modIcon->setCursor(Qt::BusyCursor);
    }
    connect(this, &ModrinthModInfoWidget::modChanged, this, disconnecter(
                connect(mod_, &ModrinthMod::iconReady, this, &ModrinthModInfoWidget::updateIcon)));

    //tags
    for(auto widget : qAsConst(tagWidgets_)){
        ui->tagsWidget->layout()->removeWidget(widget);
        widget->deleteLater();
    }
    tagWidgets_.clear();
    for(auto &&tag : mod_->tags()){
        auto label = new QLabel(this);
        label->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
        if(!tag.iconName().isEmpty())
            label->setText(QString(R"(<img src="%1" height="22" width="22"/>)").arg(tag.iconName()));
        else
            label->setText(tag.name());
        label->setToolTip(tag.name());
        if(tag.tagCategory() != TagCategory::CurseforgeCategory)
            label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.tagCategory().color().name()));
        ui->tagsWidget->layout()->addWidget(label);
        tagWidgets_ << label;
    }
}

void ModrinthModInfoWidget::updateFullInfo()
{
    updateBasicInfo();
    auto text = mod_->modInfo().description();
    text.replace(QRegExp("<br ?/?>"), "\n");
//        ui->websiteButton->setVisible(!mod_->modInfo().websiteUrl().isEmpty());
    ui->modDescription->setMarkdown(text);
    ui->modDescription->setCursor(Qt::ArrowCursor);

//        //update file list
//        auto updateFileList = [=]{
//            ui->fileListWidget->clear();
//            auto files = mod->modInfo().fileList();
//            for(const auto &fileInfo : qAsConst(files)){
//                auto *listItem = new DateTimeSortItem();
//                listItem->setData(DateTimeSortItem::Role, fileInfo.fileDate());
//                listItem->setSizeHint(QSize(500, 90));
//                auto itemWidget = new ModrinthFileItemWidget(this, mod_, fileInfo, localMod_);
//                itemWidget->setDownloadPath(downloadPath_);
//                connect(this, &ModrinthModDialog::downloadPathChanged, itemWidget, &ModrinthFileItemWidget::setDownloadPath);
//                ui->fileListWidget->addItem(listItem);
//                ui->fileListWidget->setItemWidget(listItem, itemWidget);
//            }
//            ui->fileListWidget->sortItems(Qt::DescendingOrder);
//            ui->fileListWidget->setCursor(Qt::ArrowCursor);
//        };

//        if(!mod->modInfo().fileList().isEmpty())
//            updateFileList();
//        else {
//            ui->fileListWidget->setCursor(Qt::BusyCursor);
//            mod->acquireFileList();
//            connect(mod, &ModrinthMod::fileListReady, this, updateFileList);
//        }
}

void ModrinthModInfoWidget::updateIcon()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
    ui->modIcon->setCursor(Qt::ArrowCursor);
}