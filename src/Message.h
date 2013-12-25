/*
 * Message.h
 *
 *  Created on: Nov 6, 2013
 *      Author: sk
 */

#pragma once

#include "ofMain.h"
#include <msgpack.hpp>
#include <zmq.h>

class BinaryBlob {
    size_t size;
    uint8_t *data;
};

// represent a named tuple
class Message {
public:

    enum MessageValueType {
        UNDEFINED = 0,
        STRING,
        INT,
        FLOAT,
        BINARY
    };

    msgpack::sbuffer buffer;
    msgpack::packer<msgpack::sbuffer> *packer;

    Message();
    Message(string name);
    Message(zmq_msg_t *msg); // decode the message

    void addValue(string v);
    void addIntValue(uint32_t v);
    void addValue(float v);
    void addValue(size_t size, uint8_t *data);

    void clear();

    // serialize before sending
    void send(void *socket);
    void print();

    string name;
    uint32_t type;

    string message_string; // for debugging

    // don't want to do casting all the time,
    // so store all by value
    vector<uint32_t> int_v;
    vector<string> string_v;
    vector<float> float_v;
    vector<BinaryBlob> binary_values;
};
