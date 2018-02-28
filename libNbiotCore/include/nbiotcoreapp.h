#ifndef NBIOTCOREAPP_H
#define NBIOTCOREAPP_H

#include "nbiotcoreconf.h"

#include "nbiottime.h"
#include "serial.h"
#include "nbiotmodemtools.h" /*!< implements IModem */
#include "nbiotconnectivity.h" /*!< implements INetwork (was LibModem before) */

#include "inetwork.h"
#include "imodem.h"

/*!
 * \brief The (singleton) NbiotCoreApp class is a container for the instances of NbiotConnectivity, NbiotModemTools, Serial and NbiotTime. It provides the implementations of the interfaces INetwork and IModem.
 */
class NbiotCoreApp
{
public:
    /*!
     * \brief getInstance Singleton pattern instance getter
     * \return the instance of NbiotCoreApp
     */
    static NbiotCoreApp& getInstance();

    /*!
     * \brief destroy Singleton destructor
     */
    static void destroy();


    /*!
     * \brief getNetworkInstance
     * \return INetwork as reference to NbiotConnectivity instance
     */
    INetwork& getNetworkInstance() { return m_network; }
    /*!
     * \brief getModemInstance
     * \return IModem as reference to NbiotModemTools instance
     */
    IModem& getModemInstance() { return m_modem; }
    /*!
     * \brief getTimeInstance The only instance of the former singleton NbiotTime is now a member of NbiotCoreApp
     * \return reference to NbiotTime instance
     */
    NbiotTime& getTimeInstance() { return m_time; }

    /*!
     * \brief addUrcFilter Adds an URC filter. Wrapper function for NbiotModemTools method not part of IModem interface
     * \param urc unsolicited response code to be filtered
     * \param urc_handler notification handler for URC; receives the complete response line in its parameter
     * \return true in case of success
     */
    bool addUrcFilter(const char* urc, void (*urc_handler)(const char*)) { return m_serial.addUrcFilter(urc, urc_handler); }
    /*!
     * \brief removeUrcFilter Removes an URC filter. Wrapper function for NbiotModemTools method not part of IModem interface
     * \param urc URC to be removed from the list of filters
     * \return true in case of success
     */
    bool removeUrcFilter(const char* urc) { return m_serial.removeUrcFilter(urc); }

    /*!
     * \brief nbiotCoreConfig configures the library
     * \param conf pointer to NbiotCoreConf struct
     * \return OR'ed combination of NbiotCoreConfigError values or NoError in case of success
     */
    unsigned int nbiotCoreConfig(NbiotCoreConf* conf);

private:
    NbiotCoreApp();
    virtual ~NbiotCoreApp();
    NbiotCoreApp(const NbiotCoreApp&);
    NbiotCoreApp& operator = (const NbiotCoreApp&);

    NbiotTime m_time;
    Serial m_serial;
    NbiotModemTools m_modem;
    NbiotConnectivity m_network;

    unsigned int coreConfigError;

    static const int serialDebugPrefix = 2;

};

#endif // NBIOTCOREAPP_H
