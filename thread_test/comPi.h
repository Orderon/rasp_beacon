#ifndef COMPI_H
#define COMPI_H

#include <iostream>
#include <sstream>
#include "comserial.h"

using namespace std;
enum {Data,Start, Shutdown,None};
class ComPi
{
    public:
        ComPi();
        virtual ~ComPi();
        void send_TrigData(int pos[3]);
	void read_USB();
	int state = None;
    private:
        int sendToPi();
        void getChecksum();
        string toSend;
        stringstream buffer;
        int checksum;
        ComSerial* pi;
};

#endif // COMPI_H
