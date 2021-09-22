#ifndef CURSEFORGEMODBROWSER_H
#define CURSEFORGEMODBROWSER_H

#include <QWidget>

#include "local/localmodpathinfo.h"

class CurseforgeAPI;
class CurseforgeMod;
class LocalModPath;

namespace Ui {
class CurseforgeModBrowser;
}

class CurseforgeModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModBrowser(QWidget *parent = nullptr);
    ~CurseforgeModBrowser();

signals:
    void downloadPathChanged(LocalModPath *path);

public slots:
    void searchModByPathInfo(const LocalModPathInfo &info);

private slots:
    void updateVersionList();

    void updateLocalPathList();

    void on_searchButton_clicked();

    void onSliderChanged(int i);

    void on_modListWidget_doubleClicked(const QModelIndex &index);

    void on_versionSelect_currentIndexChanged(int);

    void on_sortSelect_currentIndexChanged(int);

    void on_loaderSelect_currentIndexChanged(int);

    void on_downloadPathSelect_currentIndexChanged(int index);

    void on_openFolderButton_clicked();

private:
    Ui::CurseforgeModBrowser *ui;
    CurseforgeAPI *api_;
    QList<int> idList_;
    LocalModPath *downloadPath_;
    QString currentName_;
    int currentIndex_;
    bool isUiSet_ = false;
    bool hasMore_ = false;

    void getModList(QString name, int index = 0, int needMore = 20);
};

#endif // CURSEFORGEMODBROWSER_H