RFtransmitter Library
==========================

Library for the RFtransmitter (used by ds-cosylab-rftransmitter).

Cloning this repository
-----------------------

This repository is used by ds-cosylab-rftransmitter.

This repository depends on the SNMP++-devel yum package.

Clone this repository and install the package in order to build it:

- Build instructions are provided in documentation
- SNMP++ library package is provided inside lib-cosylab-rftransmitter

Building the RFtransmitter library
-------------------------------------

Prerequisites:

- compilation must happen on CentOS

In order to produce the static library for the device server:

    cd lib-cosylab-rftransmitter
    make static


