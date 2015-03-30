#include "rfsensor.h"
#include <sstream>

RFsensor::RFsensor(const std::shared_ptr<SNMPconnector> snmp) :
    RFcomponent(oids[OIDS::RF_LINK], "RFsensor", snmp)
{
    // Register updateParams request.
    std::vector<std::string> variablesOids;
    variablesOids.push_back(oids[OIDS::RFS_FORWARD]);
    variablesOids.push_back(oids[OIDS::RFS_REFLECTED]);
    snmp->createRequest(upadateParamsName, variablesOids);

    // Register DIAG request.
    snmp->createBulkRequest(diagRequestName, oids[OIDS::RF_LINK], noDiagNodes);

    forwardSt = 0;
    reflectedSt = 0;
}

RFsensor::~RFsensor()
{
    snmp->removeRequest(upadateParamsName);
    snmp->removeRequest(diagRequestName);
}

void RFsensor::diagnose(const std::vector<std::string>& summaryValues)
{
    try
    {
        int32_t stateValue = convertToValue<int32_t>(summaryValues[0]);

        // Get other information about the transmitter.
        std::vector<std::string> values = snmp->readRequest(diagRequestName);

        // Construct the status.
        std::stringstream statusMsg;
        statusMsg << "detailed status:" << std::endl << "txRfSensorCalibrated: " << StatesText[convertToValue<int32_t>(values.at(0))] << std::endl;
        setStateAndStatus(static_cast<States>(stateValue), statusMsg.str());
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

void RFsensor::updateReadParameters()
{
    try
    {
        // Read the values.
        std::vector<std::string> values = snmp->readRequest(upadateParamsName);

        // Convert to appropriate type and update.
        forwardSt = convertToValue<uint32_t>(values.at(0));
        reflectedSt = convertToValue<uint32_t>(values.at(1));
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
