#include "snmpconnector.h"
#include <sstream>
#include <iostream>


SNMPconnector::SNMPconnector(const std::string& ip, const std::string& community, const uint16_t port, const uint16_t timeout, const uint16_t retries)
{
    // Start the socket resource acquisition.
    //Snmp_pp::Snmp::socket_startup(); WIN Only

    // Set IP and port.
    Snmp_pp::UdpAddress address(ip.c_str());
    address.set_port(port);

    // We have the same community for READ and WRITE.
    Snmp_pp::OctetStr comm(community.c_str());

    int32_t status;

    // Start SNMP session. Use IPv6 if needed.
    snmpSession.reset(new Snmp_pp::Snmp(status, 0, (address.get_ip_version() == Snmp_pp::Address::version_ipv6)));

    if (status != SNMP_CLASS_SUCCESS) // Any ERRORs?
    {
        throw SNMPconnectorException(snmpSession->error_msg(status));
    }

    // Create a target for SNMP requests.
    cTarget.reset(new Snmp_pp::CTarget(address));

    cTarget->set_version(version);
    cTarget->set_retry(retries);
    cTarget->set_timeout(timeout/10); // Timeout goes in 10ms increments. So for 1000ms we actually write 100.
    cTarget->set_readcommunity(comm);
    cTarget->set_writecommunity(comm);

    // Log only errors and warrnings.
    Snmp_pp::DefaultLog::log()->set_filter(ERROR_LOG, 2);
    Snmp_pp::DefaultLog::log()->set_filter(WARNING_LOG, 1);
    Snmp_pp::DefaultLog::log()->set_filter(EVENT_LOG, 0);
    Snmp_pp::DefaultLog::log()->set_filter(INFO_LOG, 0);
    Snmp_pp::DefaultLog::log()->set_filter(DEBUG_LOG, 0);
}

SNMPconnector::~SNMPconnector()
{
    // Clean the socket.
    //Snmp_pp::Snmp::socket_cleanup(); WIN Only
}

void SNMPconnector::createRequest(const std::string& name, const std::vector<std::string>& oids)
{
    // Check if at least one OID is specified.
    if(oids.size() < 1)
    {
        throw SNMPconnectorException("At least 1 oid needs to be specified.");
    }

    addToMap(name);

    std::vector<Snmp_pp::Vb> VBs;
    VBs.reserve(oids.size()); // We know the number of VBs in advance.

    // Each OID gets its own VB.
    for(size_t i = 0; i < oids.size(); i++)
    {
        VBs.push_back(Snmp_pp::Vb(Snmp_pp::Oid(oids[i].c_str())));
    }

    if(0 == requests.at(name).first->set_vblist(&VBs.at(0), VBs.size())) // Add whole list at once to PDU. Check for success.
    {
        requests.erase(name);
        throw SNMPconnectorException("Failed to add VBs to PDU.");
    }
}

void SNMPconnector::createBulkRequest(const std::string& name, const std::string& oid, const uint16_t elements)
{
    addToMap(name, elements);

    // Add VB to PDU.
    *requests.at(name).first += Snmp_pp::Vb(Snmp_pp::Oid(oid.c_str()));
}

std::vector<std::string> SNMPconnector::readRequest(const std::string& name, const bool ignoreSyntaxErrors)
{
    // Check if the request exists.
    if(requests.find(name) == requests.end())
    {
        throw SNMPconnectorException("Request with this name does not exist.");
    }

    std::vector<std::string> toReturn;

    // Bulk request?
    if(requests.at(name).second > 0)
    {
        Snmp_pp::Vb tempVB;
        requests.at(name).first->get_vb(tempVB, 0); // Remember the base.

        toReturn = extractData(snmpSession->get_bulk(*requests.at(name).first, *cTarget, 0, requests.at(name).second), *requests.at(name).first, ignoreSyntaxErrors);

        requests.at(name).first->clear(); // Clear all the returns.
        *requests.at(name).first += tempVB; // Make a single base again.
    }
    else
    {        
        toReturn = extractData(snmpSession->get(*requests.at(name).first, *cTarget), *requests.at(name).first, ignoreSyntaxErrors);
    }

    return toReturn;
}

void SNMPconnector::removeRequest(const std::string& name)
{
    requests.erase(name);
}

void SNMPconnector::addToMap(const std::string& name, const uint16_t noElements)
{
    // Check if the same reqeust already exists.
    if(requests.find(name) != requests.end())
    {
        throw SNMPconnectorException("Request with this name is already registered.");
    }

    // Add to collection of request with a given name.
    requests.insert(std::pair<std::string, std::pair<std::shared_ptr<Snmp_pp::Pdu>, uint16_t> >
                    (name, std::pair<std::shared_ptr<Snmp_pp::Pdu>, uint16_t>(std::shared_ptr<Snmp_pp::Pdu>(new Snmp_pp::Pdu), noElements)));
}

std::vector<std::string> SNMPconnector::extractData(const int32_t status, const Snmp_pp::Pdu& pdu, const bool ignoreSyntaxErrors)
{
    if(status != SNMP_CLASS_SUCCESS) // Any ERRORs?
    {
        throw SNMPconnectorException(snmpSession->error_msg(status));
    }

    // Create return vector of strings.
    std::vector<std::string> toReturn;
    std::stringstream errorMsg;

    // Go through all VBs in PDU, check for error msg and extract data if possible.
    for(size_t i = 0; i < static_cast<size_t>(pdu.get_vb_count()); i++)
    {
        Snmp_pp::Vb tempVB;
        pdu.get_vb(tempVB, i);

        toReturn.push_back(tempVB.get_printable_value());

        if(!ignoreSyntaxErrors && ((tempVB.get_syntax() == sNMP_SYNTAX_ENDOFMIBVIEW) ||
            (tempVB.get_syntax() == sNMP_SYNTAX_NOSUCHINSTANCE) ||
            (tempVB.get_syntax() == sNMP_SYNTAX_NOSUCHOBJECT)))
        {
            errorMsg << "On VB with OID: " << tempVB.get_printable_oid() << " error occured: " << tempVB.get_syntax() << std::endl;
        }
    }

    if(errorMsg.str().length() > 0)
    {
        throw SNMPconnectorException(errorMsg.str());
    }

    return toReturn;
}

template<typename T>
void SNMPconnector::setValue(const std::string& oid, T value)
{
    // Create PDU and VB.
    Snmp_pp::Pdu pdu;
    Snmp_pp::Vb vb;
    vb.set_oid(oid.c_str());

    vb.set_value(value);
    pdu += vb;

    int32_t status = snmpSession->set(pdu, *cTarget); // Set value. We need the status variable for error_msg extraction.
    if(status != SNMP_CLASS_SUCCESS) // Any ERRORs?
    {
        throw SNMPconnectorException(snmpSession->error_msg(status));
    }

}
// Only allow these 3 types.
template void SNMPconnector::setValue<const char*>(const std::string& oid, const char* value);
template void SNMPconnector::setValue<int32_t>(const std::string& oid, int32_t value);
template void SNMPconnector::setValue<uint32_t>(const std::string& oid, uint32_t value);
