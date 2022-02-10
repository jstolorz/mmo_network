//
// Created by jstol on 07.02.2022.
//

#include <iostream>

#include "../framework/bluesoft_net.h"

enum class custom_msg_types : uint32_t {
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};

class cusstom_server : public bluesoft::net::server_interface<custom_msg_types> {
public:
    cusstom_server(uint16_t port)
            : bluesoft::net::server_interface<custom_msg_types>(port) {}

protected:

    bool on_client_connect(std::shared_ptr<bluesoft::net::connection<custom_msg_types>> client) override {
        bluesoft::net::message<custom_msg_types> msg;
        msg.header.id = custom_msg_types::ServerAccept;
        client->send(msg);
        return true;
    }

    void on_client_disconnect(std::shared_ptr<bluesoft::net::connection<custom_msg_types>> client) override {
        std::cout << "Removing client [" << client->get_id() << "]\n";
    }

    void on_message(std::shared_ptr<bluesoft::net::connection<custom_msg_types>> client,
                    bluesoft::net::message<custom_msg_types> msg) override {

        switch (msg.header.id) {
            case custom_msg_types::ServerPing: {
                std::cout << "[" << client->get_id() << "]: Server Ping\n";
                client->send(msg);
            }
                break;

            case custom_msg_types::MessageAll: {
                std::cout << "[" << client->get_id() << "]: Message All\n";

                bluesoft::net::message<custom_msg_types> msg;
                msg.header.id = custom_msg_types::MessageAll;
                msg << client->get_id();
                message_all_clients(msg,client);
            }
                break;
        }

    }


};

int main() {

    cusstom_server server(60000);
    server.start();

    while (1){
        server.update(-1, true);
    }

    return 0;
}
