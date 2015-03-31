#include "outstage.h"

const std::string OutStage::upadateParamsName = "OSupdate";

OutStage::OutStage(const std::shared_ptr<SNMPconnector> snmp, const std::shared_ptr<SNMPconnector> snmpW)
    : RFcomponent(oids[OIDS::OSTAGE_SUMMARY], "OutStage", snmp), snmpW(snmpW)
{
    // Register updateParams request.
    std::vector<std::string> oid;
    oid.push_back(oids[OIDS::OUT_POWER]);
    snmpW->createRequest(upadateParamsName, oid); // Only for writing.
    snmp->createRequest(upadateParamsName, oid); // Only for reading by thread.
    power = 0;
}

OutStage::~OutStage()
{
    snmpW->removeRequest(upadateParamsName);
    snmp->removeRequest(upadateParamsName);
}

void OutStage::updateReadParameters()
{    
    try
    {
        std::vector<std::string> values = snmp->readRequest(upadateParamsName);

        // Atomic assigment.
        power = convertToValue<uint32_t>(values.at(0));
        dataValid = true;
    }
    catch(const std::out_of_range&)
    {
        dataValid = false;
        throw SNMPconnectorException(dataAcquisitionFailed);
    }
    catch(const SNMPconnectorException& e)
    {
        dataValid = false;
        throw e;
    }
}

void OutStage::updateStateAndStatus()
{    
    try
    {
        std::vector<std::string> values = snmp->readRequest(componentName);

        // Convert value to int.
        int32_t stateValue = convertToValue<int32_t>(values.at(0));

        // Update the state accordingly.
        setStateAndStatus(static_cast<States>(stateValue), StatesText[stateValue]);
    }
    catch(const std::out_of_range&)
    {
        setStateAndStatus(States::UNKNOWN, dataAcquisitionFailed);
    }
    catch(const SNMPconnectorException& e)
    {
        setStateAndStatus(States::UNKNOWN, e.what());
    }

}
