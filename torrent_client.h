/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   torrent_client.h
 * Author: duocore
 *
 * Created on 2019年7月2日, 下午11:17
 */

#ifndef TORRENT_CLIENT_H
#define TORRENT_CLIENT_H

#include <boost/smart_ptr/shared_ptr.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/sha1_hash.hpp>
#include "libtorrent/time.hpp"
#include <libtorrent/session_status.hpp>
#include <map>
#include "cptype.h"

namespace libtorrent {
    class session;
}

namespace lt = libtorrent;

namespace file_seeder {

    class client_task {
    public:

        client_task() {
            id = 0;
        }
    public:
        // The task id to instead of `torrent_hash` member, because `torrent_hash` is a big data. it start from 1 and 0 is reserve to represent a `bad id`.
        int id;
        // The current status of the task.
        lt::torrent_status status;
        // The hash-info of the torrent in current session.
        lt::sha1_hash torrent_hash;
        // search `lt::add_torrent_alert::alert_type` in cpp to get more information. 
        //It is one of lt::add_torrent_params::ti::name / lt::add_torrent_params::name / lt::add_torrent_params::url / lt::add_torrent_params::hash_info
        std::string torrent_name;
    };

    struct P2P_LOCAL_SERVICE_INFO {
        int portmap_extern_tcp_port;
        int portmap_extern_udp_port;
        int listen_succeeded_tcp_port;
        int listen_succeeded_udp_port;
        std::string external_ip;
    };

    class sync_caller;
    class easy_timer;
    typedef boost::shared_ptr<client_task> task_sptr;
    typedef std::map<lt::sha1_hash, task_sptr> task_map;
#define INVALID_TASK_ID 0

    class torrent_client {

        enum {
            MAP_TYPE_NAT_PMP = 0, // 0 for NAT-PMP and 1 for UPnP.
            MAP_TYPE_UPNP = 1, // 0 for NAT-PMP and 1 for UPnP.
        };
    public:
        bool start();
        bool stop();
        DWORD add_task(const std::string& url, const std::string& save_path);
        bool start_task(DWORD task_id);
        bool suspend_task(DWORD task_id);
        bool delete_task(DWORD task_id);
        bool query_torrent_status(DWORD task_id, lt::torrent_status& ts);
        bool query_session_status(lt::session_status& sess_stat);
    private:
        // implement `add_torrent` this function will be called in sync-caller.
        DWORD add_task_imp(const std::string& url, const std::string& save_path);
        // Implement `start_task`, this function will be called in sync-caller.
        bool start_task_imp(DWORD task_id);
        // Implement `suspend_task`, this function will be called in sync-caller.
        bool suspend_task_imp(DWORD task_id);
        // Implement `delete_task`, this function will be called in sync-caller.
        bool delete_task_imp(DWORD task_id);
        bool query_torrent_status_imp(DWORD task_id, lt::torrent_status& ts);
        bool query_session_status_imp(lt::session_status& sess_stat);
        // Create the libtorrent session.
        void create_session();
        // Use to process alert generated in libtorrent session.
        void on_alert();

        task_sptr get_task(const lt::sha1_hash& hash) {
            task_map::iterator it = m_tasks.find(hash);
            if (it != m_tasks.end()) {
                return it->second;
            } else {
                return task_sptr();
            }
        }

        task_sptr get_task(DWORD task_id)
        {
            for(auto it : m_tasks)
            {
                if(it.second->id == task_id)
                    return it.second;
            }
            return task_sptr();
        }

        void remove_task_from_tasklist(const lt::sha1_hash& hash) {
            task_map::iterator it = m_tasks.find(hash);
            if (it != m_tasks.end())
                m_tasks.erase(it);
        }

    private:
        // most of the libtorrent function will be called in this caller(thread).
        boost::shared_ptr<sync_caller> m_torrent_worker;
        // the libtorrent session.
        boost::shared_ptr<lt::session> m_torrent_session;
        // the timer (thread) to retrive and process alert.
        boost::shared_ptr<easy_timer> m_alert_process_timer;
        // last time we post a status update request.
        lt::time_point m_last_post_status_update;
        // Save a task list add to libtorrent session, and recored its status in it.
        task_map m_tasks;
        // Save the port mapping, the listen port, the extern ip information.
        P2P_LOCAL_SERVICE_INFO m_local_service_info;
        // provide a callback function, it will be called when a new alert comes.
        // The first parameter is the alert message (utf-8 encode).and the second one is alert type.
        boost::function<void (const std::string&, int) > m_alert_callback;
    };
}

#endif /* TORRENT_CLIENT_H */

