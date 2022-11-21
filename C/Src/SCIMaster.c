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
#include <string.h>
#include <stdlib.h>

#include "SCIMaster.h"
#include "SCIDataframe.h"
#include "SCITransfer.h"
#include "SCIDataLink.h"
#include "Buffer.h"
#include "Helpers.h"

/******************************************************************************
 * Global variable definition
 *****************************************************************************/
static tsSCI_MASTER sSciMaster = tsSCI_MASTER_DEFAULTS;

const uint8_t ui8_byteLength[7] = {1,1,2,2,4,4,4};

/******************************************************************************
 * Function declarations
 *****************************************************************************/

void SCIMasterSM (void)
{
    teSCI_ERROR eError = eSCI_ERROR_NONE;

    switch (sSciMaster.eProtocolState)
    {
        case ePROTOCOL_IDLE:
            break;

        case ePROTOCOL_SENDING:

            if (sSciMaster.sDatalink.tState != eDATALINK_TSTATE_READY)
                SCIDatalinkTransmit(&sSciMaster.sDatalink, &sSciMaster.sTxFIFO);
            
            // Transition to next protocol state if tx is ready
            if (sSciMaster.sDatalink.tState == eDATALINK_TSTATE_READY)
            {
                // Reset Datalink Tx State
                SCIDatalinkAcknowledgeTx(&sSciMaster.sDatalink);

                // Reset the Rx Buffer
                // flushBuf(&sSciMaster.sRxFIFO);

                sSciMaster.eProtocolState = ePROTOCOL_RECEIVING;

                // Enable data receive
                SCIDatalinkStartRx(&sSciMaster.sDatalink);
            }    

        case ePROTOCOL_RECEIVING:

            // Wait until all data has been received
            if (sSciMaster.sDatalink.rState == eDATALINK_RSTATE_PENDING)
                sSciMaster.eProtocolState = ePROTOCOL_EVALUATING;

            break;

        case ePROTOCOL_EVALUATING:
            {
                tsRESPONSE sRsp = tsRESPONSE_DEFAULT;

                // Parse the response
                SCIMasterResponseParser(&sSciMaster.ui8RxBuffer, sSciMaster.sRxFIFO.i16_bufIdx + 1, &sRsp);

                

            }

            
            
            break;

        default:
            break;
    }
}

//=============================================================================
void _SCIMasterQueryNonBlocking (teREQUEST_TYPE eReqType, int16_t i16CmdNum, tuREQUESTVALUE *uVal, int16_t i16ArgNum)
{

}

//=============================================================================
void SCIReceive (uint8_t *pui8RecBuf, uint8_t ui8ByteCount)
{
    while (ui8ByteCount)
    {
        // Call the datalink-level data receiver
        SCIDataLinkReceive(&sSciMaster.sDatalink, &sSciMaster.sRxFIFO, *pui8RecBuf);

        ui8ByteCount--;
    }
}