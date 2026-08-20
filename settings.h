#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QList>

struct SearchEngine
{
    QString name;           // Nome exibido nas configurações
    QString homeUrl;        // Página inicial
    QString searchTemplate; // %1 = termo da pesquisa
};

class Settings
{
public:
    static QList<SearchEngine> availableEngines();

    static SearchEngine currentEngine();
    static int currentEngineIndex();
    static void saveEngineIndex(int index);

    static QString theme();                 // "system", "light", "dark"
    static void saveTheme(const QString &theme);
    static void applyTheme(const QString &theme);
};

#endif // SETTINGS_H