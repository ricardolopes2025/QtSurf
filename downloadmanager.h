#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QString>

class QWebEngineDownloadRequest;

class DownloadItem : public QObject
{
    Q_OBJECT
public:
    enum DownloadState { Pending, Downloading, Paused, Completed, Cancelled, Failed };

    explicit DownloadItem(QWebEngineDownloadRequest *request, QObject *parent = nullptr);

    QString id() const { return m_id; }
    QString url() const;
    QString fileName() const { return m_fileName; }
    QString path() const { return m_path; }
    qint64 totalBytes() const { return m_totalBytes; }
    qint64 receivedBytes() const { return m_receivedBytes; }
    DownloadState state() const { return m_state; }
    double progress() const;
    QString speed() const;

    void cancel();
    void remove();

signals:
    void stateChanged(DownloadState state);
    void progressChanged(double progress);
    void speedChanged(const QString &speed);

private:
    QString m_id;
    QString m_fileName;
    QString m_path;
    qint64 m_totalBytes = 0;
    qint64 m_receivedBytes = 0;
    DownloadState m_state = Pending;
    QWebEngineDownloadRequest *m_request = nullptr;
    QDateTime m_startTime;
};

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    static DownloadManager *instance();

    void startDownload(QWebEngineDownloadRequest *request);
    QList<DownloadItem *> downloads() const { return m_downloads; }
    QString defaultDownloadPath() const { return m_downloadPath; }
    void setDefaultDownloadPath(const QString &path);

signals:
    void downloadAdded(DownloadItem *download);
    void downloadRemoved(DownloadItem *download);
    void downloadCompleted(DownloadItem *download);

private:
    explicit DownloadManager(QObject *parent = nullptr);
    static DownloadManager *m_instance;
    QList<DownloadItem *> m_downloads;
    QString m_downloadPath;
};

#endif // DOWNLOADMANAGER_H