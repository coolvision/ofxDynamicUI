/*
 * ConnectedApp.h
 *
 *  Created on: Nov 6, 2013
 *      Author: sk
 */

#pragma once

#include "ofMain.h"
#include "Port.h"
#include "Message.h"

// on the other, client side, there is an application code
class ConnectedApp {
public:
    ConnectedApp();
    virtual ~ConnectedApp();

    void initConnection(string name_in);

    // main service update
    // can be called automatically ir manually
    // can be in the application thread, or in a separate one
    void processMessages();

    void processServiceRequests(); // answer to pings, requests about ports
    void processCallbacks(); // lauch registered callbacks
    void processVariables(); // update connected variables
    void sendMessages(); // process outbox

    // can implement messages processing here
    virtual void processInbox();

    Message *newMessage(string name);
    bool sendMessage();

    static void *context;
    void *responder;

    void *name_client; // for getting a communication socket name

    void *server;

    // supported messages
    vector<Port *> ports_in;
    vector<Port *> ports_out;

    // local message queues
    queue<Message *> inbox;
    queue<Message *> outbox;

    enum TransportMode {
        MODE_IPC = 0,
        MODE_IP
    };
    uint32_t transport_mode;

    string name;
    Address address;

    int port_number;

    vector<string> messages; // tmp for debugging
};
