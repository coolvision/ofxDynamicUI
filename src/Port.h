/*
 * Port.h
 *
 *  Created on: Nov 16, 2013
 *      Author: sk
 */

#pragma once

class Port {
public:

    enum PortType {
        INPUT = 0, OUTPUT
    };

    Port(string name_in, PortType type_in):
        name(name_in), type(type_in) {};

    PortType type;
    string name;
    int id; // can be an index

    bool hover;

    ofRectangle r;
    ofPoint cp; // connection point
};

class Address {
public:
    // represents application address, can be an ip, for example
    string str;
};
