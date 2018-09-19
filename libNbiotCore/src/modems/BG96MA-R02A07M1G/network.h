/* ========================================================================
 * LibNbiot: network.h
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

#ifndef NETWORK_H
#define NETWORK_H

#include <time.h>

#include "atcommands.h"
#include "nbiotstring.h"
#include "serial.h"
#include "inetwork.h"
#include "nbiotcircularbuffer.h"


#define READ_BUFFER_SIZE 256

/*!
 * \brief The Network class
 */
class Network : public INetwork
{
public:

    /*!
     * \brief C'tor
     * \param serial
     */
    explicit Network(Serial& serial);

    /*!
     * \brief D'tor
     */
    virtual ~Network() noexcept {}

    /*!
     * \brief connect
     * \param hostname
     * \param port
     * \return
     */
    virtual bool connect(const char* hostname, unsigned short port);

    /*!
     * \brief read
     * \param buffer
     * \param len
     * \param timeout_ms
     * \return
     */
    virtual int read(unsigned char* buffer, int len, unsigned short timeout_ms);

    /*!
     * \brief write
     * \param buffer
     * \param len
     * \param timeout
     * \return
     */
    virtual bool write(unsigned char* buffer, unsigned long len, unsigned short timeout);

    /*!
     * \brief disconnect
     * \return
     */
    virtual bool disconnect();

    /*!
     * \brief isConnected
     * \return
     */
    virtual bool isConnected() { return (0 <= m_connectionNumber); }

    /*!
     * \brief parseFilterResult
     * \param strFilterResult
     */
    void parseFilterResult(const char* strFilterResult);

private:

    Network(const Network&) = delete;
    Network& operator=(const Network&) = delete;

    unsigned short ipAvailable();
    int ipRead(nbiot::CircularBuffer<READ_BUFFER_SIZE>& data, int len, unsigned short timeout_ms);

    AtCommands m_cmd;
    int m_connectionNumber;
    nbiot::string m_hostname;
    unsigned short m_port;
    size_t m_bytesAvail;
    nbiot::string m_qiurc;

    static const unsigned short byteCountMask = 0xFFFF;

    static const char* cmdQIOPEN_arg;
    static const char* respQIURCrecv_arg;
    static const char* cmdQIRD_arg;
    static const char* cmdQISENDEX_arg;
    static const char* cmdQICLOSE_arg;

    static const unsigned int readInterval = 100;
    static const unsigned int oneSecond = 1000;
    static const unsigned int threeSeconds = 3000;
    static const unsigned int tenSeconds = 10000;

    nbiot::CircularBuffer<READ_BUFFER_SIZE> m_readBuffer; // specific for BG96 module

};

#endif // NETWORK_H
