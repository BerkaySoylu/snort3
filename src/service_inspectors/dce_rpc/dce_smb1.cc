//--------------------------------------------------------------------------
// Copyright (C) 2020-2021 Cisco and/or its affiliates. All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------

// dce_smb1.cc author Bhargava Jandhyala <bjandhya@cisco.com>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dce_smb1.h"

#include "memory/memory_cap.h"

#include "dce_smb_utils.h"

using namespace snort;

//-------------------------------------------------------------------------
// debug stuff
//-------------------------------------------------------------------------

#ifdef DEBUG_MSGS
static const char* smb_com_strings[SMB_MAX_NUM_COMS] =
{
    "Create Directory",            // 0x00
    "Delete Directory",            // 0x01
    "Open",                        // 0x02
    "Create",                      // 0x03
    "Close",                       // 0x04
    "Flush",                       // 0x05
    "Delete",                      // 0x06
    "Rename",                      // 0x07
    "Query Information",           // 0x08
    "Set Information",             // 0x09
    "Read",                        // 0x0A
    "Write",                       // 0x0B
    "Lock Byte Range",             // 0x0C
    "Unlock Byte Range",           // 0x0D
    "Create Temporary",            // 0x0E
    "Create New",                  // 0x0F
    "Check Directory",             // 0x10
    "Process Exit",                // 0x11
    "Seek",                        // 0x12
    "Lock And Read",               // 0x13
    "Write And Unlock",            // 0x14
    "Unknown",                     // 0X15
    "Unknown",                     // 0X16
    "Unknown",                     // 0X17
    "Unknown",                     // 0X18
    "Unknown",                     // 0X19
    "Read Raw",                    // 0x1A
    "Read Mpx",                    // 0x1B
    "Read Mpx Secondary",          // 0x1C
    "Write Raw",                   // 0x1D
    "Write Mpx",                   // 0x1E
    "Write Mpx Secondary",         // 0x1F
    "Write Complete",              // 0x20
    "Query Server",                // 0x21
    "Set Information2",            // 0x22
    "Query Information2",          // 0x23
    "Locking AndX",                // 0x24
    "Transaction",                 // 0x25
    "Transaction Secondary",       // 0x26
    "Ioctl",                       // 0x27
    "Ioctl Secondary",             // 0x28
    "Copy",                        // 0x29
    "Move",                        // 0x2A
    "Echo",                        // 0x2B
    "Write And Close",             // 0x2C
    "Open AndX",                   // 0x2D
    "Read AndX",                   // 0x2E
    "Write AndX",                  // 0x2F
    "New File Size",               // 0x30
    "Close And Tree Disc",         // 0x31
    "Transaction2",                // 0x32
    "Transaction2 Secondary",      // 0x33
    "Find Close2",                 // 0x34
    "Find Notify Close",           // 0x35
    "Unknown",                     // 0X36
    "Unknown",                     // 0X37
    "Unknown",                     // 0X38
    "Unknown",                     // 0X39
    "Unknown",                     // 0X3A
    "Unknown",                     // 0X3B
    "Unknown",                     // 0X3C
    "Unknown",                     // 0X3D
    "Unknown",                     // 0X3E
    "Unknown",                     // 0X3F
    "Unknown",                     // 0X40
    "Unknown",                     // 0X41
    "Unknown",                     // 0X42
    "Unknown",                     // 0X43
    "Unknown",                     // 0X44
    "Unknown",                     // 0X45
    "Unknown",                     // 0X46
    "Unknown",                     // 0X47
    "Unknown",                     // 0X48
    "Unknown",                     // 0X49
    "Unknown",                     // 0X4A
    "Unknown",                     // 0X4B
    "Unknown",                     // 0X4C
    "Unknown",                     // 0X4D
    "Unknown",                     // 0X4E
    "Unknown",                     // 0X4F
    "Unknown",                     // 0X50
    "Unknown",                     // 0X51
    "Unknown",                     // 0X52
    "Unknown",                     // 0X53
    "Unknown",                     // 0X54
    "Unknown",                     // 0X55
    "Unknown",                     // 0X56
    "Unknown",                     // 0X57
    "Unknown",                     // 0X58
    "Unknown",                     // 0X59
    "Unknown",                     // 0X5A
    "Unknown",                     // 0X5B
    "Unknown",                     // 0X5C
    "Unknown",                     // 0X5D
    "Unknown",                     // 0X5E
    "Unknown",                     // 0X5F
    "Unknown",                     // 0X60
    "Unknown",                     // 0X61
    "Unknown",                     // 0X62
    "Unknown",                     // 0X63
    "Unknown",                     // 0X64
    "Unknown",                     // 0X65
    "Unknown",                     // 0X66
    "Unknown",                     // 0X67
    "Unknown",                     // 0X68
    "Unknown",                     // 0X69
    "Unknown",                     // 0X6A
    "Unknown",                     // 0X6B
    "Unknown",                     // 0X6C
    "Unknown",                     // 0X6D
    "Unknown",                     // 0X6E
    "Unknown",                     // 0X6F
    "Tree Connect",                // 0x70
    "Tree Disconnect",             // 0x71
    "Negotiate",                   // 0x72
    "Session Setup AndX",          // 0x73
    "Logoff AndX",                 // 0x74
    "Tree Connect AndX",           // 0x75
    "Unknown",                     // 0X76
    "Unknown",                     // 0X77
    "Unknown",                     // 0X78
    "Unknown",                     // 0X79
    "Unknown",                     // 0X7A
    "Unknown",                     // 0X7B
    "Unknown",                     // 0X7C
    "Unknown",                     // 0X7D
    "Security Package AndX",       // 0x7E
    "Unknown",                     // 0X7F
    "Query Information Disk",      // 0x80
    "Search",                      // 0x81
    "Find",                        // 0x82
    "Find Unique",                 // 0x83
    "Find Close",                  // 0x84
    "Unknown",                     // 0X85
    "Unknown",                     // 0X86
    "Unknown",                     // 0X87
    "Unknown",                     // 0X88
    "Unknown",                     // 0X89
    "Unknown",                     // 0X8A
    "Unknown",                     // 0X8B
    "Unknown",                     // 0X8C
    "Unknown",                     // 0X8D
    "Unknown",                     // 0X8E
    "Unknown",                     // 0X8F
    "Unknown",                     // 0X90
    "Unknown",                     // 0X91
    "Unknown",                     // 0X92
    "Unknown",                     // 0X93
    "Unknown",                     // 0X94
    "Unknown",                     // 0X95
    "Unknown",                     // 0X96
    "Unknown",                     // 0X97
    "Unknown",                     // 0X98
    "Unknown",                     // 0X99
    "Unknown",                     // 0X9A
    "Unknown",                     // 0X9B
    "Unknown",                     // 0X9C
    "Unknown",                     // 0X9D
    "Unknown",                     // 0X9E
    "Unknown",                     // 0X9F
    "Nt Transact",                 // 0xA0
    "Nt Transact Secondary",       // 0xA1
    "Nt Create AndX",              // 0xA2
    "Unknown",                     // 0XA3
    "Nt Cancel",                   // 0xA4
    "Nt Rename",                   // 0xA5
    "Unknown",                     // 0XA6
    "Unknown",                     // 0XA7
    "Unknown",                     // 0XA8
    "Unknown",                     // 0XA9
    "Unknown",                     // 0XAA
    "Unknown",                     // 0XAB
    "Unknown",                     // 0XAC
    "Unknown",                     // 0XAD
    "Unknown",                     // 0XAE
    "Unknown",                     // 0XAF
    "Unknown",                     // 0XB0
    "Unknown",                     // 0XB1
    "Unknown",                     // 0XB2
    "Unknown",                     // 0XB3
    "Unknown",                     // 0XB4
    "Unknown",                     // 0XB5
    "Unknown",                     // 0XB6
    "Unknown",                     // 0XB7
    "Unknown",                     // 0XB8
    "Unknown",                     // 0XB9
    "Unknown",                     // 0XBA
    "Unknown",                     // 0XBB
    "Unknown",                     // 0XBC
    "Unknown",                     // 0XBD
    "Unknown",                     // 0XBE
    "Unknown",                     // 0XBF
    "Open Print File",             // 0xC0
    "Write Print File",            // 0xC1
    "Close Print File",            // 0xC2
    "Get Print Queue",             // 0xC3
    "Unknown",                     // 0XC4
    "Unknown",                     // 0XC5
    "Unknown",                     // 0XC6
    "Unknown",                     // 0XC7
    "Unknown",                     // 0XC8
    "Unknown",                     // 0XC9
    "Unknown",                     // 0XCA
    "Unknown",                     // 0XCB
    "Unknown",                     // 0XCC
    "Unknown",                     // 0XCD
    "Unknown",                     // 0XCE
    "Unknown",                     // 0XCF
    "Unknown",                     // 0XD0
    "Unknown",                     // 0XD1
    "Unknown",                     // 0XD2
    "Unknown",                     // 0XD3
    "Unknown",                     // 0XD4
    "Unknown",                     // 0XD5
    "Unknown",                     // 0XD6
    "Unknown",                     // 0XD7
    "Read Bulk",                   // 0xD8
    "Write Bulk",                  // 0xD9
    "Write Bulk Data",             // 0xDA
    "Unknown",                     // 0XDB
    "Unknown",                     // 0XDC
    "Unknown",                     // 0XDD
    "Unknown",                     // 0XDE
    "Unknown",                     // 0XDF
    "Unknown",                     // 0XE0
    "Unknown",                     // 0XE1
    "Unknown",                     // 0XE2
    "Unknown",                     // 0XE3
    "Unknown",                     // 0XE4
    "Unknown",                     // 0XE5
    "Unknown",                     // 0XE6
    "Unknown",                     // 0XE7
    "Unknown",                     // 0XE8
    "Unknown",                     // 0XE9
    "Unknown",                     // 0XEA
    "Unknown",                     // 0XEB
    "Unknown",                     // 0XEC
    "Unknown",                     // 0XED
    "Unknown",                     // 0XEE
    "Unknown",                     // 0XEF
    "Unknown",                     // 0XF0
    "Unknown",                     // 0XF1
    "Unknown",                     // 0XF2
    "Unknown",                     // 0XF3
    "Unknown",                     // 0XF4
    "Unknown",                     // 0XF5
    "Unknown",                     // 0XF6
    "Unknown",                     // 0XF7
    "Unknown",                     // 0XF8
    "Unknown",                     // 0XF9
    "Unknown",                     // 0XFA
    "Unknown",                     // 0XFB
    "Unknown",                     // 0XFC
    "Unknown",                     // 0XFD
    "Invalid",                     // 0xFE
    "No AndX Command"              // 0xFF
};

const char* get_smb_com_string(uint8_t b)
{ return smb_com_strings[b]; }
#endif

Dce2Smb1SessionData::Dce2Smb1SessionData(const Packet* p,
    const dce2SmbProtoConf* proto) : Dce2SmbSessionData(p, proto)
{
    ssd = { };
    ssd.max_outstanding_requests = 10;  // Until Negotiate/SessionSetupAndX
    ssd.cli_data_state = DCE2_SMB_DATA_STATE__NETBIOS_HEADER;
    ssd.srv_data_state = DCE2_SMB_DATA_STATE__NETBIOS_HEADER;
    ssd.pdu_state = DCE2_SMB_PDU_STATE__COMMAND;
    ssd.uid = DCE2_SENTINEL;
    ssd.tid = DCE2_SENTINEL;
    ssd.ftracker.fid_v1 = DCE2_SENTINEL;
    ssd.rtracker.mid = DCE2_SENTINEL;
    ssd.dialect_index = dialect_index;
    ssd.max_file_depth = max_file_depth;
    ssd.sd = sd;
    ssd.policy = policy;
    debug_logf(dce_smb_trace, p, "smb1 session created\n");
    memory::MemoryCap::update_allocations(sizeof(*this));
}

Dce2Smb1SessionData::~Dce2Smb1SessionData()
{
    DCE2_SmbDataFree(&ssd);
    memory::MemoryCap::update_deallocations(sizeof(*this));
}

void Dce2Smb1SessionData::process()
{
    DCE2_Smb1Process(&ssd);
}

void Dce2Smb1SessionData::set_reassembled_data(uint8_t* nb_ptr,uint16_t co_len)
{
    DCE2_SmbSetRdata(&ssd, nb_ptr, co_len);
}

