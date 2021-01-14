// Copyright (c) Jason Joyner. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef OBD2_DTC_H
#define OBD2_DTC_H

#include <Arduino.h>

class OBD2_DTC_Class {
public:
    OBD2_DTC_Class();
    virtual ~OBD2_DTC_Class();

    int begin();
    void end();

    //Returns vehicle subsystem that Diagnostic Trouble Code belongs to
    char[] returnDTC_Subsystem(char DTC[4]);

    //Returns text description of Diagnostic Trouble Code, Returns 1 if DTC is not found
    char[] returnDTC_Description(char DTC[4]);
    
    //Clears all stored Diagnostic Trouble Codes, and turns off MIL 
    int clearAllStoredDTC();

private:
    
    int pidRead(uint8_t mode, uint8_t pid, void* data, int length);

private:
    unsigned long _responseTimeout;
    bool _useExtendedAddressing;
    unsigned long _lastPidResponseMillis;
    uint32_t _supportedPids[32];
    
};

extern OBD2_DTC_Class OBD2_DTC;

#endif