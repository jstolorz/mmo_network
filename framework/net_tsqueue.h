//
// Created by jstol on 08.02.2022.
//

#pragma once

#include "net_common.h"

namespace bluesoft
{
    namespace net
    {
        template<typename T>
        class tsqueue{
        public:

            tsqueue() = default;
            tsqueue(const tsqueue<T>&)  = delete;

            ~tsqueue(){
                clear();
            }

        public:

            const T& front(){
                std::scoped_lock lock(mux_queue);
                return deq_queue.front();
            }

            const T& back(){
                std::scoped_lock lock(mux_queue);
                return deq_queue.back();
            }

            T pop_front(){
                std::scoped_lock lock(mux_queue);
                auto t = std::move((deq_queue.front()));
                deq_queue.pop_front();
                return t;
            }

            T pop_back(){
                std::scoped_lock lock(mux_queue);
                auto t = std::move(deq_queue.back());
                deq_queue.pop_back();
                return t;
            }

            void push_back(const T& item){
                std::scoped_lock lock(mux_queue);
                deq_queue.emplace_back(std::move(item));

                std::unique_lock<std::mutex> ul(mux_blocking);
                cv_blocking.notify_one();
            }

            void push_front(const T& item){
                std::scoped_lock lock(mux_queue);
                deq_queue.emplace_front(std::move(item));

                std::unique_lock<std::mutex> ul(mux_blocking);
                cv_blocking.notify_one();
            }

            bool empty(){
                std::scoped_lock lock(mux_queue);
                return deq_queue.empty();
            }

            size_t count(){
                std::scoped_lock lock(mux_queue);
                return deq_queue.size();
            }

            void wait(){
                while (empty()){
                    std::unique_lock<std::mutex> ul(mux_blocking);
                    cv_blocking.wait(ul);
                }
            }

            void clear(){
                std::scoped_lock lock(mux_queue);
                deq_queue.clear();
            }

        protected:
            std::mutex mux_queue;
            std::deque<T> deq_queue;
            std::condition_variable cv_blocking;
            std::mutex mux_blocking;
        };
    }
}