// Copyright (c) Jason Joyner. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

#include <CAN.h>
#include "OBD2_DTC.h"

/*==================================================================================
    CONSTRUCTORS & DESTRUCTORS
====================================================================================*/
OBD2_DTC_Class::OBD2_DTC_Class() :
    _responseTimeout(OBD2_DEFAULT_TIMEOUT),
    _lastPidResponseMillis(0)
{
    memset(_supportedPids, 0x00, sizeof(_supportedPids));
}

OBD2_DTC_Class::~OBD2_DTC_Class()
{
}

int OBD2_DTC_Class::begin()
{
    if (!CAN.begin(500E3)) {
        return 0;
    }//end if

    memset(_supportedPids, 0x00, sizeof(_supportedPids));

    // first try standard addressing
    _useExtendedAddressing = false;
    CAN.filter(0x7e8);
    if (!supportedPidsRead()) {
        // next try extended addressing
        _useExtendedAddressing = true;
        CAN.filterExtended(0x18daf110);

        if (!supportedPidsRead()) {
            return 0;
        }//end if
    }//end if

    return 1;
}   //end begin function

void OBD2_DTC_Class::end()
{
    CAN.end();
}   //end function



/*==================================================================================
    FAULT SUBSYSTEMS
====================================================================================*/
const char P00xx[] PROGMEM = "Fuel & Air Metering & Aux. Controls";
const char P01xx[] PROGMEM = "Fuel & Air Metering";
const char P02xx[] PROGMEM = "Fuel & Air Metering (Injector Circuit)";
const char P03xx[] PROGMEM = "Ignition System & Misfire";
const char P04xx[] PROGMEM = "Auxilary Emissions Controls";
const char P05xx[] PROGMEM = "Vehicle Speed Controls and Idle Control System";
const char P06xx[] PROGMEM = "Computer Output Circuit";
const char P07_09xx[] PROGMEM = "Transmission";                                         //P07, P08, and P09 all refer to the transmission
const char C00xx[] PROGMEM = "Chassis (includes ABS)";
const char B00xx[] PROGMEM = "Body (includes A/C & Airbag)";
const char U00xx[] PROGMEM = "Network";

char[] OBD2_DTC_Class::returnDTC_Subsystem(char DTC[4]) {
    //Pull subsystem prefix from trouble code
    char prefix[3] = { DTC[0], DTC[1], DTC[3] };

    switch (prefix[3]) {
        case 'P00' :
            return P00xx[];
            break;

        case 'P01':
            return P01xx[];
            break;

        case 'P02':
            return P02xx[];
            break;

        case 'P03':
            return P03xx[];
            break;

        case 'P04':
            return P04xx[];
            break;

        case 'P05':
            return P05xx[];
            break;

        case 'P06':
            return P06xx[];
            break;

        case 'P07':
            return P07_09xx[];
            break;

        case 'P08':
            return P07_09xx[];
            break;

        case 'P09':
            return P07_09xx[];
            break;

        case 'C00':
            return C00xx[];
            break;

        case 'B00':
            return B00xx[];
            break;

        case 'U08':
            return U00xx[];
            break;
    }   //end switch
};  //end returnDTC_Subsystem function

/*==================================================================================
    DTC Descriptions for Fuel & Air Metering & Aux. Controls - Prefix P00
====================================================================================*/
const char P0001[] PROGMEM = "Fuel Volume Regulator Control Circuit / Open";
const char P0002[] PROGMEM = "Fuel Volume Regulator Control Circuit Range Performance";
const char P0003[] PROGMEM = "Fuel Volume Regulator Control Circuit Low";
const char P0004[] PROGMEM = "Fuel Volume Regulator Control Circuit High";
const char P0005[] PROGMEM = "Fuel Shutoff Valve A Control Circuit / Open";
const char P0006[] PROGMEM = "Fuel Shutoff Valve A Control Circuit Low";
const char P0007[] PROGMEM = "Fuel Shutoff Valve A Control Circuit High";
const char P0008[] PROGMEM = "Engine Position System Performance Bank 1";
const char P0009[] PROGMEM = "Engine Position System Performance Bank 2";
const char P000A[] PROGMEM = "A Camshaft Position Slow Response Bank 1";
const char P000B[] PROGMEM = "B Camshaft Position Slow Response Bank 1";
const char P000C[] PROGMEM = "A Camshaft Position Slow Response Bank 2";
const char P000D[] PROGMEM = "B Camshaft Position Slow Response Bank 2";

char[] OBD2_DTC_Class::returnDTC_Description(char DTC[4])
{
    //P00XX Fuel & Air Metering Controls
    if (DTC[4] == 'P0001') { return P0001[] };
    if (DTC[4] == 'P0002') { return P0002[] };
    if (DTC[4] == 'P0003') { return P0003[] };
    if (DTC[4] == 'P0004') { return P0004[] };
    if (DTC[4] == 'P0005') { return P0005[] };
    if (DTC[4] == 'P0006') { return P0006[] };
    if (DTC[4] == 'P0007') { return P0007[] };
    if (DTC[4] == 'P0008') { return P0008[] };
    if (DTC[4] == 'P0009') { return P0009[] };
    if (DTC[4] == 'P000A') { return P000A[] };
    if (DTC[4] == 'P000B') { return P000B[] };
    if (DTC[4] == 'P000C') { return P000C[] };
    if (DTC[4] == 'P000D') { return P000D[] };


    else {
        return 1;
    };  //end else
    
}   //end returnDTC_Description function

/*==================================================================================
    Clear & Read DTC from CAN bus
====================================================================================*/
int OBD2_DTC_Class::clearAllStoredDTC() {
    //Function clears stored Diagnostic Trouble Codes (DTC)

    // make sure at least 60 ms have passed since the last response
    unsigned long lastResponseDelta = millis() - _lastPidResponseMillis;
    if (lastResponseDelta < 60) {
        delay(60 - lastResponseDelta);
    }

    for (int retries = 10; retries > 0; retries--) {
        if (_useExtendedAddressing) {
            CAN.beginExtendedPacket(0x18db33f1, 8);
        }
        else {
            CAN.beginPacket(0x7df, 8);
        }
        CAN.write(0x00); // number of additional bytes
        CAN.write(0x04); // Mode / Service 4, for clearing DTC
        if (CAN.endPacket()) {
            // send success
            break;
        }
        else if (retries <= 1) {
            return 0;
        }
    }

    return 0;
}   //end clearAllStoredDTC function

int OBD2_DTC_Class::pidRead(uint8_t mode, uint8_t pid, void* data, int length)
{
    {
        // make sure at least 60 ms have passed since the last response
        unsigned long lastResponseDelta = millis() - _lastPidResponseMillis;
        if (lastResponseDelta < 60) {
            delay(60 - lastResponseDelta);
        }

        for (int retries = 10; retries > 0; retries--) {
            if (_useExtendedAddressing) {
                CAN.beginExtendedPacket(0x18db33f1, 8);
            }
            else {
                CAN.beginPacket(0x7df, 8);
            }
            CAN.write(0x02); // number of additional bytes
            CAN.write(mode);
            CAN.write(pid);
            if (CAN.endPacket()) {
                // send success
                break;
            }
            else if (retries <= 1) {
                return 0;
            }
        }

        bool splitResponse = (length > 5);

        for (unsigned long start = millis(); (millis() - start) < _responseTimeout;) {
            if (CAN.parsePacket() != 0 &&
                (splitResponse ? (CAN.read() == 0x10 && CAN.read()) : CAN.read()) &&
                (CAN.read() == (mode | 0x40) && CAN.read() == pid)) {

                _lastPidResponseMillis = millis();

                // got a response
                if (!splitResponse) {
                    return CAN.readBytes((uint8_t*)data, length);
                }

                int read = CAN.readBytes((uint8_t*)data, 3);

                for (int i = 0; read < length; i++) {
                    delay(60);

                    // send the request for the next chunk
                    if (_useExtendedAddressing) {
                        CAN.beginExtendedPacket(0x18db33f1, 8);
                    }
                    else {
                        CAN.beginPacket(0x7df, 8);
                    }
                    CAN.write(0x30);
                    CAN.endPacket();

                    // wait for response
                    while (CAN.parsePacket() == 0 ||
                        CAN.read() != (0x21 + i)); // correct sequence number

                    while (CAN.available()) {
                        ((uint8_t*)data)[read++] = CAN.read();
                    }
                }

                _lastPidResponseMillis = millis();

                return read;
            }
        }

        return 0;
    }
}

OBD2_DTC_Class OBD2_DTC;
