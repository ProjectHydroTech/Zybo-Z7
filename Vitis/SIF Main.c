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

extern "C"{
#include "PmodALS.h"
#include "PmodHYGRO.h"
#include "PmodAQS.h"
#include "PmodESP32.h"
#include "PmodAD1.h"
}

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

void startup();
void digitalclock1();
void digitalclock2();
void digitalclock3();
void ledandpump();
void ECcheck();
void PHcheck();
void floatanddosing();

void receiveDataWIFI();
void receiveDataCONTROL(XTime time);
void receiveDataLED1(XTime time);
void receiveDataLED234(XTime time);
void receiveDataPUMP(XTime time);
void receiveDataMOTOR(XTime time);
void receiveDataDOSE(XTime time);

void setWifiMode(void);
void connectWifi(void);
void connectWifi1(void);
void disconnectWifi(void);
void establishConnection(void);
void ESP32_HardwareReset(void);

void EnableCaches();
void DisableCaches();

void cipsendSENSOR(float fakelight, float fakeCO2, float temp_degc, float hum_perrh, float ECvalue, float PHvalue); // Sensor Data Upload
void cipsendSTATUS(int ledstatus, int pumpstatus, int ECmotorstatus, int PHmotorstatus, int dosingstatus, int refillwater); // Status Data Upload
void cipsendCONTROL();
void cipsendMANUAL();

PmodALS ALS;
PmodALS ALS1;
PmodHYGRO HYGRO;
PmodAQS AQS;
PmodESP32 ESP32;
PmodAD1 AD1;
PmodAD1 AD1_1;
HostUart myHostUart;
XGpioPs_Config *ConfigPtr;
static XGpioPs Gpio;
XTime TimeStart;
XTime TimeEnd;
AD1_RawData RawData;
AD1_PhysicalData voltage;
AD1_RawData RawData1;
AD1_PhysicalData voltage1;

u8 light;
u8 light1; //ALS

u16 eCO2;
u16 TVOC;
u8 buf[5]; //AQS

u8 recv_buffer;
u32 num_received; // wifi


float ECvalue = 0;
float PHvalue = 0; // measure
float temp_degc, hum_perrh; // HYGRO
float fakelight, fakelight1, fakeCO2;
const float ReferenceVoltage = 3.3;

int countdown = 60;
int sec = 0;
int min = 0;
int hour = 0;
int x = 0;

int status;
int fakecount1 = 0;
int fakecount2 = 0;
int fakecount3 = 0;
int dosingswitch = 0;
int floatnowater = 0;
int ECmotor = 0;
int PHmotor = 0;
int DosingDone = 0;

int controlstatus = 0;
int ledstatus = 0;
int pumpstatus = 0;
int ECmotorstatus = 0;
int PHmotorstatus = 0;
int refillwater = 0;
int dosingstatus = 0;
int checkingEC = 0;
int checkingPH = 0;

int manualgled = 0;
int manualmled = 0;
int manualpump = 0;
int manualECmotor = 0;
int manualPHmotor = 0;
int manualdosing = 0;

int wifibug = 0;


int main() {

	void EnableCaches();

	startup();

	setWifiMode();
	connectWifi();

	XTime_GetTime(&TimeStart);
	TimeEnd = TimeStart + ((XTime)COUNTS_PER_SECOND);

	while(1)
	{

	establishConnection();
	cipsendCONTROL();

	digitalclock3();

	if( controlstatus == 1 ) //automatic
	{

	ECcheck();
	PHcheck();

	light = ALS_read(&ALS);
	xil_printf("Light level = %d lux\n\r", light);
	fakelight = light + 0.0;

	if ( ECmotor == 1 )
	{

	AD1_GetSample(&AD1, &RawData);

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

	printf("EC level = %.01f\r\n", ECvalue);

	}

	if ( PHmotor == 1 )
	{

		AD1_GetSample(&AD1, &RawData);

		(voltage)[0] = ((float)RawData[0]) * (1.65 / ((1 << AD1_NUM_BITS) - 1)); //[0] is ad0, for pH

		PHvalue = (( voltage[0] - 0.10549) / -0.01089 ) + 7;

		printf("pH level = %.01f\n\r", PHvalue);

	}

	AQS_GetData(&AQS, buf);
	eCO2 = (((u16)buf[0] << 8) | ((u16)buf[1])) * 1.6;
	fakeCO2 = eCO2 + 0.0;
	xil_printf("CO2 level in ppm = %d ppm\n\r", eCO2); //parts per million

	temp_degc = HYGRO_getTemperature(&HYGRO);
	hum_perrh = HYGRO_getHumidity(&HYGRO);

	temp_degc = round(temp_degc*10)/10;
	hum_perrh = round(hum_perrh);

	xil_printf
	(
	         "Temperature : %d.%d Degrees Celcius\n\rRelative Humidity : %d Percent\n\r",
	         (int) temp_degc,
	         ((int) (temp_degc * 100)) % 100,
	         (int) hum_perrh,
	         ((int) (hum_perrh * 100)) % 100
	);

	ledandpump();
	floatanddosing();

	if( XGpioPs_ReadPin(&Gpio, 12) == 0 )
		ledstatus = 1;
	else
		ledstatus = 0;

	if( XGpioPs_ReadPin(&Gpio, 15) == 0 )
		pumpstatus = 1;
	else
		pumpstatus = 0;

	if( ECmotor == 1 )
		ECmotorstatus = 1;
	else
		ECmotorstatus = 0;

	if( PHmotor == 1 )
		PHmotorstatus = 1;
	else
		PHmotorstatus = 0;

	if( ECvalue < 2 )
		dosingstatus = 1;
	else
		dosingstatus = 0;

	if( floatnowater == 0 )
		refillwater = 1;
	else
		refillwater = 0;

	establishConnection();
	cipsendSENSOR(fakelight, fakeCO2, temp_degc, hum_perrh, ECvalue, PHvalue);

	establishConnection();
	cipsendSTATUS(ledstatus, pumpstatus, ECmotorstatus, PHmotorstatus, dosingstatus, refillwater);

	if(min == 59)
	{
		setWifiMode();
		connectWifi1();
		sec = sec + 30;
	}

	digitalclock1();

	}
	else //manual
	{

		if(wifibug == 0)
		{
		establishConnection();
		cipsendMANUAL();

		if(manualmled == 0)
			XGpioPs_WritePin(&Gpio, 12, 1);
		else
			XGpioPs_WritePin(&Gpio, 12, 0);

		if(manualpump == 0)
			XGpioPs_WritePin(&Gpio, 15, 1);
		else
			XGpioPs_WritePin(&Gpio, 15, 0);

		if(manualECmotor == 0)
		{
			   for(x=50; x > 0; x -=1)
			   	   {
				   XGpioPs_WritePin(&Gpio,0, 1); //motor up
				   usleep(1600);
				   XGpioPs_WritePin(&Gpio,0, 0);
				   usleep(18400);
			   	   }
			   	   sec = sec + 1;
		}
		else
		{
			   for(x=0; x < 50; x +=1)
			   	   {
				   XGpioPs_WritePin(&Gpio,0, 1); //motor down
				   usleep(2100);
				   XGpioPs_WritePin(&Gpio,0, 0);
				   usleep(17900);
			   	   }
			   	   sec = sec + 1;

			   	AD1_GetSample(&AD1, &RawData);

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

			   	printf("EC level = %.01f\r\n", ECvalue);
		}

		if(manualPHmotor == 0)
		{
			   for(x=50; x > 0; x -=1)
			   	   {
				   XGpioPs_WritePin(&Gpio,13, 1); //motor up
				   usleep(1600);
				   XGpioPs_WritePin(&Gpio,13, 0);
				   usleep(18400);
			   	   }
			   	   sec = sec + 1;
		}
		else
		{
			   for(x=0; x < 50; x +=1)
			   	   {
				   XGpioPs_WritePin(&Gpio,13, 1); //motor down
				   usleep(2100);
				   XGpioPs_WritePin(&Gpio,13, 0);
				   usleep(17900);
			   	   }
			   	   sec = sec + 1;

					AD1_GetSample(&AD1, &RawData);

					(voltage)[0] = ((float)RawData[0]) * (1.65 / ((1 << AD1_NUM_BITS) - 1)); //[0] is ad0, for pH

					PHvalue = (( voltage[0] - 0.10549) / -0.01089 ) + 7;

					printf("pH level = %.01f\n\r", PHvalue);
		}

		if(manualdosing == 0)
			DosingDone = 0;
		else
		{
			if(DosingDone == 0)
			{
			DosingDone = 1;
			XGpioPs_WritePin(&Gpio, 14, 0);
			sleep(6);
			sec = sec + 6;
			XGpioPs_WritePin(&Gpio, 14, 1);
			xil_printf("Dosing now");
			}
		}


		light = ALS_read(&ALS);
		xil_printf("Ambient light level = %d lux\n\r", light);
		fakelight = light + 0.0;

		AQS_GetData(&AQS, buf);
		eCO2 = (((u16)buf[0] << 8) | ((u16)buf[1])) * 1.6;
		fakeCO2 = eCO2 + 0.0;
		xil_printf("CO2 level in ppm = %d ppm\n\r", eCO2); //parts per million

		temp_degc = HYGRO_getTemperature(&HYGRO);
		hum_perrh = HYGRO_getHumidity(&HYGRO);

		temp_degc = round(temp_degc*10)/10;
		hum_perrh = round(hum_perrh);

		xil_printf
		(
		         "Temperature : %d.%d Degrees Celcius\n\rRelative Humidity : %d Percent\n\r",
		         (int) temp_degc,
		         ((int) (temp_degc * 100)) % 100,
		         (int) hum_perrh,
		         ((int) (hum_perrh * 100)) % 100
		);

		establishConnection();
		cipsendSENSOR(fakelight, fakeCO2, temp_degc, hum_perrh, ECvalue, PHvalue);

		digitalclock2();

		}
		else
		{
			xil_printf("Wifi Disconnected");
		}

		if(min == 59)
		{
			setWifiMode();
			connectWifi1();
			sec = sec + 30;
		}

	}

}
}

   void startup(){

	   ConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	   XGpioPs_CfgInitialize(&Gpio, ConfigPtr, ConfigPtr->BaseAddr);

	   //      (top view)
	   //      vcc     vcc
	   //      gnd     gnd
	   //       8       4
	   //       7       3
	   //       6       2
	   //       5       1

	   XGpioPs_SetDirectionPin(&Gpio, 13, 1);
	   XGpioPs_SetOutputEnablePin(&Gpio, 13,1); //PHmotor

	   XGpioPs_SetDirectionPin(&Gpio, 10, 0); //Float sensor

	   XGpioPs_SetDirectionPin(&Gpio, 11, 1);
	   XGpioPs_SetOutputEnablePin(&Gpio, 11, 1); //Water dosing
	   XGpioPs_WritePin(&Gpio, 11, 1);

	   XGpioPs_SetDirectionPin(&Gpio,12, 1);
	   XGpioPs_SetOutputEnablePin(&Gpio,12,1); //led

	   XGpioPs_SetDirectionPin(&Gpio,0, 1);
	   XGpioPs_SetOutputEnablePin(&Gpio,0,1); //ECmotor

	   XGpioPs_SetDirectionPin(&Gpio, 9, 0); //Dosing switch

	   XGpioPs_SetDirectionPin(&Gpio, 14, 1);
	   XGpioPs_SetOutputEnablePin(&Gpio,14, 1); //Nutrient dosing
	   XGpioPs_WritePin(&Gpio, 14, 1);

	   XGpioPs_SetDirectionPin(&Gpio, 15, 1);
	   XGpioPs_SetOutputEnablePin(&Gpio,15, 1); //Pump

	   HostUart_Config *CfgPtr;
	   ESP32_Initialize(&ESP32, PMODESP32_UART_BASEADDR, PMODESP32_GPIO_BASEADDR);
	   CfgPtr = HostUart_LookupConfig(HOST_UART_DEVICE_ID);
	   HostUart_CfgInitialize(&myHostUart, CfgPtr, HostUartConfig_GetBaseAddr(CfgPtr));

	   ALS_begin(&ALS, XPAR_PMODALS_0_AXI_LITE_SPI_BASEADDR);
	   ALS_begin(&ALS1, XPAR_PMODALS_1_AXI_LITE_SPI_BASEADDR);

	   AD1_begin(&AD1, XPAR_PMODAD1_0_AXI_LITE_SAMPLE_BASEADDR);
	   usleep(10);

	   AQS_begin(&AQS, XPAR_PMODAQS_0_AXI_LITE_IIC_BASEADDR, 0x5B);

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

   void digitalclock1(){

	   sec = sec + 4 ;

	   fakecount1 = fakecount1 + 1;
	   if(fakecount1 == 10)
	   {
		   sec = sec + 2;
		   fakecount1 = 0;
	   }

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

	   sec = sec + 2 ;

	   fakecount2 = fakecount2 + 1;
	   if(fakecount2 == 10)
	   {
		   sec = sec + 1;
		   fakecount2 = 0;
	   }

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

	   sec = sec + 2 ;

	   fakecount3 = fakecount3 + 1;
	   if(fakecount3 == 10)
	   {
		   sec = sec + 1;
		   fakecount3 = 0;
	   }

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

   void ledandpump(){

		   if((hour < 6) || (hour > 11 && hour < 18))
		   {
			   XGpioPs_WritePin(&Gpio, 12, 0); //led on
		   }
		   else
		   {
			   XGpioPs_WritePin(&Gpio, 12, 1); //led off
		   }

		   if( ((min >= 0 && min < 5) ||  (min >= 20 && min < 25) ||  (min >= 40 && min < 45)) && (checkingEC == 0) && ( checkingPH == 0 ) ) //on water pump
		   {
			   XGpioPs_WritePin(&Gpio, 15, 0);
		   }
		   else
		   {
			   if( ((min >= 5 && min < 20) || (min >= 25 && min < 40) || (min >= 45 && min < 60)) && (checkingEC == 0) && (checkingPH == 0) ) //off water pump
			   {
				   XGpioPs_WritePin(&Gpio, 15, 1);
			   }
		   }

   }

   void ECcheck(){

	   dosingswitch = XGpioPs_ReadPin(&Gpio, 9);

	   if( (hour == 0) || (dosingswitch == 0) )
		   checkingEC = 1;
	   else
		   checkingEC = 0;

	   if( checkingEC == 1 )
	   {

		   XGpioPs_WritePin(&Gpio, 15, 0);

		   if( (min >= 0 && min < 5) || (min >= 15 && min < 20) || (min >= 30 && min < 35) || (min >= 45 && min < 50) )
		   {
			   ECmotor = 1;
			   DosingDone = 0;
			   for(x=0; x < 50; x +=1)
			   {
				   XGpioPs_WritePin(&Gpio,0, 1); //motor down
				   usleep(2100);
				   XGpioPs_WritePin(&Gpio,0, 0);
				   usleep(17900);
			   }
			   sec = sec + 1;
		   }
		   else
		   	   {
			   ECmotor = 0;
			   for(x=50; x > 0; x -=1)
			   	   {
				   XGpioPs_WritePin(&Gpio,0, 1); //motor up
				   usleep(1600);
				   XGpioPs_WritePin(&Gpio,0, 0);
				   usleep(18400);
			   	   }
			   	   sec = sec + 1;
		   	   }
	   }
	   else
	   {
		   ECmotor = 0;
		   for(x=50; x > 0; x -=1)
		   	   {
			   XGpioPs_WritePin(&Gpio,0, 1); //motor up
			   usleep(1600);
			   XGpioPs_WritePin(&Gpio,0, 0);
			   usleep(18400);
		   	   }
		   	   sec = sec + 1;
	   }
   }

   void PHcheck()
   {

	   if( (hour == 11 || hour == 23) && (checkingEC == 0) )
		   checkingPH = 1;
	   else
		   checkingPH = 0;

	   if( checkingPH == 1 )
	   {

		   XGpioPs_WritePin(&Gpio, 15, 0);

		   if( (min >= 0 && min < 30) )
		   {
			   PHmotor = 1;
			   for(x=0; x < 50; x +=1)
			   {
				   XGpioPs_WritePin(&Gpio,13, 1); //motor down
				   usleep(2100);
				   XGpioPs_WritePin(&Gpio,13, 0);
				   usleep(17900);
			   }
			   sec = sec + 1;
		   }
		   else
		   	   {
			   PHmotor = 0;
			   for(x=50; x > 0; x -=1)
			   	   {
				   XGpioPs_WritePin(&Gpio,13, 1); //motor up
				   usleep(1600);
				   XGpioPs_WritePin(&Gpio,13, 0);
				   usleep(18400);
			   	   }
			   	   sec = sec + 1;
		   	   }
	   }
	   else
	   {
		   PHmotor = 0;
		   for(x=50; x > 0; x -=1)
		   	   {
			   XGpioPs_WritePin(&Gpio,13, 1); //motor up
			   usleep(1600);
			   XGpioPs_WritePin(&Gpio,13, 0);
			   usleep(18400);
		   	   }
		   	   sec = sec + 1;
	   }
   }

   void floatanddosing(){

	   floatnowater = XGpioPs_ReadPin(&Gpio, 10);

	   if( (min == 5) || (min == 20) || (min == 35) || (min == 50))
	   {

		   if( (DosingDone == 0) && (ECvalue < 2.0) )
		   {
		   XGpioPs_WritePin(&Gpio, 14, 0);
		   xil_printf("Dosing now\n\r");
		   sleep(6);
		   sec = sec + 6;
		   XGpioPs_WritePin(&Gpio, 14, 1);
		   DosingDone = 1;
		   }

	   }

	   if( (floatnowater == 0) )
		   {
			   XGpioPs_WritePin(&Gpio, 11, 0);
			   xil_printf("Toping up water now\n\r");
			   sleep(10);
			   sec = sec + 10;
			   XGpioPs_WritePin(&Gpio, 11, 1);
		   }

   }

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

   void cipsendSENSOR(float fakelight, float fakeCO2, float temp_degc, float hum_perrh, float ECvalue, float PHvalue){

   	u8 command[200]; //GET Function
   	u8 finalcmd[50]; //AT+CIPSEND

   	sprintf
	(
   	(char*)command,"GET http://api.thingspeak.com/update?api_key=Q8GIC34RLESFH93Y&field1=%d&field2=%d&field3=%d.%d&field4=%d&field5=%d.%d&field6=%d.%d\r\n",
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

   void cipsendSTATUS(int ledstatus, int pumpstatus, int ECmotorstatus, int PHmotorstatus, int dosingstatus, int refillwater){

   	u8 command[200]; //GET Function
   	u8 finalcmd[50]; //AT+CIPSEND

   	sprintf(
   	(char*)command,"GET http://api.thingspeak.com/update?api_key=KDP497VZFP12I11O&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d&field7=%d\r\n",
   	(int) ledstatus,
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

   void cipsendCONTROL(void){
   	u8 command[150];
   	u8 finalcmd[50];
   	sprintf((char*)command, "GET https://api.thingspeak.com/channels/1212072/fields/7/last?key=EGVLNS6FG636HCC6\r\n");
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
    sprintf((char*)command, "GET https://api.thingspeak.com/channels/1212072/fields/8/last?key=EGVLNS6FG636HCC6\r\n");
    u32 length = strlen((char*)command);
    sprintf((char*)finalcmd, "AT+CIPSEND=%d\r\n", (int)length);
   	u32 cmdlength =strlen((char*)finalcmd);
   	ESP32_SendBuffer(&ESP32, finalcmd, cmdlength);
   	usleep(100000);
   	ESP32_SendBuffer(&ESP32, command, length);
   	receiveDataMANUAL(1);
   }

   void setWifiMode(void){
   	u8 tx[]="AT+CWMODE=3\r\n";// buffer
   	u32 num = strlen((char *) tx);// convert buffer to length
   	ESP32_SendBuffer(&ESP32, tx, num);// Send AT+CWMODE Function
   	receiveDataWIFI(1);
   }

   void connectWifi(void){
	u8 tx[] = "AT+CWJAP=\"Yan San Metals\",\"YSM2206T\"\r\n";
	//u8 tx[] = "AT+CWJAP=\"eee-iot\",\"1Q2w3E4r5T6y\"\r\n";
	//u8 tx[] = "AT+CWJAP=\"iotsucks\",\"0987654321\"\r\n";
   	u32 num = strlen((char *) tx); // convert buffer to length
   	ESP32_SendBuffer(&ESP32, tx, num);// Send AT+CWJAP Function
   	receiveDataWIFI(10);
   }

   void connectWifi1(void){
   	u8 tx[] = "AT+CWJAP=\"Yan San Metals\",\"YSM2206T\"\r\n";
	//u8 tx[] = "AT+CWJAP=\"eee-iot\",\"1Q2w3E4r5T6y\"\r\n";
	//u8 tx[] = "AT+CWJAP=\"iotsucks\",\"0987654321\"\r\n";
   	u32 num = strlen((char *) tx); // convert buffer to length
   	ESP32_SendBuffer(&ESP32, tx, num);// Send AT+CWJAP Function
   	receiveDataWIFI(30);
   }

   void establishConnection(void){
   	u8 tx[] = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n"; //buffer
   	u32 num = strlen((char *) tx); // convert buffer to length
   	ESP32_SendBuffer(&ESP32, tx, num); // Send AT+CIPSTART Function
   	//ESP32_HardwareReset(&ESP32);
   	receiveDataWIFI(1);
   }

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
