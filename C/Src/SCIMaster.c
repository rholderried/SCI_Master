/**************************************************************************//**
 * \file SCIMaster.c
 * \author Roman Holderried
 *
 * \brief Master functions for SCI communications.
 * 
 * This serial protocol has been initially written for the MMX heater controller
 * module. It provides data read/write access and a command interface to the 
 * application.
 *
 * <b> History </b>
 * 	- 2022-11-17 - File creation -
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "SCIMaster.h"
#include "SCICommands.h"

/******************************************************************************
 * Function declarations
 *****************************************************************************/

void SCIMasterSM (void)
{

    
}

void SCIMasterQueryNonBlocking (teCOMMAND_TYPE eCmdType, int16_t i16CmdNum, tuCOMMANDVALUE *uVal, int16_t i16ArgNum)
{

}