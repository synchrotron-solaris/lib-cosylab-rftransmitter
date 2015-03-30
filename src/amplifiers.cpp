#include "amplifiers.h"
#include <sstream>

Amplifiers::Amplifiers(const std::vector<uint16_t>& indexList, const std::shared_ptr<SNMPconnector> snmp) :
    RFcomponent(transformToOids(oids[OIDS::AMP_SUMMARY], indexList), "AMPs", snmp)
{
    if(indexList.size() != noAmplifiers)
    {
        throw RfComponentException("Incorrect number of amplifier indices provided.");
    }    

    std::vector<std::string> baseOids = transformToOids(oids[OIDS::AMP_SUMMARY], indexList);

    // Register diagnose requests. Each amp will have its own request.
    for(size_t i = 0; i < baseOids.size(); i++)
    {
        // Create a name.
        std::stringstream name;
        name << diagRequestName << i;
        snmp->createBulkRequest(name.str(), baseOids[i], noDiagNodes);
    }

    // Register updateVariables request.
    snmp->createRequest(upadateParamsName, transformToOids(oids[OIDS::AMP_ON], indexList));
}

Amplifiers::~Amplifiers()
{
    snmp->removeRequest(upadateParamsName);

    for(size_t i = 0; i < noAmplifiers; i++)
    {
        std::stringstream name;
        name << diagRequestName << i;
        snmp->removeRequest(name.str());
    }
}

void Amplifiers::diagnose(const std::vector<std::string>& summaryValues)
{
    States tempState = States::END_OF_STATE; // Needs to be updated.
    std::stringstream tempStatus;

    for(size_t i = 0; i < summaryValues.size(); i++)
    {
        try
        {
            States convertedValue = static_cast<States>(convertToValue<int32_t>(summaryValues[i]));

            // Check state of component.
            if(convertedValue < States::OK)
            {
                // Problematic component.

                // Are we OFF or actual problem?
                if(convertedValue != States::OFF)
                {
                    // Actual problem. We update the state only if it is worse than previous. WARN < FAULT < UNKNOWN
                    tempState = static_cast<States>(~(tempState <= convertedValue) & convertedValue);

                    // Name of request.
                    std::stringstream name;
                    name << diagRequestName << i;

                    // Read data.
                    std::vector<std::string> values = snmp->readRequest(name.str());

                    if(values.size() != noDiagNodes)
                    {
                        throw SNMPconnectorException(dataAcquisitionFailed);
                    }
                    // Get detailed status.

                    tempStatus << std::endl << i + 1 << " detailed status: " << std::endl
                               << "txAmpRfPowerFail: " << StatesText[convertToValue<int32_t>(values[0])] << std::endl
                               << "txAmpReflection: " << StatesText[convertToValue<int32_t>(values[1])] << std::endl
                               << "txAmpSupplyFail: " << StatesText[convertToValue<int32_t>(values[2])] << std::endl
                               << "txAmpRfInFail: " << StatesText[convertToValue<int32_t>(values[3])] << std::endl
                               << "txAmpMute: " << StatesText[convertToValue<int32_t>(values[4])] << std::endl
                               << "txAmpTemperatureFail: " << StatesText[convertToValue<int32_t>(values[5])] << std::endl
                               << "txAmpTransistorFail: " << StatesText[convertToValue<int32_t>(values[6])] << std::endl
                               << "txAmpRegulationFail: " << StatesText[convertToValue<int32_t>(values[7])] << std::endl
                               << "txAmpAcFail: " << StatesText[convertToValue<int32_t>(values[8])] << std::endl
                               << "txAmpDcFail: " << StatesText[convertToValue<int32_t>(values[9])] << std::endl
                               << "txAmpLink: " << StatesText[convertToValue<int32_t>(values[10])] << std::endl
                               << "txAmpBiasFail: " << StatesText[convertToValue<int32_t>(values[11])] << std::endl
                               << "txAmpInitFail: " << StatesText[convertToValue<int32_t>(values[12])] << std::endl
                               << "txAmpAbsorberFail: " << StatesText[convertToValue<int32_t>(values[13])] << std::endl
                               << "txAmpOn: " << StatesText[convertToValue<int32_t>(values[14])] << std::endl;
                }
                else
                {
                    // Device is OFF.
                    tempStatus << i + 1 << ": OFF" << std::endl;
                }
            }
            else
            {
                // Device is ON.
                tempStatus << i + 1 << ": ON" << std::endl;
            }
        }
        catch(const SNMPconnectorException& e)
        {
            tempState = States::UNKNOWN;
            tempStatus << std::endl << i + 1 << ": " << e.what() << std::endl;
        }
    }
    // In case state is still at END it means the worst state is OFF.
    if(tempState == States::END_OF_STATE)
    {
        tempState = States::OFF;
    }

    // Update status and state.
    setStateAndStatus(tempState, tempStatus.str());
}

void Amplifiers::updateReadParameters()
{
    try
    {
        std::vector<std::string> values = snmp->readRequest(upadateParamsName);

        if(values.size() != ampOn.size())
        {
            throw SNMPconnectorException(dataAcquisitionFailed);
        }

        // Convert to appropriate type and update.
        for(size_t i = 0; i < values.size(); i++)
        {
            ampOn[i] = convertToValue<int32_t>(values[i]);
        }
        dataValid = true;
    }
    catch(const SNMPconnectorException& e)
    {
        dataValid = false;
        throw e;
    }
}

bool Amplifiers::getAmpON(const uint16_t index)
{
    if(index >= noAmplifiers)
    {
        throw RfComponentException("Not that many amplifiers in the system.");
    }

    return States::OK == ampOn[index];
}
