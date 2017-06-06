#include "comPi.h"
#include "comserial.h"

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
int ComPi::read_USB(){
	pi->read();
    if(pi->newData){
	if(strcmp(pi->receivedChars,"send"))
		return Data;
	else if(strcmp(pi->receivedChars,"start"))
		return Start;
	else if(strcmp(pi->receivedChars,"shutdown"))
		return Shutdown;
	else
	cout << "something weird received...";
	}
    else
	return false;
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
