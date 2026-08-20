#include "settingsdialog.h"
#include "settings.h"

#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Configurações"));
    setMinimumWidth(450);

    auto *layout = new QVBoxLayout(this);
    auto *form   = new QFormLayout;

    // --- Mecanismo de pesquisa ---
    m_cboMecanismo = new QComboBox(this);
    const QList<SearchEngine> engines = Settings::availableEngines();
    for (const SearchEngine &e : engines)
        m_cboMecanismo->addItem(e.name);
    m_cboMecanismo->setCurrentIndex(Settings::currentEngineIndex());
    form->addRow(tr("Mecanismo de pesquisa padrão:"), m_cboMecanismo);

    // --- Tema ---
    m_cboTema = new QComboBox(this);
    m_cboTema->addItem(tr("Sistema (padrão)"), "system");
    m_cboTema->addItem(tr("Claro"),            "light");
    m_cboTema->addItem(tr("Escuro"),           "dark");
    const int idxTema = m_cboTema->findData(Settings::theme());
    if (idxTema >= 0)
        m_cboTema->setCurrentIndex(idxTema);
    form->addRow(tr("Tema da interface:"), m_cboTema);

    layout->addLayout(form);

    auto *info = new QLabel(tr("O mecanismo escolhido será usado como página inicial "
                               "e para as pesquisas digitadas na barra de endereço."), this);
    info->setWordWrap(true);
    layout->addWidget(info);

    // --- Botões ---
    auto *buttons       = new QDialogButtonBox(this);
    QPushButton *btnAplicar   = buttons->addButton(tr("Aplicar"),   QDialogButtonBox::ApplyRole);
    QPushButton *btnOk        = buttons->addButton(tr("OK"),        QDialogButtonBox::AcceptRole);
    QPushButton *btnCancelar  = buttons->addButton(tr("Cancelar"),  QDialogButtonBox::RejectRole);
    Q_UNUSED(btnOk);
    layout->addWidget(buttons);

    connect(btnAplicar,   &QPushButton::clicked, this, &SettingsDialog::aplicar);
    connect(btnCancelar,  &QPushButton::clicked, this, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, this, [this] {
        aplicar();
        accept();
    });
}

void SettingsDialog::aplicar()
{
    Settings::saveEngineIndex(m_cboMecanismo->currentIndex());

    const QString tema = m_cboTema->currentData().toString();
    Settings::saveTheme(tema);
    Settings::applyTheme(tema);
}