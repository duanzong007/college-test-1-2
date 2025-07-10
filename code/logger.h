#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QtGlobal>
#include <QThread>
#include <QCoreApplication>

/* 单例式轻量日志，不依赖第三方 */
namespace Logger {

void init(const QString& baseName = "app");   // 在 main() 最早调用
void shutdown();                              // 可选：程序退出手动关闭

} // namespace Logger


#endif // LOGGER_H
