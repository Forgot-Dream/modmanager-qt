#include "downloader.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <algorithm>

#include "downloaderthread.h"

Downloader::Downloader(QObject *parent) :
    QObject(parent),
    accessManager(new QNetworkAccessManager(this)),
    bytesReceived(THREAD_COUNT),
    bytesTotal(THREAD_COUNT)
{

}

bool Downloader::download(const QUrl &url, const QString &filename, int size)
{
    downloadUrl = url;

    qDebug() << "url:" << downloadUrl;

    //get name
    if(!filename.isEmpty())
        downloadFile.setFileName(filename);
    else if(!downloadUrl.fileName().isEmpty())
        downloadFile.setFileName(downloadUrl.fileName());
    else
        downloadFile.setFileName("index.html");

    qDebug() << "file name:" << downloadFile.fileName();

    if(!downloadFile.open(QIODevice::WriteOnly)) return false;

    //get size
    if(size)
        downloadSize = size;
    else {
        QEventLoop loop;
        QNetworkReply *reply = accessManager->head(QNetworkRequest(url));
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit, Qt::ConnectionType::DirectConnection);
        loop.exec();
        disconnect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        reply->deleteLater();
        downloadSize = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    }

    qDebug() << "file size:" << downloadSize;

    //slice file into threads
    for(int i=0; i < THREAD_COUNT; i++)
        {
            qint64 startPos = downloadSize * i / THREAD_COUNT;
            qint64 endPos = downloadSize * (i+1) / THREAD_COUNT;

            auto thread = new DownloaderThread(this);
            connect(thread, &DownloaderThread::threadFinished, this, &Downloader::threadFinished);
            connect(thread, &DownloaderThread::threadDownloadProgress, this, &Downloader::updateProgress);
            connect(thread, &DownloaderThread::threadErrorOccurred, [](int index, QNetworkReply::NetworkError code){
                qDebug() << code;
            });
            thread->download(i, downloadUrl, &downloadFile, startPos, endPos);
    }

    return true;
}

void Downloader::threadFinished(int index)
{
    finishedThreadCount++;
    qDebug() << finishedThreadCount << "/" << THREAD_COUNT;
    if(finishedThreadCount == THREAD_COUNT)
        emit finished();
}

void Downloader::updateProgress(int index, qint64 threadBytesReceived, qint64 threadBytesTotal)
{
    bytesReceived[index] = threadBytesReceived;
    bytesTotal[index] = threadBytesTotal;

    auto bytesReceivedSum = std::accumulate(bytesReceived.begin(), bytesReceived.end(), 0);
    auto bytesTotalSum = std::accumulate(bytesTotal.begin(), bytesTotal.end(), 0);

    emit downloadProgress(bytesReceivedSum, bytesTotalSum);
}
