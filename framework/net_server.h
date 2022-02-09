//
// Created by jstol on 09.02.2022.
//

#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"

namespace bluesoft{
    namespace net{

        template<typename T>
        class server_interface{
        public:

            server_interface(uint16_t port)
            : m_acceptor(m_asio_context, tcp::endpoint(tcp::v4(),port))
            {}

            ~server_interface(){
                stop();
            }

            bool start(){
                try{

                    wait_for_client_connection();

                    m_thread_context = std::thread([this](){ m_asio_context.run(); });

                }catch(std::exception& ex){
                    std::cerr << "[Server] exception: " << ex.what() << "\n";
                    return false;
                }

                std::cout << "[Server] Started!\n";
            }

            void stop(){

                m_asio_context.stop();

                if(m_thread_context.joinable()){
                    m_thread_context.join();
                }

                std::cout << "[Server] Stopped!\n";

            }

            void wait_for_client_connection(){

            }

        protected:
            tsqueue<owned_message<T>> m_q_message_in;
            std::deque<std::shared_ptr<connection<T>>> m_deq_connections;
            io::io_context m_asio_context;
            std::thread m_thread_context;
            tcp::acceptor m_acceptor;
            uint32_t n_id_counter = 10000;
        };

    }
}