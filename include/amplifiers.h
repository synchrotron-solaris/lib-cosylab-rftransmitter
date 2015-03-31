#ifndef AMPLIFIERS_H
#define AMPLIFIERS_H

#include "rfcomponent.h"
#include <array>

class Amplifiers : public RFcomponent
{
    static const uint16_t noAmplifiers = 12; //!< Number of amplifiers used at Solaris.
    static const uint16_t noDiagNodes = 15; //!< Number of nodes to read in order to get full diagnostics done.

public:
    /**
     * @brief Amplifiers Amplifiers component. Usually there is more than 1.
     * @param indexList Indices of the amplifiers in the RF transmitter.
     * @param snmp Connection used for reading.
     */
    Amplifiers(const std::vector<uint16_t>& indexList, const std::shared_ptr<SNMPconnector> snmp);
    ~Amplifiers();

    void diagnose(const std::vector<std::string>& summaryValues);
    void updateReadParameters();

    /**
     * @brief getAmpON Returns the output state of specific amplifier.
     * @param index Index of the amplifier 0 to (N-1)
     * @return ON or OFF
     */
    bool getAmpON(const uint16_t index);

private:
    std::array<std::atomic<int32_t>, noAmplifiers> ampOn; //!< Array of amp switch states.
    static const std::string upadateParamsName; //!< Name of the SNMP update parameters request.
    static const std::string diagRequestName; //!< Name of the SNMP requests for diagnostics.
};

#endif // AMPLIFIERS_H
