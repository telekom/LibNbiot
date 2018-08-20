/* ========================================================================
 * LibNbiot: inetwork.h
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

#ifndef INETWORK_H
#define INETWORK_H

/*!
 * \brief The INetwork class
 */
class INetwork
{
public:
    virtual ~INetwork() {}

    /*!
     * \brief connect
     * \param hostname
     * \param port
     * \return
     */
    virtual bool connect(const char* hostname, unsigned short port) = 0;
    /*!
     * \brief read
     * \param buffer
     * \param len
     * \param timeout
     * \return
     */
    virtual int read(unsigned char* buffer, int len, unsigned short timeout) = 0;
    /*!
     * \brief write
     * \param buffer
     * \param len
     * \param timeout
     * \return
     */
    virtual bool write(unsigned char* buffer, unsigned long len, unsigned short timeout) = 0;
    /*!
     * \brief disconnect
     * \return
     */
    virtual bool disconnect() = 0;
    /*!
     * \brief isConnected
     * \return
     */
    virtual bool isConnected() = 0;
};

#endif // INETWORK_H

