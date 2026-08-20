#include "downloadwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>

// ========== DownloadWidgetItem ==========
DownloadWidgetItem::DownloadWidgetItem(DownloadItem *download, QWidget *parent)
    : QWidget(parent)
    , m_download(download)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    // Nome do arquivo
    m_nameLabel = new QLabel(download->fileName(), this);
    m_nameLabel->setWordWrap(true);
    layout->addWidget(m_nameLabel);

    // Barra de progresso
    m_progressBar = new QProgressBar(this);
    m_progressBar->setValue(0);
    layout->addWidget(m_progressBar);

    // Layout para status e velocidade
    auto *infoLayout = new QHBoxLayout;
    m_statusLabel = new QLabel("Iniciando...", this);
    m_speedLabel = new QLabel("0 KB/s", this);
    infoLayout->addWidget(m_statusLabel);
    infoLayout->addStretch();
    infoLayout->addWidget(m_speedLabel);
    layout->addLayout(infoLayout);

    // Botões
    auto *buttonLayout = new QHBoxLayout;
    m_cancelButton = new QPushButton("Cancelar", this);
    m_openButton = new QPushButton("Abrir", this);
    m_openButton->hide();

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_openButton);
    layout->addLayout(buttonLayout);

    // Conexões
    connect(m_download, &DownloadItem::progressChanged, this, &DownloadWidgetItem::updateProgress);
    connect(m_download, &DownloadItem::stateChanged, this, &DownloadWidgetItem::updateState);
    connect(m_download, &DownloadItem::speedChanged, this, &DownloadWidgetItem::updateSpeed);
    connect(m_cancelButton, &QPushButton::clicked, this, &DownloadWidgetItem::onCancel);
    connect(m_openButton, &QPushButton::clicked, this, &DownloadWidgetItem::onOpen);

    updateState(download->state());
}

void DownloadWidgetItem::updateProgress(double progress)
{
    m_progressBar->setValue((int)progress);
}

void DownloadWidgetItem::updateState(DownloadItem::DownloadState state)
{
    switch (state) {
    case DownloadItem::Downloading:
        m_statusLabel->setText("Baixando...");
        m_cancelButton->setEnabled(true);
        break;
    case DownloadItem::Completed:
        m_statusLabel->setText("Concluído");
        m_progressBar->setValue(100);
        m_cancelButton->hide();
        m_openButton->show();
        break;
    case DownloadItem::Cancelled:
        m_statusLabel->setText("Cancelado");
        m_cancelButton->hide();
        break;
    case DownloadItem::Failed:
        m_statusLabel->setText("Falhou");
        m_cancelButton->hide();
        break;
    default:
        break;
    }
}

void DownloadWidgetItem::updateSpeed(const QString &speed)
{
    m_speedLabel->setText(speed);
}

void DownloadWidgetItem::onCancel()
{
    m_download->cancel();
}

void DownloadWidgetItem::onOpen()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_download->path()));
}

// ========== DownloadWidget ==========
DownloadWidget::DownloadWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Downloads");
    setMinimumSize(500, 400);

    m_layout = new QVBoxLayout(this);

    // Lista de downloads
    m_downloadList = new QListWidget(this);
    m_layout->addWidget(m_downloadList);

    // Informações
    auto *infoLayout = new QHBoxLayout;
    m_totalLabel = new QLabel("0 downloads", this);
    infoLayout->addWidget(m_totalLabel);
    infoLayout->addStretch();

    m_openFolderButton = new QPushButton("Abrir Pasta", this);
    m_clearButton = new QPushButton("Limpar Concluídos", this);
    infoLayout->addWidget(m_openFolderButton);
    infoLayout->addWidget(m_clearButton);

    m_layout->addLayout(infoLayout);

    // Conexões
    connect(m_clearButton, &QPushButton::clicked, [this]() {
        for (int i = m_downloadList->count() - 1; i >= 0; --i) {
            auto *item = m_downloadList->item(i);
            auto *widget = m_downloadList->itemWidget(item);
            if (widget) {
                auto *dw = qobject_cast<DownloadWidgetItem*>(widget);
                if (dw && dw->property("state").toInt() == (int)DownloadItem::Completed) {
                    delete m_downloadList->takeItem(i);
                    delete widget;
                }
            }
        }
    });

    connect(m_openFolderButton, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(
            DownloadManager::instance()->defaultDownloadPath()));
    });

    // Conectar ao DownloadManager
    connect(DownloadManager::instance(), &DownloadManager::downloadAdded,
            this, &DownloadWidget::showDownload);
}

void DownloadWidget::showDownload(DownloadItem *download)
{
    auto *item = new QListWidgetItem(m_downloadList);
    auto *widget = new DownloadWidgetItem(download, this);
    widget->setProperty("state", (int)download->state());

    item->setSizeHint(widget->sizeHint());
    m_downloadList->addItem(item);
    m_downloadList->setItemWidget(item, widget);

    m_totalLabel->setText(QString("%1 downloads").arg(m_downloadList->count()));
}