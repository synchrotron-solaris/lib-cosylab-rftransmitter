#ifndef RFSENSOR_H
#define RFSENSOR_H

#include "rfcomponent.h"
#include <memory>

class RFsensor : public RFcomponent
{
    static const uint16_t noDiagNodes = 1; //!< Number of nodes to read for diagnostics. Only sensor calibrated node available.
public:
    /**
     * @brief RFsensor RF sensor component.
     * @param snmp Connection used for reading.
     */
    RFsensor(const std::shared_ptr<SNMPconnector> snmp);
    ~RFsensor();

    void diagnose(const std::vector<std::string>& summaryValues);
    void updateReadParameters();

    /**
     * @brief getForwardSt Gets the forward power state.
     * @return Forward power state.
     */
    inline uint32_t getForwardSt() { return forwardSt; }

    /**
     * @brief getReflectedSt Gets the reflected power state.
     * @return Reflected power state.
     */
    inline uint32_t getReflectedSt() { return reflectedSt; }

private:
    std::atomic<uint32_t> forwardSt; //!< Forward power state.
    std::atomic<uint32_t> reflectedSt; //!< Reflected power state.
    const std::string diagRequestName = "diagRFS"; //!< Name of the SNMP requests for diagnostics.
    const std::string upadateParamsName = "RFSsupdate"; //!< Name of the command to update parameters.
};

#endif // RFSENSOR_H
