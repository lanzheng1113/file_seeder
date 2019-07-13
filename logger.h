/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   logger.h
 * Author: duocore
 *
 * Created on 2019年7月2日, 下午11:19
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <sstream>
#include "boost/thread/recursive_mutex.hpp"
#include <ctime>

namespace file_seeder {

    class logger {
    public:
        ~logger();
        static logger& get_instance();
        void write(const std::string& msg);
    private:
        logger();
        static logger* m_inst;
        std::ofstream m_ofs;
        boost::recursive_mutex m_file_io_mutex;
    };

    class log_helper {
    public:

        log_helper() {
            ;
        }

        ~log_helper() {
            std::string str = m_ss.str();
            if (!str.empty())
                logger::get_instance().write(str);
        };

        std::stringstream& get_log_stream() {
            return m_ss;
        }
    public:
    private:
        std::stringstream m_ss;
    };

    inline std::string from_number(int n) {
        std::stringstream ss;
        ss << n;
        return ss.str();
    }

    inline std::string current_time() {
        std::time_t curTime = time(NULL);
        struct std::tm * timeinfo = std::localtime(&curTime);
        std::string year = from_number(timeinfo->tm_year + 1900);
        std::string month = from_number(timeinfo->tm_mon + 1);
        std::string day = from_number(timeinfo->tm_mday);
        std::string hour = from_number(timeinfo->tm_hour);
        std::string minute = from_number(timeinfo->tm_min);
        std::string second = from_number(timeinfo->tm_sec);

        if (month.size() == 1) {
            month = "0" + month;
        }

        if (day.size() == 1) {
            day = "0" + day;
        }

        if (hour.size() == 1) {
            hour = "0" + hour;
        }

        if (minute.size() == 1) {
            minute = "0" + minute;
        }

        if (second.size() == 1) {
            second = "0" + second;
        }

        return year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second;
    }
}

#ifndef SLOG
#ifdef FILE_SEEDER_LOG_DISABLED
#define SLOG 
#else
#define SLOG(A) file_seeder::log_helper().get_log_stream() << "[" << current_time() << "] " << "<" << #A << "> "
#endif
#endif

#endif /* LOGGER_H */

