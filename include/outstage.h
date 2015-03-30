#ifndef OUTSTAGE_H
#define OUTSTAGE_H

#include "rfcomponent.h"

class OutStage : public RFcomponent
{
public:
    /**
     * @brief OutStage Output stage component. Overview of all amplifiers and cooling.
     * @param snmp Connection used for reading.
     * @param snmpW Connection used for writting.
     */
    OutStage(const std::shared_ptr<SNMPconnector> snmp, const std::shared_ptr<SNMPconnector> snmpW);
    ~OutStage();

    inline void diagnose(const std::vector<std::string>& summaryValues)
    {
        // OutStage only has summary so no diagnose is needed.
        throw RfComponentException("Diagnose on output stage is not possible.");
    }

    void updateReadParameters();
    void updateStateAndStatus();

    /**
     * @brief setPower Set the output stage power.
     * @param value Desired power.
     */
    inline void setPower(const uint32_t value) { snmpW->setValue(oids[OIDS::OUT_POWER], value); }

    /**
     * @brief getPower Readback of the output stage power.
     * @return Output stage power.
     */
    inline uint32_t getPower() { return power; }

private:    
    const std::shared_ptr<SNMPconnector> snmpW; //!< Write connection.
    const std::string upadateParamsName = "OSupdate"; //!< Name of the command to update parameters.

    /// No locks needed for updating and reading component parameters. ///
    std::atomic<uint32_t> power; //!< Output stage power.
};

#endif // OUTSTAGE_H
