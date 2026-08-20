#ifndef TABWEBVIEW_H
#define TABWEBVIEW_H

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineFullScreenRequest>
#include <functional>

class QContextMenuEvent;
class QWebEngineView;

class TabWebView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit TabWebView(QWidget *parent = nullptr);
    std::function<TabWebView *()> newTabFactory;

    // DevTools
    void toggleDevTools();
    void showDevTools();
    void hideDevTools();
    bool isDevToolsOpen() const { return m_devToolsView != nullptr; }

signals:
    void abrirLinkEmNovaAba(const QUrl &url);
    void solicitacaoTelaCheia(bool ativado);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void lidarComTelaCheia(QWebEngineFullScreenRequest request);

private:
    QWebEngineView *m_devToolsView = nullptr;
};

#endif // TABWEBVIEW_H