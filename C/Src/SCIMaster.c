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

// const uint8_t ui8_byteLength[7] = {1,1,2,2,4,4,4};

/******************************************************************************
 * Function declarations
 *****************************************************************************/
void SCIMasterInit (void)
void SCIMasterSM (void)
{
    teSCI_ERROR eError = eSCI_ERROR_NONE;

    switch (sSciMaster.eProtocolState)
    {
        case ePROTOCOL_IDLE:
            break;

        case ePROTOCOL_SENDING:

            if (sSciMaster.sDatalink.tState != eDATALINK_TSTATE_READY)
                SCIDatalinkTransmitStateMachine(&sSciMaster.sDatalink);
            
            // Transition to next protocol state if tx is ready
            else
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
                tsRESPONSE sRsp = tsRESPONSE_DEFAULTS;
                uint8_t *pui8Buf;
                uint8_t ui8DframeLen = readBuf(&sSciMaster.sRxFIFO, &pui8Buf);

                // Parse the response
                if (sSciMaster.ui8RecMode == SCI_RECEIVE_MODE_TRANSFER)
                    SCIMasterResponseParser(pui8Buf, ui8DframeLen, &sRsp);
                else if (sSciMaster.ui8RecMode == SCI_RECEIVE_MODE_STREAM)
                    SCIMasterStreamParser(pui8Buf, ui8DframeLen, &sRsp);

                // Process the response
                SCITransferControl(&sSciMaster.sSCITransfer, sRsp);
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

//=============================================================================
void SCIInitiateStreamReceive (uint32_t ui32ByteCount)
{
    sSciMaster.ui8RecMode = SCI_RECEIVE_MODE_STREAM;
    sSciMaster.sDatalink.sRxInfo.ui32BytesToGo = ui32ByteCount;
}

//=============================================================================
void SCIFinishStreamReceive (void)
{
    sSciMaster.ui8RecMode = SCI_RECEIVE_MODE_TRANSFER;
    sSciMaster.sDatalink.sRxInfo.ui32BytesToGo = 0;
}

//=============================================================================
void SCIInitiateRequest (tsREQUEST sReq)
{
    // Prepare transmission buffer
    flushBuf(&sSciMaster.sTxFIFO);

    // Assemble message
    if (SCIMasterRequestBuilder(sSciMaster.sTxFIFO.pui8_bufPtr, sSciMaster.sTxFIFO.ui8_bufSpace, sReq) == eSCI_ERROR_NONE)
    {
        SCIDatalinkTransmit(&sSciMaster.sDatalink, &sSciMaster.sTxFIFO);

        sSciMaster.eProtocolState = ePROTOCOL_SENDING;
    }
    else
    {
        // TODO: What to do on error?
        ;
    }
}

//=============================================================================
void SCIFinishTransfer (void)
{
    sSciMaster.eProtocolState = ePROTOCOL_IDLE;
}

//=============================================================================
void SCIRequestGetVar (int16_t i16VarNum)
{
    // Request generation by the Transfer control module
    SCITransferStart(&sSciMaster.sSCITransfer, eREQUEST_TYPE_GETVAR, i16VarNum, NULL, 0);
}

//=============================================================================
void SCIRequestSetVar (int16_t i16VarNum, tuREQUESTVALUE uVal)
{
    // Request generation by the Transfer control module
    SCITransferStart(&sSciMaster.sSCITransfer, eREQUEST_TYPE_SETVAR, i16VarNum, &uVal, 1);
}

//=============================================================================
void SCIRequestCommand (int16_t i16CmdNum, tuREQUESTVALUE *puValArr, uint8_t ui8ArgNum)
{
    // Request generation by the Transfer control module
    SCITransferStart(&sSciMaster.sSCITransfer, eREQUEST_TYPE_COMMAND, i16CmdNum, puValArr, ui8ArgNum);
}