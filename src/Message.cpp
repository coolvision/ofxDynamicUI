/*
 * Message.cpp
 *
 *  Created on: Nov 6, 2013
 *      Author: sk
 */

#include "Message.h"
#include <zmq.h>

Message::Message() {

    type = UNDEFINED;
}

Message::Message(string name) {

    this->name = name;

    type = UNDEFINED;
}

Message::Message(zmq_msg_t *msg) {

    // deserializes these objects using msgpack::unpacker.
    msgpack::unpacker pac;

    // feeds the buffer.
    pac.reserve_buffer(zmq_msg_size(msg));
    memcpy(pac.buffer(), zmq_msg_data(msg), zmq_msg_size(msg));
    pac.buffer_consumed(zmq_msg_size(msg));

    msgpack::unpacked result;
    message_string = "";

    // get the name
    if (pac.next(&result)) {
        result.get().convert(&name);
    } else {
        return;
    }
    message_string = name + " - ";

    // get the type
    cout << "decode type " << endl;
    if (pac.next(&result)) {
        result.get().convert(&type);
        cout << "success " << type << endl;
    } else {
        cout << "fail " << endl;
        return;
    }
    message_string += type;
    message_string += " - ";

    // depending on the type, convert the array
    cout << "decode array " << endl;
    if (pac.next(&result)) {

        switch (type) {
        case STRING:
            result.get().convert(&string_v);
            for (int i = 0; i < string_v.size(); i++) {
                message_string += string_v[i];
                message_string += " : ";
            }
            cout << "success " << type << message_string << endl;
            break;
        case INT:
            result.get().convert(&int_v);
            for (int i = 0; i < int_v.size(); i++) {
                message_string += int_v[i];
                message_string += " : ";
            }
            cout << "success " << type << message_string << endl;
            break;
        case FLOAT:
            result.get().convert(&float_v);
            for (int i = 0; i < float_v.size(); i++) {
                message_string += float_v[i];
                message_string += " : ";
            }
            cout << "success " << type << message_string << endl;
            break;
        };
        
    } else {

        cout << "fail " << endl;

        return;
    }
}

void Message::addValue(string v) {

    if (type == UNDEFINED) {
        type = STRING;
    }

    string_v.push_back(v);
}

void Message::addIntValue(uint32_t v) {

    if (type == UNDEFINED) {
        type = INT;
    }

    int_v.push_back(v);
}

void Message::addValue(float v) {

    if (type == UNDEFINED) {
        type = FLOAT;
    }

    float_v.push_back(v);
}

void Message::addValue(size_t size, uint8_t* data) {

    if (type == UNDEFINED) {
        type = BINARY;
    }
}

void Message::clear() {

    type = UNDEFINED;
    int_v.clear();
    string_v.clear();
    float_v.clear();
}

void Message::send(void* socket) {

    packer = new msgpack::packer<msgpack::sbuffer>(&buffer);

    packer->pack(name);
    packer->pack(type);

    switch (type) {
    case STRING:
        packer->pack(string_v);
        break;
    case INT:
        packer->pack(int_v);
        break;
    case FLOAT:
        packer->pack(float_v);
        break;
    };

    cout << "send message " << name << " " << type << endl;

    zmq_msg_t msg;
    zmq_msg_init_size(&msg, buffer.size());
    memcpy(zmq_msg_data(&msg), buffer.data(), buffer.size());

    zmq_msg_send(&msg, socket, ZMQ_NOBLOCK);
    zmq_msg_close(&msg);

    delete packer;
    buffer.clear();
}

