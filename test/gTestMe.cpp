#include "gtest/gtest.h"
#include "RFinclude.h"
#include <iostream>

std::string IP = "";

// The fixture for testing class SNMPconnector.
class SNMP : public ::testing::Test
{
protected:

    SNMP()
    {
        connected = false;
    }

    virtual ~SNMP()
    {
        usleep(100000); // Do delay so we can't overstrees the hardware.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    virtual void SetUp()
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
        try
        {
            conn.reset(new SNMPconnector(IP, "public"));
            connected = true;
        }
        catch(const SNMPconnectorException& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    virtual void TearDown()
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    std::unique_ptr<SNMPconnector> conn;
    bool connected;
};

// The fixture for testing class MTx.
class MTX : public ::testing::Test
{
protected:

    MTX()
    {
        connected = false;
    }

    virtual ~MTX()
    {
        usleep(100000); // Do delay so we can't overstrees the hardware.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    virtual void SetUp()
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
        try
        {
            conn.reset(new SNMPconnector(IP, "public"));
            connW.reset(new SNMPconnector(IP, "public"));
            mtx.reset(new MTx(conn, connW));
            connected = true;
        }
        catch(const SNMPconnectorException& e)
        {
            std::cout << e.what() << std::endl;
        }
        catch(const RfComponentException& e2)
        {
            std::cout << e2.what() << std::endl;
        }
    }

    virtual void TearDown()
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    std::shared_ptr<SNMPconnector> conn;
    std::shared_ptr<SNMPconnector> connW;
    std::unique_ptr<MTx> mtx;
    bool connected;
};

// The fixture for testing class OutStage.
class OSTAGE : public ::testing::Test
{
protected:

    OSTAGE()
    {
        connected = false;
    }

    virtual ~OSTAGE()
    {
        usleep(100000); // Do delay so we can't overstrees the hardware.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    virtual void SetUp()
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
        try
        {
            conn.reset(new SNMPconnector(IP, "public"));
            connW.reset(new SNMPconnector(IP, "public"));
            oStage.reset(new OutStage(conn, connW));
            connected = true;
        }
        catch(const SNMPconnectorException& e)
        {
            std::cout << e.what() << std::endl;
        }
        catch(const RfComponentException& e2)
        {
            std::cout << e2.what() << std::endl;
        }
    }

    virtual void TearDown()
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    std::shared_ptr<SNMPconnector> conn;
    std::shared_ptr<SNMPconnector> connW;
    std::unique_ptr<OutStage> oStage;
    bool connected;
};

// The fixture for testing class RFsensor.
class RFs : public ::testing::Test
{
protected:

    RFs()
    {
        connected = false;
    }

    virtual ~RFs()
    {
        usleep(100000); // Do delay so we can't overstrees the hardware.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    virtual void SetUp()
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
        try
        {
            conn.reset(new SNMPconnector(IP, "Public"));
            rfSensor.reset(new RFsensor(conn));
            connected = true;
        }
        catch(const SNMPconnectorException& e)
        {
            std::cout << e.what() << std::endl;
        }
        catch(const RfComponentException& e2)
        {
            std::cout << e2.what() << std::endl;
        }
    }

    virtual void TearDown()
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    std::shared_ptr<SNMPconnector> conn;
    std::unique_ptr<RFsensor> rfSensor;
    bool connected;
};

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    if(argc != 2)
    {
        std::cout << "Wrong usage!" << std::endl;
        std::cout << "Execute the program with 1 argument: IP address" << std::endl;
        std::cout << "Example: ./runTests 10.5.2.88" << std::endl;
        return -1;
    }
    IP = argv[1];    

    return RUN_ALL_TESTS();
}
/*
TEST_F(SNMP, ConnectDisconnect)
{    
    ASSERT_TRUE(connected);
}

TEST_F(SNMP, CreateRequest)
{
    ASSERT_TRUE(connected);

    std::vector<std::string> myOIDS;
    myOIDS.push_back("1.3.6.1.2.1.1.1.0");
    myOIDS.push_back("1.3.6.1.2.1.1.2.0");
    myOIDS.push_back("1.3.6.1.2.1.1.3.0");
    ASSERT_NO_THROW(conn->createRequest("MyRequest", myOIDS));
    ASSERT_NO_THROW(conn->createRequest("MyRequest2", myOIDS));
}

TEST_F(SNMP, ReadRequest)
{
    ASSERT_TRUE(connected);

    std::vector<std::string> myOIDS;
    myOIDS.push_back("1.3.6.1.2.1.1.1.0");
    myOIDS.push_back("1.3.6.1.2.1.1.2.0");

    std::string requestName("MyRequest");
    ASSERT_NO_THROW(conn->createRequest(requestName, myOIDS));
    ASSERT_NO_THROW(conn->readRequest(requestName));
}

TEST_F(SNMP, SetStringValue)
{    
    ASSERT_TRUE(connected);

    std::string value("NOWTEST");
    ASSERT_NO_THROW(conn->setValue("1.3.6.1.2.1.1.4.0", value.c_str()));
}

TEST_F(SNMP, SetStringValue2)
{    
    ASSERT_TRUE(connected);

    std::string value("TEST");
    std::vector<std::string> myOIDS;

    std::string requestName("MyRequest");
    myOIDS.push_back("1.3.6.1.2.1.1.4.0");

    ASSERT_NO_THROW(conn->createRequest(requestName, myOIDS));
    ASSERT_NO_THROW(conn->setValue(myOIDS[0], value.c_str()));

    std::vector<std::string> returnValues;
    ASSERT_NO_THROW(returnValues = conn->readRequest(requestName));
    EXPECT_STREQ(value.c_str(), returnValues.at(0).c_str());
}

TEST_F(SNMP, BulkRequest)
{
    ASSERT_TRUE(connected);

    std::string myOID("1.3.6.1.2.1.1.1.0");
    std::string requestName("MyRequest");

    conn->createBulkRequest(requestName, myOID, 2);
    ASSERT_NO_THROW(conn->readRequest(requestName));
}

TEST_F(SNMP, RemoveRequest)
{
    ASSERT_TRUE(connected);

    std::vector<std::string> myOIDS;
    myOIDS.push_back("1.3.6.1.2.1.1.1.0");

    std::string requestName("MyRequest");
    conn->createRequest(requestName, myOIDS);
    conn->removeRequest(requestName);
    ASSERT_THROW(conn->readRequest(requestName), SNMPconnectorException);
}
*/
TEST_F(MTX, Create)
{
    ASSERT_TRUE(connected);
}

TEST_F(MTX, UpdateStateAndStatus)
{
    ASSERT_TRUE(connected);

    ASSERT_NO_THROW(mtx->updateStateAndStatus());
    ASSERT_LT(mtx->getState(), States::END_OF_STATE);
}
/*
TEST_F(MTX, Reset)
{
    ASSERT_TRUE(connected);

    mtx->reset();
}
*/
TEST_F(OSTAGE, Create)
{
    ASSERT_TRUE(connected);
}

TEST_F(OSTAGE, UpdateStateAndStatus)
{
    ASSERT_TRUE(connected);

    ASSERT_NO_THROW(oStage->updateStateAndStatus());
    ASSERT_LT(oStage->getState(), States::END_OF_STATE);
}

TEST_F(OSTAGE, UpdateReadParameters)
{
    ASSERT_TRUE(connected);

    try
    {
        oStage->updateReadParameters();
    }
    catch(const SNMPconnectorException& e)
    {
        std::cout << e.what() << std::endl;
    }


    ASSERT_GE(oStage->getPower(), 0);
}
/*
TEST_F(OSTAGE, SetGetPower) // Dangerous so we set it to 0.
{
    ASSERT_TRUE(connected);

    ASSERT_NO_THROW(oStage->setPower(0));
}
*/

