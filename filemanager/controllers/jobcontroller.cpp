#include "jobcontroller.h"
#include "fileservices.h"

#include <QtConcurrent/QtConcurrent>

JobController::JobController(const DDirIteratorPointer &iterator, QObject *parent)
    : QThread(parent)
    , m_iterator(iterator)
{

}

JobController::JobController(const DUrl &fileUrl, QDir::Filters filters, QObject *parent)
    : QThread(parent)
    , m_fileUrl(fileUrl)
    , m_filters(filters)
{

}

JobController::State JobController::state() const
{
    return m_state;
}

void JobController::start()
{
    if (m_state == Started) {
        return;
    }

    setState(Started);

    if (m_state == Paused) {
        waitCondition.wakeAll();

        return;
    }

    QThread::start(TimeCriticalPriority);
}

void JobController::pause()
{
    if (m_state != Started)
        return;

    setState(Paused);
}

void JobController::stop()
{
    if (m_state == Stoped)
        return;

    if (m_iterator)
        m_iterator->close();

    setState(Stoped);
}

void JobController::stopAndDeleteLater()
{
    if (!isRunning())
        deleteLater();
    else
        autoDestroy = true;

    stop();
}

void JobController::run()
{
    if (!m_iterator) {
        emit childrenUpdated(FileServices::instance()->getChildren(m_fileUrl, m_filters));

        setState(Stoped);

        emit finished();

        return;
    }

    QQueue<AbstractFileInfoPointer> fileInfoQueue;
    QElapsedTimer *timer = new QElapsedTimer();

    timer->start();

    bool update_children = true;

    while (m_iterator->hasNext()) {
        if (m_state == Paused) {
            mutex.lock();
            waitCondition.wait(&mutex);
            mutex.unlock();
        } else if (m_state == Stoped) {
            break;
        }

        m_iterator->next();

        if (update_children) {
            fileInfoQueue.enqueue(m_iterator->fileInfo());

            if (timer->elapsed() > 500) {
                update_children = false;

                if (!fileInfoQueue.isEmpty())
                    emit childrenUpdated(fileInfoQueue);

                fileInfoQueue.clear();

                delete timer;
            }
        } else {
            emit addChildren(m_iterator->fileInfo());

            QThread::msleep(20);
        }
    }

    if (update_children) {
        if (!fileInfoQueue.isEmpty())
            emit childrenUpdated(fileInfoQueue);
    }

    setState(Stoped);

    emit finished();

    if (autoDestroy)
        deleteLater();
}

void JobController::setState(JobController::State state)
{
    if (m_state == state)
        return;

    m_state = state;

    emit stateChanged(state);
}
