#include "log.h"
#include <iostream>

namespace sylar {
    const char* LogLevel::toString(LogLevel::level level) {
        switch (level) {
#define XX(name)  \
            case LogLevel::name:  \
            return #name;  \
            break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
            default:
            return "UNKNOWN";
        }
    }

    Logger::Logger(const std::string &name) {
        this->m_name = name;
        this->m_level = LogLevel::DEBUG;
    }

    /**
     * 日志器的日志输出函数
     * @param level 日志级别
     * @param event 日志事件
     * @note 只有当日志级别大于等于日志器的日志级别时，才会输出
     *       到各个日志输出地，否则忽略该日志事件
     * @return void
     */
    void Logger::log(LogLevel::level level, const LogEvent::ptr& event) {
        auto logger = Logger::shared_from_this();
        if (level >= m_level) {
            for (auto &appender: m_appenders) {
                appender->log(logger, level, event);
            }
        }
    }

    /**
     * 五种日志级别的便捷函数，当发生对应级别的事件时，
     * 调用log函数输出到各个日志输出地
     * @param event 日志事件
     * @return void
     */
    void Logger::debug(const LogEvent::ptr& event) {
        log(LogLevel::DEBUG, event);
    }

    void Logger::info(const LogEvent::ptr& event) {
        log(LogLevel::INFO, event);
    }

    void Logger::warn(const LogEvent::ptr& event) {
        log(LogLevel::WARN, event);
    }

    void Logger::error(const LogEvent::ptr& event) {
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(const LogEvent::ptr& event) {
        log(LogLevel::FATAL, event);
    }


    /**
     * 添加日志输出地
     * @param appender 日志输出地
     * @return void
     */
    void Logger::addAppender(LogAppender::ptr appender) {
        this->m_appenders.push_back(appender);
    }

    /**
     * 删除日志输出地
     * @param appender 日志输出地
     * @return void
     */
    void Logger::delAppender(const LogAppender::ptr& appender) {
        for (auto it = m_appenders.begin();
             it != m_appenders.end(); ++it) {
            if (*it == appender) {
                m_appenders.erase(it);
                break;
            }
        }
    }

    FileLogAppender::FileLogAppender(const std::string &filename)
        : m_filename(filename) {
    }

    /**
     * appender输出地的日志输出函数
     * @param level 日志级别
     * @param event 日志事件
     * @return void
     */
    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) {
        if (level >= m_level) {
            std::cout << m_formatter->format(logger, level, event);
        }
    }

    bool FileLogAppender::reopen() {
        if (m_fileStream.is_open()) {
            m_fileStream.close();
        }
        m_fileStream.clear();
        m_fileStream.open(m_filename, std::ios::app);
        return m_fileStream.is_open();
    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) {
        if (level >= m_level) {
            m_fileStream << m_formatter->format(logger, level, event);
        }
    }

    LogFormatter::LogFormatter(const std::string &pattern)
        : m_pattern(pattern) {
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::level level ,LogEvent::ptr event) {
        std::stringstream ss;

        for (auto item: m_items) {
            item->format(ss, logger, level, event);
        }

        return ss.str();
    }

    /*
     * 匹配一个百分号开头的字符串到合法的日志格式项
     * @param str 待匹配字符串
     * @return 匹配成功返回对应的日志格式项，失败返回nullptr
     */
    /*LogFormatter::FormatItem::ptr LogFormatter::getFormatItem(const std::string &str) {
        static std::map<std::string, std::function<LogFormatter::FormatItem::ptr(const std::string &fmt)>>
            s_format_items = {
            {"m", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new MessageFormatItem(fmt)); }},
            {"p", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new LevelFormatItem(fmt)); }},
            {"r", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new ElapseFormatItem(fmt)); }},
            {"c", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new NameFormatItem(fmt)); }},
            {"d", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new DateTimeFormatItem(fmt)); }},
            {"f", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new FilenameFormatItem(fmt)); }},
            {"l", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new LineFormatItem(fmt)); }},
            {"t", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new ThreadIdFormatItem(fmt)); }},
            {"F", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new FiberIdFormatItem(fmt)); }},
            {"N", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new NewLineFormatItem(fmt)); }},
            {"T", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new TabFormatItem(fmt)); }},
            {"%", [](const std::string &fmt) { return LogFormatter::FormatItem::ptr(new StringFormatItem(fmt)); }},
        };

        auto it = s_format_items.find(str);
        if (it != s_format_items.end()) {
            return it->second("");
        }
        return nullptr;
    }*/


    /**
     * 初始化日志格式器 %XXX %XXX{XXX} %%
     * @return void
     */
    void LogFormatter::init() {
        std::vector<std::tuple<std::string, std::string, int> > vec;
        const auto len_pattern = m_pattern.size();
        std::string nstr;

        for (size_t i = 0; i < m_pattern.size(); ++i) {
            if (m_pattern[i] != '%') {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size()) {
                if (m_pattern[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }

            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;
            while (n < m_pattern.size()) {
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                                    && m_pattern[n] != '}')) {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if (fmt_status == 0) {
                    if (m_pattern[n] == '{') {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        //std::cout << "*" << str << std::endl;
                        fmt_status = 1; //解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                } else if (fmt_status == 1) {
                    if (m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        //std::cout << "#" << fmt << std::endl;
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size()) {
                    if (str.empty()) {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if (fmt_status == 0) {
                if (!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } else if (fmt_status == 1) {
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if(!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
    }

    /*
     * %m -- 消息体
     * %p -- level
     * %r -- 启动到现在的毫秒数
     * %c -- 日志名称
     * %t -- 线程id
     * %n -- 换行
     * %d -- 时间
     * %f -- 文件名
     * %l -- 行号
     */

    class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        void format(std::stringstream &os, std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) override {
            os << event->getContent();
        };
    };
    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        void format(std::stringstream &os, std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) override {
            os << LogLevel::toString(level);
        };
    };
    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
        void format(std::stringstream &os, std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) override {
            os << event->getElapse();
        };
    };

    class LoggerNameFormatItem : public LogFormatter::FormatItem {
    public:
        void format(std::stringstream &os, std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) override {
            os << logger.getName();
        };
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem {
    public:
        void format(std::stringstream &os, std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) override {
            os << logger.getThreadId();
        };
    };

    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        void format(std::stringstream &os, std::shared_ptr<Logger> logger, LogLevel::level level, LogEvent::ptr event) override {
            os << LogLevel::toString(level);
        };
    };
}
