/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "config.h"
#include "util/Path.h"
#include "util/File.h"
#include "logger.h"
#include "json/json.h"

namespace file_seeder {
    
bool config::load() 
{
            std::string cfg_path = qcutil::Path::getApplicationDirPath() + "file_seeder.json";
            if (!qcutil::File::exists(cfg_path)) {
                SLOG(error) << "the configure file: [" << cfg_path << "] does not exist." << std::endl;
                return false;
            }
            //read file content
            qcutil::FileReader fr(cfg_path);
            if (!fr.open()) {
                SLOG(error) << "failed to open file : [" << cfg_path << "]." << std::endl;
                return false;
            }
            std::string cfg_file_content = fr.read();
            fr.close();
            //parse json
            Json::Reader jr;
            Json::Value jv_root;
            if (!jr.parse(cfg_file_content, jv_root)) {
                SLOG(error) << "failed to parse configure string" << std::endl;
                return false;
            }
            if (jv_root.isMember("version")) {
                m_version = jv_root["version"].asString();
            }
            if (jv_root.isMember("manage_server")) {
                m_manager_server = jv_root["manage_server"].asString();
            }
            if (jv_root.isMember("torrent_list") && jv_root["torrent_list"].isArray()) {
                Json::Value jv_torrent_list = jv_root["torrent_list"];
                for (int i = 0; i != jv_torrent_list.size(); i++) {
                    Json::Value jv_ele = jv_torrent_list[i];
                    if (jv_ele.isObject()) {
                        std::string tf;
                        std::string desc;
                        if (jv_ele.isMember("torrent_file"))
                            tf = jv_ele["torrent_file"].asString();
                        if (jv_ele.isMember("describe"))
                            desc = jv_ele["describe"].asString();
                        if (!tf.empty()) {
                            seed_tasks_in_conf x;
                            x.torrent_file = tf;
                            x.desc = desc;
                            m_seed_tasks.push_back(x);
                        }
                    }
                }
            }
            return true;
        };
        
}