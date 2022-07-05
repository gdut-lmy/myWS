//
// Created by lmy on 2022/6/24.
//

#ifndef MYWS_LOG_H
#define MYWS_LOG_H

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <mutex.h>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <singleton.h>
#include <functional>
#include <map>
#include <config.h>
#include "threadPool.h"


#define MY_LOG_LEVEL(logger,level)    \
    if(level >= logger->getLevel())           \
        my::LogEventWrap(std::make_shared<my::LogEvent>( \
        logger,level,__FILE__,__LINE__,my::GetThreadId(),my::GetFiberId(), \
        0, time(0) ,my::threadPool::GetName()     \
        )).getSS()
#define MY_LOG_DEBUG(logger)  MY_LOG_LEVEL(logger,my::LogLevel::DEBUG)
#define MY_LOG_INFO(logger)   MY_LOG_LEVEL(logger,my::LogLevel::INFO)
#define MY_LOG_WARN(logger)   MY_LOG_LEVEL(logger,my::LogLevel::WARN)
#define MY_LOG_ERROR(logger)  MY_LOG_LEVEL(logger,my::LogLevel::ERROR)
#define MY_LOG_FATAL(logger)  MY_LOG_LEVEL(logger,my::LogLevel::FATAL)

#define LOG_DEBUG MY_LOG_DEBUG(my::LogMgr::GetInstance()->getLogger("root"))

#define MY_LOG_FMT_LEVEL(logger,level,fmt,...)    \
    if(level >= logger->getLevel())           \
        my::LogEventWrap(std::make_shared<my::LogEvent>( \
        logger,level,__FILE__,__LINE__,my::GetThreadId(),my::GetFiberId(), \
        0, time(0) ,my::Thread::GetName()     \
        )).getEvent()->format(fmt,__VA_ARGS__)
#define MY_LOG_FMT_DEBUG(logger,fmt,...)  MY_LOG_FMT_LEVEL(logger,my::LogLevel::DEBUG,fmt,__VA_ARGS__)
#define MY_LOG_FMT_INFO(logger,fmt,...)   MY_LOG_FMT_LEVEL(logger,my::LogLevel::INFO,fmt,__VA_ARGS__)
#define MY_LOG_FMT_WARN(logger,fmt,...)   MY_LOG_FMT_LEVEL(logger,my::LogLevel::WARN,fmt,__VA_ARGS__)
#define MY_LOG_FMT_ERROR(logger,fmt,...)  MY_LOG_FMT_LEVEL(logger,my::LogLevel::ERROR,fmt,__VA_ARGS__)
#define MY_LOG_FMT_FATAL(logger,fmt,...)  MY_LOG_FMT_LEVEL(logger,my::LogLevel::FATAL,fmt,__VA_ARGS__)
#define MY_LOG_ROOT() my::LogMgr::GetInstance()->getRoot()
#define MY_LOG_NAME(name) my::LogMgr::GetInstance()->getLogger(name)
    namespace my {
        class Logger;
//日志级别
        class LogLevel{
        public:
            enum Level {
                DEBUG,
                INFO,
                WARN,
                ERROR,
                FATAL,
                UNKNOW
            };
            static const char* toString(LogLevel::Level level);
            static LogLevel::Level fromString(const std::string& str);
        };
//日志事件
        class LogEvent{
        public:
            using ptr = std::shared_ptr<LogEvent>;
            LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level
                    ,const char* file,int32_t line,int32_t threadId,int32_t fiberId
                    ,int32_t elapse,int64_t timestamp
                    ,const std::string& threadName);
            const char* getFile() const { return m_file;}
            int32_t getLine() const { return m_line;}
            int32_t getThreadId() const { return m_threadId;}
            int32_t getFiberId() const { return m_fiberId;}
            int32_t getElapse() const { return m_elapse;}
            int64_t getTime() const { return m_time;}
            std::string getContent() const { return m_ss.str();}
            std::stringstream& getss() { return m_ss;}
            LogLevel::Level getLevel() const { return m_level;}
            std::string getThreadName() const { return m_threadName;}
            std::shared_ptr<Logger> getLogger() const { return m_logger;}
            void format(const char *fmt,...);
        private:
            std::shared_ptr<Logger> m_logger;
            LogLevel::Level m_level ;       //日志级别
            const char *m_file = nullptr;   //文件名
            int32_t m_line = 0;             //行号
            int32_t m_threadId = 0;         //线程id
            int32_t m_fiberId = 0;          //协程id
            int32_t m_elapse = 0;           //程序开始到现在的毫秒
            int64_t m_time = 0;             //时间戳
            std::stringstream m_ss;         //
            std::string m_threadName;
        };
//RAII LogEvent
        class LogEventWrap{
        public:
            LogEventWrap(LogEvent::ptr event):m_event(event){};
            ~LogEventWrap();
            std::stringstream& getSS() const { return m_event->getss();}
            LogEvent::ptr getEvent() const { return m_event;}
        private:
            LogEvent::ptr m_event;
        };
//日志格式化
        class LogFormatter{
        public:
            using ptr = std::shared_ptr<LogFormatter>;
            /**
             * @brief 构造函数
             * @param[in] pattern 格式模板
             * @details
             *  %m 消息
             *  %p 日志级别
             *  %r 累计毫秒数
             *  %c 日志名称
             *  %t 线程id
             *  %n 换行
             *  %d 时间
             *  %f 文件名
             *  %l 行号
             *  %T 制表符
             *  %F 协程id
             *  %N 线程名称
             *  %% %
             *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
             */
            explicit LogFormatter(const std::string& pattern="%d{%Y-%m-%d %H:%M:%S}%T[ %p%T]%T[%c]%T%t%T%N%T%F%T%T%f:%l%T%m%n");

            std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event);
            std::ostream& format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event);
            const std::string& getPattern() const { return m_pattern;}
            void init();
            bool getError() const { return m_error;}

        public:
            class FormatItem{
            public:
                using ptr = std::shared_ptr<LogFormatter::FormatItem>;
                virtual ~FormatItem(){};
                virtual void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) = 0;
            };
        private:
            std::string m_pattern;
            std::vector<FormatItem::ptr> m_items;
            bool m_error = false;
        };

//日志输出
        class LogAppender{
        public:
            friend class Logger;

            using MutexType = SpinLock;
            using ptr = std::shared_ptr<LogAppender>;
            //LogAppender();
            virtual ~LogAppender() {};
            virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr Event) = 0;

            void setFormatter(LogFormatter::ptr formatter);
            LogFormatter::ptr getFormatter() ;
            LogLevel::Level getLevel() const { return m_level;}
            void setLevel(LogLevel::Level level) { m_level = level;}
            virtual YAML::Node toYaml() = 0;
            bool getHasFormatter() const { return m_hasFormatter;}
            //bool setHasFormatter(bool f) { m_hasFormatter = f;}
        protected:
            LogLevel::Level m_level = LogLevel::DEBUG;
            MutexType m_mutex;
            LogFormatter::ptr m_formatter;
            bool m_hasFormatter = false;
        };
//控制台appender
        class StdoutLogAppender: public LogAppender{
        public:
            using ptr = std::shared_ptr<StdoutLogAppender> ;
            void log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override;
            YAML::Node toYaml() override;
        private:
        };
//文件appender
        class FileLogAppender: public LogAppender{
        public:
            using ptr = std::shared_ptr<FileLogAppender>;
            explicit FileLogAppender(const std::string& filename);
            void log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override;
            bool reopen();
            YAML::Node toYaml() override;
        private:
            std::string m_filename;
            std::ofstream m_filestream;
            uint64_t m_lastTime;
        };
        class LogManager;
//日志器
        class Logger: public std::enable_shared_from_this<Logger>{
        public:
            using ptr = std::shared_ptr<Logger>;
            friend LogManager;
            using MutexType = SpinLock;
            Logger(std::string  = "root");

            void log(LogLevel::Level level,LogEvent::ptr event);

            void trace(LogEvent::ptr event);
            void debug(LogEvent::ptr event);
            void info(LogEvent::ptr event);
            void warn(LogEvent::ptr event);
            void error(LogEvent::ptr event);
            void fatal(LogEvent::ptr event);

            void addAppender(LogAppender::ptr appender);
            void delAppender(LogAppender::ptr appender);
            void clearAppender();

            LogLevel::Level getLevel() const { return m_level;}
            void setLevel(LogLevel::Level level) { m_level = level; }

            void setFormatter(LogFormatter::ptr formatter) ;
            void setFormatter(const std::string& formatter) ;
            LogFormatter::ptr getFormatter() ;

            const std::string& getName() const { return m_name;};

            YAML::Node toYaml();

        private:
            Logger::ptr m_root;
            std::string m_name;                         //日志名称
            LogLevel::Level m_level;                    //日志级别
            std::list<LogAppender::ptr> m_appenders;    //Appender集合
            LogFormatter::ptr m_formatter;              //日志formatter
            MutexType m_mutex;
        };

        class LogManager {
        public:
            using MutexType = SpinLock;
            LogManager();
            Logger::ptr getLogger(const std::string &name);
            Logger::ptr getRoot() const { return m_root;}
            void setLogger(Logger::ptr logger);
            YAML::Node toYaml();
            std::string toString();
            void init();
        private:
            Logger::ptr m_root;
            MutexType m_mutex;
            std::map<std::string ,Logger::ptr> m_loggers;
        };

        using LogMgr = my::SingletonPtr<LogManager>;


    }



#endif //MYWS_LOG_H





