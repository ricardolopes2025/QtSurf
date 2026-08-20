#include "settings.h"

#include <QApplication>
#include <QSettings>
#include <QStyle>
#include <QStyleFactory>
#include <QPalette>
#include <QColor>

QList<SearchEngine> Settings::availableEngines()
{
    return {
            // ── NÃO ALTERE A ORDEM DESTES (índice salvo no QSettings) ──
            { "Google",
             "https://musikeson.rf.gd/",//https://www.google.com.br/webhp?hl=pt-BR&gl=BR https://www.google.com.br/search?hl=pt-BR&gl=BR&q=%1
             "https://musikeson.rf.gd/" },
            { "Bing",
             "https://www.bing.com",
             "https://www.bing.com/search?q=%1" },
            { "DuckDuckGo",
             "https://duckduckgo.com",
             "https://duckduckgo.com/?q=%1" },
            { "Yahoo",
             "https://br.search.yahoo.com",
             "https://br.search.yahoo.com/search?p=%1" },
            { "Perplexity",
             "https://www.perplexity.ai/",
             "https://www.perplexity.ai/search?q=%1" },

            // ── NOVOS: adicione sempre no final ──
            { "Brave Search",
             "https://search.brave.com/",
             "https://search.brave.com/search?q=%1" },
            { "Startpage",
             "https://www.startpage.com/",
             "https://www.startpage.com/sp/search?query=%1" },
            { "Ecosia",
             "https://www.ecosia.org/",
             "https://www.ecosia.org/search?q=%1" },
            { "Mojeek",
             "https://www.mojeek.com/",
             "https://www.mojeek.com/search?q=%1" },
            { "You.com (IA)",
             "https://you.com/",
             "https://you.com/search?q=%1" },
            { "Swisscows",
             "https://swisscows.com/",
             "https://swisscows.com/en/web?query=%1" },
            { "Qwant",
             "https://www.qwant.com/",
             "https://www.qwant.com/?q=%1" },
            };
}

int Settings::currentEngineIndex()
{
    QSettings s;
    return s.value("search/engineIndex", 0).toInt();
}

SearchEngine Settings::currentEngine()
{
    const QList<SearchEngine> engines = availableEngines();
    int idx = currentEngineIndex();
    if (idx < 0 || idx >= engines.size())
        idx = 0;
    return engines.at(idx);
}

void Settings::saveEngineIndex(int index)
{
    QSettings s;
    s.setValue("search/engineIndex", index);
}

QString Settings::theme()
{
    QSettings s;
    return s.value("appearance/theme", "system").toString();
}

void Settings::saveTheme(const QString &theme)
{
    QSettings s;
    s.setValue("appearance/theme", theme);
}

// ============================================================================
//  Visual moderno estilo Chrome
// ============================================================================
namespace {

struct CoresTema
{
    QString stripBg;        // faixa atrás das abas
    QString tabAtiva;       // aba selecionada (continua a cor da barra de ferramentas)
    QString tabHover;       // aba inativa sob o mouse
    QString barraBg;        // barra de ferramentas
    QString barraBorda;     // linha de separação sob a barra de ferramentas
    QString texto;          // texto principal
    QString textoSuave;     // texto secundário
    QString urlBg;          // fundo do omnibox
    QString urlFocusBg;     // omnibox com foco
    QString urlFocusBorda;  // borda do omnibox com foco
    QString btnHover;       // botões sob o mouse
    QString btnPress;       // botões pressionados
    QString menuBg;         // fundo dos menus popup
    QString menuBorda;      // borda dos menus popup
    QString menuHover;      // item de menu sob o mouse
    QString destaque;       // cor de seleção
    QString destaqueTexto;  // texto sobre a seleção
    QString botaoBg;        // botões de diálogos
    QString botaoBorda;
    QString botaoHover;
    QString botaoPress;
};

CoresTema coresClaras()
{
    return {
        QStringLiteral("#DEE1E6"),          // stripBg
        QStringLiteral("#FFFFFF"),          // tabAtiva
        QStringLiteral("#ECEEF1"),          // tabHover
        QStringLiteral("#FFFFFF"),          // barraBg
        QStringLiteral("#D8DBE0"),          // barraBorda
        QStringLiteral("#202124"),          // texto
        QStringLiteral("#5F6368"),          // textoSuave
        QStringLiteral("#F0F2F4"),          // urlBg
        QStringLiteral("#FFFFFF"),          // urlFocusBg
        QStringLiteral("#BDBFC7"),          // urlFocusBorda
        QStringLiteral("rgba(60,64,67,26)"),   // btnHover  (~10%)
        QStringLiteral("rgba(60,64,67,46)"),   // btnPress  (~18%)
        QStringLiteral("#FFFFFF"),          // menuBg
        QStringLiteral("#D5D8DD"),          // menuBorda
        QStringLiteral("rgba(60,64,67,26)"),   // menuHover
        QStringLiteral("#1A73E8"),          // destaque
        QStringLiteral("#FFFFFF"),          // destaqueTexto
        QStringLiteral("#FFFFFF"),          // botaoBg
        QStringLiteral("#DADCE0"),          // botaoBorda
        QStringLiteral("#F3F4F6"),          // botaoHover
        QStringLiteral("#E8EAED")           // botaoPress
    };
}

CoresTema coresEscuras()
{
    return {
        QStringLiteral("#181B1F"),          // stripBg
        QStringLiteral("#36393F"),          // tabAtiva
        QStringLiteral("#24272C"),          // tabHover
        QStringLiteral("#36393F"),          // barraBg
        QStringLiteral("#1E2124"),          // barraBorda
        QStringLiteral("#E8EAED"),          // texto
        QStringLiteral("#9AA0A6"),          // textoSuave
        QStringLiteral("#1E2124"),          // urlBg
        QStringLiteral("#2A2D32"),          // urlFocusBg
        QStringLiteral("#5B5F66"),          // urlFocusBorda
        QStringLiteral("rgba(255,255,255,23)"),  // btnHover  (~9%)
        QStringLiteral("rgba(255,255,255,41)"),  // btnPress  (~16%)
        QStringLiteral("#2B2D31"),          // menuBg
        QStringLiteral("#484C52"),          // menuBorda
        QStringLiteral("rgba(255,255,255,26)"),  // menuHover
        QStringLiteral("#8AB4F8"),          // destaque
        QStringLiteral("#202124"),          // destaqueTexto
        QStringLiteral("#36393F"),          // botaoBg
        QStringLiteral("#4A4E54"),          // botaoBorda
        QStringLiteral("#404449"),          // botaoHover
        QStringLiteral("#4C5057")           // botaoPress
    };
}

QString folhaEstiloChrome(const CoresTema &c)
{
    QString css = QStringLiteral(R"CSS(
/* ================= QtSurf — visual moderno estilo Chrome ================= */

QMainWindow,
QWidget#centralwidget {
    background: @BARRA_BG@;
}

/* ---- faixa superior: as abas ficam no topo, como no Chrome ---- */
QWidget#tabBarContainer {
    background: @STRIP_BG@;
}

QTabBar {
    background: transparent;
}

QTabBar::tab {
    background: transparent;
    border: none;
    border-radius: 11px;
    color: @TEXTO_SUAVE@;
    margin: 1px 2px;
    padding: 7px 12px;
    min-width: 110px;
    max-width: 260px;
}

QTabBar::tab:hover:!selected {
    background: @TAB_HOVER@;
    color: @TEXTO@;
}

QTabBar::tab:selected {
    background: @TAB_ATIVA@;
    color: @TEXTO@;
}

QTabBar::close-button {
    image: url(:/modern/close.svg);
    subcontrol-position: right;
    border-radius: 8px;
    margin-left: 6px;
    padding: 1px;
}
QTabBar::close-button:hover {
    background: @BTN_HOVER@;
}
QTabBar::close-button:pressed {
    background: @BTN_PRESS@;
}

/* setas de rolagem quando há muitas abas */
QTabBar QToolButton {
    background: transparent;
    border: none;
    border-radius: 9px;
    color: @TEXTO@;
}
QTabBar QToolButton:hover {
    background: @BTN_HOVER@;
}
QTabBar QToolButton:pressed {
    background: @BTN_PRESS@;
}

/* botão "+" de nova aba */
QToolButton#btnNovaAba {
    background: transparent;
    border: none;
    border-radius: 14px;
    padding: 0px;
    min-width: 28px;
    max-width: 28px;
    min-height: 28px;
    max-height: 28px;
}
QToolButton#btnNovaAba:hover {
    background: @BTN_HOVER@;
}
QToolButton#btnNovaAba:pressed {
    background: @BTN_PRESS@;
}

/* ---- barra de ferramentas ---- */
QWidget#toolbarContainer {
    background: @BARRA_BG@;
    border-bottom: 1px solid @BARRA_BORDA@;
}

#toolbarContainer QPushButton,
#toolbarContainer QToolButton {
    background: transparent;
    border: none;
    border-radius: 16px;
    padding: 0px;
    min-width: 32px;
    max-width: 32px;
    min-height: 32px;
    max-height: 32px;
}
#toolbarContainer QPushButton:hover,
#toolbarContainer QToolButton:hover {
    background: @BTN_HOVER@;
}
#toolbarContainer QPushButton:pressed,
#toolbarContainer QToolButton:pressed {
    background: @BTN_PRESS@;
}
#toolbarContainer QPushButton:disabled,
#toolbarContainer QToolButton:disabled {
    background: transparent;
}

QToolButton#btnMenu::menu-indicator {
    image: none;
    width: 0px;
}

/* omnibox (barra de endereço em formato de pílula) */
QLineEdit#urlBar {
    background: @URL_BG@;
    border: 1px solid transparent;
    border-radius: 18px;
    color: @TEXTO@;
    padding: 8px 16px 8px 36px;
    selection-background-color: @DESTAQUE@;
    selection-color: @DESTAQUE_TEXTO@;
}
QLineEdit#urlBar:hover {
    border: 1px solid @BARRA_BORDA@;
}
QLineEdit#urlBar:focus {
    background: @URL_FOCUS_BG@;
    border: 1px solid @URL_FOCUS_BORDA@;
}

/* área do conteúdo */
QStackedWidget#stackedWidget {
    background: @BARRA_BG@;
    border: none;
}

/* ---- barra de status ---- */
QStatusBar {
    background: @BARRA_BG@;
    border-top: 1px solid @BARRA_BORDA@;
    color: @TEXTO_SUAVE@;
}
QStatusBar::item {
    border: none;
}
QStatusBar QLabel {
    color: @TEXTO_SUAVE@;
}

/* ---- menus popup / de contexto ---- */
QMenu {
    background: @MENU_BG@;
    border: 1px solid @MENU_BORDA@;
    border-radius: 10px;
    padding: 6px;
}
QMenu::item {
    background: transparent;
    color: @TEXTO@;
    border-radius: 6px;
    padding: 7px 28px 7px 24px;
}
QMenu::item:selected {
    background: @MENU_HOVER@;
}
QMenu::item:disabled {
    color: @TEXTO_SUAVE@;
}
QMenu::separator {
    height: 1px;
    background: @MENU_BORDA@;
    margin: 5px 10px;
}
QMenu::indicator {
    width: 16px;
    height: 16px;
}

/* ---- tooltips ---- */
QToolTip {
    background: @MENU_BG@;
    color: @TEXTO@;
    border: 1px solid @MENU_BORDA@;
    border-radius: 6px;
    padding: 6px 9px;
}

/* ---- botões e campos do restante do app (diálogos, downloads) ---- */
QPushButton {
    background: @BOTAO_BG@;
    border: 1px solid @BOTAO_BORDA@;
    border-radius: 9px;
    color: @TEXTO@;
    padding: 7px 16px;
}
QPushButton:hover {
    background: @BOTAO_HOVER@;
}
QPushButton:pressed {
    background: @BOTAO_PRESS@;
}
QPushButton:default {
    border-color: @DESTAQUE@;
}
QPushButton:disabled {
    color: @TEXTO_SUAVE@;
}

QToolButton {
    background: transparent;
    border: none;
    border-radius: 8px;
    padding: 3px;
}
QToolButton:hover {
    background: @BTN_HOVER@;
}
QToolButton:pressed {
    background: @BTN_PRESS@;
}

QComboBox {
    background: @BOTAO_BG@;
    border: 1px solid @BOTAO_BORDA@;
    border-radius: 8px;
    color: @TEXTO@;
    padding: 6px 10px;
    min-height: 20px;
}
QComboBox:hover {
    border-color: @TEXTO_SUAVE@;
}
QComboBox::drop-down {
    border: none;
    width: 24px;
}
QComboBox QAbstractItemView {
    background: @MENU_BG@;
    border: 1px solid @MENU_BORDA@;
    color: @TEXTO@;
    selection-background-color: @MENU_HOVER@;
    selection-color: @TEXTO@;
    outline: none;
    padding: 4px;
}

QLineEdit {
    background: @URL_FOCUS_BG@;
    border: 1px solid @BOTAO_BORDA@;
    border-radius: 8px;
    color: @TEXTO@;
    padding: 6px 10px;
    selection-background-color: @DESTAQUE@;
    selection-color: @DESTAQUE_TEXTO@;
}
QLineEdit:focus {
    border: 1px solid @URL_FOCUS_BORDA@;
}

QListWidget {
    background: @URL_BG@;
    border: 1px solid @BARRA_BORDA@;
    border-radius: 8px;
}

QProgressBar {
    background: @URL_BG@;
    border: none;
    border-radius: 5px;
    color: @TEXTO_SUAVE@;
    text-align: center;
    max-height: 10px;
}
QProgressBar::chunk {
    background: @DESTAQUE@;
    border-radius: 5px;
}
)CSS");

    const auto rep = [&css](const char *token, const QString &valor) {
        css.replace(QString::fromUtf8(token), valor);
    };
    rep("@STRIP_BG@",        c.stripBg);
    rep("@TAB_ATIVA@",       c.tabAtiva);
    rep("@TAB_HOVER@",       c.tabHover);
    rep("@BARRA_BG@",        c.barraBg);
    rep("@BARRA_BORDA@",     c.barraBorda);
    rep("@TEXTO@",           c.texto);
    rep("@TEXTO_SUAVE@",     c.textoSuave);
    rep("@URL_BG@",          c.urlBg);
    rep("@URL_FOCUS_BG@",    c.urlFocusBg);
    rep("@URL_FOCUS_BORDA@", c.urlFocusBorda);
    rep("@BTN_HOVER@",       c.btnHover);
    rep("@BTN_PRESS@",       c.btnPress);
    rep("@MENU_BG@",         c.menuBg);
    rep("@MENU_BORDA@",      c.menuBorda);
    rep("@MENU_HOVER@",      c.menuHover);
    rep("@DESTAQUE@",        c.destaque);
    rep("@DESTAQUE_TEXTO@",  c.destaqueTexto);
    rep("@BOTAO_BG@",        c.botaoBg);
    rep("@BOTAO_BORDA@",     c.botaoBorda);
    rep("@BOTAO_HOVER@",     c.botaoHover);
    rep("@BOTAO_PRESS@",     c.botaoPress);
    return css;
}

} // namespace

void Settings::applyTheme(const QString &theme)
{
    if (!qApp)
        return;

    // Captura o padrão original do sistema na primeira chamada
    static const QPalette s_defaultPalette = qApp->palette();

    if (theme == QLatin1String("dark")) {
        qApp->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

        QPalette p;
        p.setColor(QPalette::Window,          QColor("#36393F")); // Fundo da janela/toolbar
        p.setColor(QPalette::WindowText,      QColor("#E8EAED")); // Texto de menus e rótulos
        p.setColor(QPalette::Base,            QColor("#1E2124")); // Fundo de campos de texto
        p.setColor(QPalette::AlternateBase,   QColor("#26292E")); // Fundo alternado de listas
        p.setColor(QPalette::ToolTipBase,     QColor("#2B2D31")); // Fundo do tooltip
        p.setColor(QPalette::ToolTipText,     QColor("#E8EAED")); // Texto do tooltip
        p.setColor(QPalette::Text,            QColor("#E8EAED")); // Texto digitado na URL
        p.setColor(QPalette::Button,          QColor("#36393F")); // Fundo dos botões
        p.setColor(QPalette::ButtonText,      QColor("#E8EAED")); // Texto dos botões
        p.setColor(QPalette::BrightText,      QColor("#FF6B5E")); // Texto de alto contraste
        p.setColor(QPalette::Link,            QColor("#8AB4F8")); // Links
        p.setColor(QPalette::Highlight,       QColor("#8AB4F8")); // Seleção
        p.setColor(QPalette::HighlightedText, QColor("#202124")); // Texto selecionado
        p.setColor(QPalette::PlaceholderText, QColor("#9AA0A6")); // Placeholder da URL
        p.setColor(QPalette::Disabled, QPalette::Text,       QColor("#7A7E85")); // Texto desativado
        p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#7A7E85")); // Botão desativado
        p.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#7A7E85")); // Janela desativada
        qApp->setPalette(p);
        qApp->setStyleSheet(folhaEstiloChrome(coresEscuras()));

    } else if (theme == QLatin1String("light")) {
        qApp->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

        QPalette p;
        p.setColor(QPalette::Window,          QColor("#FFFFFF")); // Fundo BRANCO da janela
        p.setColor(QPalette::WindowText,      QColor("#202124")); // Texto quase preto
        p.setColor(QPalette::Base,            QColor("#FFFFFF")); // Fundo branco da barra de URL
        p.setColor(QPalette::AlternateBase,   QColor("#F0F2F4")); // Linhas alternadas de listas
        p.setColor(QPalette::ToolTipBase,     QColor("#FFFFFF")); // Fundo do tooltip
        p.setColor(QPalette::ToolTipText,     QColor("#202124")); // Texto do tooltip
        p.setColor(QPalette::Text,            QColor("#202124")); // Texto digitado na URL
        p.setColor(QPalette::Button,          QColor("#FFFFFF")); // Fundo dos botões
        p.setColor(QPalette::ButtonText,      QColor("#202124")); // Texto dos botões
        p.setColor(QPalette::BrightText,      QColor("#D93025")); // Texto de alto contraste
        p.setColor(QPalette::Link,            QColor("#1A73E8")); // Links azul
        p.setColor(QPalette::Highlight,       QColor("#1A73E8")); // Seleção
        p.setColor(QPalette::HighlightedText, QColor("#FFFFFF")); // Texto selecionado branco
        p.setColor(QPalette::PlaceholderText, QColor("#5F6368")); // Placeholder da URL
        p.setColor(QPalette::Disabled, QPalette::Text,       QColor("#A7ABB0")); // Texto desativado
        p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#A7ABB0")); // Botão desativado
        p.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#A7ABB0")); // Janela desativada
        qApp->setPalette(p);
        qApp->setStyleSheet(folhaEstiloChrome(coresClaras()));

    } else { // "system"
        qApp->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
        qApp->setPalette(s_defaultPalette);

        // Segue o claro/escuro da paleta do sistema
        const bool escuro = s_defaultPalette.color(QPalette::Window).lightness() < 128;
        qApp->setStyleSheet(folhaEstiloChrome(escuro ? coresEscuras() : coresClaras()));
    }
}
