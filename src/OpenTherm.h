/*
OpenTherm.h - OpenTherm Library for the ESP8266/Arduino platform
https://github.com/ihormelnyk/OpenTherm
http://ihormelnyk.com/pages/OpenTherm
Licensed under MIT license
Copyright 2018, Ihor Melnyk

Frame Structure:
P MGS-TYPE SPARE DATA-ID  DATA-VALUE
0 000      0000  00000000 00000000 00000000
*/

#ifndef OpenTherm_h
#define OpenTherm_h

#include <stdint.h>
#include <Arduino.h>

enum OpenThermResponseStatus {
	NONE,
	SUCCESS,
	INVALID,
	TIMEOUT
};


enum OpenThermMessageType {
	/*  Master to Slave */
	READ_DATA       = B000,
	READ            = READ_DATA, // for backwared compatibility
	WRITE_DATA      = B001,
	WRITE           = WRITE_DATA, // for backwared compatibility
	INVALID_DATA    = B010,
	RESERVED        = B011,
	/* Slave to Master */
	READ_ACK        = B100,
	WRITE_ACK       = B101,
	DATA_INVALID    = B110,
	UNKNOWN_DATA_ID = B111
};

typedef OpenThermMessageType OpenThermRequestType; // for backwared compatibility

enum OpenThermMessageID {
	Status, //R flag8/flag8  Master and Slave Status flags.
	TSet, //W f8.8  Control setpoint  ie CH  water temperature setpoint (°C)
	MConfigMMemberIDcode, //W flag8/u8  Master Configuration Flags / Master MemberID Code
	SConfigSMemberIDcode, //R flag8/u8  Slave Configuration Flags / Slave MemberID Code
	Command, //W u8/u8  Remote Command
	ASFflags, //R flag8/u8  ASF-flags/OEM-fault-code  Application-specific fault flags and OEM fault code
	RBPflags, //R flag8/flag8  Remote boiler parameter transfer-enable & read/write flags
	CoolingControl, //W f8.8  Cooling control signal (%)
	TsetCH2, //W f8.8  Control setpoint for 2e CH circuit (°C)
	TrOverride, //R f8.8  Remote override room setpoint
	TSP, //R u8/u8  Number of Transparent-Slave-Parameters supported by slave
	TSPindexTSPvalue, //RW u8/u8  Index number / Value of referred-to transparent slave parameter.
	FHBsize, //R u8/u8  Size of Fault-History-Buffer supported by slave
	FHBindexFHBvalue, //R u8/u8  Index number / Value of referred-to fault-history buffer entry.
	MaxRelModLevelSetting, //W f8.8  Maximum relative modulation level setting (%)
	MaxCapacityMinModLevel, //R u8/u8  Maximum boiler capacity (kW) / Minimum boiler modulation level(%)
	TrSet, //W f8.8  Room Setpoint (°C)
	RelModLevel, //R f8.8  Relative Modulation Level (%)
	CHPressure, //R f8.8  Water pressure in CH circuit  (bar)
	DHWFlowRate, //R f8.8  Water flow rate in DHW circuit. (litres/minute)
	DayTime, //RW special/u8  Day of Week and Time of Day
	Date, //RW u8/u8  Calendar date
	Year, //RW u16  Calendar year
	TrSetCH2, //W f8.8  Room Setpoint for 2nd CH circuit (°C)
	Tr, //W f8.8  Room temperature (°C)
	Tboiler, //R f8.8  Boiler flow water temperature (°C)
	Tdhw, //R f8.8  DHW temperature (°C)
	Toutside, //R f8.8  Outside temperature (°C)
	Tret, //R f8.8  Return water temperature (°C)
	Tstorage, //R f8.8  Solar storage temperature (°C)
	Tcollector, //R f8.8  Solar collector temperature (°C)
	TflowCH2, //R f8.8  Flow water temperature CH2 circuit (°C)
	Tdhw2, //R f8.8  Domestic hot water temperature 2 (°C)
	Texhaust, //R s16  Boiler exhaust temperature (°C)
	TdhwSetUBTdhwSetLB = 48, //R s8/s8  DHW setpoint upper & lower bounds for adjustment  (°C)
	MaxTSetUBMaxTSetLB, //R s8/s8  Max CH water setpoint upper & lower bounds for adjustment  (°C)
	HcratioUBHcratioLB, //R s8/s8  OTC heat curve ratio upper & lower bounds for adjustment
	TdhwSet = 56, //RW f8.8  DHW setpoint (°C)    (Remote parameter 1)
	MaxTSet, //RW f8.8  Max CH water setpoint (°C)  (Remote parameters 2)
	Hcratio, //RW f8.8  OTC heat curve ratio (°C)  (Remote parameter 3)
	RemoteOverrideFunction = 100, //R flag8/-  Function of manual and program changes in master and remote room setpoint.
	OEMDiagnosticCode = 115, //R u16  OEM-specific diagnostic/service code
	BurnerStarts, //RW u16  Number of starts burner
	CHPumpStarts, //RW u16  Number of starts CH pump
	DHWPumpValveStarts, //RW u16  Number of starts DHW pump/valve
	DHWBurnerStarts, //RW u16  Number of starts burner during DHW mode
	BurnerOperationHours, //RW u16  Number of hours that burner is in operation (i.e. flame on)
	CHPumpOperationHours, //RW u16  Number of hours that CH pump has been running
	DHWPumpValveOperationHours, //RW u16  Number of hours that DHW pump has been running or DHW valve has been opened
	DHWBurnerOperationHours, //RW u16  Number of hours that burner is in operation during DHW mode
	OpenThermVersionMaster, //W f8.8  The implemented version of the OpenTherm Protocol Specification in the master.
	OpenThermVersionSlave, //R f8.8  The implemented version of the OpenTherm Protocol Specification in the slave.
	MasterVersion, //W u8/u8  Master product version number and type
	SlaveVersion, //R u8/u8  Slave product version number and type
};

enum OpenThermStatus {
	NOT_INITIALIZED,
	READY,
	DELAY,
	REQUEST_SENDING,
	RESPONSE_WAITING,
	RESPONSE_START_BIT,
	RESPONSE_RECEIVING,
	RESPONSE_READY,
	RESPONSE_INVALID
};

class OpenTherm
{
public:
	OpenTherm(int inPin = 4, int outPin = 5, bool isSlave = false);
	void begin(void(*handleInterruptCallback)(void));
	void begin(void(*handleInterruptCallback)(void), void(*processResponseCallback)(unsigned long, OpenThermResponseStatus));
	bool isReady();
	volatile OpenThermStatus status;
	unsigned long sendRequest(unsigned long request);
	bool sendResponse(unsigned long request);
	bool sendRequestAync(unsigned long request);
	unsigned long buildRequest(OpenThermMessageType type, OpenThermMessageID id, unsigned int data);
	unsigned long buildResponse(OpenThermMessageType type, OpenThermMessageID id, unsigned int data);
	unsigned long getLastResponse();
	OpenThermResponseStatus getLastResponseStatus();
	const char *statusToString(OpenThermResponseStatus status);
	void handleInterrupt();
	void process();
	void end();

	bool parity(unsigned long frame);
	OpenThermMessageType getMessageType(unsigned long message);
	OpenThermMessageID getDataID(unsigned long frame);
	const char *messageTypeToString(OpenThermMessageType message_type);
	bool isValidRequest(unsigned long request);
	bool isValidResponse(unsigned long response);

	//requests
	unsigned long buildSetBoilerStatusRequest(bool enableCentralHeating, bool enableHotWater = false, bool enableCooling = false, bool enableOutsideTemperatureCompensation = false, bool enableCentralHeating2 = false);
	unsigned long buildSetBoilerTemperatureRequest(float temperature);
	unsigned long buildGetBoilerTemperatureRequest();

	//responses
	bool isFault(unsigned long response);
	bool isCentralHeatingActive(unsigned long response);
	bool isHotWaterActive(unsigned long response);
	bool isFlameOn(unsigned long response);
	bool isCoolingActive(unsigned long response);
	bool isDiagnostic(unsigned long response);
	uint16_t getUInt(const unsigned long response) const;
	float getFloat(const unsigned long response) const;	
	unsigned int temperatureToData(float temperature);

	//basic requests
	unsigned long setBoilerStatus(bool enableCentralHeating, bool enableHotWater = false, bool enableCooling = false, bool enableOutsideTemperatureCompensation = false, bool enableCentralHeating2 = false);
	bool setBoilerTemperature(float temperature);
	float getBoilerTemperature();
    float getReturnTemperature();
    bool setDHWSetpoint(float temperature);
    float getDHWTemperature();
    float getModulation();
    float getPressure();
    unsigned char getFault();

private:
	const int inPin;
	const int outPin;
	const bool isSlave;

	volatile unsigned long response;
	volatile OpenThermResponseStatus responseStatus;
	volatile unsigned long responseTimestamp;
	volatile byte responseBitIndex;

	int readState();
	void setActiveState();
	void setIdleState();
	void activateBoiler();

	void sendBit(bool high);
	void(*handleInterruptCallback)();
	void(*processResponseCallback)(unsigned long, OpenThermResponseStatus);
};

#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif

#ifndef IRAM_ATTR
#define IRAM_ATTR ICACHE_RAM_ATTR
#endif

#endif // OpenTherm_h
