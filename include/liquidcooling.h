#ifndef LIQUIDCOOLING_H
#define LIQUIDCOOLING_H

#include "rfcomponent.h"
#include <array>

class LiquidCooling : public RFcomponent
{
    static const uint16_t numberOfLiquidDevices = 2; //!< Number of liquid cooling devices.
    static const uint16_t noDiagNodes = 4; //!< Number of nodes to read in order to get full diagnostics done.
public:

    /**
     * @brief LiquidCooling Liquid cooling component with multiple liquit cooling devices.
     * @param indexList List of indexes for liquid cooling devices.
     * @param snmp Connection used for reading.
     */
    LiquidCooling(const std::vector<uint16_t>& indexList, const std::shared_ptr<SNMPconnector> snmp);
    ~LiquidCooling();

    void diagnose(const std::vector<std::string>& summaryValues);
    void updateReadParameters();

    /**
     * @brief getInTemps Returns the inlet temperatures.
     * @return Array of inlet temperatures. In our case 2 values.
     */
    inline const std::array<std::atomic<int32_t>, numberOfLiquidDevices>& getInTemps() { return inTemp; }

    /**
     * @brief getOutTemps Returns the outlet temperatures.
     * @return Array of outlet temperatures. In our case 2 values.
     */
    inline const std::array<std::atomic<int32_t>, numberOfLiquidDevices>& getOutTemps() { return outTemp; }

private:    
    std::array<std::string, numberOfLiquidDevices> diagRequestName; //!< Name of the SNMP requests for diagnostics for each component.
    std::string upadateParamsName; //!< Name of the SNMP update parameters request.

    /// No locks needed for updating and reading component parameters. ///
    std::array<std::atomic<int32_t>, numberOfLiquidDevices> inTemp; //!< Holder for inlet temperatures.
    std::array<std::atomic<int32_t>, numberOfLiquidDevices> outTemp; //!< Holder for outlet temperatures.
};

#endif // LIQUIDCOOLING_H
