/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wapper.h
 * Author: duocore
 *
 * Created on 2019年7月2日, 下午9:38
 */

#ifndef WAPPER_H
#define WAPPER_H

#include "cptype.h"
#include "task_status.h"
#include "boost/function.hpp"
#include <vector>

//
// Basic interface for download compatible with XunLei interface.
// Make it easy for developers using the Thunder interface to use this new library
//
namespace file_seeder {
    /**
     * \brief Initialize the data struct and worker thread, it must call called before you add/start a task.
     * \return Always return true, it is used to compatible with `XunLei` interface.
     */
    bool initialize();

    /**
     * \brief Stop all worker thread, and destroy all internal data struct.
     * \return Always return true, it is used to compatible with `XunLei` interface.
     */
    bool uninitialization();

    /**
     * \brief create a task from a torrent file or an `magnet` URL.
     * 
     * \param lpszURL this parameter must be a full path of `.torrent` file if the task is created from a torrent task, or it is a `magnet` URL if the task is `magnet` type.
     * \param lpszPath lpszPath Set the save path for the file to download.
     * \param lpszFileName Not used, to be compatible with `XunLei`.
     * \param bIsResume Not used, to be compatible with `XunLei`.
     * \return 
     * - INVALID_TASK_ID(0) means failed to add task, the reason can be invalid parameter or other error caused failed to add torrent failed such as invalid `magnet` url or the `.torrent` file is not exists.
     * - none-zero the task id which identify a task added to queue.
     * \remark
     * - The user must check the returned value, you can not start/stop/delete/query a task with id INVALID_TASK_ID(0).
     * - This function is a sync-call which means it will block until add task finished.
     */
    DWORD create_task_by_url(wchar_t *lpszURL, wchar_t *lpszPath, wchar_t *lpszFileName, bool bIsResume);


    /**
     * \brief start a task
     * 
     * \param h_task the task id return by create_task_by_url`.
     * \return Return false if failed to start the task, the reason maybe the task was not found or INVALID_TASK_ID(0) is passed as a task id, or you can use `query_task_info` to query more information about the failed reason.
     */
    bool start_task(DWORD h_task);

    /**
     * \brief delete a task specified by the task id return by `create_task_by_url`
     * 
     * \param h_task the task id return by `create_task_by_url`.
     * \return return true if delete the task successfully, otherwise return false.
     */
    bool delete_task(DWORD h_task);

    /**
     * \brief Stop a task specified by task id, or stop all tasks if the parameter "task_is" is 0 (`libtorrent` only).
     * 
     * \param the task id return by `create_task_by_url`.
     * \return return true if stop the task successfully, otherwise return false.
     * \remark 
     * - `XunLei` module have a similar interface, Do not pass 0 to it since it works only in `libtorrent`.
     */
    bool stop_task(DWORD h_task);

    /**
     * \brief Same as `stop_task`, it is for compatible with `XunLei` interface.
     * 
     * \param The task id return by `create_task_by_url`.
     * \return Same as `stop_task`.
     */
    bool force_stop_task(DWORD h_task);

    /**
     * \brief Query a task detail information and save it to a `STRUCT_XL_DOWN_TASK_INFO` struct.
     * 
     * \param h_task The task id return by `create_task_by_url`
     * \param pTaskInfo The task status save to.
     * \return Return true if query the task status successfully, otherwise return false.
     * \remark
     * The task status is save to a `STRUCT_XL_DOWN_TASK_INFO` specified the pointer `pTaskInfo` if the function return true.
     * for more information about the task status, \see STRUCT_XL_DOWN_TASK_INFO.
     */
    bool query_task_info(DWORD h_task, STRUCT_XL_DOWN_TASK_INFO* pTaskInfo);

    //
    // Extend functions which are not compatible with XunLei Interface.
    // They are works only in `libtorrent` module
    //

    /**
     * \brief basic information in a `.torrent` file.
     */
    typedef struct tag_torrent_baisc_info {
        char tracker_server[512];
        char creation_date[32]; //Not used currently.Reserved for future.
        int length[32]; //Not used currently.Reserved for future.
        char name[256];
        char piece_length[32]; //Not used currently.Reserved for future.
        char pieces[32]; //Not used currently.Reserved for future.
    } torrent_baisc_info;
    
    /**
     * \brief Parse a file and return information in it.
     */
    void ex_query_basic_info_in_torrent_file(const char *torrent_file_name, torrent_baisc_info *ptbi);

    /**
     * \brief Set alert message callback.
     */
    void ex_set_alert_callback(int64_t mask_high, int64_t mask_low, boost::function<void((char* /*pAlertMsg*/, int /*iMsgTypeId*/))> cbk);

    /**
     * \brief Set callback for `pingpong`, the callback will be called after `pingpong` finish its work to detect the connectivity of TCP and UDP from the server outside to local server.
     * \param cbk The callback function address
     * \remark 
     * There are two parameters for `FunctionTypePingCBK` type function, the first is means TCP connectivity, the second is for UDP connectivity. And value `1` means can be connected otherwise 0.
     */
    void ex_set_ping_callback(boost::function<void((int /*is_tcp_ok*/, int /*is_udp_ok*/))> cbk);

    /**
     * \brief Set the timeout value for `pingpong` tasks.
     * \param pingpong_ready_timeout default 10 seconds
     * \param pingpong_work_timeout default 5 seconds
     */
    void ex_set_ping_time_out(int pingpong_ready_timeout, int pingpong_work_timeout);

    /**
     * \brief query the peers in a task specified by a task id;
     * \param task_id the task id return from `create_task_by_url`.
     * \param pPeersInfo Return the peers information if the funtion is return true.
     * \return Return true if get peers information, otherwise teturn false.
     */
    bool ex_query_peers_info(DWORD task_id, std::vector<PEERS_INFOMATION>& pPeersInfo);

    /**
     * \brief query the session status.
     */
    void ex_query_session_status(session_status& SessionStatus);

    /**
     * \brief if this function is called with paramter true, the ;libtorremt session is seeding only (downloading is disabled).
     * \remark it's ignored if `initialize` was not called previously.
     */
    void ex_set_seed_mode(bool bSeedMode);

    /**
     * \brief Set timeout for downloading. if the time of 0 downloading speed is more than the given value, the task will be stopped.
     * \remark
     * - it's ignored if `txk_initialize` not called.
     * - it will not work in seed mode (`ex_set_seed_mode(true)`).
     */
    void ex_set_time_out(int time_out_seconds);

}


#endif /* WAPPER_H */

