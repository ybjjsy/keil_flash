/* -----------------------------------------------------------------------------
 * Copyright (c) 2016 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        10. October 2018
 * $Revision:    V1.0.0
 *
 * Project:      Flash Device Description for
 *               STM32H743 W25Q256 SPIFI Flash
 * --------------------------------------------------------------------------- */

#include "..\FlashOS.H"        // FlashOS Structures
#include "sys.h"
#include "delay.h"
#include "w25qxx.h"
#include "qspi.h"
#include "LED.h"
#include "usart.h"


#define PAGE_SIZE            4096

/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
 
uint8_t aux_buf[PAGE_SIZE];
uint32_t base_adr;

//这个函数在keil下载后，在擦除、编程都会重新调用一次此函数，而且在调用此函数之前keil会复位单片机，相当于单片机跑起来的初始化功能要在擦除、编程时都要初始化一遍
int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {

	Stm32_Clock_Init(100,1,2,2);  //设置时钟,400Mhz 
	delay_init(400);						  //延时初始化
	uart_init(100,115200);
	LED_Init();
	W25QXX_Init();		            //初始化W25QXX
	base_adr = adr;
	
  return (0);
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc) {

  return (0);
}


/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip (void) {

  W25QXX_Erase_Chip();
  return (0);                                        /* Finished without Errors */
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector (unsigned long adr) {

	W25QXX_Erase_Sector(adr-base_adr);
  return (0);                                        /* Finished without Errors */
}


/*
 *  Blank Check Checks if Memory is Blank
 *    Parameter:      adr:  Block Start Address
 *                    sz:   Block Size (in bytes)
 *                    pat:  Block Pattern
 *    Return Value:   0 - OK,  1 - Failed
 */

int BlankCheck (unsigned long adr, unsigned long sz, unsigned char pat) {

  return (1);                                        /* Always Force Erase */
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {
	
	W25QXX_Write_NoCheck(buf,adr-base_adr,sz);
  return (0);                                        /* Finished without Errors */
}


/*  
 *  Verify Flash Contents
 *    Parameter:      adr:  Start Address
 *                    sz:   Size (in bytes)
 *                    buf:  Data
 *    Return Value:   (adr+sz) - OK, Failed Address
 */

/*
   Verify function is obsolete because all other function leave 
    the SPIFI in memory mode so a memory compare could be used.
 */
unsigned long Verify (unsigned long adr, unsigned long sz, unsigned char *buf) {

  int i;
  W25QXX_Read(aux_buf, adr-base_adr, sz);
  for (i = 0; i< PAGE_SIZE; i++) 
	{
    if (aux_buf[i] != buf[i]) 
      return (adr+i);                   // Verification Failed (return address)
  }
  return (adr+sz);                      // Done successfully
}
