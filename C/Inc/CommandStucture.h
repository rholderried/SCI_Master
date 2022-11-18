/**************************************************************************//**
 * \file CommandStructure.h
 * \author Roman Holderried
 *
 * \brief Declares datatypes necessary for setting up the command structure.
 *
 * <b> History </b>
 * 	- 2022-11-17 - File copied from SCI
 *****************************************************************************/

#ifndef _COMMANDSTRUCTURE_H_
#define _COMMANDSTRUCTURE_H_

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "SCIMasterConfig.h"

/******************************************************************************
 * Type definitions
 *****************************************************************************/


typedef struct
{
    union
    {
        struct
        {
            uint8_t dataBufDynamic      : 1;
            uint8_t upstreamBufDynamic  : 1;
            uint8_t reserved            : 6;
        }ui8_infoFlagBits;

        uint8_t ui8_infoFlagByte;
    };
    uint8_t  *pui8_upStreamBuf;
    uint32_t *pui32_dataBuf;
    uint32_t ui32_datLen;
    uint16_t ui16_error;
}PROCESS_INFO;

#define PROCESS_INFO_DEFAULT {{.ui8_infoFlagByte = 0}, NULL, NULL, 0, 0}

#endif // _COMMANDSTRUCTURE_H_