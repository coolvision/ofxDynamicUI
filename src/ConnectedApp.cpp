/*
 * ConnectedApp.cpp
 *
 *  Created on: Nov 6, 2013
 *      Author: sk
 */

#include "ConnectedApp.h"

#include <zmq.h>
#include <msgpack.hpp>

void *ConnectedApp::context = NULL;

ConnectedApp::ConnectedApp() {

    context = zmq_ctx_new();
}

ConnectedApp::~ConnectedApp() {
}

void ConnectedApp::initConnection(string name_in) {

    int rc;

    responder = zmq_socket(context, ZMQ_REP);
    rc = zmq_bind(responder, "ipc:///tmp/0");

    name_client = zmq_socket(context, ZMQ_REQ);
    rc = zmq_connect(name_client, "ipc:///tmp/name");

    // send a name request
    Message m("name");
    m.send(name_client);

    // get a name reply
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    if (zmq_msg_recv(&msg, name_client, 0) == -1) {

    }
    cout << "got name" << endl;
    Message in_m(&msg);
    messages.push_back(in_m.message_string);
    zmq_msg_close(&msg);

    this->name = name_in + in_m.name;

    address.str = "ipc:///tmp/" + name;
    server = zmq_socket(context, ZMQ_REP);
    rc = zmq_bind(server, address.str.c_str());
}

void ConnectedApp::processMessages() {

    // server update
    zmq_msg_t msg;
    zmq_msg_init(&msg);

    if (zmq_msg_recv(&msg, responder, ZMQ_DONTWAIT) != -1) {

        cout << "got message" << endl;

        Message in_m(&msg);
        messages.push_back(in_m.message_string);

        // send a reply message
        // make a message object
        Message m_out;
        m_out.addValue("Reply");
        m_out.addValue("message");
        m_out.send(responder);
        m_out.clear();
    }
    zmq_msg_close(&msg);

    // btw if there are no ping messages for too long,
    // should probably exit

    zmq_msg_t in_msg;
    zmq_msg_init(&in_msg);
    if (zmq_msg_recv(&in_msg, server, ZMQ_DONTWAIT) != -1) {

        // reply to service messages
        Message in_m(&in_msg);

        if (in_m.name == "ping") {

            // 1) ping message
            Message m("status");
            m.addValue("ok");
            m.send(server);

        } else if (in_m.name == "input_ports") {

            // ports info request
            Message m("ports");
            for (int i = 0; i < ports_in.size(); i++) {
                m.addValue(ports_in[i]->name);
            }
            m.send(server);

        } else if (in_m.name == "output_ports") {

            // ports info request
            Message m("ports");
            for (int i = 0; i < ports_out.size(); i++) {
                m.addValue(ports_out[i]->name);
            }
            m.send(server);

        } else {

            // 1) ping message
            Message m("unknown");
            m.send(server);
        }

        messages.push_back(in_m.message_string);

    }
    zmq_msg_close(&in_msg);

}

void ConnectedApp::processServiceRequests() {

}

void ConnectedApp::processCallbacks() {

}

void ConnectedApp::processVariables() {
}

void ConnectedApp::sendMessages() {
}

void ConnectedApp::processInbox() {
}

Message* ConnectedApp::newMessage(string name) {

    return NULL;
}

bool ConnectedApp::sendMessage() {

    return false;
}
