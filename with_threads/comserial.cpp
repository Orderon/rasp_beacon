#include "comserial.h"
using namespace std;
extern "C" {
#include <wiringSerial.h>
}

ComSerial::ComSerial(std::string dev)
{
    newData = false;
    device_ready = false;
    device=-1;
    //"/dev/ttyACM0"
    device = serialOpen(dev.c_str(),9600);

    if(device==-1){
        cout << "comSerial: no device at" << dev << "\n";
        return;
    }else{
        cout << "comarduino: connected to device: " << device << "\n";
    }

    // waiting for device to be ready.
    // EV implement non blocking way to wait..
    while (!device_ready){
        cout << ".";
        this->read();
        if (newData == true){
            cout << "\n";
            device_ready = true;
            newData = false;
            for (int i = 0;i<CS_numchars;i++){
                cout << receivedChars[i];
            }
            cout << endl;
        }
    }
}

void ComSerial::write(std::string text, int checksum){
    std::string toSend(1,startMarker);
    toSend.append(text);
    toSend.push_back(endMarker);
    serialPuts(device,toSend.c_str());
}

void ComSerial::read(){

    char rc;
    static bool recvInProgress = false;
    static int ndx = 0;

    while(serialDataAvail (device)>0){
        rc = (char) serialGetchar(device);

        if (recvInProgress == true){
            if (rc != endMarker){
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= CS_numchars){
                    ndx = CS_numchars -1;
            	}
            }
            else {
                receivedChars[ndx] = '\0'; //terminate string
                recvInProgress = false;
                ndx = 0;
                newData = true;
		break;
            }
        }
        else if (rc == startMarker){
            recvInProgress = true;
            // clear data
            memset(&receivedChars[0], 0, sizeof(receivedChars));
        }
    }
}
