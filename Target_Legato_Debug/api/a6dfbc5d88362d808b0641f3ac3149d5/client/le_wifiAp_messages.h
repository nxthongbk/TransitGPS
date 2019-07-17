/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_WIFIAP_MESSAGES_H_INCLUDE_GUARD
#define LE_WIFIAP_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "376b52b7f3ec3cdfa5c5d29caa6777f6"

#ifdef MK_TOOLS_BUILD
    extern const char** le_wifiAp_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_wifiAp_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_wifiAp"
#endif


#define _MAX_MSG_SIZE 81

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_wifiAp_AddNewEventHandler 0
#define _MSGID_le_wifiAp_RemoveNewEventHandler 1
#define _MSGID_le_wifiAp_Start 2
#define _MSGID_le_wifiAp_Stop 3
#define _MSGID_le_wifiAp_SetSsid 4
#define _MSGID_le_wifiAp_SetSecurityProtocol 5
#define _MSGID_le_wifiAp_SetPassPhrase 6
#define _MSGID_le_wifiAp_SetPreSharedKey 7
#define _MSGID_le_wifiAp_SetDiscoverable 8
#define _MSGID_le_wifiAp_SetChannel 9
#define _MSGID_le_wifiAp_SetMaxNumberOfClients 10
#define _MSGID_le_wifiAp_SetIpRange 11


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_wifiAp_PackEvent
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_wifiAp_Event_t value
)
{
    return le_pack_PackUint32(bufferPtr, sizePtr, value);
}

static inline bool le_wifiAp_UnpackEvent
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_wifiAp_Event_t* valuePtr
)
{
    bool result;
    uint32_t value;
    result = le_pack_UnpackUint32(bufferPtr, sizePtr, &value);
    if (result)
    {
        *valuePtr = value;
    }
    return result;
}

static inline bool le_wifiAp_PackSecurityProtocol
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_wifiAp_SecurityProtocol_t value
)
{
    return le_pack_PackUint32(bufferPtr, sizePtr, value);
}

static inline bool le_wifiAp_UnpackSecurityProtocol
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_wifiAp_SecurityProtocol_t* valuePtr
)
{
    bool result;
    uint32_t value;
    result = le_pack_UnpackUint32(bufferPtr, sizePtr, &value);
    if (result)
    {
        *valuePtr = value;
    }
    return result;
}

// Define pack/unpack functions for all structures, including included types


#endif // LE_WIFIAP_MESSAGES_H_INCLUDE_GUARD