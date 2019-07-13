/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   config.h
 * Author: duocore
 *
 * Created on 2019年7月9日, 下午9:59
 */

#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <vector>
#include "util/Singleton.h"

/**
 * \class config
 * {
 * "version":"1.0.0.1",
 * "manage_server":"pe.joy189.com:6636",
 * "torrent_list" : [
 * {
 * "torrent_file":"windows_7_ultramate.iso.torrent",
 * "describe":"MSDN Windows 7 with sp1 4 in 1"
 * },
 * {
 * "torrent_file":"windows_81_home.iso.torrent",
 * "describe" : "MSDN Windows 8.1 home edition."
 * }
 * ]
 * }
 */

namespace file_seeder {

    class config : public qcutil::Singleton<config> {
    public:
        struct seed_tasks_in_conf {
            std::string torrent_file;
            std::string desc;
        };

        bool load() ;
        std::string get_version() const { return m_version; }
        void set_version(const std::string& v) { m_version = v; }
        
        std::string get_manager_server() const { return m_manager_server; }
        void set_manager_server(const std::string& m) { m_manager_server = m; }
        
        std::vector<seed_tasks_in_conf> get_tasks() const { return m_seed_tasks; }
        void set_tasks(const std::vector<seed_tasks_in_conf>& s) { m_seed_tasks = s; }
    private:
        std::string m_version;
        std::string m_manager_server;
        std::vector<seed_tasks_in_conf> m_seed_tasks;
    };
}

#endif /* CONFIG_H */

