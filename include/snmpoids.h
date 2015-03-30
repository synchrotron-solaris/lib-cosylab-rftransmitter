#ifndef SNMPOIDS_H
#define SNMPOIDS_H

#endif // SNMPOIDS_H

/**
 * @brief The OIDS enum Index of OID in oids array.
 */
enum OIDS
{
    AMP_SUMMARY = 0,
    LQC_SUMMARY,
    TRANS_SUMMARY,
    MTX_SUMMARY,
    OSTAGE_SUMMARY,
    RF_LINK,
    TRANS_RESET,
    MTX_RESET,
    NOMINAL_POWER,
    TRANS_FP,
    TRANS_RP,
    TRANS_PAE,
    OUT_POWER,
    RFS_FORWARD,
    RFS_REFLECTED,
    TRANS_ON,
    AMP_ON,
    LQ_TIN,
    LQ_TOUT,
    OIDS_LENGTH
};

/**
 * @brief The oids array Array of all OIDs used for communication. Some need to be tranformed before usage.
 */
static const std::string oids[OIDS_LENGTH] =
{
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.10.1.1.8.1.1.X.10000", // X will be replaced by the amp index.
    "1.3.6.1.4.1.2566.127.1.2.216.100.1.1.1.1.6.X.1", // X will be replaced by the lqc index.
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.1.1.1.6.1.1",
    "1.3.6.1.4.1.2566.127.1.2.216.4.1.1.1.5.100",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.9.1.1.7.1.1.9000",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.13.1.1.7.1.1.12000",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.1.2.1.1.1",
    "1.3.6.1.4.1.2566.127.1.2.216.4.1.4.2.1.0",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.1.2.1.3.1",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.1.3.1.1.1",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.1.3.1.2.1",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.1.3.1.6.1",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.9.3.1.1.1.1",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.13.2.1.14.1.1",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.13.2.1.15.1.1",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.1.2.1.2.1",
    "1.3.6.1.4.1.2566.127.1.2.216.3.1.10.1.1.8.1.1.X.10016", // X will be replaced by the amp index.
    "1.3.6.1.4.1.2566.127.1.2.216.100.1.1.2.1.2.X", // X will be replaced by the lq index.
    "1.3.6.1.4.1.2566.127.1.2.216.100.1.1.2.1.5.X" // X will be replaced by the lq index.
};
