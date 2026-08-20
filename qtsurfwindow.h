#ifndef QTSURFWINDOW_H
#define QTSURFWINDOW_H

#include <QMainWindow>
#include <QUrl>

class TabWebView;
class QAction;

QT_BEGIN_NAMESPACE
namespace Ui { class BrowserWindow; }
QT_END_NAMESPACE

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BrowserWindow(QWidget *parent = nullptr);
    ~BrowserWindow() override;

private slots:
    void navegar();
    void irParaHome();
    void atualizarPagina();
    void voltar();
    void avancar();
    void onSolicitacaoTelaCheia(bool ativado);
    void novaAba();
    void fecharAba(int index);
    void fecharAbaAtual();
    void abaAtualMudou(int index);
    void abrirConfiguracoes();

private:
    void configurarConexoes();
    void configurarMenuBar();
    void configurarMenuPrincipal();
    void aplicarIconesDoTema();
    void atualizarBotoesNavegacao();
    QUrl processarUrl(const QString &texto);
    TabWebView *criarNovaAba(const QUrl &url = QUrl(), bool switchTo = true, bool loadHome = true);
    TabWebView *currentView() const;
    void atualizarBarraUrl();
    void atualizarTitulo();

    Ui::BrowserWindow *ui;
    QAction *m_acaoBusca = nullptr; // lupa dentro do omnibox
    QString m_homePage;
    bool m_emTelaCheia = false;
};

#endif // QTSURFWINDOW_H
