#include "comPi.h"
#include "comserial.h"

#include <mutex>          // std::mutex

std::mutex mtx;           // mutex for critical section


ComPi::ComPi()
{
    pi = new ComSerial("/dev/ttyACM0");
    //ctor
}

ComPi::~ComPi()
{
    //dtor
}

void ComPi::send_TrigData(int angles[4], bool valid[4]){
	// empty buffer
	buffer.str(std::string(""));
	buffer << "angles " << angles[0]<< " "<< angles[1]<< " "<< angles[2] << " "<< angles[3] << " "<< valid[0] << " " << valid[1] << " "<< valid[2] << " "<< valid[3]<< " ";
	toSend = buffer.str();
	//cout << buffer.str();
	getChecksum();
	sendToPi();
	return;
}

void ComPi::read_USB(){
	pi->read();
    if(pi->newData){
	mtx.lock();
	if(strcmp(pi->receivedChars,"send"))
		state = Data;
	else if(strcmp(pi->receivedChars,"start"))
		state = Start;
	else if(strcmp(pi->receivedChars,"shutdown"))
		state = Shutdown;
	else{
		state = None;
		cout << "something weird received...";
	}
	mtx.unlock();
	}
    else
	mtx.lock();
	state = None;
	mtx.unlock();
}

void ComPi::getChecksum(){
    int len=toSend.length();
    //int c_value;
    checksum=0;
    cout << toSend << endl;
    for(int i=0;i<len;i++){
        //c_value = toSend.at(i);
        //cout << toSend.at(i) << " = " << c_value << endl;
        checksum+=toSend.at(i);
    }
 //   cout << "checksum " << checksum << endl;
}

int ComPi::sendToPi(){
    pi->write(toSend,checksum);
 //   cout << "comm over" << endl;
    return 1;
}
