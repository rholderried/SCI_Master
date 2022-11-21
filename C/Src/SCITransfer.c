/**************************************************************************//**
 * \file SCITransfer.c
 * \author Roman Holderried
 *
 * \brief Transfer control functions
 *
 * <b> History </b>
 * 	- 2022-11-21 - File creation 
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "SCITransfer.h"

/******************************************************************************
 * Global variable definition
 *****************************************************************************/

/******************************************************************************
 * Function definitions
 *****************************************************************************/
void SCITransferStart (tsSCI_TRANSFER *psSciTransfer, teREQUEST_TYPE eReqType, int16_t i16CmdNum, tuREQUESTVALUE *uVal, uint8_t ui8ArgNum)
{
    tsREQUEST sReq = tsREQUEST_DEFAULTS;
    // Take over the arguments
    sReq.eReqType       = eReqType;
    sReq.i16Num         = i16CmdNum;
    sReq.uValArr        = uVal;
    sReq.ui8ValArrLen   = ui8ArgNum;

    psSciTransfer->sTransferInfo.ui32ReceivedDataCnt = 0;
    psSciTransfer->sTransferInfo.ui32TransferCnt = 0;
    psSciTransfer->sTransferInfo.ui32ExpectedDataCnt = 0;

    if (psSciTransfer->sCallbacks.RequestCB != NULL)
        psSciTransfer->sCallbacks.RequestCB(sReq);

    psSciTransfer->sTransferInfo.sReq = sReq;

    psSciTransfer->eTransferState = eTRANSFER_STATE_BUSY;
}

bool SCITransferControl (tsSCI_TRANSFER *psSciTransfer, tsRESPONSE sRsp)
{
    teTRANSFER_ACK eTransferAck;
    bool ret = true;

    switch (sRsp.eReqType)
    {
        case eREQUEST_TYPE_SETVAR:
            eTransferAck = (teTRANSFER_ACK)psSciTransfer->sCallbacks.SetVarCB((uint8_t)sRsp.eReqAck, sRsp.i16Num);
            break;
        
        case eREQUEST_TYPE_GETVAR:
            eTransferAck = (teTRANSFER_ACK)psSciTransfer->sCallbacks.GetVarCB((uint8_t)sRsp.eReqAck, sRsp.i16Num, sRsp.uValArr[0].ui32_hex);
            break;

        case eREQUEST_TYPE_COMMAND:
            if (sRsp.eReqAck == eREQUEST_ACK_STATUS_SUCCESS_DATA)
            {
                // Generate a transfer value buffer and copy data
                // In first message
                if (psSciTransfer->sTransferInfo.ui32TransferCnt == 0)
                {
                    psSciTransfer->sTransferInfo.ui32ExpectedDataCnt = sRsp.ui32DataLength;

                    // Allocate the memory for the COMMAND results
                    psSciTransfer->sTransferInfo.uTransferResults = malloc(psSciTransfer->sTransferInfo.ui32ExpectedDataCnt * sizeof(tuRESPONSEVALUE));

                    // TODO: Handling of not enough memory ?!?
                    if(psSciTransfer->sTransferInfo.uTransferResults == NULL)
                        return false;
                }

                // Copy buffer values into the transfer memory
                memcpy(&psSciTransfer->sTransferInfo.uTransferResults[psSciTransfer->sTransferInfo.ui32ReceivedDataCnt], 
                        sRsp.uValArr, 
                        sRsp.ui8ResponseDataLength * sizeof(tuRESPONSEVALUE));

                psSciTransfer->sTransferInfo.ui32ReceivedDataCnt += sRsp.ui8ResponseDataLength;

                // Increment number of COMMAND transfers
                psSciTransfer->sTransferInfo.ui32TransferCnt++;

                // All command transfers ready
                if (sRsp.ui32DataLength == psSciTransfer->sTransferInfo.ui32ReceivedDataCnt)
                {
                    // Callback invocation
                    eTransferAck = (teTRANSFER_ACK)psSciTransfer->sCallbacks.CommandCB((uint8_t)sRsp.eReqAck, sRsp.i16Num, psSciTransfer->sTransferInfo.uTransferResults, psSciTransfer->sTransferInfo.ui32ReceivedDataCnt);

                    // Free data memory
                    free(psSciTransfer->sTransferInfo.uTransferResults);
                }
                // Invoke command again to get the remaining data
                else
                {
                    // For all consecutive transfers, parameters do not have to be passed.
                    psSciTransfer->sTransferInfo.sReq.ui8ValArrLen = 0;

                    psSciTransfer->sCallbacks.RequestCB(psSciTransfer->sTransferInfo.sReq);
                }

            }
            // Upstream invocation
            else if (sRsp.eReqAck == eREQUEST_ACK_STATUS_SUCCESS_DATA)
            {

            }
            break;
    }

    // switch (psSciTransfer->eTransferState)
    // {
    //     case eTRANSFER_STATE_IDLE:
    //         break;

    //     case eTRANSFER_STATE_BUSY:

    //         if ()

    //         break;

    //     case eTRANSFER_STATE_READY:

    //         psSciTransfer->eTransferState = eTRANSFER_STATE_IDLE;
    //         break;
        
    //     default:
    //         break;
    // }
}

