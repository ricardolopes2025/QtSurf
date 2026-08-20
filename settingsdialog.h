#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class QComboBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void aplicar();

private:
    QComboBox *m_cboMecanismo;
    QComboBox *m_cboTema;
};

#endif // SETTINGSDIALOG_H