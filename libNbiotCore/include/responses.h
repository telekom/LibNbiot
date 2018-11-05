/* ========================================================================
 * LibNbiot: responses.h
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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/
#ifndef RESPONSES_H
#define RESPONSES_H

struct CeregResponse
{
    int stat;
    char tac[3];
    char ci[5];
    int act;
    int causeType;
    int rejectCause;
    char activeTime[9];
    char periodicTau[9];

};
typedef struct CeregResponse CeregResponse;

typedef enum pmChangeFlag
{
    CF_NONE,
    CF_PSM,
    CF_CON,
    CF_EDRX
} PmChangeFlag;

struct PmResponse
{
    bool psmStatus;
    char psmRequestedActiveTime[9];
    char psmRequestedPeriodicTau[9];
    bool conStatus;
    int eDrxMode;
    int eDrxActType;
    char eDrxRequestedValue[5];
    char eDrxNwProvidedValue[5];
    char eDrxPagingTimeWindow[5];
    PmChangeFlag changeFlag;
};
typedef struct PmResponse PmResponse;

#endif // RESPONSES_H

