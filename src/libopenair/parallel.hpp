/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   thread_pool.hpp
 * Author: nsamson
 *
 * Created on November 23, 2015, 8:50 PM
 */

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include "../libopenair/common.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>

namespace openair {

    template <typename UnaryFunction, typename ForwardIterator, typename ForwardOutputIterator>
    void parallel_apply_unordered(UnaryFunction func,
                                  ForwardIterator begin,
                                  ForwardIterator end,
                                  ForwardOutputIterator o_begin,
                                  size_t max_cpu = 4) {

        std::vector<std::thread> thread_pool;
        std::mutex input_lock;
        std::mutex output_lock;

        for (size_t i = 0; i < max_cpu; i++) {
            thread_pool.emplace_back([&]() -> void{
                while(true) {
                    ForwardIterator it;
                    {
                        std::unique_lock<std::mutex> lock(input_lock);
                        if (begin != end) {
                            it = begin;
                            begin++;
                        } else {
                            return;
                        }
                        lock.unlock();
                    }

                    ForwardOutputIterator ot;
                    {
                        std::unique_lock<std::mutex> lock(output_lock);
                        ot = o_begin;
                        o_begin++;
                        lock.unlock();
                    }

                    *ot = func(*it);

                }
            });
        }

        for (size_t i = 0; i < max_cpu; i++) {
            thread_pool[i].join();
        }

    };

}

#endif /* THREAD_POOL_HPP */

