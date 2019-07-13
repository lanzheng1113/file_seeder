/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   easy_timer.h
 * Author: duocore
 *
 * Created on 2019年7月3日, 下午4:26
 */

#ifndef EASY_TIMER_H
#define EASY_TIMER_H

#pragma once
#include "boost/thread.hpp"
#include "boost/function.hpp"
#include "boost/thread/recursive_mutex.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"



namespace file_seeder {
    typedef boost::function<void() > timer_cbk;
    typedef boost::function<void() > task_fun_obj_type;

    class easy_timer : public boost::noncopyable {
    public:

        easy_timer() {
            m_is_started = true;
            m_thd = boost::make_shared<boost::thread>(&easy_timer::thread_function, this);
        }

        ~easy_timer() {
            m_is_started = false;
            if (m_thd->joinable()) {
                m_thd->join();
            }
        }

        struct timer_imp {
            int id;
            int inter_val;
            timer_cbk cbk;
            boost::posix_time::ptime next_triggle_time;
        };

    public:

        void set_timer(int id, int milli_seconds, const timer_cbk& cbk) {
            timer_imp p;
            p.id = id;
            p.inter_val = milli_seconds;
            p.cbk = cbk;
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
            p.next_triggle_time = now + boost::posix_time::milliseconds(milli_seconds);
            boost::recursive_mutex::scoped_lock l(m_mu);
            m_timers.push_back(p);
        }

        void kill_timer(int id) {
            boost::recursive_mutex::scoped_lock l(m_mu);
            for (std::vector<timer_imp>::iterator it = m_timers.begin(); it != m_timers.end(); ++it) {
                if (it->id == id) {
                    m_timers.erase(it);
                    break;
                }
            }
        }
    private:

        void reset_timer(int id) {
            boost::recursive_mutex::scoped_lock l(m_mu);
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
            for (timer_imp& i : m_timers) {
                if (i.id == id) {
                    i.next_triggle_time = now + boost::posix_time::milliseconds(i.inter_val);
                    break;
                }
            }
        }

        task_fun_obj_type pop_timer_task(int& id) {
            boost::recursive_mutex::scoped_lock l(m_mu);
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
            task_fun_obj_type ret;
            for (auto i : m_timers) {
                if (now > i.next_triggle_time) {
                    id = i.id;
                    ret = i.cbk;
                    break;
                }
            }
            return ret;
        }
    protected:

        void thread_function() {
            while (m_is_started) {
                int id = 0;
                task_fun_obj_type t = pop_timer_task(id);
                if (!t) {
                    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                    continue;
                } else {
                    //
                    // Note we must reset the timer next triggle time ONLY AFTER the callback function has called.
                    // Because if a task use many many time, the other task may be have no chance to execute.
                    //
                    t(); // <== break here.
                    reset_timer(id);
                }

            }
        }

    private:
        boost::recursive_mutex m_mu;
        std::vector<timer_imp> m_timers;
        boost::shared_ptr<boost::thread> m_thd;
        bool m_is_started;
    };

}

#endif /* EASY_TIMER_H */

