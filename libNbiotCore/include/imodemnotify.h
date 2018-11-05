/* ========================================================================
 * LibNbiot: imodemnotify.h
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
#ifndef IMODEMNOTIFY_H
#define IMODEMNOTIFY_H

#include "responses.h"
/*!
 * \brief The IModemNotify class
 */
class IModemNotify
{
public:

    virtual ~IModemNotify() {}

    /*!
     * \brief ceregHandler
     * \param ceregResp
     */
    virtual void ceregHandler(const CeregResponse& ceregResp) = 0;

    /*!
     * \brief powerMgmtHandler
     * \param pmResp
     */
    virtual void powerMgmtHandler(const PmResponse& pmResp) = 0;

};

#endif // IMODEMNOTIFY_H

