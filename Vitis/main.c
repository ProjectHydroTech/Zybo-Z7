//////////////////////////////////////////////////////////////////////////////////
// Company:  Singapore Polytechnic
// Engineer: P1938238 Calvin Ng Yi Zhuang
//
// Create Date:     13/9/2021
// Project Name:    HydroTech
// Target Device:   Zybo Z7-20
// Tool Versions:   Vitis IDE 2019.2
// Description:     This is the source code for the HydroTech system.
//////////////////////////////////////////////////////////////////////////////////





/********************************
********** Libraries ************
********************************/

// Common libraries

#include "xil_cache.h"
#include "xparameters.h"
#include "stdio.h"
#include "xparameters.h"
#include "xuartps.h"
#include "xtime_l.h"
#include "xgpiops.h"
#include "sleep.h"
#include "math.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xil_printf.h"
#include "sleep.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "xtime_l.h"
#include <iostream>
#include <ArduinoJson-v5.13.5.h>

// Pmod sensor libraries

extern "C"{
#include "PmodALS.h"
#include "PmodHYGRO.h"
#include "PmodAQS.h"
#include "PmodESP32.h"
#include "PmodAD1.h"
}

/********************************
************ Misc ***************
********************************/

#ifdef __MICROBLAZE__
#define HOST_UART_DEVICE_ID XPAR_AXI_UARTLITE_0_BASEADDR
#define HostUart XUartLite
#define HostUart_Config XUartLite_Config
#define HostUart_CfgInitialize XUartLite_CfgInitialize
#define HostUart_LookupConfig XUartLite_LookupConfig
#define HostUart_Recv XUartLite_Recv
#define HostUartConfig_GetBaseAddr(CfgPtr) (CfgPtr->RegBaseAddr)
#include "xuartlite.h"
#include "xil_cache.h"
#else
#define HOST_UART_DEVICE_ID XPAR_PS7_UART_1_DEVICE_ID
#define HostUart XUartPs
#define HostUart_Config XUartPs_Config
#define HostUart_CfgInitialize XUartPs_CfgInitialize
#define HostUart_LookupConfig XUartPs_LookupConfig
#define HostUart_Recv XUartPs_Recv
#define HostUartConfig_GetBaseAddr(CfgPtr) (CfgPtr->BaseAddress)
#include "xuartps.h"
#endif

#define HOST_UART_DEVICE_ID XPAR_PS7_UART_1_DEVICE_ID
#define HostUart XUartPs
#define HostUart_Config XUartPs_Config
#define HostUart_CfgInitialize XUartPs_CfgInitialize
#define HostUart_LookupConfig XUartPs_LookupConfig
#define HostUart_Recv XUartPs_Recv
#define HostUartConfig_GetBaseAddr(CfgPtr) (CfgPtr->BaseAddress)
#define PMODESP32_UART_BASEADDR XPAR_PMODESP32_0_AXI_LITE_UART_BASEADDR
#define PMODESP32_GPIO_BASEADDR XPAR_PMODESP32_0_AXI_LITE_GPIO_BASEADDR
#define COUNTS_PER_SECOND	(XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ /2)
#define TIMER_FREQ_HZ 100000000
#define MAX_WIDTH 320
#define MAX_HEIGHT 240
#define MAX_BUTTON 16
#define PMODESP32_UART_BASEADDR XPAR_PMODESP32_0_AXI_LITE_UART_BASEADDR
#define PMODESP32_GPIO_BASEADDR XPAR_PMODESP32_0_AXI_LITE_GPIO_BASEADDR
#define BLOCK_SIZE 80

/********************************
********* Definitions ***********
********************************/

// Gpio and Sensor declarations

static XGpioPs Gpio;
XGpioPs_Config *ConfigPtr;
PmodALS ALS;
PmodHYGRO HYGRO;
PmodAQS AQS;
PmodESP32 ESP32;
PmodAD1 AD1;
HostUart myHostUart;
XTime TimeStart;
XTime TimeEnd;

// RawData and PhysicalData for PH

AD1_RawData RawData;
AD1_PhysicalData voltage;

// RawData1 and PhysicalData1 for EC

AD1_RawData RawData1;
AD1_PhysicalData voltage1;

/********************************
********** Functions ************
********************************/

// Cache functions

void EnableCaches();
void DisableCaches();

// Control functions

void startup();
void digitalclock1();
void digitalclock2();
void digitalclock3();
void ledandpump();
void ECcheck();
void PHcheck();
void floatanddosing();

// Pmod ESP32 functions

void setWifiMode(void);
void connectWifi(void);
void connectWifi1(void);
void disconnectWifi(void);
void establishConnection(void);
void ESP32_HardwareReset(void);

// Thingspeak functions

void receiveDataWIFI();
void receiveDataCONTROL(XTime time);
void receiveDataMANUAL(XTime time);

void cipsendSENSOR(float fakelight, float fakeCO2, float temp_degc, float hum_perrh, float ECvalue, float PHvalue); // Sensor Data Upload
void cipsendSTATUS(int gledstatus, int mledstatus, int pumpstatus, int ECmotorstatus, int PHmotorstatus, int dosingstatus, int refillwater); // Status Data Upload
void cipsendCONTROL();
void cipsendMANUAL();

/********************************
********** Variables ************
********************************/

// Pmod ALS variables

u8 light;

// Pmod AQS variables

u16 eCO2;
u16 TVOC;
u8 buf[5];

// Pmod ESP32 variables

u8 recv_buffer;
u32 num_received;

// EC and PH sensor variables

float ECvalue = 0;
float PHvalue = 0;
const float ReferenceVoltage = 3.3;

// Pmod HYGRO variables

float temp_degc, hum_perrh;

// Digital clock variables

int countdown = 60;
int sec = 0;
int min = 0;
int hour = 0;
int x = 0;

// Dummy variables

float fakelight;
float fakeCO2;
int fakecount1 = 0;
int fakecount2 = 0;
int fakecount3 = 0;
int wifibug = 0;
int checkingEC = 0;
int checkingPH = 0;
int ECmotor = 0;
int PHmotor = 0;
int DosingDone = 0;
int controlstatus = 0;
int checkingEC = 0;
int checkingPH = 0;
int dosingswitch = 0;
int floatnowater = 0;

// Sending status to Thingspeak

int mledstatus = 0;
int gledstatus = 0;
int pumpstatus = 0;
int ECmotorstatus = 0;
int PHmotorstatus = 0;
int refillwater = 0;
int dosingstatus = 0;

// Receiving data from Thingspeak

int manualgled = 0;
int manualmled = 0;
int manualpump = 0;
int manualECmotor = 0;
int manualPHmotor = 0;
int manualdosing = 0;





/********************************
************ Main ***************
********************************/

int main() {

	void EnableCaches(); // Enable Cache

	startup(); // Initializing GPIO pins and Pmod sensors

	setWifiMode();
	connectWifi(); // WiFi setup

	XTime_GetTime(&TimeStart); // Run the Time start counter for some functions that uses the Pmod ESP32
	TimeEnd = TimeStart + ((XTime)COUNTS_PER_SECOND);

	while(1) // Infinite loop
	{

	establishConnection(); // Establishing connection with Thingspeak to receive control status of the rack ( 1 is Automatic mode and 0 is Manual mode )
	cipsendCONTROL();

	digitalclock3(); // Counter to synchronize timings

	if( controlstatus == 1 ) // Automatic mode
	{

	ECcheck(); // Checking EC servo motor conditions ( Checking EC at 1st hour of the day or the Dosing switch is activated )
	PHcheck(); // Checking PH servo motor conditions ( Checking PH Twice a day, once at the 12th hour and once at the 24th hour and only if the system is NOT currently checking EC)
	
	light = ALS_read(&ALS);
	xil_printf("Light level = %d lux\n\r", light); // Reading the Pmod ALS for Ambient Light Level and printing it to serial terminal
	fakelight = light + 0.0; // Changing data type from unsigned integer to float to send to thingspeak

	if ( ECmotor == 1 ) // Only measure EC values if the EC sensor probe is in the water, so that there will not be any incorrect readings e.g. measuring EC when the probe is in the air ( EC = 0 )
	{

	// Calibrating EC values based on the 12 bit Raw Data that is received from the EC sensor
		
	AD1_GetSample(&AD1, &RawData); // Capturing Raw Data from Pmod AD1

	if(RawData[1] > 0)
		ECvalue = 0.0;

	if(RawData[1] > 70)
		ECvalue = 0.2;

	if(RawData[1] > 126)
		ECvalue = 0.4;

	if(RawData[1] > 174)
		ECvalue = 0.6;

	if(RawData[1] > 254)
		ECvalue = 0.8;

	if(RawData[1] > 318)
		ECvalue = 1.0;

	if(RawData[1] > 382)
		ECvalue = 1.2;

	if(RawData[1] > 470)
		ECvalue = 1.4;

	if(RawData[1] > 590)
		ECvalue = 1.6;

	if(RawData[1] > 654)
		ECvalue = 1.8;

	if(RawData[1] > 700)
		ECvalue = 2.0;

	if(RawData[1] > 763)
		ECvalue = 2.2;

	if(RawData[1] > 814)
		ECvalue = 2.4;

	if(RawData[1] > 900)
		ECvalue = 2.6;

	if(RawData[1] > 1030)
		ECvalue = 2.8;

	printf("EC level = %.01f\r\n", ECvalue); // Printing EC value to serial terminal

	}

	if ( PHmotor == 1 ) // Only measure PH values if the PH sensor probe is in the water, so that there will not be any incorrect readings e.g. measuring PH when the probe is in the air ( Fluctuating PH values )
	{
		
		AD1_GetSample(&AD1, &RawData); // Capturing Raw Data from Pmod AD1

		(voltage)[0] = ((float)RawData[0]) * (1.65 / ((1 << AD1_NUM_BITS) - 1)); // Calculating voltage using Raw Data from the Pmod AD1 and the set reference voltage of 1.65v of the Pmod AD1
		
		PHvalue = (( voltage[0] - 0.10549) / -0.01089 ) + 7; // Calibrating PH values using the calculated voltage from a certain PH value and plotting it on a graph,
		//then drawing a best fit line for the graph and finding the y = mx + c to calculate PH values
		
		printf("pH level = %.01f\n\r", PHvalue); // Printing PH value to serial terminal

	}
	
	AQS_GetData(&AQS, buf);
	eCO2 = (((u16)buf[0] << 8) | ((u16)buf[1])) * 1.6;
	xil_printf("CO2 level in ppm = %d ppm\n\r", eCO2); // Reading the Pmod AQS for carbon dioxide levels in ppm (parts per million ) and printing it to serial terminal
	fakeCO2 = eCO2 + 0.0; // Changing data type of CO2 from unsigned integer to float to send to thingspeak
	
	temp_degc = HYGRO_getTemperature(&HYGRO); // Reading Pmod HYGRO for Temperature in degeree celcius
	hum_perrh = HYGRO_getHumidity(&HYGRO); // Reading Pmod HYGRO for humidity percentage

	temp_degc = round(temp_degc*10)/10; // Changing temperature to 1 decimal point only
	hum_perrh = round(hum_perrh); // Changing humidity to 1 decimal point only

	xil_printf // Printing temperature and humidity to the serial terminal
	(
	         "Temperature : %d.%d Degrees Celcius\n\rRelative Humidity : %d Percent\n\r",
	         (int) temp_degc,
	         ((int) (temp_degc * 100)) % 100,
	         (int) hum_perrh,
	         ((int) (hum_perrh * 100)) % 100
	);

	// Turning on or off the Germination Led, Main Led and Water pump depending different conditions
	// Germination Led ( 9 hours on a day only )
	// Main Led ( Alternating 6 hours on and off throughout the day)
	// Pump ( 5 mins On and 15 Minutes Off throughtout the day if not measuring EC or PH, IF measuring EC or PH pump stays on throughout for good water circulation )
	
	ledandpump();
	
	// Read value from float sensor and doses nutrients if conditions are met
	// Only dose nutrients if measured EC is below preset value e.g. EC = 2 and only once every 5th, 20th, 35th and 50th minute for a set duration ( 6 seconds )
	
	floatanddosing();

	// Dummy variables created to send to Thingspeak to show user on the dashboard the current working status of the rack
	
	if( XGpioPs_ReadPin(&Gpio, 14) == 0 ) // Germination Led on or off
		gledstatus = 1;
	else
		gledstatus = 0;

	if( XGpioPs_ReadPin(&Gpio, 12) == 0 ) // Main Led on or off
		mledstatus = 1;
	else
		mledstatus = 0;

	if( XGpioPs_ReadPin(&Gpio, 15) == 0 ) // Water pump on or off
		pumpstatus = 1;
	else
		pumpstatus = 0;

	if( ECmotor == 1 ) // EC servo motor down or up position
		ECmotorstatus = 1;
	else
		ECmotorstatus = 0;

	if( PHmotor == 1 ) // PH servo motor down or up position
		PHmotorstatus = 1;
	else
		PHmotorstatus = 0;

	if( ECvalue < 2 ) // In progress of nutrient dosing (EC has not reached preset value) or finished nutrient dosing ( EC has reached preset value)
		dosingstatus = 1;
	else
		dosingstatus = 0;

	if( floatnowater == 0 ) // Float sensor activated ( Not enough water ) or not activated ( Enough water )
		refillwater = 1;
	else
		refillwater = 0;

	establishConnection();
	cipsendSENSOR(fakelight, fakeCO2, temp_degc, hum_perrh, ECvalue, PHvalue); // Establishing connection with Thingspeak to send Pmod sensor data

	establishConnection();
	cipsendSTATUS(gledstatus, mledstatus, pumpstatus, ECmotorstatus, PHmotorstatus, dosingstatus, refillwater); // Establishing connection with Thingspeak to send working status of the rack

	digitalclock1(); // Counter to synchronize timings
	
	if(min == 59) // Reconnect WiFi every hour for 30 seconds in case of any poor WiFi connections or disconnections
	{
		setWifiMode();
		connectWifi1();
		sec = sec + 30;
	}

	}
	else // Manual mode
	{

		if(wifibug == 0) // Fake variable for when Pmod ESP32 does not get a stable connection with Thingspeak, and will immediately request for a connection again
		{
			
		establishConnection(); // Establishing connection with Thingspeak to receive working controls of the rack to turn on or off set variables
		cipsendMANUAL();

		if(manualgled == 0) // Control Germination Led on or off ( 0 is off and 1 is on )
			XGpioPs_WritePin(&Gpio, 14, 1);
		else
			XGpioPs_WritePin(&Gpio, 14, 0);

		if(manualmled == 0) // Control Main Led on or off ( 0 is off and 1 is on )
			XGpioPs_WritePin(&Gpio, 12, 1);
		else
			XGpioPs_WritePin(&Gpio, 12, 0);

		if(manualpump == 0) // Control Water pump on or off ( 0 is off and 1 is on )
			XGpioPs_WritePin(&Gpio, 15, 1);
		else
			XGpioPs_WritePin(&Gpio, 15, 0);

		if(manualECmotor == 0) // Control EC servo motor down or up ( 0 is up and 1 is down )
		{
			   for(x=50; x > 0; x -=1)
			   	   {
				   XGpioPs_WritePin(&Gpio,13, 1); // Servo motor up (pulse range of 1600 us)
				   usleep(1600);
				   XGpioPs_WritePin(&Gpio,13, 0);
				   usleep(18400);
			   	   }
			   	   sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions
		}
		else
		{
			   for(x=0; x < 50; x +=1)
			   	   {
				   XGpioPs_WritePin(&Gpio,13, 1); // Servo motor down (pulse range of 2100 us)
				   usleep(2100);
				   XGpioPs_WritePin(&Gpio,13, 0);
				   usleep(17900);
			   	   }
			   	   sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions
			   	   
			   	   // Reading EC values when EC probe is in the water ( Servo motor down )
			   	
					AD1_GetSample(&AD1, &RawData); // Capturing Raw Data from Pmod AD1
					
					if(RawData[1] > 0)
						ECvalue = 0.0;
					
					if(RawData[1] > 70)
						ECvalue = 0.2;
					
					if(RawData[1] > 126)
						ECvalue = 0.4;
					
					if(RawData[1] > 174)
						ECvalue = 0.6;
					
					if(RawData[1] > 254)
						ECvalue = 0.8;
					
					if(RawData[1] > 318)
						ECvalue = 1.0;
					
					if(RawData[1] > 382)
						ECvalue = 1.2;
					
					if(RawData[1] > 470)
						ECvalue = 1.4;
					
					if(RawData[1] > 590)
						ECvalue = 1.6;
					
					if(RawData[1] > 654)
						ECvalue = 1.8;
					
					if(RawData[1] > 700)
						ECvalue = 2.0;
					
					if(RawData[1] > 763)
						ECvalue = 2.2;
					
					if(RawData[1] > 814)
						ECvalue = 2.4;
					
					if(RawData[1] > 900)
						ECvalue = 2.6;
					
					if(RawData[1] > 1030)
						ECvalue = 2.8;
					
					printf("EC level = %.01f\r\n", ECvalue); // Printing EC value to serial terminal
			   	
		}

		if(manualPHmotor == 0) // Control PH servo motor down or up ( 0 is up and 1 is down )
		{
			
				for(x=50; x > 0; x -=1) // Servo motor up (pulse range of 1600 us)
					{
						XGpioPs_WritePin(&Gpio,0, 1);
						usleep(1600);
						XGpioPs_WritePin(&Gpio,0, 0);
						usleep(18400);
					}
				
				sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions
				
		}
		else
		{
			
				for(x=0; x < 50; x +=1) // Servo motor down (pulse range 2100 us)
					{
						XGpioPs_WritePin(&Gpio,0, 1);
						usleep(2100);
						XGpioPs_WritePin(&Gpio,0, 0);
						usleep(17900);
					}
				
				sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions
				
				AD1_GetSample(&AD1, &RawData); // Capturing Raw Data from Pmod AD1

				(voltage)[0] = ((float)RawData[0]) * (1.65 / ((1 << AD1_NUM_BITS) - 1)); // Calculating voltage using Raw Data from the Pmod AD1 and the set reference voltage of 1.65v of the Pmod AD1
				
				PHvalue = (( voltage[0] - 0.10549) / -0.01089 ) + 7; // Calibrating PH values using the calculated voltage from a certain PH value and plotting it on a graph,
				//then drawing a best fit line for the graph and finding the y = mx + c to calculate PH values
				
				printf("pH level = %.01f\n\r", PHvalue); // Printing PH value to serial terminal
		}

		// Control dosing of Nutrients ( 1 to turn on Nutrient dosing pump for a set time of 6 seconds only )
		// ( 0 is then required to reset the Nutrient dosing pump before it is allowed to dose any nutrients again )
		
		if(manualdosing == 0)
			DosingDone = 0;
		else
		{
			if(DosingDone == 0)
			{
			DosingDone = 1;
			XGpioPs_WritePin(&Gpio, 11, 0);
			sleep(6);
			sec = sec + 6;
			XGpioPs_WritePin(&Gpio, 11, 1);
			xil_printf("Dosing now");
			}
		}
		
		light = ALS_read(&ALS);
		xil_printf("Light level = %d lux\n\r", light); // Reading the Pmod ALS for Ambient Light Level and printing it to serial terminal
		fakelight = light + 0.0; // Changing data type from unsigned integer to float to send to thingspeak

		AQS_GetData(&AQS, buf);
		eCO2 = (((u16)buf[0] << 8) | ((u16)buf[1])) * 1.6;
		xil_printf("CO2 level in ppm = %d ppm\n\r", eCO2); // Reading the Pmod AQS for carbon dioxide levels in ppm (parts per million ) and printing it to serial terminal
		fakeCO2 = eCO2 + 0.0; // Changing data type of CO2 from unsigned integer to float to send to thingspeak
		
		temp_degc = HYGRO_getTemperature(&HYGRO); // Reading Pmod HYGRO for Temperature in degeree celcius
		hum_perrh = HYGRO_getHumidity(&HYGRO); // Reading Pmod HYGRO for humidity percentage

		temp_degc = round(temp_degc*10)/10; // Changing temperature to 1 decimal point only
		hum_perrh = round(hum_perrh); // Changing humidity to 1 decimal point only

		xil_printf // Printing temperature and humidity to the serial terminal
		(
		         "Temperature : %d.%d Degrees Celcius\n\rRelative Humidity : %d Percent\n\r",
		         (int) temp_degc,
		         ((int) (temp_degc * 100)) % 100,
		         (int) hum_perrh,
		         ((int) (hum_perrh * 100)) % 100
		);

		establishConnection();
		cipsendSENSOR(fakelight, fakeCO2, temp_degc, hum_perrh, ECvalue, PHvalue);  // Establishing connection with Thingspeak to send Pmod sensor data

		digitalclock2();  // Counter to synchronize timings

		}
		else
		{
			xil_printf("Poor Connection"); // Print Poor Connection to serial for when Pmod ESP32 does not get a stable connection with Thingspeak, and will immediately request for a connection again
		}

		if( min == 59 ) // Reconnect WiFi every hour for 30 seconds in case of any poor WiFi connections or disconnections
		{
			setWifiMode();
			connectWifi1();
			sec = sec + 30;
		}

	}

}
}

/********************************
******* Initialization **********
********************************/

void startup(){

	// Initialize GPIO pins

	// Top view of Zybo Z7-20 board GPIO pins
	//
	//      vcc     vcc
	//      gnd     gnd
	//      pin 8   pin 4
	//      pin 7   pin 3
	//      pin 6   pin 2
	//      pin 5   pin 1
	//

	ConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	XGpioPs_CfgInitialize(&Gpio, ConfigPtr, ConfigPtr->BaseAddr);

	XGpioPs_SetDirectionPin(&Gpio, 13, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, 13,1); //ECmotor (pin 1)

	XGpioPs_SetDirectionPin(&Gpio, 10, 0); //Float sensor (pin 2)

	XGpioPs_SetDirectionPin(&Gpio, 11, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, 11, 1); //Nutrient dosing (pin 3)
	XGpioPs_WritePin(&Gpio, 11, 1);

	XGpioPs_SetDirectionPin(&Gpio,12, 1);
	XGpioPs_SetOutputEnablePin(&Gpio,12,1); //Led main (pin 4)

	XGpioPs_SetDirectionPin(&Gpio,0, 1);
	XGpioPs_SetOutputEnablePin(&Gpio,0, 1); //ECmotor (pin 5)

	XGpioPs_SetDirectionPin(&Gpio, 9, 0); //Dosing switch (pin 6)

	XGpioPs_SetDirectionPin(&Gpio, 14, 1);
	XGpioPs_SetOutputEnablePin(&Gpio,14, 1); //Led germination (pin 7)

	XGpioPs_SetDirectionPin(&Gpio, 15, 1);
	XGpioPs_SetOutputEnablePin(&Gpio,15, 1); //Pump (pin 8)

	HostUart_Config *CfgPtr;
	ESP32_Initialize(&ESP32, PMODESP32_UART_BASEADDR, PMODESP32_GPIO_BASEADDR);
	CfgPtr = HostUart_LookupConfig(HOST_UART_DEVICE_ID);
	HostUart_CfgInitialize(&myHostUart, CfgPtr, HostUartConfig_GetBaseAddr(CfgPtr));

	// Inititialize Pmod ALS

	ALS_begin(&ALS, XPAR_PMODALS_0_AXI_LITE_SPI_BASEADDR);

	// Initialize Pmod AD1 (Compulsory 10 micro second startup)

	AD1_begin(&AD1, XPAR_PMODAD1_0_AXI_LITE_SAMPLE_BASEADDR);
	usleep(10);

	// Inititialize Pmod AQS

	AQS_begin(&AQS, XPAR_PMODAQS_0_AXI_LITE_IIC_BASEADDR, 0x5B);

	// Initialize Pmod HYGRO

	HYGRO_begin
	(
	&HYGRO,
	XPAR_PMODHYGRO_0_AXI_LITE_IIC_BASEADDR,
	0x40,
	XPAR_PMODHYGRO_0_AXI_LITE_TMR_BASEADDR,
	XPAR_PMODHYGRO_0_DEVICE_ID,
	TIMER_FREQ_HZ
	);

}

/********************************
******* Digital Clocks **********
********************************/

void digitalclock1(){

	// Digital clock to count the time for sending sensor data (light level, CO2 level, EC, PH, Temperature and Humidity) and status of rack e.g. led/pump working or not

	sec = sec + 4 ;

	// Added a fake count for the set microseconds needed to establish connection to thingspeak on top of time taken to send the data
	// Without this fakecount the realtime and system time would not be fully accurate

	fakecount1 = fakecount1 + 1;
	if(fakecount1 == 10)
	{
	   sec = sec + 2;
	   fakecount1 = 0;
	}

	// Standard digital clock count and printing to serial terminal

	if(sec >= 60)
	   {
		   min = min + 1;
		   sec = sec - 60;
	   }

	if(min == 60)
	   {
		   hour = hour + 1;
		   min = 0;
	   }

	if(hour == 24)
	   {
		   hour = 0;
	   }

	xil_printf("-----------------------------------------------------------------\n\r");

	xil_printf("Time : Hour = %d   Minutes = %d   Seconds = %d\n\r", hour, min, sec);

	xil_printf("-----------------------------------------------------------------\n\r");

}

void digitalclock2(){

	// Digital clock to count the time for sending sensor data ONLY (light level, CO2 level, EC, PH, Temperature and Humidity)

	sec = sec + 2 ;

	// Added a fake count for the set microseconds needed to establish connection to thingspeak on top of time taken to send the data
	// Without this fakecount the realtime and system time would not be fully accurate

	fakecount2 = fakecount2 + 1;
	if(fakecount2 == 10)
	{
	sec = sec + 1;
	fakecount2 = 0;
	}

	// Standard digital clock count and printing to serial terminal

	if(sec >= 60)
	{
	   min = min + 1;
	   sec = sec - 60;
	}

	if(min == 60)
	{
	   hour = hour + 1;
	   min = 0;
	}

	if(hour == 24)
	{
	   hour = 0;
	}

	xil_printf("-----------------------------------------------------------------\n\r");

	xil_printf("Time : Hour = %d   Minutes = %d   Seconds = %d\n\r", hour, min, sec);

	xil_printf("-----------------------------------------------------------------\n\r");

}

void digitalclock3()
{

	// Digital clock to count the time for reading Automation status of rack ( Automatic or Manual modes)

	sec = sec + 2 ;

	// Added a fake count for the set microseconds needed to establish connection to thingspeak on top of time taken to send the data
	// Without this fakecount the realtime and system time would not be fully accurate

	fakecount3 = fakecount3 + 1;
	if(fakecount3 == 10)
	{
	sec = sec + 1;
	fakecount3 = 0;
	}

	// Standard digital clock count

	if(sec >= 60)
	   {
		   min = min + 1;
		   sec = sec - 60;
	   }

	if(min == 60)
	   {
		   hour = hour + 1;
		   min = 0;
	   }

	if(hour == 24)
	   {
		   hour = 0;
	   }

}

/********************************
********* LED and Pump **********
********************************/

void ledandpump(){

	// Germination row Led on for 9 hours a day

	if(hour < 9)
	   XGpioPs_WritePin(&Gpio, 14, 0); // Germination Led on
	else
	   XGpioPs_WritePin(&Gpio, 14, 1); // Germination Led off

	// Main row Led alternating on and off for 6 hours throughout the day

	if((hour < 6) || (hour > 11 && hour < 18))
	{
	   XGpioPs_WritePin(&Gpio, 12, 0); // Main Led on
	}
	else
	{
	   XGpioPs_WritePin(&Gpio, 12, 1); // Main Led off
	}

	// Pump does a rotation of staying on for 5 minutes and off for 15 minutes if the EC and PH sensor probes are NOT in the water

	if( ((min >= 0 && min < 5) ||  (min >= 20 && min < 25) ||  (min >= 40 && min < 45)) && (checkingEC == 0) && ( checkingPH == 0 ) )
	{
	   XGpioPs_WritePin(&Gpio, 15, 0); // Water pump on
	}
	else
	{
	   if( ((min >= 5 && min < 20) || (min >= 25 && min < 40) || (min >= 45 && min < 60)) && (checkingEC == 0) && (checkingPH == 0) )
	   {
		   XGpioPs_WritePin(&Gpio, 15, 1); // Water pump off
	   }
	}

}

/********************************
******** EC Servo motor *********
********************************/

void ECcheck(){

	// Reading GPIO 9 (pin 6) to check if the dosing switch has been activated

	// Example : User has refilled the tank with water leading to the EC level in the tank dropping, flip the dosing switch to tell system to check the EC and dose nutrients accordingly
	// Flip the switch back after nutrients have been added and reached the set EC level

	dosingswitch = XGpioPs_ReadPin(&Gpio, 9);

	// System will check EC for the first hour of the day or when the dosing switch has been flipped

	if( (hour == 0) || (dosingswitch == 0) )
	   checkingEC = 1;
	else
	   checkingEC = 0;

	// If the condition of checking EC has been met, the servo motor will lower the EC sensor probe into the water to get EC readings, else will stay in the original up position (out of water)
	// Pump will be set ON throughout this process as measuring the EC readings water with good circulation is more accurate

	if( checkingEC == 1 )
	{

		XGpioPs_WritePin(&Gpio, 15, 0); // Water pump on

		// Servo motor will be lowered for 5 minutes every 15 minutes to get a accurate reading of the EC level in the tank

		if( (min >= 0 && min < 5) || (min >= 15 && min < 20) || (min >= 30 && min < 35) || (min >= 45 && min < 50) )
		{

		ECmotor = 1; // Current status of the EC servo motor
		DosingDone = 0; // Variable to let system know that dosing has not yet been done and may proceed to do so if right conditions are met

		for(x=0; x < 50; x +=1) // Servo motor down (pulse range 2100 us)
			{
			XGpioPs_WritePin(&Gpio,13, 1);
			usleep(2100);
			XGpioPs_WritePin(&Gpio,13, 0);
			usleep(17900);
			}

		sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions

		}

		// For the other 10 minutes EC servo motor will remain in the up position and allow any nutrients that has been added in the tank to flow for a sufficient time and will not be taking any readings

		else
		{

		ECmotor = 0; // Current status of the EC servo motor

		for(x=50; x > 0; x -=1) // Servo motor up (pulse range of 1600 us)
			{
			XGpioPs_WritePin(&Gpio,13, 1);
			usleep(1600);
			XGpioPs_WritePin(&Gpio,13, 0);
			usleep(18400);
			}

		sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions

		}
	}

	// EC servo motor does not meet any of the conditions and will stay in the original up position (out of water)

	else
	{

	   ECmotor = 0; // Current status of the EC servo motor

	   for(x=50; x > 0; x -=1) // Servo motor up (pulse range of 1600 us)
		   {
			XGpioPs_WritePin(&Gpio,13, 1);
			usleep(1600);
			XGpioPs_WritePin(&Gpio,13, 0);
			usleep(18400);
		   }

	   sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions

	}
}

/********************************
******** PH Servo motor *********
********************************/

void PHcheck()
{

	// System will check PH twice everyday if the system is NOT currently checking EC
	// Measuring EC of water takes piority over measuring PH of water

	if( (hour == 11 || hour == 23) && (checkingEC == 0) )
	checkingPH = 1;
	else
	checkingPH = 0;

	// If the condition of checking PH has been met, the servo motor will lower the PH sensor probe into the water to get PH readings, else will stay in the original up position (out of water)
	// Pump will be set ON throughout this process as measuring the PH readings water with good circulation is more accurate

	if( checkingPH == 1 )
	{

	XGpioPs_WritePin(&Gpio, 15, 0); // Water pump on

	// Minimizing the time that PH sensor probe stays in the water to 30 minutes, as it takes about 20 - 30 minutes for the PH value to stop fluctuating

	if( (min >= 0 && min < 30) )
	{

		PHmotor = 1; // Current status of the PH servo motor

		for(x=0; x < 50; x +=1) // Servo motor down (pulse range 2100 us)
			{
			XGpioPs_WritePin(&Gpio,0, 1);
			usleep(2100);
			XGpioPs_WritePin(&Gpio,0, 0);
			usleep(17900);
			}

		sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions

	}

	// Other 30 minutes PH servo motor will stay in the original up position (out of water)

	else
		{

			PHmotor = 0; // Current status of the PH servo motor

			for(x=50; x > 0; x -=1) // Servo motor up (pulse range of 1600 us)
				{
				XGpioPs_WritePin(&Gpio,0, 1);
				usleep(1600);
				XGpioPs_WritePin(&Gpio,0, 0);
				usleep(18400);
				}

			sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions

		}
	}

	// PH servo motor does not meet any of the conditions and will stay in the original up position (out of water)

	else
	{

		PHmotor = 0; // Current status of the PH servo motor

		for(x=50; x > 0; x -=1) // Servo motor up (pulse range of 1600 us)
			{
			XGpioPs_WritePin(&Gpio,0, 1);
			usleep(1600);
			XGpioPs_WritePin(&Gpio,0, 0);
			usleep(18400);
			}

		sec = sec + 1; // Add a second to the digital clock as the servo motor requires at least 1 second to change positions

	}
}

/********************************************
***** Float sensor and Nutrient dosing ******
********************************************/

void floatanddosing(){

	// Reading GPIO 10 (pin 2) to check if the float sensor has been activated

	// Example : Water tank does not have enough water and activated the float sensor at the bottom of the water tank, alerting the user to refill more water

	floatnowater = XGpioPs_ReadPin(&Gpio, 10);

	// After system has checked the EC level in the tank, every 5th, 20th, 35th, 50th minute if dosing has not been done and the EC is below the preset value, system will dose the nutrients for a set duration

	if( (min == 5) || (min == 20) || (min == 35) || (min == 50))
	{

		if( (DosingDone == 0) && (ECvalue < 2.0) ) // If dosing has not yet been done and EC is below preset value
		{

		// Dose nutrients for preset duration and print it to serial terminal

		XGpioPs_WritePin(&Gpio, 11, 0);
		xil_printf("Dosing now\n\r");
		sleep(6);
		sec = sec + 6;
		XGpioPs_WritePin(&Gpio, 11, 1);

		// Variable to let the system know that dosing has already been done and will not repeat this loop again
		// This variable will be reset to 0 when conditions for checking EC have been fulfilled

		DosingDone = 1;

		}

	}

}

/***********************************************************************
* This function enables the Pmod ESP32 to connect to a WiFi connection *
* and also allows the Pmod ESP32 to send data to Thingspeak            *
***********************************************************************/

void receiveDataWIFI(XTime time){
XTime tEnd, tCur;
u8 recv_buffer=0;
u32 num_received=0;


XTime_GetTime(&tCur);
tEnd  = tCur + (time * COUNTS_PER_SECOND);
do
   {
	num_received = ESP32_Recv(&ESP32, &recv_buffer,1);
			if(num_received >0){
				num_received = ESP32_Recv(&ESP32, &recv_buffer,1);
				xil_printf("%c", recv_buffer);
			}
	if(tCur == tCur + COUNTS_PER_SECOND){
		countdown = countdown -1;
	}
	else
		XTime_GetTime(&tCur);
   } while (tCur < tEnd);

}

/***********************************************************************
* This function enables the Pmod ESP32 to receive data from Thingspeak *
* and determine if the system should be in Automatic / Manual mode     *
* based on the input of the user (controlled by the mendix dashboard)  *
***********************************************************************/

void receiveDataCONTROL(XTime time){
   XTime tEnd, tCur;
   u8 recv_buffer=0;
   u32 num_received=0;

   int i=0;
   int max_index = BLOCK_SIZE-1;

   int *ptr= (int *) malloc(sizeof(int)*BLOCK_SIZE);

   if(ptr==NULL)
   {
	   perror("some error");
   }

   XTime_GetTime(&tCur);
   tEnd  = tCur + (time * COUNTS_PER_SECOND);
   do
   {
	   num_received = ESP32_Recv(&ESP32, &recv_buffer,1);
			   if(num_received >0){
				   num_received = ESP32_Recv(&ESP32, &recv_buffer,1);

					   if(i > max_index)
					   {
						   ptr=(int *)realloc(ptr, (max_index+1 + BLOCK_SIZE)*sizeof(int));

						   if(ptr == NULL)
						   {
							   perror("insufficient memory!");
							   break;
						   }
						   max_index += BLOCK_SIZE;
					   }
					   ptr[i]=recv_buffer;
					   i++;



			   }
	   if(tCur == tCur + COUNTS_PER_SECOND){
		  countdown = countdown -1;
	   }
	   else
		   XTime_GetTime(&tCur);
   } while (tCur < tEnd);

   xil_printf("Data %d: %c\n\r",i ,ptr[45]);

   if (ptr[45]=='1'){
	xil_printf("AUTOMATIC CONTROL\n\r");
	controlstatus = 1;
   }else if (ptr[45]=='0')
   {
	wifibug = 0;
	xil_printf("MANUAL CONTROL\n\r");
	controlstatus = 0;
   } else {
	wifibug = 1;
	xil_printf("Wifi Error\n\r");
   }

   free(ptr); // Clear the memory to prevent "ptr==null"

}

/***********************************************************************
* This function enables the Pmod ESP32 to receive data from Thingspeak *
* and determine the working controls of the system based on the input  *
* of the user (controlled by the mendix dashboard)                     *
*                                                                      *
* This function basically does a decimal to binary conversion paired   *
* with an array to toggle outputs ( Germination Led, Main Led,         *
* Water Pump, EC servo motor, PH servo motor and Nutrient dosing pump) *
* depending on what the user has inputed into the dashboard            *
*                                                                      *
* (LSB)                                                                *
* Germination Led        - 1                                           *
* Main Led               - 2                                           *
* Pump                   - 4                                           *
* EC servo motor         - 8                                           *
* PH servo motor         - 16                                          *
* Nutrient dosing pump   - 32                                          *
* (MSB)                                                                *
***********************************************************************/

void receiveDataMANUAL(XTime time){
   XTime tEnd, tCur;
   u8 recv_buffer=0;
   u32 num_received=0;

   int i=0;
   int max_index = BLOCK_SIZE-1;

   int *ptr= (int *) malloc(sizeof(int)*BLOCK_SIZE);

   if(ptr==NULL)
   {
	   perror("some error");
   }

   XTime_GetTime(&tCur);
   tEnd  = tCur + (time * COUNTS_PER_SECOND);
   do
   {
	   num_received = ESP32_Recv(&ESP32, &recv_buffer,1);
			   if(num_received >0){
				   num_received = ESP32_Recv(&ESP32, &recv_buffer,1);

					   if(i > max_index)
					   {
						   ptr=(int *)realloc(ptr, (max_index+1 + BLOCK_SIZE)*sizeof(int));

						   if(ptr == NULL)
						   {
							   perror("insufficient memory!");
							   break;
						   }
						   //printf("\nReallocated!");
						   max_index += BLOCK_SIZE;
					   }
					   ptr[i]=recv_buffer;
					   i++;



			   }
	   if(tCur == tCur + COUNTS_PER_SECOND){
		  countdown = countdown -1;
	   }
	   else
		   XTime_GetTime(&tCur);
   } while (tCur < tEnd);

   xil_printf("Data %d: %c%c\n\r",i ,ptr[45], ptr[46]);

   if (ptr[45]=='0')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='1')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='2')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='3')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='4')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='5')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='6')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='7')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='8')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='9')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='0')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='1')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='2')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='3')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='4')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='5')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='6')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='7')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='8')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='1' && ptr[46]=='9')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='0')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='1')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='2')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='3')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='4')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='5')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='6')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='7')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='8')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='2' && ptr[46]=='9')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='3' && ptr[46]=='0')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='3' && ptr[46]=='1')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 0;

   }

   if (ptr[45]=='3' && ptr[46]=='2')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='3' && ptr[46]=='3')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='3' && ptr[46]=='4')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='3' && ptr[46]=='5')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='3' && ptr[46]=='6')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='3' && ptr[46]=='7')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='3' && ptr[46]=='8')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='3' && ptr[46]=='9')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='0')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='1')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='2')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='3')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='4')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='5')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='6')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='7')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 0;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='8')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='4' && ptr[46]=='9')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='0')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='1')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='2')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='3')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='4')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='5')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 0;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='6')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='7')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='8')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='5' && ptr[46]=='9')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 0;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='6' && ptr[46]=='0')
   {

	   manualgled = 0;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='6' && ptr[46]=='1')
   {

	   manualgled = 1;
	   manualmled = 0;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='6' && ptr[46]=='2')
   {

	   manualgled = 0;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   if (ptr[45]=='6' && ptr[46]=='3')
   {

	   manualgled = 1;
	   manualmled = 1;
	   manualpump = 1;
	   manualECmotor = 1;
	   manualPHmotor = 1;
	   manualdosing = 1;

   }

   free(ptr); // Clear the memory to prevent "ptr==null"

   }

/**************************************************************************
* This function enables the Pmod ESP32 to send sensor data to Thingspeak  *
* ( Light level, Carbon Dioxide level, Temperature, Humidity, EC and PH ) *
**************************************************************************/

void cipsendSENSOR(float fakelight, float fakeCO2, float temp_degc, float hum_perrh, float ECvalue, float PHvalue){

	u8 command[200]; //GET Function
	u8 finalcmd[50]; //AT+CIPSEND

	sprintf
	(
	(char*)command,"GET http://api.thingspeak.com/update?api_key=HW4ITVDA2T8JQR34&field1=%d&field2=%d&field3=%d.%d&field4=%d&field5=%d.%d&field6=%d.%d\r\n",
	(int) fakelight,
	(int) fakeCO2,
	(int) temp_degc,((int) (temp_degc * 10)) % 10,
	(int) hum_perrh,
	(int) ECvalue,((int) (ECvalue * 10)) % 10,
	(int) PHvalue,((int) (PHvalue * 10)) % 10
	);

	u32 length = strlen((char*)command); // count how many bits for GET function
	sprintf((char*)finalcmd, "AT+CIPSEND=%d\r\n", (int)length); // convert the AT+CIPSEND Command to char form
	u32 cmdlength =strlen((char*)finalcmd); // count how many bits  for AT+CIPSEND function

	//xil_printf("Length %d\r\n", length); // Display how many bits does the length have
	//xil_printf((char *)finalcmd); // Display AT+CIPSEND= "finalcmd"
	ESP32_SendBuffer(&ESP32, finalcmd, cmdlength); // Send AT+CIPSEND command to server
	usleep(100000);
	//xil_printf((char *)command); // Display GET+"update link"
	ESP32_SendBuffer(&ESP32, command, length); // Send GET+"update link" to server
	receiveDataWIFI(1);
}

/************************************************************************************
* This function enables the Pmod ESP32 to send the status of the rack to Thingspeak *
* ( Germination Led on or off, Main Led on or off, Water pump on or off,            *
* EC servo motor at down or up position, PH servo motor at down or up position,     *
* Dosing in progress or completed and if water level of tank is too low )           *
************************************************************************************/

void cipsendSTATUS(int gledstatus, int mledstatus, int pumpstatus, int ECmotorstatus, int PHmotorstatus, int dosingstatus, int refillwater){

	u8 command[200]; //GET Function
	u8 finalcmd[50]; //AT+CIPSEND

	sprintf(
	(char*)command,"GET http://api.thingspeak.com/update?api_key=14ECD6A6WVDHGBTT&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d&field7=%d\r\n",
	(int) gledstatus,
	(int) mledstatus,
	(int) pumpstatus,
	(int) ECmotorstatus,
	(int) PHmotorstatus,
	(int) dosingstatus,
	(int) refillwater);

	u32 length = strlen((char*)command); // count how many bits for GET function
	sprintf((char*)finalcmd, "AT+CIPSEND=%d\r\n", (int)length); // convert the AT+CIPSEND Command to char form
	u32 cmdlength =strlen((char*)finalcmd); // count how many bits  for AT+CIPSEND function

	//xil_printf("Length %d\r\n", length); // Display how many bits does the length have
	//xil_printf((char *)finalcmd); // Display AT+CIPSEND= "finalcmd"
	ESP32_SendBuffer(&ESP32, finalcmd, cmdlength); // Send AT+CIPSEND command to server
	usleep(100000);
	//xil_printf((char *)command); // Display GET+"update link"
	ESP32_SendBuffer(&ESP32, command, length); // Send GET+"update link" to server
	receiveDataWIFI(1);
}

/****************************************************************************
* This 2 functions enables the Pmod ESP32 to send a command to thingspeak   *
* to let thingspeak know that we would like to retrieve some data, and      *
* by doing that the Pmod ESP32 is able to retrive the control status        *
* ( Automatic / Manual ) and the working controls of the rack ( On or Off ) *
* that is controlled by the user through the mendix dashboard               *
****************************************************************************/

void cipsendCONTROL(void){
	u8 command[150];
	u8 finalcmd[50];
	sprintf((char*)command, "GET https://api.thingspeak.com/channels/1630931/fields/7/last?key=KL2G7YPN0YCGXZWE\r\n");
	u32 length = strlen((char*)command);
	sprintf((char*)finalcmd, "AT+CIPSEND=%d\r\n", (int)length);
	u32 cmdlength =strlen((char*)finalcmd);
	ESP32_SendBuffer(&ESP32, finalcmd, cmdlength);
	usleep(100000);
	ESP32_SendBuffer(&ESP32, command, length);
	receiveDataCONTROL(1);
}

void cipsendMANUAL(void){
	u8 command[150];
	u8 finalcmd[50];
	sprintf((char*)command, "GET https://api.thingspeak.com/channels/1630931/fields/8/last?key=KL2G7YPN0YCGXZWE\r\n");
	u32 length = strlen((char*)command);
	sprintf((char*)finalcmd, "AT+CIPSEND=%d\r\n", (int)length);
	u32 cmdlength =strlen((char*)finalcmd);
	ESP32_SendBuffer(&ESP32, finalcmd, cmdlength);
	usleep(100000);
	ESP32_SendBuffer(&ESP32, command, length);
	receiveDataMANUAL(1);
}

/*****************************************************
* This function sets the Wifi Mode of the Pmod ESP32 *
*****************************************************/

void setWifiMode(void){
	u8 tx[]="AT+CWMODE=3\r\n";// buffer
	u32 num = strlen((char *) tx);// convert buffer to length
	ESP32_SendBuffer(&ESP32, tx, num);// Send AT+CWMODE Function
	receiveDataWIFI(1);
}

/*********************************************************
* This function connects the Pmod ESP32 to the WiFi      *
*                                                        *
* Hardware reset can be activated if the WiFi connection *
* is relatively slow or else it will be unable to        *
* reconnect by itself during WiFi reconnection           *
*********************************************************/

void connectWifi(void){

	// This function is given 10 seconds for the WiFi to connect during startup

	u8 tx[] = "AT+CWJAP=\"eee-iot\",\"1Q2w3E4r5T6y\"\r\n"; // SSID and Password of WiFi
	//u8 tx[] = "AT+CWJAP=\"Calvin\",\"12345678\"\r\n";
	u32 num = strlen((char *) tx); // convert buffer to length
	ESP32_SendBuffer(&ESP32, tx, num);// Send AT+CWJAP Function
	//ESP32_HardwareReset(&ESP32);
	receiveDataWIFI(10); // Increase or Decrease this number based on the speed of the WiFi

}

/*********************************************************
* This function connects the Pmod ESP32 to the WiFi      *
*                                                        *
* Hardware reset can be activated if the WiFi connection *
* is relatively slow or else it will be unable to        *
* reconnect by itself during WiFi reconnection           *
*********************************************************/

void connectWifi1(void){

	// This function is given 30 seconds for the WiFi to reconnect 2 times every hour in case WiFi disconnects

	u8 tx[] = "AT+CWJAP=\"eee-iot\",\"1Q2w3E4r5T6y\"\r\n"; // SSID and Password of WiFi
	//u8 tx[] = "AT+CWJAP=\"Calvin\",\"12345678\"\r\n";
	u32 num = strlen((char *) tx); // convert buffer to length
	ESP32_SendBuffer(&ESP32, tx, num);// Send AT+CWJAP Function
	//ESP32_HardwareReset(&ESP32);
	receiveDataWIFI(30); // Increase or Decrease this number based on the speed of the WiFi

}

/********************************************************************
* This function is to establish the connection to thingspeak before *
* sending or receiving any data and has to be established again     *
* after any data has been sent or received by the Pmod ESP32        *
********************************************************************/

void establishConnection(void){
	u8 tx[] = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n"; //buffer
	u32 num = strlen((char *) tx); // convert buffer to length
	ESP32_SendBuffer(&ESP32, tx, num); // Send AT+CIPSTART Function
	receiveDataWIFI(1);
}

/*******************************************************************************
* This 2 functions are to Enable and Diasable Caches to clear memory if needed *
*******************************************************************************/

void EnableCaches() {
	#ifdef __MICROBLAZE__
	#ifdef XPAR_MICROBLAZE_USE_ICACHE
	  Xil_ICacheEnable();
	#endif
	#ifdef XPAR_MICROBLAZE_USE_DCACHE
	  Xil_DCacheEnable();
	#endif
	#endif
}

void DisableCaches() {
	#ifdef __MICROBLAZE__
	#ifdef XPAR_MICROBLAZE_USE_ICACHE
	  Xil_ICacheDisable();
	#endif
	#ifdef XPAR_MICROBLAZE_USE_DCACHE
	  Xil_DCacheDisable();
	#endif
	#endif
}
