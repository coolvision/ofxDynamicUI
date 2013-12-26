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

    name_client = zmq_socket(context, ZMQ_REQ);
    rc = zmq_connect(name_client, "ipc:///tmp/name");

    // send a name request
    Message m("name");
    m.send(name_client);

    cout << "ConnectedApp: send name request " << endl << flush;

    // poll for a few seconds
    zmq_pollitem_t items [] = {
        { name_client,  0, ZMQ_POLLIN, 0 }
    };
    zmq_poll(items, 1, 2 * 1000);

    if (items[0].revents & ZMQ_POLLIN) {

        // use the received name
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        int r = zmq_msg_recv(&msg, name_client, 0);

        cout << "ConnectedApp: got name reply " << r << endl << flush;
        Message in_m(&msg);
        messages.push_back(in_m.message_string);
        zmq_msg_close(&msg);

        cout << "got name " << in_m.name << endl;
        this->name = in_m.name;

        address.str = "ipc:///tmp/" + name;
        server = zmq_socket(context, ZMQ_REP);
        rc = zmq_bind(server, address.str.c_str());

    } else {

        // if did not get a reply, use the tcp socket
        address.str = "tcp://*:" + ofToString(port_number);
        server = zmq_socket(context, ZMQ_REP);
        rc = zmq_bind(server, address.str.c_str());

        cout << "ConnectedApp: no name reply" << endl << flush;

    }

}

void ConnectedApp::processMessages() {

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
