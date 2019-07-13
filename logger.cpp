/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   logger.cpp
 * Author: duocore
 * 
 * Created on 2019年7月2日, 下午11:19
 */

#include "logger.h"
#include <ctime>
#include <sstream>

namespace file_seeder {
    logger* logger::m_inst = nullptr;
    logger::logger()
    {
        time_t cur_time = time(nullptr);
        struct tm * plocal_time = localtime (&cur_time);
        std::stringstream ss;
        ss << "file_seeder_" << plocal_time->tm_year + 1900 << "_" << plocal_time->tm_mon+1 << "_" << plocal_time->tm_mday
                << "_" << plocal_time->tm_hour << "_" << plocal_time->tm_min << "_" << plocal_time->tm_sec << ".txt";
        m_ofs.open(ss.str().c_str());
    }

    logger::~logger() {
        if (m_ofs.is_open())
            m_ofs.close();
    }
    
    logger& logger::get_instance()
    {
        if(m_inst == nullptr)
        {
            m_inst = new logger();
        }
        return *m_inst;
    }
    
    void logger::write(const std::string& msg)
    {
        if (m_ofs.is_open())
        {
            boost::recursive_mutex::scoped_lock l(m_file_io_mutex);
            m_ofs << msg;
            m_ofs.flush();
        }
    }
}