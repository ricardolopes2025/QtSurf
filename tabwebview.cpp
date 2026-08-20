#include "tabwebview.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineHistory>
#include <QWebEngineView>
#include <QKeyEvent>

TabWebView::TabWebView(QWidget *parent)
    : QWebEngineView(parent)
    , m_devToolsView(nullptr)
{
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

    connect(page(), &QWebEnginePage::fullScreenRequested,
            this, &TabWebView::lidarComTelaCheia);
}

void TabWebView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F12) {
        toggleDevTools();
        return;
    }

    if ((event->modifiers() & Qt::ControlModifier) &&
        (event->modifiers() & Qt::ShiftModifier) &&
        event->key() == Qt::Key_I) {
        toggleDevTools();
        return;
    }

    QWebEngineView::keyPressEvent(event);
}

void TabWebView::toggleDevTools()
{
    if (m_devToolsView) {
        hideDevTools();
    } else {
        showDevTools();
    }
}

void TabWebView::showDevTools()
{
    if (m_devToolsView)
        return;

    m_devToolsView = new QWebEngineView();
    m_devToolsView->setWindowTitle("DevTools - QtSurf");
    m_devToolsView->resize(800, 600);

    page()->setDevToolsPage(m_devToolsView->page());
    m_devToolsView->show();
}

void TabWebView::hideDevTools()
{
    if (m_devToolsView) {
        m_devToolsView->close();
        m_devToolsView->deleteLater();
        m_devToolsView = nullptr;
    }
}

void TabWebView::lidarComTelaCheia(QWebEngineFullScreenRequest request)
{
    request.accept();
    emit solicitacaoTelaCheia(request.toggleOn());
}

QWebEngineView *TabWebView::createWindow(QWebEnginePage::WebWindowType type)
{
    Q_UNUSED(type);
    if (newTabFactory)
        return newTabFactory();
    return nullptr;
}

void TabWebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *internalOpenLink = page()->action(QWebEnginePage::OpenLinkInNewTab);
    QAction *internalCopy     = page()->action(QWebEnginePage::Copy);
    QAction *internalPaste    = page()->action(QWebEnginePage::Paste);

    const bool overLink   = internalOpenLink && internalOpenLink->isEnabled();
    const bool hasSelection = internalCopy && internalCopy->isEnabled();
    const bool canPaste   = internalPaste && internalPaste->isEnabled();

    QAction *actVoltar = menu.addAction(tr("Voltar"), this, &TabWebView::back);
    actVoltar->setEnabled(history()->canGoBack());

    QAction *actAvancar = menu.addAction(tr("Avançar"), this, &TabWebView::forward);
    actAvancar->setEnabled(history()->canGoForward());

    menu.addAction(tr("Recarregar"), this, &TabWebView::reload);
    menu.addSeparator();

    if (overLink) {
        menu.addAction(tr("Abrir link em nova aba"), this, [this] {
            page()->triggerAction(QWebEnginePage::OpenLinkInNewTab);
        });
        menu.addAction(tr("Copiar endereço do link"), this, [this] {
            page()->triggerAction(QWebEnginePage::CopyLinkToClipboard);
        });
        menu.addSeparator();
    }

    if (hasSelection) {
        menu.addAction(tr("Copiar"), this, [this] {
            page()->triggerAction(QWebEnginePage::Copy);
        });
    }
    if (canPaste) {
        menu.addAction(tr("Colar"), this, [this] {
            page()->triggerAction(QWebEnginePage::Paste);
        });
    }
    menu.addAction(tr("Selecionar tudo"), this, [this] {
        page()->triggerAction(QWebEnginePage::SelectAll);
    });
    menu.addSeparator();

    menu.addAction(tr("Inspecionar elemento"), this, [this] {
        showDevTools();
        page()->triggerAction(QWebEnginePage::InspectElement);
    });

    menu.addAction(tr("Ferramentas de Desenvolvedor (F12)"), this, [this] {
        toggleDevTools();
    });

    menu.addAction(tr("Exibir código-fonte"), this, [this] {
        page()->triggerAction(QWebEnginePage::ViewSource);
    });

    menu.exec(event->globalPos());
}