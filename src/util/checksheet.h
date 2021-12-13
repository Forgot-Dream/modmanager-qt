#ifndef CHECKSHEET_H
#define CHECKSHEET_H

#include <QObject>

class CheckSheet : public QObject
{
    Q_OBJECT
public:
    explicit CheckSheet(QObject *parent = nullptr);

    template <typename Func1, typename Func2>
    void add(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 startSignal, Func2 finishSignal)
    {
        if(startConnections_.isEmpty())
            emit started();
        startConnections_ << connect(sender, startSignal, this, [=]{
            finishConnections_ << connect(sender, finishSignal, this, &CheckSheet::onOneFinished);
        });
    }

    bool isWaiting();

public slots:
    void reset();
    void cancel();

signals:
    void started();
    void progress(int doneCount, int count);
    void finished();

private slots:
    void onOneFinished();

private:
    QList<QMetaObject::Connection> startConnections_;
    QList<QMetaObject::Connection> finishConnections_;
    int finishedCount_ = 0;
};

#endif // CHECKSHEET_H