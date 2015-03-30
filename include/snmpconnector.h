#ifndef SNMPCONNECTOR_H
#define SNMPCONNECTOR_H

#include "snmp_pp/snmp_pp.h"
#include <string>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

/**
 * @brief The SNMPconnectorException class Used to wrap return codes and throw SNMP exceptions.
 */
class SNMPconnectorException : public std::runtime_error
{
public:
    SNMPconnectorException(const std::string& description) : std::runtime_error(description) {}
};

/**
 * @brief The SNMPconnector class Wrapper around SNMP++ library. Exposing functions needed for sync reading and writting. Not thread safe.
 */
class SNMPconnector
{
public:
    /**
     * @brief SNMPconnector Constructor.
     * @param ip IP to connect to.
     * @param community READ and WRITE community to use.
     * @param port Port number.
     * @param timeout Timeout in ms. It will be rounded to 10ms precission.
     * @param retries Number of read retries to do before failing.
     */
    SNMPconnector(const std::string& ip, const std::string& community = "public", const uint16_t port = 161, const uint16_t timeout = 1000, const uint16_t retries = 1);
    ~SNMPconnector();

    /**
     * @brief createRequest Create a normal SNMP GET request.
     * @param name Name of the request. Used to execute the read method.
     * @param oids Collection of OIDs to read with SNMP GET command.
     */
    void createRequest(const std::string& name, const std::vector<std::string>& oids);

    /**
     * @brief createBulkRequest Creates a bulk request (reads SNMP data with GETNEXT command).
     * @param name Name of the request. Used to execute the read method.
     * @param oid OID where the request starts.
     * @param elements Number of elements to read.
     */
    void createBulkRequest(const std::string& name, const std::string& oid, const uint16_t elements);

    /**
     * @brief readRequest It reads the values that were registered as a request and returns them in a vector of strings in order as registered.
     * @param name Name of the request.
     * @param ignoreSyntaxErrors Trows if SNMP syntax errors are detected.
     * @return Values in a string format.
     */
    std::vector<std::string> readRequest(const std::string& name, const bool ignoreSyntaxErrors = true);

    /**
     * @brief Sets a value to the given OID.
     * @param oid OID to set value to.
     * @param value Desired value. It can be const char*, int32_t or uint32_t type.
     */
    template<typename T> void setValue(const std::string& oid, T value);

    /**
     * @brief removeRequest Removes the request from the map.
     * @param name Name of the request to remove.
     */
    void removeRequest(const std::string& name);


private:

    static const Snmp_pp::snmp_version version = Snmp_pp::version2c; //!< SNMPv2c
    static const uint16_t maxSize = 1500; //!< Max size of the UDP packet.

    std::unique_ptr<Snmp_pp::CTarget> cTarget; //!< Target for SNMPv2c messages.
    std::unique_ptr<Snmp_pp::Snmp> snmpSession; //!< SNMP session.
    std::unordered_map<std::string, std::pair<std::unique_ptr<Snmp_pp::Pdu>, uint16_t> > requests; //!< Map of all registered requests that the user can execute.

    void addToMap(const std::string& name, const uint16_t noElements = 0); /// Helper function for adding new requests to the map.
    std::vector<std::string> extractData(const int32_t status, const Snmp_pp::Pdu& pdu, const bool ignoreSyntaxErrors);
};

#endif // SNMPCONNECTOR_H
