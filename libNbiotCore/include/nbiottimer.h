/* ========================================================================
 * LibNbiot: nbiottimer.h
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

#ifndef NBIOTTIMER_H
#define NBIOTTIMER_H

#include "itimer.h"

namespace nbiot {

/*!
 * \brief The Timer class
 */
class Timer:public ITimer
{
public:
    /*!
     * \brief Timer
     */
    Timer(): start_ms(0), end_ms(0) {}
    /*!
     * \brief Timer
     * \param end
     */
    Timer(unsigned long end): start_ms(0), end_ms(0) { start(end); }
    /*!
     * \brief Timer
     * \param end
     */
//    Timer(int end): start_ms(0), end_ms(0) { start(end); }
//    /*!
//     * \brief Timer
//     * \param end
//     */
//    Timer(unsigned short end): start_ms(0), end_ms(0) { start(end); }
//    /*!
//     * \brief ~Timer
//     */
    virtual ~Timer() {}

    /*!
     * \brief start
     */
    virtual void start();
    /*!
     * \brief start
     * \param end
     */
    virtual void start(unsigned long end);
//    /*!
//     * \brief start
//     * \param end
//     */
//    virtual void start(int end);
//    /*!
//     * \brief start
//     * \param end
//     */
//    virtual void start(unsigned short end);

    /*!
     * \brief setTime
     * \param end
     */
    virtual void setTime(unsigned long end);
//    /*!
//     * \brief setTime
//     * \param end
//     */
//    virtual void setTime(int end);
//    /*!
//     * \brief setTime
//     * \param end
//     */
//    virtual void setTime(unsigned short end);

    /*!
     * \brief getTime
     * \return
     */
    virtual unsigned long getTime() const;
    /*!
     * \brief expired
     * \return
     */
    virtual unsigned long expired() const;
    /*!
     * \brief remaining
     * \return
     */
    virtual unsigned long remaining() const;
    /*!
     * \brief clear
     */
    virtual void clear();
    /*!
     * \brief valid
     * \return
     */
    virtual bool valid() const;
    /*!
     * \brief _expired
     * \return
     */
    bool _expired() { return (0 == remaining()); }
    /*!
     * \brief countdown_ms
     * \param ms
     */
    void countdown_ms (unsigned long ms) { start(ms); }
    /*!
     * \brief countdown
     * \param s
     */
    void countdown (unsigned long s) { start(ms_per_sec * s); }
    /*!
     * \brief left_ms
     * \return
     */
    unsigned short left_ms()
    {
        unsigned short ret = static_cast<unsigned short>(remaining());
        return ret;
    }

private:
    long long start_ms;
    long long end_ms;

    static const unsigned long ms_per_sec = 1000;
};

}


#endif // NBIOTTIMER_H
