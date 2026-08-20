#include "qtsurfwindow.h"
#include "settings.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QIcon>

int main(int argc, char *argv[])
{
    // Força otimizações agressivas de GPU para AMD e WebGL
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
            "--ignore-gpu-blocklist "
            "--enable-gpu-rasterization "
            "--enable-zero-copy "
            "--enable-accelerated-video-decode "
            "--enable-accelerated-2d-canvas "
            "--enable-native-gpu-memory-buffers "
            "--enable-webgl2 "
            "--use-angle=d3d11 "
            "--disable-gpu-sandbox "
            "--disable-software-rasterizer");

    qputenv("QT_LOGGING_RULES", "*.debug=false;*.warning=false");

    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication a(argc, argv);

    // Nome e organização do aplicativo
    a.setApplicationName("QtSurf");
    a.setApplicationDisplayName("QtSurf");
    a.setOrganizationName("QtSurf");
    a.setApplicationVersion("1.1.0");

    // Define o ícone da aplicação
    a.setWindowIcon(QIcon(":/Windows-Icone/icones/Browser.png"));

    Settings::applyTheme(Settings::theme());

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "browser_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    BrowserWindow w;
    w.showMaximized();

    return a.exec();
}
