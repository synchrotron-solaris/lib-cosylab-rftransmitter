#include "mtx.h"

MTx::MTx(const std::shared_ptr<SNMPconnector> snmp, const std::shared_ptr<SNMPconnector> snmpW)
    : RFcomponent(oids[OIDS::MTX_SUMMARY], "MTx", snmp), snmpW(snmpW)
{}

void MTx::diagnose(const std::vector<std::string>& summaryValues)
{
    // Empty function since there is no diagnosis for MTx.
}

void MTx::updateStateAndStatus()
{    
    try
    {
        std::vector<std::string> values = snmp->readRequest(componentName);
        int32_t statesValue = convertToValue<int32_t>(values.at(0));

        // Update the state accordingly.
        setStateAndStatus(static_cast<States>(statesValue), StatesText[statesValue]);
    }
    catch(const SNMPconnectorException& e)
    {
        setStateAndStatus(States::UNKNOWN, e.what());
    }
    catch(const std::out_of_range&)
    {
        setStateAndStatus(States::UNKNOWN, dataAcquisitionFailed);
    }
}
