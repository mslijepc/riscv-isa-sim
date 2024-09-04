// Copyright (c) 2023 SiFive, Inc. -- Proprietary and Confidential
// All Rights Reserved.
//
// NOTICE: All information contained herein is, and remains the
// property of SiFive, Inc. The intellectual and technical concepts
// contained herein are proprietary to SiFive, Inc. and may be covered
// by U.S. and Foreign Patents, patents in process, and are protected by
// trade secret or copyright law.
//
// This work may not be copied, modified, re-published, uploaded,
// executed, or distributed in any way, in any medium, whether in whole
// or in part, without prior written permission from SiFive, Inc.
//
// The copyright notice above does not evidence any actual or intended
// publication or disclosure of this source code, which includes
// information that is confidential and/or proprietary, and is a trade
// secret, of SiFive, Inc.


#ifndef ZMQ_H
#define ZMQ_H
#include <iostream>
#include <cstdint>
#include <cstddef>


struct zmq_data {
    static constexpr uint32_t DATA_SZ = 4096; 

    uint8_t id;
    enum : uint8_t {
        CLK = 0x01, // from SC
        END_CLK = 0x02,    // from JS

        TLM_RD = 0x03,      // from JS
        TLM_RD_DONE = 0x04, // from SC
        TLM_WR  = 0x05,      // from JS
        TLM_WR_DONE = 0x06, // from SC

        PORT_RD = 0x07,      // from JS
        PORT_WR = 0x08,      // from JS
        PORT_RD_DONE = 0x09, // from SC
        PORT_WR_DONE = 0x0A, // from SC

        END_SIM = 0x0B,   // from JS

        MEM_RD = 0x0C,         // from JS
        MEM_RD_DONE = 0x0D,    // from SC
        MEM_WR = 0x0E,         // from JS
        MEM_WR_DONE = 0x0F,    // from SC

        DBG_F = 0x10, // from JS // not used for now
        ERROR = 0x11,  // from SC
        END_JS = 0x12,  // from JS
        SET_PC_TRIG_32 = 0x13, 
        SET_OPC_TRIG_MASK_32 = 0x14, 
        TRIGGERED = 0x15, 
        SET_PC_TRIG_64 = 0x16, 
        SET_OPC_TRIG_MASK_64 = 0x17, 
        GET_INFO = 0x18,
        SEND_INFO = 0x19,
        SET_PORT_VALID = 0x1A,
        SET_PORT_VALID_DONE = 0x1B
    } command;
    uint16_t data_len{1};
    uint8_t errResp{0};
    uint64_t addr;
    uint8_t used_data[DATA_SZ];
};
    std::string e_to_string(uint8_t val) {
        switch(val) {
        case (0x01):
            return "CLK";
            break;
        case (0x02):
            return "END_CLK";
            break;
        case (0x03):
            return "TLM_RD";
            break;
        case (0x04):
            return "TLM_RD_DONE";
            break;
        case (0x05):
            return "TLM_WR";
            break;
        case (0x06):
            return "TLM_WR_DONE";
            break;
        case (0x07):
            return "PORT_RD";
            break;
        case (0x08):
            return "PORT_WR";
            break;
        case (0x09):
            return "PORT_RD_DONE";
            break;
        case (0x0A):
            return "PORT_WR_DONE";
            break;
        case (0x0B):
            return "END_SIM";
            break;
        case (0x0C):
            return "MEM_RD";
            break;
        case (0x0D):
            return "MEM_RD_DONE";
            break;
        case (0x0E):
            return "MEM_WR";
            break;
        case (0x0F):
            return "MEM_WR_DONE";
            break;
        case (0x10):
            return "DBG_F";
            break;
        case (0x11):
            return "ERROR";
            break;
        case (0x12):
            return "END_JS";
            break;
        case (0x13):
            return "SET_PC_TRIG_32";
            break;
        case (0x14):
            return "SET_OPC_TRIG_MASK_32";
            break;
        case (0x15):
            return "TRIGGERED";
            break;
        case (0x16):
            return "SET_PC_TRIG_64";
            break;
        case (0x17):
            return "SET_OPC_TRIG_MASK_64";
            break;
        case (0x18):
            return "GET_INFO";
            break;
        case (0x19):
            return "SEND_INFO";
            break;
        default:
            return "SMTH WRONG";
        }
    }
    // ostream& operator<<(ostream& os, zmq_data& d) {
    //     return os << "ID: " << std::dec << static_cast<int>(d.id) << " CMD: " << e_to_string(d.command)
    //               << " data_len " << d.data_len << " addr: 0x" << hex << d.addr
    //               << endl; //<< " data: 0x" << d.used_data << endl;
    // }

#endif