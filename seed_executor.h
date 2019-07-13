/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   seed_executor.h
 * Author: duocore
 *
 * Created on 2019年7月11日, 下午8:49
 */

#ifndef SEED_EXECUTOR_H
#define SEED_EXECUTOR_H
#include <string>
#include <vector>
#include "cptype.h"
#include "config.h"
#include <boost/shared_ptr.hpp>


namespace file_seeder {
    class torrent_client;
    class easy_timer;

    class seed_executor {
    public:
        seed_executor(const std::string& work_dir, const std::vector<config::seed_tasks_in_conf>& tasks);
        ~seed_executor();
    public:
        bool start();
        bool stop();
    private:
        void query_torrent_status();
    private:
        boost::shared_ptr<file_seeder::torrent_client> m_seed_client;
        boost::shared_ptr<file_seeder::easy_timer> m_query_timer;
        std::string m_work_dir;
        std::vector<config::seed_tasks_in_conf> m_tasks_conf;
        std::vector<DWORD> m_task_id_array;
    };
}

#endif /* SEED_EXECUTOR_H */

