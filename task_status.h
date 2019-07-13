/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   task_status.h
 * Author: duocore
 *
 * Created on 2019年7月2日, 下午9:55
 */

#ifndef TASK_STATUS_H
#define TASK_STATUS_H

#include <stdint.h>
#include <vector>
#include "cptype.h"

typedef
enum _EM_DOWN_TASK_STATUS {
    NOITEM = 0,
    TSC_ERROR,
    TSC_PAUSE,
    TSC_DOWNLOAD,
    TSC_COMPLETE,
    TSC_STARTPENDING,
    TSC_STOPPENDING
} EM_DOWN_TASK_STATUS;

/*
typedef
enum _EM_XL_TASK_ERROR_TYPE {
    // 未知错误
    TASK_ERROR_UNKNOWN = 0x00,
    // 创建文件失败
    TASK_ERROR_DISK_CREATE = 0x01,
    // 写文件失败
    TASK_ERROR_DISK_WRITE = 0x02,
    // 读文件失败
    TASK_ERROR_DISK_READ = 0x03,
    // 重命名失败
    TASK_ERROR_DISK_RENAME = 0x04,
    // 文件片校验失败
    TASK_ERROR_DISK_PIECEHASH = 0x05,
    // 文件全文校验失败
    TASK_ERROR_DISK_FILEHASH = 0x06,
    // 删除文件失败失败
    TASK_ERROR_DISK_DELETE = 0x07,
    // 无效的DOWN地址
    TASK_ERROR_DOWN_INVALID = 0x10,
    // 代理类型未知
    TASK_ERROR_PROXY_AUTH_TYPE_UNKOWN = 0x20,
    // 代理认证失败
    TASK_ERROR_PROXY_AUTH_TYPE_FAILED = 0x21,
    // http下载中无ip可用
    TASK_ERROR_HTTPMGR_NOT_IP = 0x30,
    // 任务超时
    TASK_ERROR_TIMEOUT = 0x40,
    // 任务取消
    TASK_ERROR_CANCEL = 0x41,
    // MINITP崩溃
    TASK_ERROR_TP_CRASHED = 0x42,
    // TaskId 非法
    TASK_ERROR_ID_INVALID = 0x43

} EM_XL_TASK_ERROR_TYPE;

#ifndef ZeroMemory
#include <cstring>
#define ZeroMemory(p, cc) memset((p), 0, (cc)) 
#endif


typedef
struct _STRUCT_XL_DOWN_TASK_INFO {

    _STRUCT_XL_DOWN_TASK_INFO()
    : status(TSC_PAUSE)
    , ErrorType(TASK_ERROR_UNKNOWN)
    , fPercent(0)
    , bIsOriginUsable(false)
    , fHashPercent(0)
    , lUseTime(0) {

        ZeroMemory(lpszFileName, sizeof (lpszFileName) / sizeof (lpszFileName[0]));
        ZeroMemory(lpszReserved0, sizeof (lpszReserved0) / sizeof (lpszReserved0[0]));
        ZeroMemory(dwReserve, sizeof (dwReserve) / sizeof (dwReserve[0]));
    }

    //任务状态
    EM_DOWN_TASK_STATUS status;
    //错误码
    EM_XL_TASK_ERROR_TYPE ErrorType;
    //文件名
    wchar_t lpszFileName[MAX_PATH];
    wchar_t lpszReserved0[MAX_PATH];
    //该任务总大小(字节)
    INT64 llTotalSize;
    // 下载有效字节数(可能存在回退的情况)
    INT64 llDownSize;
    // 下载进度
    float fPercent;
    int iReserved0;
    // 总资源数
    int iSrcTotal;
    // 可用资源数
    int iSrcUsing;
    int iRserved1, iRserved2, iRserved3, iRserved4, iRserved5;
    // p2p贡献字节数
    INT64 llDonationP2P;
    INT64 llReserved6;
    // 原始资源共享字节数
    INT64 llDonationOrgin;
    // 镜像资源共享字节数
    INT64 llDonationP2S;
    INT64 llReserved7;
    INT64 llReserved8;
    // 即时速度(字节/秒)
    int iSpeed;
    // HTTP即时速度(字节/秒)
    int iSpeedP2S;
    // P2P即时速度(字节/秒)
    int iSpeedP2P;
    // 原始资源是否有效
    bool bIsOriginUsable;
    // 现不提供该值
    float fHashPercent;
    // 是否正在创建文件
    int iIsCreatingFile;
    DWORD dwReserve[64];
    //耗时时间
    long lUseTime;

} STRUCT_XL_DOWN_TASK_INFO, *PSTRUCT_XL_DOWN_TASK_INFO, *LPSTRUCT_XL_DOWN_TASK_INFO;
*/

//
// 仅Libtorrent
//

typedef struct tagPEERS_INFOMATION {
    char client[64];
    int source;
    int connection_type;
    int64_t total_download;
    int64_t total_upload;
    int up_speed;
    int down_speed;
    int payload_up_speed;
    int payload_down_speed;
    int rtt;
    int num_pieces;
    int progress_ppm;
    char remote_ip[128];
    char local_ip[128];
    int upload_limit;
    int download_limit;
} PEERS_INFOMATION, *PPEERS_INFOMATION;

//
// 仅libtorrent
//
#ifndef INVALID_TASK_ID
#define INVALID_TASK_ID 0
#endif

//
// session status
//
namespace file_seeder {

    struct dht_lookup {
        char type[64];
        int outstanding_requests;
        int timeouts;
        int responses;
        int branch_factor;
        int nodes_left;
        int last_sent;
        int first_timeout;
    };

    struct dht_routing_bucket {
        int num_nodes;
        int num_replacements;
        int last_active;
    };

    struct utp_status {
        int num_idle;
        int num_syn_sent;
        int num_connected;
        int num_fin_sent;
        int num_close_wait;
        uint64_t packet_loss;
        uint64_t timeout;
        uint64_t packets_in;
        uint64_t packets_out;
        uint64_t fast_retransmit;
        uint64_t packet_resend;
        uint64_t samples_above_target;
        uint64_t samples_below_target;
        uint64_t payload_pkts_in;
        uint64_t payload_pkts_out;
        uint64_t invalid_pkts_in;
        uint64_t redundant_pkts_in;
    };

    struct session_status {
        bool has_incoming_connections;
        int upload_rate;
        int download_rate;
        int64_t total_download;
        int64_t total_upload;
        int payload_upload_rate;
        int payload_download_rate;
        int64_t total_payload_download;
        int64_t total_payload_upload;
        int ip_overhead_upload_rate;
        int ip_overhead_download_rate;
        int64_t total_ip_overhead_download;
        int64_t total_ip_overhead_upload;
        int dht_upload_rate;
        int dht_download_rate;
        int64_t total_dht_download;
        int64_t total_dht_upload;
        int tracker_upload_rate;
        int tracker_download_rate;
        int64_t total_tracker_download;
        int64_t total_tracker_upload;
        int64_t total_redundant_bytes;
        int64_t total_failed_bytes;
        int num_peers;
        int num_dead_peers;
        int num_unchoked;
        int allowed_upload_slots;
        int up_bandwidth_queue;
        int down_bandwidth_queue;
        int up_bandwidth_bytes_queue;
        int down_bandwidth_bytes_queue;
        int optimistic_unchoke_counter;
        int unchoke_counter;
        int disk_write_queue;
        int disk_read_queue;
        int dht_nodes;
        int dht_node_cache;
        int dht_torrents;
        int64_t dht_global_nodes;
        int dht_total_allocations;
        utp_status utp_stats;
        int peerlist_size;
        int num_torrents;
        int num_paused_torrents;
        int64_t recv_webseed_bytes;
        int64_t recv_webseed_payload_bytes;
        std::vector<dht_lookup> active_requests;
        std::vector<dht_routing_bucket> dht_routing_table;
    };
}

#endif /* TASK_STATUS_H */

