#include "logger.h"

static QFile              s_logFile;
static QFile              s_tempLogFile;
static QTextStream        s_logStream;
static QTextStream        s_tempLogStream;
static QMutex             s_mutex;
static QtMessageHandler   s_prevHandler = nullptr;   // 保留旧处理器（可继续输出到控制台）

/* 格式辅助：把 QtMsgType → 字符串 */
static inline QString levelName(QtMsgType t)
{
    switch (t) {
    case QtDebugMsg:    return "Debug";
    case QtInfoMsg:     return "Info";
    case QtWarningMsg:  return "Warning";
    case QtCriticalMsg: return "Critical";
    case QtFatalMsg:    return "Fatal";
    }
    return "Unknown";
}

/* 真正的消息处理器 */
static void messageHandler(QtMsgType type,
                           const QMessageLogContext& ctx,
                           const QString& msg)
{
    QMutexLocker lock(&s_mutex);

    /* 时间戳 + 线程ID */
    const QString ts  = QDateTime::currentDateTime()
                           .toString("yyyy-MM-dd hh:mm:ss.zzz");
    const QString thr = QStringLiteral("0x%1")
                            .arg((quintptr)QThread::currentThreadId(), 0, 16);

    const QString tss = QDateTime::currentDateTime().toString("hh:mm:ss");

    s_logStream  << ts << ' '
                << levelName(type) << " [T" << thr << "] "
                << msg;

    s_tempLogStream << tss << ' '
                    << levelName(type)<< ' '
                    << msg;


    /* 附加源码位置信息（可选） */
    if (ctx.file && *ctx.file)
        s_logStream << "  (" << ctx.file << ':' << ctx.line << ')';

    s_logStream << '\n';
    s_logStream.flush();        // 立刻落盘，崩溃时也能保住

    s_tempLogStream << '\n';
    s_tempLogStream.flush();

    /* 若想同时保留控制台输出：把消息继续交给原处理器 */
    if (s_prevHandler)
        s_prevHandler(type, ctx, msg);

    if (type == QtFatalMsg)      // fatal 依旧终止
        ::abort();
}

/* =========== 对外 API =========== */
void Logger::init(const QString& baseName)
{
    /* ——① 目标目录：<exeDir>/logs —— */
    QString dirPath = QCoreApplication::applicationDirPath() + "/logs";
    QDir().mkpath(dirPath);                       // 若不存在就创建

    /* ——② 拼完整文件名 —— */
    const QString filePath =
        dirPath + '/' + baseName + '_' +
        QDateTime::currentDateTime().toString("yyyyMMdd") + ".log";


    s_logFile.setFileName(filePath);
    if (!s_logFile.open(QIODevice::Append | QIODevice::Text))
        qFatal("Cannot open log file: %s", qPrintable(filePath));

    // 创建临时日志文件
    const QString tempFilePath = dirPath + "/log_temp.txt";  // 临时日志文件
    s_tempLogFile.setFileName(tempFilePath);
    if (!s_tempLogFile.open(QIODevice::Append | QIODevice::Text))  // 临时文件
        qFatal("Cannot open temporary log file: %s", qPrintable(tempFilePath));

    s_logStream.setDevice(&s_logFile);
    s_tempLogStream.setDevice(&s_tempLogFile);  // 临时日志流

    /* 2. 安装自定义 handler 并保存旧 handler */
    s_prevHandler = qInstallMessageHandler(messageHandler);

    /* 3. 可选：控制台格式（不影响文件） */
    qSetMessagePattern("%{type} %{message}");

qDebug() << "Log file:" << filePath;
}

void Logger::shutdown()
{
    qInstallMessageHandler(s_prevHandler);   // 恢复
    s_logStream.flush();
    s_logFile.close();

    // 刷新并关闭临时日志文件
    s_tempLogStream.flush();
    s_tempLogFile.close();

    // 删除临时日志文件
    QFile::remove(s_tempLogFile.fileName());
}
