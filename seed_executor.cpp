/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "seed_executor.h"
#include "cptype.h"
#include <iostream>
using std::cout;
using std::endl;
#include "torrent_client.h"
#include "easy_timer.h"
#include "util/File.h"
#include "logger.h"

namespace file_seeder {

    seed_executor::seed_executor(const std::string& work_dir,
            const std::vector<file_seeder::config::seed_tasks_in_conf>& tasks) {
        m_work_dir = work_dir;
        m_tasks_conf = tasks;
    }

    seed_executor::~seed_executor() {

    }

    bool seed_executor::start() {
        if (m_seed_client)
            return false;
        if (m_query_timer)
            return false;
        m_seed_client = boost::make_shared<file_seeder::torrent_client>();
        m_seed_client->start();
        for (auto i : m_tasks_conf) {
            std::string torrent_file_full_path = m_work_dir + i.torrent_file;
            if (!qcutil::File::exists(torrent_file_full_path)) {
                SLOG(error) << "Task file " << torrent_file_full_path << " not exist, skip it." << endl;
                continue;
            } else {
                DWORD task_id = m_seed_client->add_task(torrent_file_full_path, m_work_dir);
                if (task_id == 0) {
                    SLOG(error) << "failed to add task " << torrent_file_full_path << endl;
                    continue;
                }
                SLOG(info) << "add task " << torrent_file_full_path << " successfully." << endl;
                if (!m_seed_client->start_task(task_id)) {
                    SLOG(error) << "failed to start task " << torrent_file_full_path << endl;
                    m_seed_client->delete_task(task_id);
                    continue;
                }
                SLOG(info) << "start task successfully." << endl;
                m_task_id_array.push_back(task_id);
            }
        }
        m_query_timer = boost::make_shared<file_seeder::easy_timer>();
        m_query_timer->set_timer(1, 3000, boost::bind(&seed_executor::query_torrent_status, this));
        return true;
    }

    static std::string torrent_status_to_string(lt::torrent_status::state_t es) {
        std::string ret;
        switch (es) {
            case lt::torrent_status::queued_for_checking:
                ret = "queued_for_checking";
                break;
            case lt::torrent_status::checking_files:
                ret = "checking_files";
                break;
            case lt::torrent_status::downloading_metadata:
                ret = "downloading_metadata";
                break;
            case lt::torrent_status::downloading:
                ret = "downloading";
                break;
            case lt::torrent_status::finished:
                ret = "finished";
                break;
            case lt::torrent_status::seeding:
                ret = "seeding";
                break;
            case lt::torrent_status::allocating:
                ret = "allocating";
                break;
            case lt::torrent_status::checking_resume_data:
                ret = "checking_resume_data";
                break;
            default:
                ret = "unknown";
                break;
        }
        return ret;
    }

    void seed_executor::query_torrent_status() {
        if (!m_seed_client)
            return;
        lt::session_status sess_stat;
        if (m_seed_client->query_session_status(sess_stat)) {
            SLOG(info) << "Session down: " << sess_stat.payload_download_rate << ", up " << sess_stat.payload_upload_rate;
        } else {
            SLOG(error) << "Failed to query the session status." << endl;
        }

        for (auto task_id : m_task_id_array) {
            lt::torrent_status ts;
            if (m_seed_client->query_torrent_status(task_id, ts)) {
                SLOG(info) << "s:" << torrent_status_to_string(ts.state) << ",d:" << ts.download_payload_rate / 1000 << "k,u:" << ts.upload_payload_rate / 1000 << "k,ns:" << ts.num_seeds << ",np:" << ts.num_peers << endl;
            } else {
                SLOG(error) << "Task " << task_id << " Failed to query the status." << endl;
            }
        }

    }

    bool seed_executor::stop() {
        if (!m_query_timer)
            return false;
        if (!m_seed_client)
            return false;
        // if there are any pending query, wait until it finish.
        //
        m_query_timer->kill_timer(1);
        m_query_timer.reset();
        m_seed_client->stop();
        m_seed_client.reset();
        return true;
    }
}
