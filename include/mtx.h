#ifndef MTX_H
#define MTX_H

#include "rfcomponent.h"

class MTx : public RFcomponent
{
    static const int32_t resetValue = 2; //!< Walue to write to trigger reset command.
public:
    /**
     * @brief MTx MultiTxTransmitter component.
     * @param snmp Connection used for reading.
     * @param snmpW Connection used for writting.
     */
    MTx(const std::shared_ptr<SNMPconnector> snmp, const std::shared_ptr<SNMPconnector> snmpW);
    ~MTx() {}

    void diagnose(const std::vector<std::string>& summaryValues);
    inline void updateReadParameters() { /* Nothing to do here. */ }
    void updateStateAndStatus();

    /**
     * @brief reset Resets the MTx.
     */
    inline void reset() { snmpW->setValue(oids[OIDS::MTX_RESET], resetValue); }

private:
    const std::shared_ptr<SNMPconnector> snmpW; //!< Write connection.    
};

#endif // MTX_H
