//
// Created by jstol on 09.02.2022.
//

#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"

namespace bluesoft {
    namespace net {

        template<typename T>
        class server_interface {
        public:

            server_interface(uint16_t port)
                    : m_acceptor(m_asio_context, tcp::endpoint(tcp::v4(), port)) {}

            ~server_interface() {
                stop();
            }

            bool start() {
                try {

                    wait_for_client_connection();

                    m_thread_context = std::thread([this]() { m_asio_context.run(); });

                } catch (std::exception &ex) {
                    std::cerr << "[Server] exception: " << ex.what() << "\n";
                    return false;
                }

                std::cout << "[Server] Started!\n";
            }

            void stop() {

                m_asio_context.stop();

                if (m_thread_context.joinable()) {
                    m_thread_context.join();
                }

                std::cout << "[Server] Stopped!\n";

            }

            void wait_for_client_connection() {
                m_acceptor.async_accept(
                        [this](error_code ec, tcp::socket socket) {
                            if (!ec) {
                                std::cout << "[Server] New connection: " << socket.remote_endpoint() << "\n";

                                std::shared_ptr<connection<T>> new_conn =
                                        std::make_shared<connection<T>>(connection<T>::owner::server,
                                                                        m_asio_context, std::move(socket),
                                                                        m_q_message_in);

                                if (on_client_connect(new_conn)) {
                                    m_deq_connections.push_back(std::move(new_conn));
                                    m_deq_connections.back()->connect_to_client(n_id_counter++);
                                    std::cout << "[" << m_deq_connections.back()->get_id() << "]"
                                              << "Connection Aproved\n";
                                } else {
                                    std::cout << "[----] Connection Denied\n";
                                }

                            } else {
                                std::cout << "[Server] New connection error: " << ec.message() << "\n";
                            }

                            wait_for_client_connection();
                        }
                );
            }

            void message_client(std::shared_ptr<connection<T>> client, const message<T> &msg) {

                if (client && client->is_connected()) {
                    client->send(msg);
                } else {
                    on_client_disconnect(client);
                    client.reset();
                    m_deq_connections.erase(std::remove(m_deq_connections.begin(), m_deq_connections.end(), client),
                                            m_deq_connections.end());
                }
            }

            void message_all_clients(const message<T>& msg, std::shared_ptr<connection<T>> p_ignore_client = nullptr){

                bool b_invalid_client_exist = false;

                for(auto& client : m_deq_connections){
                    if(client != p_ignore_client){
                        client->send(msg);
                    }else{
                        on_client_disconnect(client);
                        client.reset();
                        b_invalid_client_exist = true;
                    }
                }

                if(b_invalid_client_exist){
                    m_deq_connections.erase(std::remove(m_deq_connections.begin(), m_deq_connections.end(), nullptr),
                                            m_deq_connections.end());
                }
            }

            void update(size_t n_max_message = -1, bool b_wait = false){
                size_t n_message_count = 0;

                while (n_message_count < n_max_message && !m_q_message_in.empty()){
                    auto msg = m_q_message_in.pop_front();

                    on_message(msg.remote, msg.msg);

                    n_message_count++;
                }
            }


        protected:

            virtual bool on_client_connect(std::shared_ptr<connection<T>> client) {
                return false;
            }

            virtual void on_client_disconnect(std::shared_ptr<connection<T>> client) {

            }

            virtual void on_message(std::shared_ptr<connection<T>> client, message<T> msg) {

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