/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   sync_call.h
 * Author: duocore
 *
 * Created on 2019年7月3日, 上午12:58
 */

#ifndef SYNC_CALL_H
#define SYNC_CALL_H

#include <cstdlib>
#include <iostream>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <deque>
#include <exception>

/**
 * Test sync-call in two thread.
 */
namespace file_seeder {

    class sync_caller : public boost::enable_shared_from_this<sync_caller> {
    public:
        typedef boost::function<void() > sync_call_type;

        sync_caller() {
            m_thread = NULL;
            m_thread_quit = false;
            m_thread_is_running = false;
            //cout << __FUNCTION__ << endl;
        }

        ~sync_caller() {
            //cout << __FUNCTION__ << endl;
            stop();
        }

    public:
        void start() {
            if (m_thread)
                return;
            m_thread = boost::make_shared<boost::thread>(&sync_caller::thread_function, this);
        }
        
        bool is_started()
        {
            return m_thread_is_running;
        }
        
        void stop()
        {
            if (m_thread_is_running)
            {
                m_thread_quit = true;
                if (m_thread->joinable())
                    m_thread->join();
            }
        }
        
        void do_call(sync_call_type& fun) {
            if (boost::this_thread::get_id() == m_thread->get_id()) {
                // `SyncWait` never return if `SyncCall` is called in the work thread.
                // Because the task posted is the next `task_fun_obj_type` object,
                // and you are now try to wait a next executing work complete, but next one will not execute if the current task is not finished.
                throw std::runtime_error("Can not post a sync operation in the thread itself");
                return;
            }
            bool done = false;
            boost::condition_variable cv;
            boost::mutex m;
            sync_call_type f = boost::bind(&sync_caller::sync_call_proxy, this,
                    boost::ref(fun),
                    boost::ref(done),
                    boost::ref(cv),
                    boost::ref(m));
            add_task(f);
            //std::cout << "Task added." << std::endl;
            sync_call_wait(done, cv, m);
            //std::cout << "OK, sync_call finished." << std::endl;
        }
        
    private:
        sync_call_type get_from_local() {
            sync_call_type c;
            boost::recursive_mutex::scoped_lock a(m_mutex_task_array);
            if (!m_task_array.empty()) {
                // pop first task in task array.
                c = m_task_array.front();
                m_task_array.pop_front();
                //cout << "Start a new task from array." << endl;
            }
            return c;
        }

        void thread_function() {
            //cout << "sync thread start "<< endl;
            m_thread_is_running = true;
            while (!m_thread_quit) {
                sync_call_type c = get_from_local();
                if (c) {
                    c();
                } else
                    boost::this_thread::sleep(boost::posix_time::milliseconds(20));
            }
            m_thread_is_running = false;
        }

        void sync_call_wait(bool& done, boost::condition_variable& cv, boost::mutex& m) {
            boost::mutex::scoped_lock a(m);
            while (!done) {
                //std::cout << "Waiting for task finish." << std::endl;
                cv.wait(a);
            }
        }

        void sync_call_proxy(sync_call_type& fun, bool& done, boost::condition_variable& cv, boost::mutex& m) {
            fun();
            done = true;
            boost::mutex::scoped_lock a(m);
            //std::cout << __FUNCTION__ << " Finish sync call and notify all." << std::endl;
            cv.notify_all();
        }

        void add_task(sync_call_type& f) {
            boost::recursive_mutex::scoped_lock a(m_mutex_task_array);
            //std::cout << "try add a task." << std::endl;
            m_task_array.push_back(f);
        }
    private:
        boost::shared_ptr<boost::thread> m_thread;
        bool m_thread_quit;
        bool m_thread_is_running;
        std::deque<sync_call_type>m_task_array;
        boost::recursive_mutex m_mutex_task_array;
    };

    class sync_caller_test_suit {
    public:
        void TestCall() {
            for (int i = 0; i != 10; i++) {
                std::cout << "timer : " << i << std::endl;
                boost::this_thread::sleep(boost::posix_time::seconds(1));
            }
        }
        
        int FunctionB(int a) {
            while (!m_caller.is_started()) {
                std::cout << "thread is not running, wait 1 second" << std::endl;
                boost::this_thread::sleep(boost::posix_time::seconds(1));
            }

            {
                // add an task.
                sync_caller::sync_call_type f = boost::bind(&sync_caller_test_suit::TestCall, this);
                m_caller.do_call(f);
            }
        }

        void TestSyncCall() {
            m_caller.start();
            FunctionB(2);
            std::cout << "Enter any key to quit." << std::endl;
            char b;
            std::cin >> b;
            m_caller.stop();
        }
    private:
        sync_caller m_caller;
    };


}


#endif /* SYNC_CALL_H */

