#include "log.h"

namespace sylar {
    Logger::Logger(const std::string& name = "root") {
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
    void Logger::log(LogLevel::level level, LogEvent::ptr event) {
        if (level >= m_level) {
            for (auto& appender : m_appenders) {
                appender->log(level, event);
            }
        }
    }

    /**
     * 五种日志级别的便捷函数，当发生对应级别的事件时，
     * 调用log函数输出到各个日志输出地
     * @param event 日志事件
     * @return void
     */
    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);
    }
    void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);
    }
    void Logger::warn(LogEvent::ptr event) {
        log(LogLevel::WARN, event);
    }
    void Logger::error(LogEvent::ptr event) {
        log(LogLevel::ERROR, event);
    }
    void Logger::fatal(LogEvent::ptr event) {
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
    void Logger::delAppender(LogAppender::ptr appender) {
        for (auto it = m_appenders.begin();
            it != m_appenders.end(); ++it) {
            if (*it == appender) {
                m_appenders.erase(it);
                break;
            }
            }
    }
}