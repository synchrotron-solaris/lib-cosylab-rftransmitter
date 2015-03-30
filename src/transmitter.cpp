#include "transmitter.h"
#include <sstream>

Transmitter::Transmitter(const std::shared_ptr<SNMPconnector> snmp, const std::shared_ptr<SNMPconnector> snmpW)
    : RFcomponent(oids[OIDS::TRANS_SUMMARY], "Transmitter", snmp), snmpW(snmpW)
{
    // Register diagnose request.
    snmp->createBulkRequest(diagRequestName, oids[OIDS::TRANS_SUMMARY], noDiagNodes);

    // Register updateVariables request.
    // Create a vector with all oids.
    std::vector<std::string> variablesOids;
    variablesOids.push_back(oids[OIDS::TRANS_FP]);
    variablesOids.push_back(oids[OIDS::TRANS_RP]);
    variablesOids.push_back(oids[OIDS::TRANS_PAE]);
    variablesOids.push_back(oids[OIDS::TRANS_ON]);
    variablesOids.push_back(oids[OIDS::NOMINAL_POWER]);

    snmp->createRequest(upadateParamsName, variablesOids);

    // Initialize values
    nominalPower = 0;
    on = 0;
    forwardPower = 0;
    reflectedPower = 0;
    paEfficiency = 0;
}

Transmitter::~Transmitter()
{
    snmp->removeRequest(upadateParamsName);
    snmp->removeRequest(diagRequestName);
}

void Transmitter::diagnose(const std::vector<std::string>& summaryValues)
{
    try
    {
        // There is only one summary for TX
        int32_t stateValue = convertToValue<int32_t>(summaryValues[0]);

        // Get other information about the transmitter.
        std::vector<std::string> values = snmp->readRequest(diagRequestName);

        if(values.size() != noDiagNodes)
        {
            throw SNMPconnectorException(dataAcquisitionFailed);
        }

        // Construct the status.
        std::stringstream statusMsg;

        statusMsg << "detailed status: " << std::endl
                  << "txRF: " << StatesText[convertToValue<int32_t>(values[0])] << std::endl
                  << "txReflection: " << StatesText[convertToValue<int32_t>(values[1])] << std::endl
                  << "txRfSensorSummary: " << StatesText[convertToValue<int32_t>(values[2])] << std::endl
                  << "txLocal: " << StatesText[convertToValue<int32_t>(values[3])] << std::endl;

        setStateAndStatus(static_cast<States>(stateValue), statusMsg.str());
    }
    catch(const SNMPconnectorException& e)
    {
        setStateAndStatus(States::UNKNOWN, e.what());
    }
}

void Transmitter::updateReadParameters()
{
    try
    {
        std::vector<std::string> values = snmp->readRequest(upadateParamsName);

        if(values.size() != 5)
        {
            throw SNMPconnectorException("Update parameters failed. Not enough data recieved.");
        }

        // Convert to appropriate type and update.
        forwardPower = convertToValue<uint32_t>(values[0]);
        reflectedPower = convertToValue<uint32_t>(values[1]);
        paEfficiency = convertToValue<uint32_t>(values[2]);
        on = convertToValue<uint32_t>(values[3]);
        nominalPower = convertToValue<uint32_t>(values[4]);

        dataValid = true;
    }
    catch(const SNMPconnectorException& e)
    {
        dataValid = false;
        throw e;
    }
}
