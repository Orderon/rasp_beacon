#ifndef COMSERIAL_H
#define COMSERIAL_H

#include <iostream>
#include <sstream>
#include <string.h>
#define CS_numchars 64

class ComSerial
{
    bool device_ready;
    int device;
    static char const startMarker = '<';
    static char const endMarker = '>';

public:
    bool newData;
    char receivedChars[CS_numchars];
    ComSerial(std::string dev);
    void read();
    void write(std::string text, int checksum);
};

#endif // COMSERIAL_H
