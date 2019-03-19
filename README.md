Encephalan Driver for OpenVibe system
=====================================
> Actual for version OpenVibe 2.2.0

> Compatible only with Windows

This driver allows you to receive data from Encephalan software for OpenVibe system.

Registering the driver in system
-------------------
1. [Get](http://openvibe.inria.fr/repo-instructions/) OpenVibe system
2. Put this driver to folder ***meta/extras/contrib/plugins/server-drivers/***
    - Get an anonymous clone of the repository ***git clone https://github.com/AlexeyMinin/encephalan-driver.git***
    - Or simply download this repository
3. Edit ***meta/extras/contrib/common/contribAcquisitionServer.cmake***. This script should make your driver visible to the build. Add the line:
    ```C++
    OV_ADD_CONTRIB_DRIVER("${CMAKE_SOURCE_DIR}/contrib/plugins/server-drivers/encephalan-driver")
    ```
4. Edit ***meta/extras/contrib/common/contribAcquisitionServer.inl***. This inline file is compiled with Acquisition Server and registers the driver.
    - Add the line:
    ```C++
    #include "ovasCDriverEncephalan.h"
    ```
    - And the lines to the function **initiateContributions**:
    ```C++
    #if defined TARGET_OS_Windows
		    vDriver->push_back(new OpenViBEAcquisitionServer::CDriverEncephalan(pAcquisitionServer->getDriverContext()));
    #endif
    ```
6. [Build](http://openvibe.inria.fr/build-instructions/) OpenVibe system
