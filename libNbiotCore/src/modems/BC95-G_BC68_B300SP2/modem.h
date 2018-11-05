
/* ========================================================================
 * LibNbiot: modem.h
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

#ifndef MODEM_H
#define MODEM_H

#include "nbiotstring.h"
#include "modembase.h"

/*!
 * \brief The Modem class
 */
class Modem : public ModemBase
{
public:
    /*!
     * \brief C'tor
     * \param serial
     */
    explicit Modem(Serial& serial) :
        ModemBase(serial)
    {}

    /*!
     * \brief D'tor
     */
    virtual ~Modem(){}

    /*!
     * \brief reboot
     * \return
     */
    virtual bool reboot();

    /*!
     * \brief initialize
     * \return
     */
    virtual bool initialize();

    //! @todo error struct and lastError() method

    /*!
     * \brief attach calls AT+COPS=0 for automatic PLMN selection. The modem should be attached after. You may watch the +CEREG URCs.
     * \returns true if the command was executed successful and responding "OK"
     */
    virtual bool attach();

    /*!
     * \brief setupPmUrcs
     */
    virtual bool setupPmUrcs();

    /*!
     * \brief parseNpsmrUrc
     * \param strNpsmrUrc
     */
    void parseNpsmrUrc(const char* strNpsmrUrc);

    /*!
     * \brief parseCsconUrc
     * \param strCsconUrc
     */
    void parseCsconUrc(const char* strCsconUrc);

private:

    Modem();
    Modem(const Modem&);
    Modem& operator=(const Modem&);

    bool checkQRegSwt();
    bool setQRegSwt2();

    static const int QREGSWT2 = 2;

    static const char* cmdNRB;
    static const char* cmdNSONMI3;
    static const char* cmdCEDRXS0;
    static const char* cmdNPSMR1;
    static const char* respNPSMR;
    static const char* cmdCSCON1;
    static const char* respCSCON;
    static const char* cmdQREGSWT2;
    static const char* cmdQREGSWTquery;
    static const char* respQREGSWTquery;
    static const char* cmdCOPS0;

#ifdef MANUAL_CONTEXT
    static const char* cmdCGACT_arg;
#endif
};

#endif // MODEM_H

