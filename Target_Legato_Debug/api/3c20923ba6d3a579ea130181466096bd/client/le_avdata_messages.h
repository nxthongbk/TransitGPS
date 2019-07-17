/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_AVDATA_MESSAGES_H_INCLUDE_GUARD
#define LE_AVDATA_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "d042ee9f795ba3ff64aacf91d970b8fc"

#ifdef MK_TOOLS_BUILD
    extern const char** le_avdata_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_avdata_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_avdata"
#endif


#define _MAX_MSG_SIZE 798

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_avdata_AddResourceEventHandler 0
#define _MSGID_le_avdata_RemoveResourceEventHandler 1
#define _MSGID_le_avdata_CreateResource 2
#define _MSGID_le_avdata_SetNamespace 3
#define _MSGID_le_avdata_SetNull 4
#define _MSGID_le_avdata_GetInt 5
#define _MSGID_le_avdata_SetInt 6
#define _MSGID_le_avdata_GetFloat 7
#define _MSGID_le_avdata_SetFloat 8
#define _MSGID_le_avdata_GetBool 9
#define _MSGID_le_avdata_SetBool 10
#define _MSGID_le_avdata_GetString 11
#define _MSGID_le_avdata_SetString 12
#define _MSGID_le_avdata_GetBoolArg 13
#define _MSGID_le_avdata_GetFloatArg 14
#define _MSGID_le_avdata_GetIntArg 15
#define _MSGID_le_avdata_GetStringArg 16
#define _MSGID_le_avdata_GetStringArgLength 17
#define _MSGID_le_avdata_ReplyExecResult 18
#define _MSGID_le_avdata_Push 19
#define _MSGID_le_avdata_PushStream 20
#define _MSGID_le_avdata_CreateRecord 21
#define _MSGID_le_avdata_DeleteRecord 22
#define _MSGID_le_avdata_RecordInt 23
#define _MSGID_le_avdata_RecordFloat 24
#define _MSGID_le_avdata_RecordBool 25
#define _MSGID_le_avdata_RecordString 26
#define _MSGID_le_avdata_PushRecord 27
#define _MSGID_le_avdata_AddSessionStateHandler 28
#define _MSGID_le_avdata_RemoveSessionStateHandler 29
#define _MSGID_le_avdata_RequestSession 30
#define _MSGID_le_avdata_ReleaseSession 31


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_avdata_PackAccessMode
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_AccessMode_t value
)
{
    return le_pack_PackUint32(bufferPtr, sizePtr, value);
}

static inline bool le_avdata_UnpackAccessMode
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_AccessMode_t* valuePtr
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

static inline bool le_avdata_PackAccessType
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_AccessType_t value
)
{
    return le_pack_PackUint32(bufferPtr, sizePtr, value);
}

static inline bool le_avdata_UnpackAccessType
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_AccessType_t* valuePtr
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

static inline bool le_avdata_PackNamespace
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_Namespace_t value
)
{
    return le_pack_PackUint32(bufferPtr, sizePtr, value);
}

static inline bool le_avdata_UnpackNamespace
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_Namespace_t* valuePtr
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

static inline bool le_avdata_PackDataType
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_DataType_t value
)
{
    return le_pack_PackUint32(bufferPtr, sizePtr, value);
}

static inline bool le_avdata_UnpackDataType
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_DataType_t* valuePtr
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

static inline bool le_avdata_PackPushStatus
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_PushStatus_t value
)
{
    return le_pack_PackUint32(bufferPtr, sizePtr, value);
}

static inline bool le_avdata_UnpackPushStatus
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_PushStatus_t* valuePtr
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

static inline bool le_avdata_PackSessionState
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_SessionState_t value
)
{
    return le_pack_PackUint32(bufferPtr, sizePtr, value);
}

static inline bool le_avdata_UnpackSessionState
(
    uint8_t **bufferPtr,
    size_t* sizePtr,
    le_avdata_SessionState_t* valuePtr
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


#endif // LE_AVDATA_MESSAGES_H_INCLUDE_GUARD