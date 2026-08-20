#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include "downloadmanager.h"

class DownloadWidgetItem : public QWidget
{
    Q_OBJECT

public:
    DownloadWidgetItem(DownloadItem *download, QWidget *parent = nullptr);

private slots:
    void updateProgress(double progress);
    void updateState(DownloadItem::DownloadState state);
    void updateSpeed(const QString &speed);
    void onCancel();
    void onOpen();

private:
    DownloadItem *m_download;
    QLabel *m_nameLabel;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QLabel *m_speedLabel;
    QPushButton *m_cancelButton;
    QPushButton *m_openButton;
};

class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(QWidget *parent = nullptr);
    void showDownload(DownloadItem *download);

private:
    QVBoxLayout *m_layout;
    QListWidget *m_downloadList;
    QPushButton *m_clearButton;
    QPushButton *m_openFolderButton;
    QLabel *m_totalLabel;
};

#endif // DOWNLOADWIDGET_H