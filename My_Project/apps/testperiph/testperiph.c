/*
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A 
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR 
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION 
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE 
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO 
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO 
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE 
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * 
 *
 * This file is a generated sample test application.
 *
 * This application is intended to test and/or illustrate some 
 * functionality of your system.  The contents of this file may
 * vary depending on the IP in your system and may use existing
 * IP driver functions.  These drivers will be generated in your
 * SDK application project when you run the "Generate Libraries" menu item.
 *
 */


#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"
#include "xintc.h"
#include "intc_header.h"
#include "uartlite_header.h"
#include "xbasic_types.h"
#include "xgpio.h"
#include "gpio_header.h"
#include "gpio_intr_header.h"
#include "xbasic_types.h"
#include "xgpio.h"
#include "gpio_header.h"
#include "xemaclite.h"
#include "xemaclite_example.h"
#include "emaclite_header.h"
#include "xtmrctr.h"
#include "tmrctr_header.h"
#include "tmrctr_intr_header.h"
#include "xspi.h"
#include "spi_header.h"


#define GPIO_CHANNEL1 1

int main() 
{

   static XIntc intc;
   static XGpio dip_switches_4bits_Gpio;
   static XTmrCtr axi_timer_0_Timer;

   Xil_ICacheEnable();
   Xil_DCacheEnable();

   print("---Entering main---\n\r");

   

   {
      int status;
      
      print("\r\n Running IntcSelfTestExample() for microblaze_0_intc...\r\n");
      
      status = IntcSelfTestExample(XPAR_MICROBLAZE_0_INTC_DEVICE_ID);
      
      if (status == 0) {
         print("IntcSelfTestExample PASSED\r\n");
      }
      else {
         print("IntcSelfTestExample FAILED\r\n");
      }
   } 
	
   {
       int Status;

       Status = IntcInterruptSetup(&intc, XPAR_MICROBLAZE_0_INTC_DEVICE_ID);
       if (Status == 0) {
          print("Intc Interrupt Setup PASSED\r\n");
       } 
       else {
         print("Intc Interrupt Setup FAILED\r\n");
      } 
   }
   

   {
      int status;
      
      print("\r\nRunning UartLiteSelfTestExample() for debug_module...\r\n");
      status = UartLiteSelfTestExample(XPAR_DEBUG_MODULE_DEVICE_ID);
      if (status == 0) {
         print("UartLiteSelfTestExample PASSED\r\n");
      }
      else {
         print("UartLiteSelfTestExample FAILED\r\n");
      }
   }
   
   /*
    * Peripheral SelfTest will not be run for usb_uart
    * because it has been selected as the STDOUT device
    */

   

   {
      u32 status;
      
      print("\r\nRunning GpioInputExample() for dip_switches_4bits...\r\n");

      u32 DataRead;
      
      status = GpioInputExample(XPAR_DIP_SWITCHES_4BITS_DEVICE_ID, &DataRead);
      
      if (status == 0) {
         xil_printf("GpioInputExample PASSED. Read data:0x%X\r\n", DataRead);
      }
      else {
         print("GpioInputExample FAILED.\r\n");
      }
   }
   {
      
      int Status;
        
      u32 DataRead;
      
      print(" Press button to Generate Interrupt\r\n");
      
      Status = GpioIntrExample(&intc, &dip_switches_4bits_Gpio, \
                               XPAR_DIP_SWITCHES_4BITS_DEVICE_ID, \
                               XPAR_MICROBLAZE_0_INTC_DIP_SWITCHES_4BITS_IP2INTC_IRPT_INTR, \
                               GPIO_CHANNEL1, &DataRead);
	
      if (Status == 0 ){
             if(DataRead == 0)
                print("No button pressed. \r\n");
             else
                print("Gpio Interrupt Test PASSED. \r\n"); 
      } 
      else {
         print("Gpio Interrupt Test FAILED.\r\n");
      }
	
   }
   

   {
      u32 status;
      
      print("\r\nRunning GpioOutputExample() for leds_4bits...\r\n");

      status = GpioOutputExample(XPAR_LEDS_4BITS_DEVICE_ID,4);
      
      if (status == 0) {
         print("GpioOutputExample PASSED.\r\n");
      }
      else {
         print("GpioOutputExample FAILED.\r\n");
      }
   }
   

   {
      int status;
      
      print("\r\nRunning EmacLitePolledExample() for ethernet_mac...\r\n");
      status = EmacLitePolledExample(XPAR_ETHERNET_MAC_DEVICE_ID);
      if (status == 0) {
         print("EmacLite Polled Example PASSED\r\n");
      }
      else {
         print("EmacLite Polled Example FAILED\r\n");
      }
   }
   

   {
      int status;
      
      print("\r\n Running TmrCtrSelfTestExample() for axi_timer_0...\r\n");
      
      status = TmrCtrSelfTestExample(XPAR_AXI_TIMER_0_DEVICE_ID, 0x0);
      
      if (status == 0) {
         print("TmrCtrSelfTestExample PASSED\r\n");
      }
      else {
         print("TmrCtrSelfTestExample FAILED\r\n");
      }
   }
   {
      int Status;

      print("\r\n Running Interrupt Test  for axi_timer_0...\r\n");
      
      Status = TmrCtrIntrExample(&intc, &axi_timer_0_Timer, \
                                 XPAR_AXI_TIMER_0_DEVICE_ID, \
                                 XPAR_MICROBLAZE_0_INTC_AXI_TIMER_0_INTERRUPT_INTR, 0);
	
      if (Status == 0) {
         print("Timer Interrupt Test PASSED\r\n");
      } 
      else {
         print("Timer Interrupt Test FAILED\r\n");
      }

   }
   

   {
      XStatus status;
      
      print("\r\n Runnning SpiSelfTestExample() for spi_flash...\r\n");
      
      status = SpiSelfTestExample(XPAR_SPI_FLASH_DEVICE_ID);
      
      if (status == 0) {
         print("SpiSelfTestExample PASSED\r\n");
      }
      else {
         print("SpiSelfTestExample FAILED\r\n");
      }
   }

   print("---Exiting main---\n\r");

   Xil_DCacheDisable();
   Xil_ICacheDisable();

   return 0;
}

