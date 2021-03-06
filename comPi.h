#ifndef COMPI_H
#define COMPI_H

#include <iostream>
#include <sstream>
#include "comserial.h"

using namespace std;
enum {Data,Start, Shutdown};
class ComPi
{
    public:
        ComPi();
        virtual ~ComPi();
        void send_TrigData(int angles[4], bool valid[4]);
	int read_USB();
    private:
        int sendToPi();
        void getChecksum();
        string toSend;
        stringstream buffer;
        int checksum;
        ComSerial* pi;
};

#endif // COMPI_H
