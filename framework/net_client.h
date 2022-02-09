//
// Created by jstol on 09.02.2022.
//

#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_connection.h"
#include "net_message.h"

namespace bluesoft {
    namespace net {

        template<typename T>
        class client_interface {
        public:
            client_interface() {}

            ~client_interface() {
                disconnect();
            }

        public:
            bool connect(const std::string &host, const uint16_t port) {

                try {

                    tcp::resolver resolver(m_context);
                    tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

                    m_connection = std::make_unique<connection<T>>(connection<T>::owner::client, m_context,
                                                                   tcp::socket(m_context), m_q_message_in);

                    m_connection->connect_to_server(endpoints);

                    thr_context = std::thread([&](){ m_context.run(); });

                } catch (std::exception &ex) {
                    std::cerr << "Client exception: " << ex.what() << "\n";
                    return false;
                }

                return true;
            }

            void disconnect() {
                  if(is_connected()){
                      m_connection->disconnect();
                  }

                  m_context.stop();

                  if(thr_context.joinable()){
                      thr_context.join();
                  }

                  m_connection.release();
            }

            bool is_connected(){
                if(m_connection){
                    return m_connection->is_connected();
                }else{
                    return false;
                }
            }

        public:

            void send(const message<T>& msg){
                if(is_connected()){
                    m_connection->send(msg);
                }
            }

            tsqueue<owned_message<T>>& incoming(){
                return m_q_message_in;
            }

        protected:
            io::io_context m_context;
            std::thread thr_context;
            std::unique_ptr<connection<T>> m_connection;

        private:
            tsqueue<owned_message<T>> m_q_message_in;
        };

    }
}