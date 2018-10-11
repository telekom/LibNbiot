/* ========================================================================
 * LibNbiot: Tuino096Example for Arduino
 *
 * Copyright (c) 2018, Edgar Hindemith, Yassine Amraue, Thorsten
 * Krautscheid, Kolja Vornholt, T-Systems International GmbH
 * contact: libnbiot@t-systems.com, opensource@telekom.de
 *
 * This file is distributed under the conditions of the Apache License,
 * Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * For details see the file LICENSE at the toplevel.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/

#include <libnbiot.h>
#include <libnbiotcore.h>
#include <nbiotstring.h>
#include <nbiottimer.h>
#include <RTCZero.h>
#include <Adafruit_ZeroTimer.h>
#include <nbiotstringlist.h>


// Type in your Username and Password for authentication at the MQTT Broker here
// In case you are using Deutsche Telekom's Cloud of Things, the IMSI of the SIM Card is used as the Username
#define AUTH_IMSI "111111111111111"
#define AUTH_PWD "PASSWORD"

// Pins of the Arduino that are connected to the Power and Reset Pin of the BG96 module.
#define   BG96_RESET  11
#define   BG96_PWR    12

// Minimum and maximum values for the sawtooth signal
#define MAX_VAL 20
#define MIN_VAL 0

// read buffer for 1 byte 
unsigned char m_char_buffer;

// status variable that is required for answering incoming operations (CoT specific)
unsigned char messageArrived = 0;

// variable that counts incoming MQTT-SN Publishes
int message_counter = 0;

// Timer used to periodically call the tick method of the Library 
Adafruit_ZeroTimer globalTimer = Adafruit_ZeroTimer(3);

// status variable that tracks the success of the Library initialization
unsigned char retInit = 0;

// varibale that tracks the current status of the EventLoop of the Library
enum NbiotResult rc = LC_Pending;

// char arrays to store preconfigured MQTT topic names 
char topicCmd[32];
char topicInf[32];
char topicTemp[32];
char payload[11];

// variable that tracks whether the rtc on the Tuino Board was set to a time stamp synchronized by mobile network
bool rtcSet=true;

RTCZero rtc;

// variable that counts incoming notifications from the Library
int notification_id = 0;

// variables for generating the sawtooth signal
unsigned char reverseFlag = 0;
int valueCounter = MIN_VAL;

unsigned char readByte() {
  unsigned char ret = 0;
    if (Serial2) {
        return m_char_buffer;
    }
    return ret;
}

void writeByte(unsigned char buf) {
    if (Serial2) {
        Serial2.write(buf);
    }
}

ReadStatus readStatus() {
    ReadStatus ret = rx_empty;
    if (Serial2) {
      if(0 < Serial2.readBytes(&m_char_buffer, 1))
        ret = rx_avail;
    }
    return ret;
}

WriteStatus writeStatus() {
    WriteStatus ret = tx_full;
    if (Serial2) {
        ret = tx_empty;
    }
    return ret;
}


void TC3_Handler(){
  Adafruit_ZeroTimer::timerHandler(3);
}
void Timer3Callback0()
{
  tick();
}


void bg96PinsInit()
{
    pinMode(BG96_RESET,OUTPUT);
    pinMode(PIN_BG96_PWRKEY,OUTPUT);    
    digitalWrite(BG96_RESET,LOW);
    digitalWrite(PIN_BG96_PWRKEY,LOW);   
}

// powers up the bg96
void bg96PowerUp()
{
  digitalWrite(PIN_BG96_PWRKEY,LOW);
  delay(500);
  digitalWrite(PIN_BG96_PWRKEY,HIGH);
  delay(500);
  digitalWrite(PIN_BG96_PWRKEY,LOW);
}

void notificationHandler(const Notification *n) {
    // Handle your notifications here
    notification_id++;

    debugPrintf("[0;34m[ NOTIFY   ][0m ");

    debugPrintf("----- New notification %03d -----\r\n", notification_id);

    debugPrintf("[0;34m[----------][0m State: ");
    debugPrintf("%d\r\n", n->state);

    debugPrintf("[0;34m[----------][0m Action: ");
    debugPrintf("%d\r\n", n->action);

    debugPrintf("[0;34m[----------][0m ReturnCode: ");
    debugPrintf("%d\r\n", n->returnCode);

    debugPrintf("[0;34m[----------][0m ErrorNumber: ");
    debugPrintf("%d\r\n", n->errorNumber);
}

void subscriptionHandler(MessageData *msg) {
    messageArrived = 1;
    message_counter++;

    debugPrintf("\033[0;35m[ MSG      ]\033[0m ");

    debugPrintf("----- New message %03d -----\r\n", message_counter);

    debugPrintf("[0;35m[----------][0m QoS: ");
    debugPrintf("%d\r\n", msg->message->qos);

    debugPrintf("[0;35m[----------][0m Id: ");
    debugPrintf("%d\r\n", msg->message->id);

    debugPrintf("[0;35m[----------][0m Payload: ");
    debugPrintf("%s\r\n", (char *) msg->message->payload);

    if (msg->topicName) {
        debugPrintf("[0;35m[----------][0m Topic: ");
        debugPrintf("%s\r\n", (char *) msg->topicName);
    }
}

void dbgWrite(const unsigned char *data, unsigned short len) {
    for (int i = 0; i < len; ++i) {
        SerialUSB.write(data[i]);
    }
}

unsigned char init(char *imsi, char *pw) {
    unsigned char ret = 0;
    bg96PinsInit();
    bg96PowerUp();
    
    delay(2000);
    SerialUSB.begin(9600);
    Serial2.begin(115200);
    delay(2000);
    
    if (Serial2) {
        ret = 1;
    }

    if (1 == ret) {

        NbiotCoreConf core_conf;
        core_conf.tickFrequency = 1000;
        core_conf.readstatus_fu = readStatus;
        core_conf.readbyte_fu = readByte;
        core_conf.putchar_fu = writeByte;
        core_conf.writestatus_fu = writeStatus;
        core_conf.apn = "";  
        core_conf.apnUser = "";
        core_conf.apnPwd = "";
        core_conf.plmn = "20416";
          
        unsigned int retCoreConf = nbiotCoreConfig(&core_conf);
        
        if (NoError != (~(CoreWarnApnUser | CoreWarnApnPwd) & retCoreConf))
        {
           debugPrintf("\033[0;31m[ DEBUG    ]\033[0m ");
           debugPrintf("\033[0;31mError in core config: %i", retCoreConf);
           debugPrintf("\033[0m \r\n");
        }

        NbiotConf conf;

        conf.keepAliveInterval = 10000;
        conf.autoPollInterval = 300;
        conf.maxTopicCount = 3;
        conf.gateway = "172.25.102.151";
        conf.port = 1883;
        conf.notify_fu = notificationHandler;
        conf.login = imsi;
        conf.password = pw;
        conf.maxResend = 10;
        conf.pollInterval = 10000;

        unsigned int retConf = nbiotConfig(&conf);
        if (NoError != retConf) {
          debugPrintf("\033[0;31m[ DEBUG    ]\033[0m ");
          debugPrintf("\033[0;31mError in config: $i", retConf);
          debugPrintf("\033[0m \r\n");
        }
        
        // Setup the statemachine, initialize internal varibles.
        ret = nbiotStart();

        if ((NoError == retConf) && (NoError == (~(CoreWarnApnUser | CoreWarnApnPwd) & retCoreConf))) {
            
            if (1 == ret) {
                debugPrintf("\033[0;32m[ DEBUG    ]\033[0m ");
                debugPrintf("\033[0;32mInit Successfull\033[0m \r\n");
            }
        } else {
            ret = 0;
        }
    }
    if (0 == ret) {
        debugPrintf("\033[0;31m[ DEBUG    ]\033[0m ");
        debugPrintf("\033[0;31mInit Error\033[0m \r\n");
    }

    return ret;
}

int getSensorValue() {
    if (MAX_VAL == valueCounter) {
        reverseFlag = 1;
    } else if (MIN_VAL == valueCounter) {
        reverseFlag = 0;
    }
    return (reverseFlag) ? valueCounter-- : valueCounter++;
}

void setup() {

    // initialize RTC
    rtc.begin(); 
    rtc.setEpoch(0);
    setTime(rtc.getEpoch());

    // configure timer for tick
    ticker.configure(TC_CLOCK_PRESCALER_DIV1,TC_COUNTER_SIZE_16BIT,TC_WAVE_GENERATION_MATCH_FREQ);
    ticker.setCompare(0, SystemCoreClock/1000 -1 );
    ticker.setCallback(true, TC_CALLBACK_CC_CHANNEL0, Timer3Callback0);
    ticker.enable(true);
    
    setDebugWriteFunction(dbgWrite);


    char *imsi = AUTH_IMSI;
    char *pw = AUTH_PWD;

    // Set topics

    sprintf(topicCmd, "NBIoT/%s/CMD/MyCmd", imsi);

    sprintf(topicInf, "NBIoT/%s/INF/MyCmd", imsi);

    sprintf(topicTemp, "NBIoT/%s/MES/1", imsi);

    // Initialize library
    retInit = init(imsi, pw);

}

void loop() {
    if (retInit) {
      // configure the RTC to the right time as soon as module is attached to the mobile network
      if ((rtcSet == false) && isNbiotAttached())
      {
        char timeBuffer[50]={'0'};
        unsigned char ret = sendAtCommand("AT+QLTS",timeBuffer,49,100);
        nbiot::string timeString(timeBuffer);
        if (ret==1)
        {
          if (timeString.split("\"").count() >=2 )
          {
            if (timeString.split("\"")[1].split(",+").count() >= 2)
            {
               if ((timeString.split("\"")[1].split(",+")[0].split("/").count() >= 3) && (timeString.split("\"")[1].split(",+")[1].split(":").count() >= 3))
               {
                   rtc.setYear(atoi(timeString.split("\"",false)[1].split(",+")[0].split("/")[0].c_str())-2000);
                   rtc.setMonth(atoi(timeString.split("\"",false)[1].split(",+")[0].split("/")[1].c_str()));
                   rtc.setDay(atoi(timeString.split("\"",false)[1].split(",+")[0].split("/")[2].c_str()));
  
                   rtc.setHours(atoi(timeString.split("\"",false)[1].split(",+")[1].split(":")[0].c_str()));
                   rtc.setMinutes(atoi(timeString.split("\"",false)[1].split(",+")[1].split(":")[1].c_str()));
                   rtc.setSeconds(atoi(timeString.split("\"",false)[1].split(",+")[1].split(":")[2].c_str()));
  
                   rtcSet = true;
               }
            }
          }
        }            
      }
      
      if (isNbiotConnected()) {
        if (LC_Idle == rc) {
            // If not subscribed already, subscribe to command topic.
            if (!isNbiotSubscribedTo(topicCmd)) {
                nbiotSubscribe(topicCmd, subscriptionHandler);
            }
  
            if (messageArrived) {
                nbiotPublish(topicInf, "0", 1, QOS0);
                messageArrived = 0;                   
            } else {
                sprintf(payload, "%d", getSensorValue());
  
                debugPrintf("\033[0;32m[ DEBUG    ]\033[0m ");
                debugPrintf("Publish: %s\r\n", payload);
  
                nbiotPublish(topicTemp, payload, strlen(payload), QOS0);
            }
        }
       } else {
            if (LC_Idle == rc) {
                nbiotConnect(1); // Connect with cleanSession=1
            }
       }

      // correct Timer to RTC Timer if there is a deviation of more than one second
      if (rtc.getEpoch() - getTime() != 0) 
      {
        setTime(rtc.getEpoch());
      }
      
        rc = nbiotEventLoop(EventModeActive);
        delay(50);
    }
}
