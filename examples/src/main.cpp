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
#include <cstring>
#include <cstdlib>

// Unix
#include <getopt.h>
#include <unistd.h>


extern SerialCom *pSerCom;


void dbgWrite(const unsigned char *data, unsigned short len) {

    for (int i = 0; i < len; ++i) {
        putchar(static_cast<int>(data[i]));
    }
}


bool ticking = true;
void invokeTick()
{
    const std::chrono::duration<long long, std::ratio<1LL, 1000LL> >
            tickFrequencyMs = std::chrono::milliseconds(1);
  
    while (ticking) {
        tick();
        std::this_thread::sleep_for(tickFrequencyMs);
    }
}


bool reverseFlag =false;
int valueCounter = 0;
int getSensorValue()
{
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
	      << "\033[0;34m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "State:"
	      << n->state
              << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;34m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "Action:"
              << n->action
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;34m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "ReturnCode:"
              << n->returnCode
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;34m[----------]\033[0m "
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
	      << "\033[0;35m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "QoS:"
              << msg->message->qos
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;35m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "Id:"
              << msg->message->id
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;35m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "Payload:"
              << (char *) msg->message->payload
	      << std::endl;
    
    std::cout << std::setw(12)
	      << "\033[0;35m[----------]\033[0m "
	      << std::setw(14)
	      << std::left
	      << "Topic:"
              << (char *) msg->topicName
	      << std::endl;
}


unsigned char init(char* imsi, char* pw, char* ser)
{
    setDebugWriteFunction(dbgWrite);

    pSerCom = new SerialCom(ser);
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
    core_conf.imsi = imsi;
    core_conf.imsiPwd = pw;

    unsigned int retCoreConf = nbiotCoreConfig(&core_conf);

    NbiotConf conf;
    conf.keepAliveInterval = 10000;
    conf.autoPollInterval = 300;
    conf.maxTopicCount = 3;
    conf.gateway = "172.25.102.151";
    conf.port = 1883;
    conf.notify_fu = notificationHandler;
    conf.pollInterval = 10000;
    
    unsigned int retConf = nbiotConfig(&conf);
    unsigned char ret=0;

    if (retConf==NoError && !(~(CoreWarnApnUser | CoreWarnApnPwd) & retCoreConf))
    {
        // Setup the statemachine, initialize internal varibles.
	ret = pSerCom->isConnected() ? nbiotStart() : 0;
    }
    else
    {
	ret=0;
    }

    if (ret==1)
    {
	std::cout << std::setw(12)
		  << "\033[0;32m[ Debug    ]\033[0m "
		  << std::setw(14)
		  << std::left
		  << "\033[0;32mInit Successfull\033[0m "
		  << std::endl;
    }
    else
    {
	std::cerr << std::setw(12)
		  << "\033[0;31m[ Debug    ]\033[0m "
		  << std::setw(14)
		  << std::left
		  << "\033[0;31mInit Error\033[0m "
		  << std::endl;
    }
    
    return ret;
}


void usage()
{
    std::cout << "Usage: \r\n"
	      << "\t-h Print usage and exit.\r\n"
	      << "\t-i imsi - mandatory parameter 15 digit imsi\r\n"
	      << "\t-p password - mandatory parameter 8 characters\r\n"
              << "\t-s serial - mandatory parameter, path to serial device (e.g. /dev/ttyACM0)"
              << std::endl;
}


int main(int argc, char** argv)
{

  char* imsi = nullptr;
  char* pw = nullptr;
  char* ser = nullptr;

  int c = -1;
  while((c = getopt (argc, argv, "i:p:s:")) != -1)
  {
      switch(c)
      {
      case 'i':
	{
	    imsi = optarg;
	    if(15 == strlen(imsi))
	    {
		std::cout << std::setw(12)
			  << "\033[0;32m[ Debug    ]\033[0m "
			  << std::setw(14)
			  << std::left		
			  << "imsi: " << imsi << std::endl;
	    }
	    else
	    {
		std::cerr << std::setw(12)
			  << "\033[0;31m[ Debug    ]\033[0m "
			  << std::setw(14)
			  << std::left << "imsi: invalid value (imsi has to be of length 15) - aborting" << std::endl;
		imsi = nullptr;
	    }
	    break;
	}
      case 'p':
	{
	    pw = optarg;
	    if(8 == strlen(pw))
	    {
		std::cout << std::setw(12)
			  << "\033[0;32m[ Debug    ]\033[0m "
			  << std::setw(14)
			  << std::left				
			  << "pw: " << pw << std::endl;
	    }
	    else
	    {
		std::cerr << std::setw(12)
			  << "\033[0;31m[ Debug    ]\033[0m "
			  << std::setw(14)
			  << std::left		
			  << "pw: invalid value (pw has to be of length 8) - aborting" << std::endl;
		pw = nullptr;
	    }
	    break;
	}
      case 's':
      {
	  ser = optarg;
	  if(-1 != access(ser, F_OK))
	  {
	      std::cout << std::setw(12)
			<< "\033[0;32m[ Debug    ]\033[0m "
			<< std::setw(14)
			<< std::left			      
			<< "serial node: " << ser << std::endl;
	  }
	  else
	  {
	      std::cerr << std::setw(12)
			<< "\033[0;31m[ Debug    ]\033[0m "
			<< std::setw(14)
			<< std::left << "serial: no device node with path \"" << ser << " exists - aborting" << std::endl;
	      ser = nullptr;
	  }
	  break;
      }
      case 'h':
      {
	  usage();
	  exit(EXIT_SUCCESS);
      }
      default:
	  std::cout << "skipping unsupported option: " << c << std::endl;
	  break;
      }
  }

  if(nullptr == imsi || nullptr == pw || nullptr == ser)
  {

      std::cerr << std::setw(12)
		<< "\033[0;31m[ Error    ]\033[0m "
		<< std::setw(14)
		<< std::left
		<< "parameters imsi, pw and serial, have to be set as described in usage: \r\n" << std::endl;
      
      usage();
      exit(EXIT_FAILURE);
  }

  // Set topics
  char topicCmd[32];
  sprintf (topicCmd, "NBIoT/%s/CMD/MyCmd", imsi);
  
  char topicInf[32];
  sprintf(topicInf, "NBIoT/%s/INF/MyCmd", imsi);
  
  char topicTemp[32];
  sprintf(topicTemp, "NBIoT/%s/MES/1", imsi);

  // Start timer thread.
  std::thread* ticker =  new std::thread(invokeTick);
    

  // Initialize library
  unsigned char retInit=init(imsi, pw, ser);

  NbiotResult rc = LC_Pending;

  while(true && retInit)
  {
      if (isNbiotConnected())
      {	  
	  if (LC_Idle == rc)
	  {
	      // If not subscribed already, subscribe to command topic.
	      if(!isNbiotSubscribedTo(topicCmd))
	      {
		  nbiotSubscribe(topicCmd, subscriptionHandler );
	      }
		
	      if(messageArrived)
	      {
		  nbiotPublish(topicInf, "0", 1, QOS0);
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
	  
		  nbiotPublish(topicTemp, s.c_str(), s.length(), QOS0);		  
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

  exit(EXIT_SUCCESS);
}


