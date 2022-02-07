//
// Created by jstol on 07.02.2022.
//

#pragma once

#include "../common/types.h"

namespace bluesoft
{
    namespace net
    {
       template<typename T>
       struct message_header{
           T id{};
           uint32_t size{0};
       };

       template<typename T>
       struct message{
           message_header<T> header{};
           std::vector<uint8_t> body;

           size_t size(){
               return sizeof(message_header<T>) + body.size();
           }

           friend std::ostream& operator << (std::ostream& os, const message<T>& msg){
               os << "ID: " << int(msg.header.id) << " Size: " << msg.header.size;
               return os;
           }

           // Pushing any data into the message buffer
           template<typename DataType>
           friend message<T>& operator << (message<T>& msg, const DataType& data){
               static_assert(std::is_standard_layout<DataType>::value, "Data is to complex to be pushed into vector");

               size_t i = msg.body.size();

               msg.body.resize(msg.body.size() + sizeof(DataType));
               std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

               msg.header.size = msg.size();

               return msg;
           }


           template<typename DataType>
           friend message<T>& operator >> (message<T>& msg, DataType& data) {
               static_assert(std::is_standard_layout<DataType>::value, "Data is to complex to be get from vector");

               size_t i = msg.body.size() - sizeof(DataType);

               std::memcpy(&data, msg.body.data() + i, sizeof(DataType));


               msg.body.resize(i);

               msg.header.size = msg.size();

               return msg;

           }
       };

    }
}