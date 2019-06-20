#include "legato.h"
#include "interfaces.h"
#include "le_signals.h"

// [DeclareVariables]
#define APP_RUNNING_DURATION_SEC 600        //run this app for 10min
// [AssetDataPath]

/* variables */
// string - device name
#define DEVICE_NAME_VAR_RES   "/transit/gps/deviceName"

// float - room temperature reading
#define DEVICE_LOC_READING_VAR_RES "/transit/gps/loc"


// float - room temperature reading
#define DEVICE_LAT_READING_VAR_RES "/transit/gps/lat"


// float - room temperature reading
#define DEVICE_ACCURACY_READING_VAR_RES "/transit/gps/accuracy"



// [AssetDataPath]
//-------------------------------------------------------------------------------------------------
/**
 * AVC related variable and update timer
 */
//-------------------------------------------------------------------------------------------------
// reference timer for app session
le_timer_Ref_t sessionTimer;
//reference to AVC event handler
le_avdata_SessionStateHandlerRef_t  avcEventHandlerRef = NULL;
//reference to AVC Session handler
le_avdata_RequestSessionObjRef_t sessionRef = NULL;
//reference to temperature update timer
le_timer_Ref_t tempUpdateTimerRef = NULL;
//reference to push asset data timer
le_timer_Ref_t serverUpdateTimerRef = NULL;

//-------------------------------------------------------------------------------------------------
/**
 * Counters recording the number of times certain hardwares are accessed.
 */
//-------------------------------------------------------------------------------------------------
static int ReadDeviceVarCounter = 0;


//-------------------------------------------------------------------------------------------------
/**
 * Target temperature related declarations.
 */
//-------------------------------------------------------------------------------------------------
static char* DeviceNameVar;
static int32_t DeviceLatVar = 0;           ///< [OUT] latitude of device - set to NULL if not needed
static int32_t DeviceLocVar = 0;          ///< [OUT] longitude of device - set to NULL if not needed
static int32_t DeviceAccuracyVar = 0; ///< [OUT] horizontal accuracy of device - set to NULL if not

// [DeclareVariables]
// [GetLocation]
//-------------------------------------------------------------------------------------------------
/**
 * Update Location
 * This function will update location
 */
//
void GetDevicelocation ()
{
    //Get Location of Device
    le_result_t res;
    le_posCtrl_Request();
    res = le_pos_Get2DLocation(&DeviceLatVar, &DeviceLocVar, &DeviceAccuracyVar);

    if (res == LE_OK)
        {
            LE_INFO("Location: latitude=%d, longitude=%d, hAccuracy=%d", DeviceLatVar, DeviceLocVar, DeviceAccuracyVar);
        }
    else if (res == LE_FAULT)
        {
            LE_INFO("Failed to get the 2D location's data");
        }
    else if (res == LE_OUT_OF_RANGE)
        {
            LE_INFO("One of the retrieved parameter is invalid (set to INT32_MAX)");
        }
    else
        {
            LE_INFO("unknown location error (%d)", res);
        }

    //Get Speed of Device
    // le_result_t res_speed;

    // res_speed=le_pos_GetMotion(&hor_speed,&acchor_speed,&ver_speed,&accver_speed);
    // if(res_speed == LE_OK)
    // {
    //     LE_INFO("hor_speed= %u and ver_speed= %d",hor_speed,ver_speed);
    // }
    // else if (res_speed == LE_FAULT)
    // {
    //     LE_INFO("Failed to get the speed data");
    // }
    // else if (res_speed == LE_OUT_OF_RANGE)
    // {
    //     LE_INFO("One of the retrieved parameter is invalid (set to INT32_MAX)");
    // }
    // else
    // {
    //     LE_INFO("unknown speed error %d ",res_speed);
    // }
}
// [UpdateLocation]
//-------------------------------------------------------------------------------------------------
/**
 * Update Location
 * This function will update location
 */
//-------------------------------------------------------------------------------------------------

void UpdateLocation(le_timer_Ref_t  timerRef)
{

    GetDevicelocation();

    LE_INFO("Device update, %s Loc is %d °C", DeviceNameVar, DeviceLocVar);
    le_result_t resultSetDeviceLoc = le_avdata_SetInt(DEVICE_LOC_READING_VAR_RES, DeviceLocVar);
    if (LE_FAULT == resultSetDeviceLoc)
    {
        LE_ERROR("Error in getting latest DEVICE_LOC_READING_VAR_RES");
    }

    LE_INFO("Device update, %s Lat is %d °C", DeviceNameVar, DeviceLatVar);
    le_result_t resultSetDeviceLat = le_avdata_SetInt(DEVICE_LAT_READING_VAR_RES, DeviceLatVar);
    if (LE_FAULT == resultSetDeviceLat)
    {
        LE_ERROR("Error in getting latest DEVICE_LOC_READING_VAR_RES");
    }

    LE_INFO("Device update, %s Accuracy is %d °C", DeviceNameVar, DeviceAccuracyVar);
    le_result_t resultSetDeviceAccuracy = le_avdata_SetInt(DEVICE_ACCURACY_READING_VAR_RES, DeviceAccuracyVar);
    if (LE_FAULT == resultSetDeviceAccuracy)
    {
        LE_ERROR("Error in getting latest DEVICE_LOC_READING_VAR_RES");
    }
}

// [VariableDataHandler]
//-------------------------------------------------------------------------------------------------
/**
 * Variable data handler.
 * This function is returned whenever AirVantage performs a read on the room's temperature
 */
//-------------------------------------------------------------------------------------------------
static void ReadDeviceVarHandler
(
    const char* path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void* contextPtr
)
{
    ReadDeviceVarCounter++;

    LE_INFO("------------------- Server reads Device var [%d] times ------------",
            ReadDeviceVarCounter);
}
//-------------------------------------------------------------------------------------------------
/**
 * Asset data push
 */
//-------------------------------------------------------------------------------------------------

// [PushCallbackHandler]
//-------------------------------------------------------------------------------------------------
/**
 * Push ack callback handler
 * This function is called whenever push has been performed successfully in AirVantage server
 */
//-------------------------------------------------------------------------------------------------
static void PushCallbackHandler
(
    le_avdata_PushStatus_t status,
    void* contextPtr
)
{
    switch (status)
    {
        case LE_AVDATA_PUSH_SUCCESS:
            LE_INFO("Legato assetdata push successfully");
            break;
        case LE_AVDATA_PUSH_FAILED:
            LE_INFO("Legato assetdata push failed");
            break;
    }
}

//-------------------------------------------------------------------------------------------------
/**
 * Push ack callback handler
 * This function is called every 10 seconds to push the data and update data in AirVantage server
 */
//-------------------------------------------------------------------------------------------------
void PushResources(le_timer_Ref_t  timerRef)
{
    // if session is still open, push the values
    if (NULL != avcEventHandlerRef)
    {
        le_result_t resultPushDeviceName;
        resultPushDeviceName = le_avdata_Push(DEVICE_NAME_VAR_RES, PushCallbackHandler, NULL);
        if (LE_FAULT == resultPushDeviceName)
        {
            LE_ERROR("Error pushing ROOM_NAME_VAR_RES");
        }

        le_result_t resultPushDeviceLoc;
        resultPushDeviceLoc = le_avdata_Push(DEVICE_LOC_READING_VAR_RES, PushCallbackHandler, NULL);
        if (LE_FAULT == resultPushDeviceLoc)
        {
            LE_ERROR("Error pushing ROOM_TEMP_READING_VAR_RES");
        }

        le_result_t resultPushDeviceLat;
        resultPushDeviceLat = le_avdata_Push(DEVICE_LAT_READING_VAR_RES, PushCallbackHandler, NULL);
        if (LE_FAULT == resultPushDeviceLat)
        {
            LE_ERROR("Error pushing ROOM_TEMP_READING_VAR_RES");
        }

        le_result_t resultPushDeviceAccuracy;
        resultPushDeviceAccuracy = le_avdata_Push(DEVICE_ACCURACY_READING_VAR_RES, PushCallbackHandler, NULL);
        if (LE_FAULT == resultPushDeviceAccuracy)
        {
            LE_ERROR("Error pushing ROOM_TEMP_READING_VAR_RES");
        }

    }
}
// [PushCallbackHandler]

//--------------------------------------------------------------------------------------------------
//                                       Test Function Variable defines
//--------------------------------------------------------------------------------------------------
#define TEST_SSID_STR "wifiApSSID"

#define TEST_SECU_PROTO LE_WIFIAP_SECURITY_WPA2
#define TEST_PASSPHRASE "passphrase"

//--------------------------------------------------------------------------------------------------
// PREREQUISITE!!     IP HANDLING
// Please note that IP handling is not provided by the WiFi Service,
// but the following is provided as an example of how the user can setup IP.
// For the following to work the file :
//  /etc/dnsmasq.d/wifiAP.conf
// must contain the following:
// _________________
// dhcp-range=wlan0,192.168.10.10,192.168.10.199,24h
// EOF
// _________________
// Then reboot! Because this is read at startup.
//--------------------------------------------------------------------------------------------------

// IP & mask of subnet created on the wlan
#define SUBNET "192.168.10.0/24"
#define HOST_IP "192.168.10.1"
// IP range allotted to clients
#define IP_RANGE_START "192.168.10.10"
#define IP_RANGE_END "192.168.10.20"

//--------------------------------------------------------------------------------------------------
//                                       Internal defines
//--------------------------------------------------------------------------------------------------

// interface of the access point (LAN - Local Area Network)
#define ITF_LAN "wlan0"
// bridge interface to access the Internet (WAN - Wide Area Network)
#define ITF_WAN "rmnet_data0"

// defines because the SSID is actually uint8 not char.
#define TEST_SSID ((const uint8_t *)TEST_SSID_STR)
#define TEST_SSID_NBR_BYTES (sizeof(TEST_SSID_STR) - 1)

//--------------------------------------------------------------------------------------------------
/**
 * Event handler reference.
 */
//--------------------------------------------------------------------------------------------------
static le_wifiAp_NewEventHandlerRef_t HdlrRef = NULL;



static void RunSystemCommand(
    char *commandStringPtr)
{
	int systemResult;

	if (NULL == commandStringPtr)
	{
		LE_ERROR("ERROR Parameter is NULL.");
		return;
	}
	if ('\0' == *commandStringPtr)
	{
		LE_INFO("INFO Nothing to execute.");
		return;
	}

	systemResult = system(commandStringPtr);
	// Return value of -1 means that the fork() has failed (see man system).
	if (0 == WEXITSTATUS(systemResult))
	{
		LE_INFO("Success: %s", commandStringPtr);
	}
	else
	{
		LE_ERROR("Error %s Failed: (%d)", commandStringPtr, systemResult);
	}
}

//--------------------------------------------------------------------------------------------------
/**
 * Sets the credentials
 * and bridge the wlan0 to ethernet
 */
//--------------------------------------------------------------------------------------------------
static void Testle_startDhcpServerAndbridgeConnection(
    void)
{
	RunSystemCommand("ifconfig " ITF_LAN " " HOST_IP " up");

	// Turn on IP forwarding
	RunSystemCommand("echo 1 > /proc/sys/net/ipv4/ip_forward");
	// Load masquerade module
	RunSystemCommand("modprobe ipt_MASQUERADE");

	RunSystemCommand("iptables -I POSTROUTING -t nat -o " ITF_WAN " -j MASQUERADE");
	RunSystemCommand("iptables -I FORWARD --match state "
			 "--state RELATED,ESTABLISHED --jump ACCEPT");
	RunSystemCommand("iptables -I FORWARD -i " ITF_LAN " --destination " SUBNET
			 " --match state --state NEW --jump ACCEPT");
	RunSystemCommand("iptables -I INPUT -s " SUBNET " --jump ACCEPT");
	RunSystemCommand("iptables -I FORWARD -i " ITF_WAN " --jump ACCEPT");
	RunSystemCommand("iptables -I FORWARD -o " ITF_WAN " --jump ACCEPT");

	LE_INFO("Start data connection");
	RunSystemCommand("/legato/systems/current/bin/cm data connect&");
	sleep(10);

	RunSystemCommand("/legato/systems/current/bin/cm data info");

	LE_INFO("Turn on IP forwarding");
	RunSystemCommand("/sbin/sysctl -w net.ipv4.ip_forward=1");
	sleep(5);
}

//! [SetCred]
//--------------------------------------------------------------------------------------------------
/**
 * Sets the credentials
 */
//--------------------------------------------------------------------------------------------------
static void Testle_setCredentials(
    void)
{
	LE_ASSERT(LE_OK == le_wifiAp_SetPassPhrase(TEST_PASSPHRASE));

	LE_ASSERT(LE_OK == le_wifiAp_SetSecurityProtocol(TEST_SECU_PROTO));

	LE_ASSERT(LE_OK == le_wifiAp_SetDiscoverable(true));
}
//! [SetCred]

//! [Subscribe]
//--------------------------------------------------------------------------------------------------
/**
 * Handler for WiFi client changes
 *
 */
//--------------------------------------------------------------------------------------------------
static void myMsgHandler(
    le_wifiAp_Event_t event,
    ///< [IN]
    ///< WiFi event to process
    void *contextPtr
    ///< [IN]
    ///< Associated event context
)
{
	LE_INFO("WiFi access point event received");
	switch (event)
	{
	case LE_WIFIAP_EVENT_CLIENT_CONNECTED:
	{
		// Client connected to AP
		LE_INFO("LE_WIFIAP_EVENT_CLIENT_CONNECTED");
	}
	break;

	case LE_WIFIAP_EVENT_CLIENT_DISCONNECTED:
	{
		// Client disconnected from AP
		LE_INFO("LE_WIFICLIENT_EVENT_DISCONNECTED");
	}
	break;

	default:
		LE_ERROR("ERROR Unknown event %d", event);
		break;
	}
}

//--------------------------------------------------------------------------------------------------
/**
 * Tests the WiFi access point.
 *
 */
//--------------------------------------------------------------------------------------------------
void Testle_wifiApStart(
    void)
{
	LE_INFO("Start Test WiFi access point");

	// Add an handler function to handle message reception
	HdlrRef = le_wifiAp_AddNewEventHandler(myMsgHandler, NULL);

	LE_ASSERT(HdlrRef != NULL);

	LE_ASSERT(LE_OK == le_wifiAp_SetSsid(TEST_SSID, TEST_SSID_NBR_BYTES));

	Testle_setCredentials();

	if (LE_OK == le_wifiAp_Start())
	{
		LE_INFO("Start OK");

		Testle_startDhcpServerAndbridgeConnection();
	}
	else
	{
		LE_ERROR("Start ERROR");
	}

	LE_ASSERT(LE_OK == le_wifiAp_SetIpRange(HOST_IP, IP_RANGE_START, IP_RANGE_END));
}

//--------------------------------------------------------------------------------------------------
/**
 * Stop the WiFi AP
 */
//--------------------------------------------------------------------------------------------------

static void Testle_wifiApStop(
    int signalId)
{
	LE_INFO("WIFI AP STOP : Received signal %d", signalId);

	// Stop the AP
	le_wifiAp_Stop();

	LE_INFO("Disconnect data connection");
	RunSystemCommand("/legato/systems/current/bin/cm data disconnect&");
	sleep(5);

	// Turn off IP forwarding
	LE_INFO("WIFI AP STOP - Disabling IP forwarding");
	RunSystemCommand("echo 0 > /proc/sys/net/ipv4/ip_forward");
	// Removing masquerade modules
	LE_INFO("WIFI AP STOP - Removing the masquerading module...");
	RunSystemCommand("rmmod ipt_MASQUERADE");

	// Turn off the iptables
	RunSystemCommand("iptables -t nat -f");
	RunSystemCommand("iptables -t mangle -F");
	RunSystemCommand("iptables -F");
	RunSystemCommand("iptables -X");

	// Flush the IP address of the wlan0 interface
	RunSystemCommand("ip addr flush dev " ITF_LAN);
}


//! [Subscribe]

//-------------------------------------------------------------------------------------------------
/**
 * Function relevant to AirVantage server connection
 */
//-------------------------------------------------------------------------------------------------
static void sig_appTermination_cbh(int sigNum)
{
    LE_INFO("Close AVC session");
    le_avdata_ReleaseSession(sessionRef);
    if (NULL != avcEventHandlerRef)
    {
        //unregister the handler
        LE_INFO("Unregister the session handler");
        le_avdata_RemoveSessionStateHandler(avcEventHandlerRef);
    }
    Testle_wifiApStop(sigNum);
}



//-------------------------------------------------------------------------------------------------
/**
 * Status handler for avcService updates
 */
//-------------------------------------------------------------------------------------------------
static void avcStatusHandler
(
    le_avdata_SessionState_t updateStatus,
    void* contextPtr
)
{
    switch (updateStatus)
    {
        case LE_AVDATA_SESSION_STARTED:

        	 putenv("PATH=/legato/systems/current/bin:/usr/local/bin:"
        	    	       "/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin");
            LE_INFO("Legato session started successfully");
//			Testle_wifiApStart();
//            allow_wifi_connect = 1;
            break;

        case LE_AVDATA_SESSION_STOPPED:
            LE_INFO("Legato session stopped");
            break;
    }
}



    // [StartAVCSession]
COMPONENT_INIT
{
    LE_INFO("Start Legato AssetDataApp");


	putenv("PATH=/legato/systems/current/bin:/usr/local/bin:"
	       "/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin");


	le_sig_Block(SIGINT);
	le_sig_Block(SIGTERM);

	// Setup the signal event handler.
	//le_sig_SetEventHandler(SIGINT, Testle_wifiApStop);
	//le_sig_SetEventHandler(SIGTERM, Testle_wifiApStop);


    //le_sig_Block(SIGTERM);
    le_sig_SetEventHandler(SIGINT, sig_appTermination_cbh);
    le_sig_SetEventHandler(SIGTERM, sig_appTermination_cbh);

    Testle_wifiApStart();

    LE_INFO("Start Legato AssetDataApp");

       le_sig_Block(SIGTERM);
       le_sig_SetEventHandler(SIGTERM, sig_appTermination_cbh);

       //Start AVC Session
       //Register AVC handler
       avcEventHandlerRef = le_avdata_AddSessionStateHandler(avcStatusHandler, NULL);
       //Request AVC session. Note: AVC handler must be registered prior starting a session
       le_avdata_RequestSessionObjRef_t sessionRequestRef = le_avdata_RequestSession();
       if (NULL == sessionRequestRef)
       {
           LE_ERROR("AirVantage Connection Controller does not start.");
       }else{
           sessionRef=sessionRequestRef;
           LE_INFO("AirVantage Connection Controller started.");
       }
       // [StartAVCSession]
       // [CreateTimer]
       LE_INFO("Started LWM2M session with AirVantage");


       // [CreateResources]
       LE_INFO("Create instances AssetData ");
       le_result_t resultCreateDeviceName;
       resultCreateDeviceName = le_avdata_CreateResource(DEVICE_NAME_VAR_RES,LE_AVDATA_ACCESS_VARIABLE);
       if (LE_FAULT == resultCreateDeviceName)
       {
           LE_ERROR("Error in creating ROOM_NAME_VAR_RES");
       }

       le_result_t resultCreateDeviceLoc;
       resultCreateDeviceLoc = le_avdata_CreateResource(DEVICE_LOC_READING_VAR_RES,LE_AVDATA_ACCESS_VARIABLE);
       if (LE_FAULT == resultCreateDeviceLoc)
       {
           LE_ERROR("Error in creating ROOM_TEMP_READING_VAR_RES");
       }

       le_result_t resultCreateDeviceLat;
       resultCreateDeviceLat = le_avdata_CreateResource(DEVICE_LAT_READING_VAR_RES,LE_AVDATA_ACCESS_VARIABLE);
       if (LE_FAULT == resultCreateDeviceLat)
       {
           LE_ERROR("Error in creating ROOM_TEMP_READING_VAR_RES");
       }

       le_result_t resultCreateDeviceAccuracy;
       resultCreateDeviceAccuracy = le_avdata_CreateResource(DEVICE_ACCURACY_READING_VAR_RES,LE_AVDATA_ACCESS_VARIABLE);
       if (LE_FAULT == resultCreateDeviceAccuracy)
       {
           LE_ERROR("Error in creating ROOM_TEMP_READING_VAR_RES");
       }


       // [CreateResources]
       // [AssignValues]
       //setting the variable initial value

       DeviceNameVar = "Device1";
       le_result_t resultSetDeviceName = le_avdata_SetString(DEVICE_NAME_VAR_RES, DeviceNameVar);
       if (LE_FAULT == resultSetDeviceName)
       {
           LE_ERROR("Error in setting DEVICE_NAME_VAR_RES");
       }
       le_result_t resultSetDeviceLoc = le_avdata_SetFloat(DEVICE_LOC_READING_VAR_RES,DeviceLocVar);
       if (LE_FAULT == resultSetDeviceLoc)
       {
           LE_ERROR("Error in setting DEVICE_LOC_READING_VAR_RES");
       }
       le_result_t resultSetDeviceLat = le_avdata_SetFloat(DEVICE_LAT_READING_VAR_RES,DeviceLocVar);
       if (LE_FAULT == resultSetDeviceLat)
       {
           LE_ERROR("Error in setting DEVICE_LOC_READING_VAR_RES");
       }
       le_result_t resultSetDeviceAccuracy = le_avdata_SetFloat(DEVICE_ACCURACY_READING_VAR_RES,DeviceLocVar);
       if (LE_FAULT == resultSetDeviceAccuracy)
       {
           LE_ERROR("Error in setting DEVICE_LOC_READING_VAR_RES");
       }

       // [AssignValues]
       // [RegisterHandler]
       //Register handler for Variables, Settings and Commands
       LE_INFO("Register handler of paths");
       le_avdata_AddResourceEventHandler(DEVICE_LOC_READING_VAR_RES, ReadDeviceVarHandler, NULL);

       //Register handler for Variables, Settings and Commands
       LE_INFO("Register handler of paths");
       le_avdata_AddResourceEventHandler(DEVICE_LAT_READING_VAR_RES, ReadDeviceVarHandler, NULL);

       //Register handler for Variables, Settings and Commands
       LE_INFO("Register handler of paths");
       le_avdata_AddResourceEventHandler(DEVICE_ACCURACY_READING_VAR_RES, ReadDeviceVarHandler, NULL);

       // [RegisterHandler]
       // [SetTimer]
       //Set timer to update temperature on a regular basis
       tempUpdateTimerRef = le_timer_Create("tempUpdateTimer");     //create timer
       le_clk_Time_t tempUpdateInterval = { 10, 0 };            //update temperature every 20 seconds
       le_timer_SetInterval(tempUpdateTimerRef, tempUpdateInterval);
       le_timer_SetRepeat(tempUpdateTimerRef, 0);                   //set repeat to always
       //set callback function to handle timer expiration event
       le_timer_SetHandler(tempUpdateTimerRef, UpdateLocation);
       //start timer
       le_timer_Start(tempUpdateTimerRef);
       // [SetTimer]
       // [PushTimer]
       //Set timer to update on server on a regular basis
       serverUpdateTimerRef = le_timer_Create("serverUpdateTimer");     //create timer
       le_clk_Time_t serverUpdateInterval = { 10, 0 };            //update server every 10 seconds
       le_timer_SetInterval(serverUpdateTimerRef, serverUpdateInterval);
       le_timer_SetRepeat(serverUpdateTimerRef, 0);                   //set repeat to always
       //set callback function to handle timer expiration event
       le_timer_SetHandler(serverUpdateTimerRef, PushResources);
       //start timer
       le_timer_Start(serverUpdateTimerRef);
       // [PushTimer]
}
