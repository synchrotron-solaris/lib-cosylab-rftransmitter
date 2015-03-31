#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include "rfcomponent.h"

class Transmitter : public RFcomponent
{
    static const uint16_t noDiagNodes = 4; //!< Number of nodes to read in order to get full diagnostics done.
    static const int32_t resetValue = 2; //!< Walue to write to trigger reset command.
public:
    /**
     * @brief Transmitter Transmitter component.
     * @param snmp Connection used for reading.
     * @param snmpW Connection used for writting.
     */
    Transmitter(const std::shared_ptr<SNMPconnector> snmp, const std::shared_ptr<SNMPconnector> snmpW);
    ~Transmitter();

    void diagnose(const std::vector<std::string>& summaryValues);
    void updateReadParameters();

    /**
     * @brief reset Resets the transmitter.
     */
    inline void reset() { snmpW->setValue(oids[OIDS::TRANS_RESET], resetValue); }

    /**
     * @brief powerSwitch Power switch for the transmitter.
     * @param value ON or OFF as integer.
     */
    inline void powerSwitch(const int32_t value) { snmpW->setValue(oids[OIDS::TRANS_ON], value); } /// TODO: Test which value to write for what.

    /**
     * @brief setNominalPower Sets the desired nominal power.
     * @param nominalPower Desired nominal power.
     */
    inline void setNominalPower(const uint32_t nominalPower) { snmpW->setValue(oids[OIDS::NOMINAL_POWER], nominalPower); }

    /**
     * @brief getNominalPower Reads the desired nominal power.
     * @return Desired nominal power.
     */
    inline uint32_t getNominalPower() { return nominalPower; }

    /**
     * @brief getSwitchOn Reads the state of the power switch.
     * @return ON or OFF.
     */
    inline int32_t getSwitchOn() { return on; }

    /**
     * @brief getForwardPower Reads the forward power output.
     * @return Forward power.
     */
    inline uint32_t getForwardPower() { return forwardPower; }

    /**
     * @brief getReflectedPower Reads the reflected power.
     * @return Reflected power.
     */
    inline uint32_t getReflectedPower() { return reflectedPower; }

    /**
     * @brief getPaEfficiency Reads efficiency of all amplifiers.
     * @return Efficiency. Negative value means it could not be read.
     */
    inline int32_t getPaEfficiency() { return paEfficiency; }

    enum switchON
    {
        ON = 1,
        OFF = 2
    };

private:
    const std::shared_ptr<SNMPconnector> snmpW; //!< Write connection.
    static const std::string diagRequestName; //!< Name of the SNMP requests for diagnostics.
    static const std::string upadateParamsName; //!< Name of the command to update parameters.

    /// No locks needed for updating and reading component parameters. ///
    std::atomic<uint32_t> nominalPower; //!< Holder for nominal power.
    std::atomic<int32_t> on; //!< Holder for power switch state.
    std::atomic<uint32_t> forwardPower; //!< Holder for forward power.
    std::atomic<uint32_t> reflectedPower; //!< Holder for reflected power.
    std::atomic<int32_t> paEfficiency; //!< Holder for pa efficiency.    
};

#endif // TRANSMITTER_H
