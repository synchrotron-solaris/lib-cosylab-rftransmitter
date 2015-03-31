#ifndef RFCOMPONENT_H
#define RFCOMPONENT_H

#include <string>
#include <mutex>
#include "snmpconnector.h"
#include "snmpoids.h"
#include <sstream>
#include <cstdatomic>

class RfComponentException : public std::runtime_error
{
public:
    RfComponentException(const std::string& description) : std::runtime_error(description) {}
};

/**
 *  Helper function to convert string to value.
 */
template<typename T> T convertToValue(const std::string& value)
{
    // Convert value.
    T converted;
    std::istringstream toConvert(value);
    if(!(toConvert >> converted))
    {
        throw SNMPconnectorException("Failed to parse SNMP values.");
    }
    return converted;
}

/**
 * @brief The States enum Possible states returned from SNMP agent.
 */
enum States
{
    NOT_POSSIBLE = 0,
    UNKNOWN,
    OFF,
    FAULT,
    WARNING,
    OK,
    END_OF_STATE
};


static const std::string StatesText[States::END_OF_STATE + 1] = //!< Helper to transform enum to text.
{
    "DOES NOT EXIST",
    "UNKNOWN",
    "OFF",
    "FAULT",
    "WARNING",    
    "OK",
    "END OF POSSIBLE STATES"
};


static const std::string dataAcquisitionFailed = "SNMP data acquisition failed."; //!< Unified message for SNMP error.

class RFcomponent
{    

public:    

    /**
     * @brief transformToOids Some OIDS have X inside. That X is replaced with appropriate index number.
     * @param baseOid Based OID with X inside.
     * @param indexList List of all X values to be replaced.
     * @return Vector with proper OIDs.
     */
    static std::vector<std::string> transformToOids(const std::string& baseOid, const std::vector<uint16_t>& indexList);

    /**
     * @brief RFcomponent Constructor for components with multiple summary nodes.
     * @param summaryNodes Vector of OIDs of all summary nodes to be used in order to determine state.
     * @param componentName Name of the component.
     * @param snmp SNMP connection used for reading.
     */
    RFcomponent(const std::vector<std::string>& summaryNodes, const std::string& componentName, const std::shared_ptr<SNMPconnector> snmp);

    /**
     * @brief RFcomponent Constructor for components that have only one summary node.
     * @param summaryNode OID for summary value to be used for state.
     * @param componentName Name of the component.
     * @param snmp SNMP connection used for reading.
     */
    RFcomponent(const std::string& summaryNode, const std::string& componentName, const std::shared_ptr<SNMPconnector> snmp);

    virtual ~RFcomponent();
    /**
     * @brief diagnose Used to diagnose non OK states.
     * @param summaryValues Values read from SNMP agent.
     */
    virtual void diagnose(const std::vector<std::string>& summaryValues) = 0;

    /**
     * @brief updateReadParameters Reads and updates component parameters.
     */
    virtual void updateReadParameters() = 0;

    /**
     * @brief updateStateAndStatus Default function to update state and status or trigger diagnose.
     */
    virtual void updateStateAndStatus();

    /**
     * @brief getStatus Returns the status.
     * @return Status in string format.
     */
    inline std::string getStatus()
    {
        std::unique_lock<std::mutex> l(lock);
        return status;
    }

    /**
     * @brief getState Returns the state.
     * @return State as enumeration.
     */
    inline States getState()
    {
        std::unique_lock<std::mutex> l(lock);
        return state;
    }

    /**
     * @brief getComponentName Returns the name of the component.
     * @return Name in string format.
     */
    inline std::string getComponentName() { return componentName; }

    /**
     * @brief getDataValid Can we trust internal parameter data?
     * @return Yes or No.
     */
    inline bool getDataValid() { return dataValid; }

protected:
    std::string componentName; //!< Name of the component.
    std::shared_ptr<SNMPconnector> snmp; //!< Connection used for reading.
    std::string status; //!< Current status message.
    States state; //!< Current state.
    std::mutex lock; //!< Mutex for parameters reading and updating.
    uint16_t numberOfSummaries; //!< Number of summary nodes used.
    std::atomic<bool> dataValid; //!< Is the internal parameter data valid.

    void setStateAndStatus(const States newState, const std::string& newStatus);
};




#endif // RFCOMPONENT_H
