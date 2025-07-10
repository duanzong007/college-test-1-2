#include "mainwindow.h"
#include "core_system.h"
#include "logger.h"
#include "gui/sideBar.h"
#include "gui/LoadWindowLay.h"
#include <QApplication>
#include <QIcon>
#include <QCoreApplication>
#include <QtGlobal>
#include <QMessageLogContext>


static LoadWindowLay *g_splash = nullptr;
static QtMessageHandler g_prevHandler = nullptr;

void uiMessageHandler(QtMsgType type,
                      const QMessageLogContext &context,
                      const QString &msg)
{
    if (g_splash) {
        g_splash->appendMessage(msg);
        QCoreApplication::processEvents();
    }

    if (g_prevHandler) {
        g_prevHandler(type, context, msg);
    }
}


int main(int argc, char *argv[])
{
    // qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu --disable-software-rasterizer --disable-gpu-compositing --disable-features=Vulkan --single-process --no-sandbox");


    // QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    // QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);


    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/images/logo.ico"));

    LoadWindowLay splash;
    splash.show();
    QCoreApplication::processEvents();
    g_splash = &splash;

    g_prevHandler = qInstallMessageHandler(uiMessageHandler);

    //日志记录启动
    Logger::init("log");


    //核心代码启动
    core_system system;
    if(!system.run())  return -1;
    system.refile();



    splash.close();


    //创建主窗口
    MainWindow w;
    // 创建 sideBar 实例
    sideBar *m_sideBar = new sideBar(&system, w.getUi()->contentWidget);
    // 获取 contentWidget 下的布局
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(w.getUi()->contentWidget->layout());
    // 将 sideBar 添加到布局中
    layout->addWidget(m_sideBar);
    //启动窗口
    w.show();

    // 连接 aboutToQuit 信号，确保程序退出时删除临时日志文件
    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        Logger::shutdown();  // 在应用退出时关闭并删除临时日志文件
    });

    return a.exec();
}
