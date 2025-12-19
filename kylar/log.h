#pragma once 
#include <string>
#include <stdint.h>
#include <memory>
#include <vector>
#include <list>

namespace sylar {
    class LogLevel {
    public:
        enum level {
            DEBUG = 0,      // DEBUG 级别
            INFO = 1,       // INFO 级别
            WARN = 2,       // WARN 级别
            ERROR = 3,      // ERROR 级别
            FATAL = 4       // FATAL 级别
        };
    };

    // 日志事件
    class LogEvent {
    public:
        using ptr = std::shared_ptr<LogEvent>;
        LogEvent();

    private:
        const char* m_file = nullptr;       // 文件名
        int32_t m_line = 0;                 // 行号
        int32_t m_elapse = 0;               // 程序启动开始到现在的毫秒数
        int32_t m_threadId = 0;             // 线程ID
        int32_t m_fiberId = 0;              // 协程ID
        uint64_t m_time = 0;                // 时间戳
        std::string m_content;              // 日志内容
    };

    // 日志格式器
    class LogFormatter {
    public:
        using ptr = std::shared_ptr<LogFormatter>;

        std::string format(LogEvent::ptr event);
    private:
    };

    // 日志输出地
    class LogAppender {
    public:
        using ptr = std::shared_ptr<LogAppender>;
        virtual ~LogAppender() {}

        virtual void log(LogLevel::level level, LogEvent::ptr event) = 0;
    private:
        LogLevel::level m_level;
    };

    // 日志器
    class Logger {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string& name = "root");
        void log(LogLevel::level level, LogEvent::ptr event);

        // 五种日志级别的便捷函数
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);

        inline
            void setLevel(LogLevel::level level) { m_level = level; }
        inline
            LogLevel::level getLevel() const { return m_level; }
    private:
        std::string m_name;                          // 日志名称
        LogLevel::level m_level;                     // 日志级别
        std::list<LogAppender::ptr> m_appenders;     // 日志输出地集合
    };

    /**
     * 输出到控制台的日志器
     * @note 输出到控制台的日志器通常用于调试阶段
     */
    class StdoutLogAppender : public LogAppender {
    public:
        using ptr = std::shared_ptr<StdoutLogAppender>;
        void log(LogLevel::level level, LogEvent::ptr event) override;
    };

    /**
     * 输出到文件的日志器
     * @note 每次写文件之前，都会重新打开文件，适合日志文件不大的情况
     */
    class FileLogAppender : public LogAppender {
    public:
        using ptr = std::shared_ptr<FileLogAppender>;

        /**
         * 构造函数
         * @param filename 文件名
         */
        FileLogAppender(const std::string& filename);
        void log(LogLevel::level level, LogEvent::ptr event) override;
    private:
        std::string m_filename;
    };
}