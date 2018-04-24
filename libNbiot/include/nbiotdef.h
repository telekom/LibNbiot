/* ========================================================================
 * LibNbiot: nbiotdef.h - enum definition for return and error codes
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

#ifndef NBIOTDEF_H
#define NBIOTDEF_H

/*!
 * \brief The MQTTSN_returnCodes enum
 */
enum MQTTSN_returnCodes
{
    RC_ACCEPTED,
    RC_REJECTED_CONGESTED,
    RC_REJECTED_INVALID_TOPIC_ID,
    RC_REJECTED_NOT_SUPPORTED
};

/*!
 * \brief The StateIds enum
 */
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

/*!
 * \brief The ErrorNumber enum
 */
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

/*!
 * \brief The NbiotAction enum
 */
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

/*!
 * \brief The NbiotEventMode enum
 */
enum NbiotEventMode
{
    EventModeActive,
    EventModeAsleep
};

/*!
 * \brief The NbiotResult enum
 */
enum NbiotResult
{
    LC_Success,
    LC_Idle,
    LC_Pending,
    LC_Fail
};

/*!
 * \brief The NbiotLoopId enum
 */
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

