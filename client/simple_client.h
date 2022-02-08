//
// Created by jstol on 07.02.2022.
//
#pragma once

#include "../common/types.h"

std::vector<char> v_buffer(20*1024);

void grab_some_data(tcp::socket& socket){
    socket.async_read_some(io::buffer(v_buffer.data(),v_buffer.size()),
                           [&](error_code ec, size_t length){
        if(!ec){
            std::cout << "\n\nRead " << length << " bytes\n\n";

            for (int i = 0; i < length; ++i) {
                std::cout << v_buffer[i];
            }

            grab_some_data(socket);
        }
    });
}

int start_client(){

    error_code ec;
    io::io_context context;

    io::io_context::work io_work(context);

    std::thread thr_context = std::thread([&](){ context.run(); });

    tcp::endpoint endpoint(io::ip::make_address("127.0.0.1",ec),17234);

    tcp::socket socket(context);

    socket.connect(endpoint,ec);

    if(!ec){
        std::cout << "Connected!\n";

        if(socket.is_open()){

            grab_some_data(socket);

            std::string request =
                    "GET /index.html HTTP/1.1\r\n"
                    "Host: example.com\r\n"
                    "Connection: close\r\n\r\n";


            //std::string request = "WAVSTART cowbell.wav 0 0 ";
            //std::string request = "URL www.google.com";

            socket.write_some(io::buffer(request.data(), request.size()),ec);

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(2000ms);

            context.stop();
            if(thr_context.joinable()){
                thr_context.join();
            }

        }

    } else{
        std::cout <<"Failed to connect to address:\n" << ec.message() << "\n";
    }

    return 0;
}