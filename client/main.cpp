//
// Created by jstol on 07.02.2022.
//

#include <iostream>

#include "../framework/net_client.h"

enum class custom_types : uint32_t {
    FIRE_BULLET,
    MOVE_PLAYER
};

class custom_client : public bluesoft::net::client_interface<custom_types>{
public:
    bool fire_bullet(float x, float y){
        bluesoft::net::message<custom_types> msg;
        msg.header.id = custom_types::FIRE_BULLET;
        msg << x << y;
        send(msg);

        return true;
    }
};

int main(){

    custom_client c;
    c.connect("93.184.216.34",80);
    c.fire_bullet(2.0f,4.9f);

    return 0;
}

