/* ==============================================================
 * File Name: nbiotdef.h
 *
 * Author: Edgar Hindemith
 *
 * Copyright T-Systems & Ingenieurbuero Edgar Hindemith, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF T-Systems.
 *
 * ==============================================================
*/
#ifndef NBIOTDEF_H
#define NBIOTDEF_H

enum MQTTSN_returnCodes
{
    RC_ACCEPTED,
    RC_REJECTED_CONGESTED,
    RC_REJECTED_INVALID_TOPIC_ID,
    RC_REJECTED_NOT_SUPPORTED
};

enum StateIds
{
    InvalidState = 0,
    InitialState,
    DisconnectedState,
    ErrorState,
    ConnectedState,
    ConnectedSleepState,
    DeepSleepState,
    DisconnectedSleepState,
    ConnectedAwakeState,
    WaitForGprsState,
    WaitForConnectState
};

enum ErrorNumber
{
    Success = 0,
    InitializeError,
    GprsNotConnected,
    IpNotConnected,
    ConnectionError,
    NotStartedError,
    MaxTopicsExceeded,
    LoopCtrlBusyError,
    InvalidTopicID
};

enum NbiotAction
{
    Initialize,
    ReInitialize,
    Connect,
    RegisterTopic,
    Publish,
    Subscribe,
    Unsubscribe,
    RemoveTopic,
    Disconnect,
    Sleep,
    Hibernate,
    Wakeup
};

enum NbiotEventMode
{
    EventModeActive,
    EventModeAsleep
};

enum NbiotResult
{
    LC_Success,
    LC_Idle,
    LC_Pending,
    LC_Fail
};

enum NbiotLoopId
{
    LI_Unknown = -1,
    LI_Cgatt,
    LI_Yield,
    LI_Connect,
    LI_Publish,
    LI_Register,
    LI_Subscribe,
    LI_Unsubscribe,
    LI_Disconnect
};

typedef enum NbiotEventMode NbiotEventMode;

#endif // NBIOTDEF_H

