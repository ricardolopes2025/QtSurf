#include "qtsurfwindow.h"
#include "ui_qtsurfwindow.h"
#include "tabwebview.h"
#include "settingsdialog.h"
#include "settings.h"
#include "downloadmanager.h"
#include "downloadwidget.h"

#include <QTabBar>
#include <QStackedWidget>
#include <QLineEdit>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QPushButton>
#include <QToolButton>
#include <QPalette>
#include <QPainter>
#include <QFile>
#include <QSvgRenderer>
#include <QWebEngineHistory>
#include <QWebEnginePage>
#include <QWebEngineProfile> // <-- Essencial para o downloadRequested

namespace {

// Renderiza um SVG do pacote :/modern recolorido para o tema atual
QIcon iconeSvgTematico(const QString &caminho, const QColor &cor,
                       const QSize &tamanho = QSize(20, 20))
{
    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::ReadOnly))
        return QIcon();

    QByteArray dados = arquivo.readAll();
    dados.replace("#5F6368", cor.name().toUtf8()); // cor base usada nos SVGs

    QSvgRenderer renderizador(dados);
    if (!renderizador.isValid())
        return QIcon();

    const qreal dpr = qApp ? qApp->devicePixelRatio() : 1.0;
    QPixmap pixmap(tamanho * dpr);
    pixmap.fill(Qt::transparent);
    pixmap.setDevicePixelRatio(dpr);

    QPainter pintor(&pixmap);
    pintor.setRenderHint(QPainter::Antialiasing);
    pintor.setRenderHint(QPainter::SmoothPixmapTransform);
    renderizador.render(&pintor, QRectF(QPointF(0, 0), QSizeF(tamanho)));

    return QIcon(pixmap);
}

} // namespace

BrowserWindow::BrowserWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BrowserWindow)
    , m_homePage("https://www.google.com.br/webhp?hl=pt-BR&gl=BR")
    , m_emTelaCheia(false)
{
    ui->setupUi(this);

    // Visual limpo estilo Chrome: sem barra de menus clássica.
    // As ações (e seus atalhos) continuam ativas e moram no menu "⋮".
    ui->menubar->hide();

    // Registra as ações na própria janela para Ctrl+T, Ctrl+W, F5 etc.
    // seguirem funcionando mesmo com a barra de menus oculta.
    addActions({ ui->actionNovaAba, ui->actionFecharAba, ui->actionVoltar,
                 ui->actionAvancar, ui->actionAtualizar, ui->actionHome,
                 ui->actionConfigura_es, ui->actionSair });

    configurarMenuPrincipal();
    configurarConexoes();
    configurarMenuBar();
    aplicarIconesDoTema();

    // Primeira aba
    criarNovaAba();
}

BrowserWindow::~BrowserWindow()
{
    delete ui;
}

// ---------------------------------------------------------------- conexões
void BrowserWindow::configurarConexoes()
{
    connect(ui->urlBar, &QLineEdit::returnPressed,   this, &BrowserWindow::navegar);
    connect(ui->home, &QPushButton::clicked,         this, &BrowserWindow::irParaHome);
    connect(ui->atualizar, &QPushButton::clicked,    this, &BrowserWindow::atualizarPagina);
    connect(ui->btnVoltar, &QPushButton::clicked,    this, &BrowserWindow::voltar);
    connect(ui->btnAvancar, &QPushButton::clicked,   this, &BrowserWindow::avancar);
    connect(ui->btnNovaAba, &QToolButton::clicked,   this, &BrowserWindow::novaAba);

    connect(ui->tabBar, &QTabBar::currentChanged,    this, &BrowserWindow::abaAtualMudou);
    connect(ui->tabBar, &QTabBar::tabCloseRequested, this, &BrowserWindow::fecharAba);
}

void BrowserWindow::configurarMenuBar()
{
    connect(ui->actionSair, &QAction::triggered,         qApp, &QApplication::quit);
    connect(ui->actionVoltar, &QAction::triggered,       this, &BrowserWindow::voltar);
    connect(ui->actionAvancar, &QAction::triggered,      this, &BrowserWindow::avancar);
    connect(ui->actionAtualizar, &QAction::triggered,    this, &BrowserWindow::atualizarPagina);
    connect(ui->actionHome, &QAction::triggered,         this, &BrowserWindow::irParaHome);
    connect(ui->actionNovaAba, &QAction::triggered,      this, &BrowserWindow::novaAba);
    connect(ui->actionFecharAba, &QAction::triggered,    this, &BrowserWindow::fecharAbaAtual);
    connect(ui->actionConfigura_es, &QAction::triggered, this, &BrowserWindow::abrirConfiguracoes);
}

// ---------------------------------------------------------------- menu "⋮"
void BrowserWindow::configurarMenuPrincipal()
{
    QMenu *menuPrincipal = new QMenu(this);
    menuPrincipal->addAction(ui->actionNovaAba);
    menuPrincipal->addAction(ui->actionFecharAba);
    menuPrincipal->addSeparator();
    menuPrincipal->addAction(ui->actionVoltar);
    menuPrincipal->addAction(ui->actionAvancar);
    menuPrincipal->addAction(ui->actionAtualizar);
    menuPrincipal->addAction(ui->actionHome);
    menuPrincipal->addSeparator();
    menuPrincipal->addAction(ui->actionConfigura_es);
    menuPrincipal->addSeparator();
    menuPrincipal->addAction(ui->actionSair);

    ui->btnMenu->setMenu(menuPrincipal);
    ui->btnMenu->setPopupMode(QToolButton::InstantPopup);
}

// ---------------------------------------------------------------- ícones
//
// IMPORTANTE: os arquivos estão registrados no QtSurf.qrc com o prefixo
// "/modern" e o caminho de arquivo "icones/modern/<nome>.svg", então o
// caminho de recurso final é ":/modern/icones/modern/<nome>.svg".
// (Antes o código usava ":/modern/<nome>.svg", que não existia — por isso
// os ícones da toolbar não apareciam / ficavam inconsistentes com o tema.)
void BrowserWindow::aplicarIconesDoTema()
{
    const bool escuro = qApp->palette().color(QPalette::Window).lightness() < 128;
    const QColor corIcone = escuro ? QColor("#C9CDD5") : QColor("#5F6368");

    // Tamanho único para todos os botões da toolbar -> visual consistente
    const QSize tamanhoToolbar(20, 20);

    ui->btnVoltar->setIcon(iconeSvgTematico(":/modern/modern/back.svg", corIcone, tamanhoToolbar));
    ui->btnVoltar->setIconSize(tamanhoToolbar);

    ui->btnAvancar->setIcon(iconeSvgTematico(":/modern/modern/forward.svg", corIcone, tamanhoToolbar));
    ui->btnAvancar->setIconSize(tamanhoToolbar);

    ui->atualizar->setIcon(iconeSvgTematico(":/modern/modern/reload.svg", corIcone, tamanhoToolbar));
    ui->atualizar->setIconSize(tamanhoToolbar);

    ui->home->setIcon(iconeSvgTematico(":/modern/modern/home.svg", corIcone, tamanhoToolbar));
    ui->home->setIconSize(tamanhoToolbar);

    ui->btnMenu->setIcon(iconeSvgTematico(":/modern/modern/menu.svg", corIcone, tamanhoToolbar));
    ui->btnMenu->setIconSize(tamanhoToolbar);

    const QSize tamanhoNovaAba(18, 18);
    ui->btnNovaAba->setIcon(iconeSvgTematico(":/modern/modern/add.svg", corIcone, tamanhoNovaAba));
    ui->btnNovaAba->setIconSize(tamanhoNovaAba);

    // Lupa dentro do omnibox, como no Chrome
    const QColor corBusca = escuro ? QColor("#9AA0A6") : QColor("#5F6368");
    const QIcon iconeBusca = iconeSvgTematico(":/modern/modern/search.svg", corBusca, QSize(16, 16));
    if (!m_acaoBusca)
        m_acaoBusca = ui->urlBar->addAction(iconeBusca, QLineEdit::LeadingPosition);
    else
        m_acaoBusca->setIcon(iconeBusca);
}

void BrowserWindow::atualizarBotoesNavegacao()
{
    TabWebView *view = currentView();
    ui->btnVoltar->setEnabled(view && view->history()->canGoBack());
    ui->btnAvancar->setEnabled(view && view->history()->canGoForward());
}

// ---------------------------------------------------------------- abas
TabWebView *BrowserWindow::criarNovaAba(const QUrl &url, bool switchTo, bool loadHome)
{
    TabWebView *view = new TabWebView(this);

    // target="_blank" abre em nova aba
    view->newTabFactory = [this]() -> TabWebView * {
        return criarNovaAba(QUrl(), true, false);
    };

    connect(view, &TabWebView::abrirLinkEmNovaAba, this, [this](const QUrl &u) {
        criarNovaAba(u, true, true);
    });

    // Conexão para tela cheia (YouTube, etc.)
    connect(view, &TabWebView::solicitacaoTelaCheia,
            this, &BrowserWindow::onSolicitacaoTelaCheia);

    // Conexão para downloads (API Qt 6.10 - QWebEngineProfile)
    connect(view->page()->profile(), &QWebEngineProfile::downloadRequested,
            [](QWebEngineDownloadRequest *request) {
                DownloadManager::instance()->startDownload(request);
            });

    connect(view, &QWebEngineView::titleChanged, this, [this, view](const QString &title) {
        const int idx = ui->stackedWidget->indexOf(view);
        if (idx >= 0) {
            const QString texto = title.isEmpty() ? tr("Nova Aba") : title;
            ui->tabBar->setTabText(idx, texto);
            ui->tabBar->setTabToolTip(idx, texto);
        }
        if (currentView() == view)
            atualizarTitulo();
    });

    // Favicon exibido na aba, como no Chrome
    connect(view, &QWebEngineView::iconChanged, this, [this, view](const QIcon &icon) {
        const int idx = ui->stackedWidget->indexOf(view);
        if (idx >= 0)
            ui->tabBar->setTabIcon(idx, icon);
    });

    connect(view, &QWebEngineView::urlChanged, this, [this, view](const QUrl &u) {
        Q_UNUSED(u);
        if (currentView() == view) {
            atualizarBarraUrl();
            atualizarBotoesNavegacao();
        }
    });

    connect(view, &QWebEngineView::loadStarted, this, [this, view] {
        if (currentView() == view)
            statusBar()->showMessage(tr("Carregando..."), 0);
    });

    connect(view, &QWebEngineView::loadFinished, this, [this, view](bool ok) {
        if (currentView() != view)
            return;
        statusBar()->showMessage(ok ? tr("Página carregada com sucesso.")
                                    : tr("Erro ao carregar a página."),
                                 ok ? 3000 : 5000);
        atualizarBotoesNavegacao();
    });

    connect(view, &QWebEngineView::loadProgress, this, [this, view](int progress) {
        if (currentView() == view)
            statusBar()->showMessage(tr("Carregando... %1%").arg(progress), 0);
    });

    const int idxStack = ui->stackedWidget->addWidget(view);
    const int idx = ui->tabBar->insertTab(idxStack, tr("Nova Aba"));
    if (switchTo)
        ui->tabBar->setCurrentIndex(idx);

    if (url.isValid())
        view->setUrl(url);
    else if (loadHome)
        view->setUrl(QUrl(Settings::currentEngine().homeUrl));

    return view;
}

TabWebView *BrowserWindow::currentView() const
{
    return qobject_cast<TabWebView *>(ui->stackedWidget->currentWidget());
}

void BrowserWindow::novaAba()
{
    criarNovaAba();
    ui->urlBar->setFocus();
    ui->urlBar->selectAll();
}

void BrowserWindow::fecharAba(int index)
{
    if (index < 0 || index >= ui->tabBar->count())
        return;

    QWidget *w = ui->stackedWidget->widget(index);
    ui->tabBar->removeTab(index);
    ui->stackedWidget->removeWidget(w);
    if (w)
        w->deleteLater();

    if (ui->tabBar->count() == 0)
        criarNovaAba(); // sempre manter pelo menos 1 aba
}

void BrowserWindow::fecharAbaAtual()
{
    fecharAba(ui->tabBar->currentIndex());
}

void BrowserWindow::abaAtualMudou(int index)
{
    if (index >= 0 && index < ui->stackedWidget->count())
        ui->stackedWidget->setCurrentIndex(index);
    atualizarBarraUrl();
    atualizarTitulo();
    atualizarBotoesNavegacao();
}

void BrowserWindow::atualizarBarraUrl()
{
    TabWebView *view = currentView();
    if (view)
        ui->urlBar->setText(view->url().toString());
}

void BrowserWindow::atualizarTitulo()
{
    TabWebView *view = currentView();
    const QString title = view ? view->title() : QString();
    setWindowTitle(title.isEmpty() ? tr("QtSurf")
                                   : title + " - " + tr("QtSurf"));
}

// ---------------------------------------------------------------- navegação
void BrowserWindow::navegar()
{
    TabWebView *view = currentView();
    if (!view)
        return;

    const QString texto = ui->urlBar->text().trimmed();
    if (texto.isEmpty())
        return;

    view->setUrl(processarUrl(texto));
}

QUrl BrowserWindow::processarUrl(const QString &texto)
{
    if (texto.startsWith("http://") || texto.startsWith("https://") ||
        texto.startsWith("file://") || texto.startsWith("ftp://"))
        return QUrl(texto);

    if (texto.contains('.') && !texto.contains(' '))
        return QUrl("https://" + texto);

    // Pesquisa com o mecanismo configurado
    const SearchEngine engine = Settings::currentEngine();
    const QString query = QString::fromUtf8(QUrl::toPercentEncoding(texto));
    return QUrl(engine.searchTemplate.arg(query));
}

void BrowserWindow::irParaHome()
{
    TabWebView *view = currentView();
    if (!view)
        return;

    const QUrl home(Settings::currentEngine().homeUrl);
    view->setUrl(home);
    ui->urlBar->setText(home.toString());
}

void BrowserWindow::atualizarPagina()
{
    TabWebView *view = currentView();
    if (view)
        view->reload();
}

void BrowserWindow::voltar()
{
    TabWebView *view = currentView();
    if (view && view->history()->canGoBack())
        view->back();
}

void BrowserWindow::avancar()
{
    TabWebView *view = currentView();
    if (view && view->history()->canGoForward())
        view->forward();
}

void BrowserWindow::abrirConfiguracoes()
{
    SettingsDialog dlg(this);
    dlg.exec();
    aplicarIconesDoTema(); // o tema (e a cor dos ícones) pode ter mudado
}

// ---------------------------------------------------------------- tela cheia
void BrowserWindow::onSolicitacaoTelaCheia(bool ativado)
{
    m_emTelaCheia = ativado;

    if (ativado) {
        showFullScreen();
        ui->tabBarContainer->hide();
        ui->toolbarContainer->hide();
        ui->statusbar->hide();
    } else {
        showNormal();
        ui->tabBarContainer->show();
        ui->toolbarContainer->show();
        ui->statusbar->show();
    }
}
