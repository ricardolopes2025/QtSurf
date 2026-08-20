#include "downloadmanager.h"

#include <QWebEngineDownloadRequest>
#include <QStandardPaths>
#include <QDateTime>
#include <QUuid>
#include <QFile>
#include <QDir>

// ========== DownloadItem ==========
DownloadItem::DownloadItem(QWebEngineDownloadRequest *request, QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_request(request)
{
    m_fileName      = request->downloadFileName();
    m_path          = QDir(request->downloadDirectory()).filePath(m_fileName);
    m_totalBytes    = request->totalBytes();
    m_receivedBytes = request->receivedBytes();
    m_state         = Downloading;
    m_startTime     = QDateTime::currentDateTime();

    // LAMBDAS SEM ARGUMENTOS: imunes a erros de assinatura (Qt 6.10)
    connect(request, &QWebEngineDownloadRequest::receivedBytesChanged,
            this, [this]() {
        m_receivedBytes = m_request->receivedBytes();
        emit progressChanged(progress());
        emit speedChanged(speed());
    });

    connect(request, &QWebEngineDownloadRequest::totalBytesChanged,
            this, [this]() {
        m_totalBytes = m_request->totalBytes();
        emit progressChanged(progress());
    });

    connect(request, &QWebEngineDownloadRequest::stateChanged,
            this, [this]() {
        switch (m_request->state()) {
        case QWebEngineDownloadRequest::DownloadInProgress: m_state = Downloading; break;
        case QWebEngineDownloadRequest::DownloadCompleted:  m_state = Completed;  break;
        case QWebEngineDownloadRequest::DownloadCancelled:  m_state = Cancelled;  break;
        case QWebEngineDownloadRequest::DownloadInterrupted:m_state = Failed;     break;
        default: break;
        }
        emit stateChanged(m_state);
    });

    connect(request, &QWebEngineDownloadRequest::isFinishedChanged,
            this, [this]() {
        if (m_state == Completed)
            emit DownloadManager::instance()->downloadCompleted(this);
    });
}

QString DownloadItem::url() const
{
    return m_request ? m_request->url().toString() : QString();
}

double DownloadItem::progress() const
{
    if (m_totalBytes <= 0)
        return 0.0;
    return (double)m_receivedBytes / (double)m_totalBytes * 100.0;
}

QString DownloadItem::speed() const
{
    const qint64 elapsed = m_startTime.secsTo(QDateTime::currentDateTime());
    if (elapsed <= 0)
        return QStringLiteral("0 KB/s");

    const qint64 speed = m_receivedBytes / elapsed;
    if (speed < 1024)
        return QString::number(speed) + QStringLiteral(" B/s");
    if (speed < 1024 * 1024)
        return QString::number(speed / 1024.0, 'f', 1) + QStringLiteral(" KB/s");
    return QString::number(speed / (1024.0 * 1024.0), 'f', 1) + QStringLiteral(" MB/s");
}

void DownloadItem::cancel()
{
    if (m_request) {
        m_request->cancel();
        m_state = Cancelled;
        emit stateChanged(m_state);
    }
}

void DownloadItem::remove()
{
    QFile::remove(m_path);
}

// ========== DownloadManager ==========
DownloadManager *DownloadManager::m_instance = nullptr;

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
{
    m_downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QDir().mkpath(m_downloadPath);
}

DownloadManager *DownloadManager::instance()
{
    if (!m_instance)
        m_instance = new DownloadManager();
    return m_instance;
}

void DownloadManager::startDownload(QWebEngineDownloadRequest *request)
{
    request->setDownloadDirectory(m_downloadPath);
    request->accept();

    DownloadItem *item = new DownloadItem(request, this);
    m_downloads.append(item);
    emit downloadAdded(item);
}

void DownloadManager::setDefaultDownloadPath(const QString &path)
{
    m_downloadPath = path;
    QDir().mkpath(m_downloadPath);
}