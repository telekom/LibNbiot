// Project 
#include "libnbiot.h"
#include "libnbiotcore.h"
#include "nbiottimer.h"
#include "serialcom.h"
#include "nbiotstring.h"

// STD C++
#include <thread>
#include <iostream>
#include <iomanip>
#include <atomic>


extern SerialCom *pSerCom;


void dbgWrite(const unsigned char *data, unsigned short len) {
    for (int i = 0; i < len; ++i) {
        putchar(static_cast<int>(data[i]));
    }
}


bool ticking = true;
void invokeTick() {

    const std::chrono::duration<long long, std::ratio<1LL, 1000LL> >
            tickFrequencyMs = std::chrono::milliseconds(1);
  
    while (ticking) {
        tick();
        std::this_thread::sleep_for(tickFrequencyMs);
    }
}


bool reverseFlag =false;
int valueCounter = 0;
int getSensorValue() {
    if (valueCounter == 20) {
        reverseFlag = true;
    } else if (valueCounter == 0) {
        reverseFlag = false;
    }

    return (reverseFlag) ? valueCounter-- : valueCounter++;
}
    

int notification_id = 0;
void notificationHandler(const Notification *n)
{
    // Handle your notifications here
    notification_id++;

    std::cout << std::setw(9) << "\033[0;32m[ DEBUG    ]\033[0m ";

    std::cout << "----- New notification "
	      << std::setw(3)
              << std::setfill('0')
	      << std::right
	      << notification_id
	      << " -----" << std::endl;

    std::cout << std::setfill(' ');

    std::cout << std::setw(12)
	      << "\033[0;32m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "State:"
	      << n->state
              << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;32m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "Action:"
              << n->action
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;32m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "ReturnCode:"
              << n->returnCode
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;32m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "ErrorNumber:"
              << n->errorNumber
	      << std::endl;
}

bool messageArrived = false;
int message_counter = 0;
void subscriptionHandler(MessageData* msg)
{
    messageArrived = true;
    message_counter++;

    std::cout << std::setw(9)
	      << "\033[0;32m[ DEBUG    ]\033[0m ";

    std::cout << "----- New message "
	      << std::setw(3)
              << std::setfill('0')
	      << std::right
	      << message_counter
	      << " -----"
	      << std::endl;

    std::cout << std::setfill(' ');
    std::cout << std::setw(12)
	      << "\033[0;32m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "QoS:"
              << msg->message->qos
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;32m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "Id:"
              << msg->message->id
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;32m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "Payload:"
              << (char *) msg->message->payload
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;32m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "Topic:"
              << (char *) msg->topicName
	      << std::endl;
}


unsigned char init()
{
	unsigned int retCoreConf = CoreNoError;
	unsigned int retConf = NoError;

    setDebugWriteFunction(dbgWrite);

    pSerCom = new SerialCom("/dev/tty.usbserial-FTA34780");
    pSerCom->connect();
  
    NbiotCoreConf core_conf;
    core_conf.tickFrequency = 1000;
    core_conf.readstatus_fu = readStatus;
    core_conf.readbyte_fu = readByte;
    core_conf.putchar_fu = writeByte;
    core_conf.writestatus_fu = writeStatus;
    core_conf.apn = "nb-cloud.ic.m2mportal.de";
    core_conf.apnUser = "";
    core_conf.apnPwd = "";
    core_conf.operMccMnc = "26201";
    core_conf.imsi = "111111111112345";
    core_conf.imsiPwd = "thorsten";

    retCoreConf = nbiotCoreConfig(&core_conf);

    NbiotConf conf;
    conf.keepAliveInterval = 10000;
    conf.autoPollInterval = 5;
    conf.maxTopicCount = 3;
    conf.gateway = "172.25.102.151";
    conf.port = 1883;
    conf.notify_fu = notificationHandler;
    conf.pollInterval = 1000;
    
    retConf = nbiotConfig(&conf);

	if (retConf==0 && (retCoreConf==0 || (retCoreConf == CoreWarnApnPwd) || (retCoreConf == CoreWarnApnUser) || (retCoreConf == (CoreWarnApnPwd | CoreWarnApnUser))))
	{
		// Setup the statemachine, initialize internal varibles.
		unsigned char ret = nbiotStart();
		return ret;
	}
	else
	{
		return 0;
	}

}


int main(int argc, char** argv) {

    // Start timer thread.
    std::thread* ticker =  new std::thread(invokeTick);

    // Initialize library
    unsigned char retInit=init();

    std::atomic<NbiotResult> rc(LC_Pending);

    while(true && retInit)
    {
        if (isNbiotConnected())
	{	  
	    if (LC_Idle == rc)
	    {
	        // If not subscribed already, subscribe to command topic.
	        if(!isNbiotSubscribedTo( "NBIoT/111111111112345/CMD/MyCmd") )
		{
		    nbiotSubscribe( "NBIoT/111111111112345/CMD/MyCmd", subscriptionHandler );
	        }
		
	        if(messageArrived)
	        {
		    nbiotPublish("NBIoT/111111111112345/INF/MyCmd", "0", 1, QOS0);
		    messageArrived = false;
	        }
		else
		{
	            const std::string s = std::to_string(getSensorValue());

		    std::cout << std::setw(9)
			      << "\033[0;32m[ DEBUG    ]\033[0m "
			      << "Publish: "
			      << s
			      << std::endl;
	  
		    nbiotPublish("NBIoT/111111111112345/MES/1", s.c_str(), s.length(), QOS0);		  
		}
	    }	
	}
	else
	{
	    if (LC_Idle == rc){
	        nbiotConnect(1); // Connect with cleanSession=1
	    }
	}
	
	rc = nbiotEventLoop(EventModeActive);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    ticking = false;
    ticker->join();
}


