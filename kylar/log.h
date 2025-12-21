#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <iostream>

namespace sylar {
    class Logger;

    class LogLevel {
    public:
        enum level {
            DEBUG = 0, // DEBUG 级别
            INFO = 1, // INFO 级别
            WARN = 2, // WARN 级别
            ERROR = 3, // ERROR 级别
            FATAL = 4 // FATAL 级别
        };

        static const char* toString(LogLevel::level level);
    };

    // 日志事件
    class LogEvent {
    public:
        using ptr = std::shared_ptr<LogEvent>;

        LogEvent() = default;

        int32_t getLine() const { return m_line; }
        const char* getFile() const { return m_file; }
        int32_t getElapse() const { return m_elapse; }
        int32_t getThreadId() const { return m_threadId; }
        int32_t getFiberId() const { return m_fiberId; }
        uint64_t getTime() const { return m_time; }
        const std::string& getContent() const {
            return this->m_content;
        }

    private:
        const char* m_file = nullptr; // 文件名
        int32_t m_line = 0; // 行号
        int32_t m_elapse = 0; // 程序启动开始到现在的毫秒数
        int32_t m_threadId = 0; // 线程ID
        int32_t m_fiberId = 0; // 协程ID
        uint64_t m_time = 0; // 时间戳
        std::string m_content; // 日志内容
    };

    // 日志格式器
    class LogFormatter {
    public:
        using ptr = std::shared_ptr<LogFormatter>;

        LogFormatter(const std::string& pattern);

        /*
         * 格式化日志事件,返回格式化后的字符串
         * @param event 日志事件
         */
        std::string format(std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event);

        /**
         * @brief 是否有错误
         */
        bool isError() const { return m_error; }

        /**
         * @brief 返回日志模板
         */
        std::string getPattern() const { return m_pattern; }

        void init(); // 初始化，解析用户初始化传入的pattern
        class FormatItem {
        public:
            using ptr = std::shared_ptr<FormatItem>;

            FormatItem(const std::string& fmt = "") {};
            virtual ~FormatItem() = default;

            virtual void format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) = 0;
        };

    private:
        std::vector<FormatItem::ptr> m_items;
        std::string m_pattern;
        bool m_error = false;
    };

    // 日志输出地
    class LogAppender {
    public:
        using ptr = std::shared_ptr<LogAppender>;

        virtual ~LogAppender() = default;

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) = 0;

        void setFormatter(LogFormatter::ptr formatter) {
            m_formatter = std::move(formatter);
        }

        LogFormatter::ptr getFormatter() const {
            return m_formatter;
        }

    protected: //
        LogLevel::level m_level = LogLevel::DEBUG;
        LogFormatter::ptr m_formatter;
    };

    // 日志器
    class Logger : public std::enable_shared_from_this<Logger> {
    public:
        using ptr = std::shared_ptr<Logger>;

        Logger(const std::string& name = "root");

        void log(LogLevel::level level, const LogEvent::ptr& event);

        // 五种日志级别的便捷函数
        void debug(const LogEvent::ptr& event);

        void info(const LogEvent::ptr& event);

        void warn(const LogEvent::ptr& event);

        void error(const LogEvent::ptr& event);

        void fatal(const LogEvent::ptr& event);

        void addAppender(LogAppender::ptr appender);

        void delAppender(const LogAppender::ptr& appender);

        inline
            void setLevel(LogLevel::level level) { m_level = level; }

        inline
            LogLevel::level getLevel() const { return m_level; }

        inline const std::string& getName() const { return m_name; }

        // inline const auto& getThreadId() const { return 0; /* TODO */ }

    private:
        std::string m_name; // 日志名称
        LogLevel::level m_level = LogLevel::DEBUG; // 日志级别
        std::list<LogAppender::ptr> m_appenders; // 日志输出地集合
    };

    /**
     * 输出到控制台的日志器
     * @note 输出到控制台的日志器通常用于调试阶段
     */
    class StdoutLogAppender : public LogAppender {
    public:
        using ptr = std::shared_ptr<StdoutLogAppender>;

        void log(std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) override;
    };

    /**
     * 输出到文件的日志器
     * @note 每次写文件之前，都会重新打开文件，适合日志文件不大的情况
     */
    class FileLogAppender : public LogAppender {
    public:
        using ptr = std::shared_ptr<FileLogAppender>;

        FileLogAppender(const std::string& filename);

        void log(std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) override;

        // 重新打开文件，文件打开成功返回true，否则返回false
        bool reopen();

    private:
        std::string m_filename;
        std::ofstream m_fileStream;
    };
}
