#include "liquidcooling.h"
#include <sstream>

LiquidCooling::LiquidCooling(const std::vector<uint16_t>& indexList, const std::shared_ptr<SNMPconnector> snmp)
    : RFcomponent(transformToOids(oids[OIDS::LQC_SUMMARY], indexList), "LiquidCooling", snmp)
{
    std::vector<std::string> nodes = transformToOids(oids[OIDS::LQC_SUMMARY], indexList);
    // Create diag request. There are 2 since we have 2 LQs in the system.
    snmp->createBulkRequest(diagRequestName[0], nodes.at(0), noDiagNodes);
    snmp->createBulkRequest(diagRequestName[1], nodes.at(1), noDiagNodes);

    // Register updateVariables request.
    // Create a vector with all oids.
    std::vector<std::string> nodesIn = transformToOids(oids[OIDS::LQ_TIN], indexList);
    std::vector<std::string> nodesOut = transformToOids(oids[OIDS::LQ_TOUT], indexList);
    std::vector<std::string> variablesOids;

    variablesOids.push_back(nodesIn.at(0));
    variablesOids.push_back(nodesIn.at(1));
    variablesOids.push_back(nodesOut.at(0));
    variablesOids.push_back(nodesOut.at(1));

    snmp->createRequest(upadateParamsName, variablesOids);
}

LiquidCooling::~LiquidCooling()
{
    snmp->removeRequest(diagRequestName[0]);
    snmp->removeRequest(diagRequestName[1]);
    snmp->removeRequest(upadateParamsName);
}

void LiquidCooling::diagnose(const std::vector<std::string>& summaryValues)
{
    std::array<int32_t, 2> states;

    try
    {
        states = { convertToValue<int32_t>(summaryValues[0]), convertToValue<int32_t>(summaryValues[1]) };
    }
    catch(const SNMPconnectorException& e)
    {
        setStateAndStatus(States::UNKNOWN, e.what());
    }

    // Create status msg.
    std::stringstream statusMsg;
    for(size_t i = 0; i < states.size(); i++)
    {
        statusMsg << std::endl << i;

        if(static_cast<States>(states[i]) != States::OK)
        {
            try
            {
                std::vector<std::string> values = snmp->readRequest(diagRequestName[i]);
                if(values.size() != noDiagNodes)
                {
                    throw SNMPconnectorException(dataAcquisitionFailed);
                }

                statusMsg << " detailed status: " << std::endl
                          << "lqFilterSummary: " << StatesText[convertToValue<int32_t>(values[0])] << std::endl
                          << "lqSensorsSummary: " << StatesText[convertToValue<int32_t>(values[1])] << std::endl
                          << "lqSiteWarning: " << StatesText[convertToValue<int32_t>(values[2])] << std::endl
                          << "lqSiteFault: " << StatesText[convertToValue<int32_t>(values[3])] << std::endl;

            }
            catch(const SNMPconnectorException& e)
            {
                states[i] = static_cast<int32_t>(States::UNKNOWN);
                statusMsg << " :" << e.what() << std::endl;
            }
        }
        else
        {
            statusMsg << ": OK" << std::endl;
        }
    }

    /// TODO: Check this at actual hardware.
    int32_t stateValue = (states[0] <= states[1]) ? states[0] : states[1]; // Always chose the lowest state (worse).

    // Update status and state.
    setStateAndStatus(static_cast<States>(stateValue), statusMsg.str());
}

void LiquidCooling::updateReadParameters()
{
    try
    {
        std::vector<std::string> values = snmp->readRequest(upadateParamsName);

        if(values.size() != inTemp.size() + outTemp.size())
        {
            throw SNMPconnectorException(dataAcquisitionFailed);
        }

        for(size_t i = 0; i < values.size()/2; i++)
        {
            inTemp[i] = convertToValue<int32_t>(values[i]);
            outTemp[i] = convertToValue<int32_t>(values[i+2]);
        }
        dataValid = true;
    }
    catch(const SNMPconnectorException& e)
    {
        dataValid = false;
        throw e;
    }
}
