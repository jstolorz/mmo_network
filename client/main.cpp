//
// Created by jstol on 07.02.2022.
//

#include <iostream>

#include "../framework/net_message.h"

enum class custom_types : uint32_t {
    FIRE_BULLET,
    MOVE_PLAYER
};

int main(){

    bluesoft::net::message<custom_types> msg;
    msg.header.id = custom_types::FIRE_BULLET;

    int a = 1;
    bool b = true;
    float  c = 3.14567f;

    msg << a << b << c;

    a = 99;

    std::cout << msg << "\n";

    msg >> c >> b >> a;

    std::cout << msg;


    return 0;
}

