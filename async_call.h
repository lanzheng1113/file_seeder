/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   async_call.h
 * Author: duocore
 *
 * Created on 2019年7月3日, 上午12:38
 */

#ifndef ASYNC_CALL_H
#define ASYNC_CALL_H

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

namespace file_seeder {

    class async_caller : public boost::enable_shared_from_this<async_caller> {
    public:
        typedef boost::function<void() > async_fun;

        async_caller() {
            //std::cout << __FUNCTION__ << endl;
            end_thread = false;
            m_thread_started = false;
        }

        ~async_caller() {
            //std::cout << __FUNCTION__ << endl;
            if (m_thread_started)
                stop();
        }

        void start() {
            if (m_thread)
                return;
            m_thread = boost::make_shared<boost::thread>(&async_caller::thread_function, this);
            while (!m_thread_started)
                boost::this_thread::sleep(boost::posix_time::milliseconds(20));
        }

        void stop() {
            if (end_thread)
                return;
            end_thread = true;
            if (m_thread->joinable())
                m_thread->join();
        }

        bool is_started() {
            return m_thread_started;
        }

        void do_call(async_fun& f, boost::function<void()>& cbk) {
            async_fun c = boost::bind(&async_caller::async_call_proxy, this, f, cbk);
            boost::recursive_mutex::scoped_lock a(m_mutex);
            m_tasks.push_back(c);
        }
    private:

        async_fun get_task() {
            async_fun c;
            boost::recursive_mutex::scoped_lock a(m_mutex);
            if (!m_tasks.empty()) {
                c = m_tasks.front();
                m_tasks.pop_front();
            }
            return c;
        }

        void thread_function() {
            //cout << "async caller thread start." << endl;
            m_thread_started = true;
            while (!end_thread) {
                async_fun c = get_task();
                if (!c)
                    boost::this_thread::sleep(boost::posix_time::milliseconds(20));
                else {
                    c();
                }
            }
            m_thread_started = false;
        }

        void async_call_proxy(async_fun& f, boost::function<void()>& cbk) {
            f();
            cbk();
        }

    private:
        boost::shared_ptr<boost::thread> m_thread;
        boost::recursive_mutex m_mutex;
        std::deque<async_fun> m_tasks;
        bool end_thread;
        bool m_thread_started;
    };

    class async_call_test_suit {

        async_call_test_suit() {
            ;
        }

        ~async_call_test_suit() {
            m_caller.stop();
        }
    public:

        void test_task() {
            for (int i = 0; i != 10; i++) {
                std::cout << "Test counter:" << i << std::endl;
            }
        }

        void async_cbk() {
            std::cout << "Hi, this is async call back." << std::endl;
        }

        void AddTestTask() {
            async_caller::async_fun task = boost::bind(&async_call_test_suit::test_task, this);
            boost::function<void() > cbk = boost::bind(&async_call_test_suit::async_cbk, this);
            if (!m_caller.is_started())
                m_caller.start();
            m_caller.do_call(task, cbk);
        }

        async_caller m_caller;
    };
}

#endif /* ASYNC_CALL_H */

