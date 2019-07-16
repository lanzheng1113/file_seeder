/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 * file_seeder project is designed to run on a resource limited linux system with 1GB RAM, 1 core cpu and about a 30GB disk.
 * - The main perpor is running as a `torrent seeder`
 * - addtionally it can retrive the configure from a http server and update the current seed task.
 * So it also must have the ability to download files from the other client and web url.
 * - to update the tasks delete the old out-time files.
 */

/* 
 * File:   main.cpp
 * Author: duocore
 *
 * Created on 2019年7月2日, 下午9:23
 */

#include <cstdlib>
#include <iostream>
#include "torrent_client.h"
#include "config.h"
#include "file_seeder_error_code.h"
#include <string>
#include "util/Path.h"
#include "util/File.h"
#include "seed_executor.h"
#include "logger.h"
#include "boost/thread.hpp"
#include <signal.h>

using std::cout;
using std::endl;

volatile sig_atomic_t quit_flag = 0;
static void my_handler(int sig) 
{
	quit_flag = 1;   // set flag
}

int main(int argc, char** argv) 
{
    //---------------------
    // print basic information
    //--------------------
    cout << "File Seeder based on libtorrent." << endl;
    cout << "For more information about libtorrent, please visit https://github.com/arvidn/libtorrent" << endl;
   std::string cwd = qcutil::Path::getApplicationDirPath();
    cout << "Current work directory: " << cwd << endl;
	cout << "Defualt configure file: " << "/usr/local/file_seeder/file_seeder.json" << endl;
	std::string data_dir = "/usr/local/file_seeder/seed";
	cout << "Default seeding file folder: " << data_dir << endl;
    // load configure.
    if(!file_seeder::config::getInstance().load())
    {
        cout << "Failed to load config file" << endl;
        return ERROR_FAILED_LOAD_CONFIG;
    }
    std::string ver = file_seeder::config::getInstance().get_version();
    cout << "Current version: " << ver << endl;
    std::string monitor_server = file_seeder::config::getInstance().get_manager_server();
    cout << "Moniter server: " << monitor_server << endl;
    std::vector<file_seeder::config::seed_tasks_in_conf> tasks = file_seeder::config::getInstance().get_tasks();
    cout << "Tasks:"  << endl;
    int task_index = 0;
    for (auto i : tasks)
    {
        cout << ++task_index << " " << i.desc << endl;
        cout << "with torrent file [" << i.torrent_file << "]" << endl;
    }
    
	signal(SIGINT, my_handler);
	
	file_seeder::seed_executor se(data_dir, tasks);
    se.start();
    while(!quit_flag)
    {
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
	cout << "Got the singal to quit, please wait ..." << endl;
    se.stop();
    return 0;
}

