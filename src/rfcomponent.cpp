#include "rfcomponent.h"
#include <sstream>

RFcomponent::RFcomponent(const std::vector<std::string>& summaryNodes, const std::string& componentName, const std::shared_ptr<SNMPconnector> snmp)
    : componentName(componentName), snmp(snmp)
{
    if(summaryNodes.size() < 1)
    {
        throw RfComponentException("No summary nodes provided for component: " + componentName);
    }

    snmp->createRequest(componentName, summaryNodes);
    numberOfSummaries = summaryNodes.size();

    // Initialize values
    state = States::UNKNOWN;
    dataValid = false;
}

RFcomponent::RFcomponent(const std::string& summaryNode, const std::string& componentName, const std::shared_ptr<SNMPconnector> snmp)
    : componentName(componentName), snmp(snmp)
{
    snmp->createRequest(componentName, std::vector<std::string>(1, summaryNode));
    numberOfSummaries = 1;

    // Initialize values
    state = States::UNKNOWN;
}

RFcomponent::~RFcomponent()
{
    snmp->removeRequest(componentName);
}

void RFcomponent::updateStateAndStatus()
{
    try
    {
        std::vector<std::string> values = snmp->readRequest(componentName);

        if(values.size() != numberOfSummaries)
        {
            throw SNMPconnectorException(dataAcquisitionFailed);
        }

        // Check for errors.
        for(size_t i = 0; i < values.size(); i++)
        {
            // Convert value to uint.
            int32_t value = convertToValue<int32_t>(values[i]);

            // At least one component is bad.
            if(value % States::OK != 0)
            {
                diagnose(values); // Do more deep investigation.
                return; // Diagnose is in charge of state and status in this case.
            }
        }

        // If we get here everything is fine.
        setStateAndStatus(States::OK, StatesText[States::OK]);

    }
    catch(const SNMPconnectorException& e)
    {
        // Problem with SNMP.
        setStateAndStatus(States::UNKNOWN, e.what());
    }
}

void RFcomponent::setStateAndStatus(const States newState, const std::string& newStatus)
{
    std::unique_lock<std::mutex> l(lock);
    state = newState;
    status = componentName + ": " + newStatus + "\n";
}

std::vector<std::string> RFcomponent::transformToOids(const std::string& baseOid, const std::vector<uint16_t>& indexList)
{
    std::vector<std::string> toReturn;

    for(const auto& value : indexList)
    {
        std::stringstream idx;
        if(!(idx << value))
        {
            throw RfComponentException("Failed to transform index value to string.");
        }

        size_t indexPos = baseOid.find_last_of('X'); // Find the X to replace. Usually at the end that is why last_of is used.

        // String operations.
        std::string temp;
        temp.reserve(baseOid.size() + idx.str().size() - 1);
        temp = baseOid.substr(0, indexPos); // Copy till X.
        temp += idx.str(); // Add correct number.
        temp += baseOid.substr(indexPos+1); // Copy the rest.

        toReturn.push_back(temp); // Push the finished string to vector.
    }
    return toReturn;
}
