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
#include "SCIRequests.h"
#include "DataLink.h"
#include "Buffer.h"
#include "Helpers.h"

/******************************************************************************
 * Global variable definition
 *****************************************************************************/
static tsSCI_MASTER sSciMaster = tsSCI_MASTER_DEFAULTS;
// Note: The idizes correspond to the values of the C enum values!
static const char acknowledgeArr [5][4] = {"ACK", "DAT", "UPS", "ERR", "NAK"};
static const uint8_t cmdIdArr[6] = {'#', '?', '!', ':', '>', '<'};
const uint8_t ui8_byteLength[7] = {1,1,2,2,4,4,4};

/******************************************************************************
 * Function declarations
 *****************************************************************************/

void SCIMasterSM (tsSCI_MASTER *psSciMaster)
{
    switch (psSciMaster->eProtocolState)
    {
        case ePROTOCOL_IDLE:
            break;

        case ePROTOCOL_SENDING:

            if (psSciMaster->sDatalink.tState != eDATALINK_TSTATE_READY)
                SCIDatalinkTransmit(&psSciMaster->sDatalink, &psSciMaster->sTxFIFO);
            
            // Transition to next protocol state if tx is ready
            if (psSciMaster->sDatalink.tState == eDATALINK_TSTATE_READY)
            {
                // Reset Datalink Tx State
                SCIDatalinkAcknowledgeTx(&psSciMaster->sDatalink);

                // Reset the Rx Buffer
                // flushBuf(&psSciMaster->sRxFIFO);

                psSciMaster->eProtocolState = ePROTOCOL_RECEIVING;
                SCIDatalinkStartRx(&psSciMaster->sDatalink);
            }    

        case ePROTOCOL_RECEIVING:

            // RX has been ended
            if (psSciMaster->sDatalink.rState == eDATALINK_RSTATE_PENDING)
            {
                
            }

            
    }
    
}

void _SCIMasterQueryNonBlocking (tsSCI_MASTER *psSciMaster, teREQUEST_TYPE eReqType, int16_t i16CmdNum, tuREQUESTVALUE *uVal, int16_t i16ArgNum)
{

}

//=============================================================================
teSCI_ERROR _SCIMasterRequestBuilder(uint8_t *pui8Buf, uint8_t *pui8Size, tsREQUEST sReq)
{
    uint8_t ui8AsciiSize;
    uint8_t ui8DatBuf[30]   = {0};
    uint8_t ui8DataCnt      = 0;
    bool bCommaSet = false;

    // Convert variable number to ASCII
    #ifdef VALUE_MODE_HEX
    *pui8Size = (uint8_t)hexToStrWord(pui8Buf, (uint16_t*)&sReq.i16Num, true);
    #else
    *ui8Size = ftoa(pui8Buf, (float)sReq.i16Num, true);
    #endif

    // Increase Buffer index and write request type identifier
    pui8Buf += *pui8Size;
    *pui8Buf++ = cmdIdArr[sReq.eReqType];
    (*pui8Size)++;

    for(uint8_t i = 0; i < MAX_NUM_REQUEST_VALUES; i++)
    {
        #ifdef VALUE_MODE_HEX
        ui8AsciiSize = (uint8_t)hexToStrDword(ui8DatBuf, &sReq.uValArr[0].ui32_hex, true);
        #else
        ui8AsciiSize = ftoa(ui8DatBuf, sReq.val.f_float, true);
        #endif

        if((*pui8Size + ui8AsciiSize) < TX_PACKET_LENGTH)
        {
            memcpy(pui8Buf, ui8DatBuf, ui8AsciiSize);
            (*pui8Size) += ui8AsciiSize;
            ui8DataCnt++;

            if (ui8DataCnt < sReq.ui8ValArrLen)
            {
                *pui8Buf++ = ',';
                (*pui8Size)++;
            }
            else
                break;
        }            
        
        else
        {
            // Ignore the last comma
            (*pui8Size)--;

            return eSCI_ERROR_MESSAGE_EXCEEDS_TX_BUFFER_SIZE;
        }

    }

    return eSCI_ERROR_NONE;
}

//=============================================================================
teSCI_ERROR _SCIMasterResponseParser(uint8_t* pui8Buf, uint8_t ui8MsgSize, tsRESPONSE *pRsp)
{
    uint8_t i = 0;
    uint32_t ui32_tmp;
    // uint8_t cmdIdx  = 0;
    // COMMAND cmd     = COMMAND_DEFAULT;

    for (; i < ui8MsgSize; i++)
    {

        if (pui8Buf[i] == GETVAR_IDENTIFIER)
        {
            pRsp->eReqType = eREQUEST_TYPE_GETVAR;
            break;
        }
        else if (pui8Buf[i] == SETVAR_IDENTIFIER)
        {
            pRsp->eReqType = eREQUEST_TYPE_SETVAR;
            break;
        }
        else if (pui8Buf[i] == COMMAND_IDENTIFIER)
        {
            pRsp->eReqType = eREQUEST_TYPE_COMMAND;
            break;
        }
        else if (pui8Buf[i] == UPSTREAM_IDENTIFIER)
        {
            pRsp->eReqType = eREQUEST_TYPE_UPSTREAM;
            break;
        }
        else if (pui8Buf[i] == DOWNSTREAM_IDENTIFIER)
        {
            pRsp->eReqType = eREQUEST_TYPE_DOWNSTREAM;
            break;
        }      
    }

    // No valid command identifier found (TODO: Error handling)
    if (pRsp->eReqType == eREQUEST_TYPE_NONE)
        return eSCI_ERROR_COMMAND_IDENTIFIER_NOT_FOUND;
    
    /*******************************************************************************************
     * Command Number Conversion
    *******************************************************************************************/
    // Loop breaks when i reflects the buffer position of the command identifier
    // Variable number conversion
    {
        // One additional character necessary for string termination
        uint8_t *pui8NumStr = (uint8_t*)malloc(i+1);

        // copy the number string into new array
        memcpy(pui8NumStr,pui8Buf,i);
        // Properly terminate string to use the atoi buildin
        pui8NumStr[i] = '\0';
        // Convert
        #ifdef VALUE_MODE_HEX
        if(!strToHex(pui8NumStr, &ui32_tmp))
           return eSCI_ERROR_NUMBER_CONVERSION_FAILED; 
        pRsp->i16Num = *(int16_t*)(&ui32_tmp);
        #else
        pRsp->i16Num = (int16_t)(atoi((char*)pui8NumStr));
        #endif

        free(pui8NumStr);
    }

    // let i correspond to the position of the char after the ID
    i++;

    /*******************************************************************************************
     * Find the command acknowledge
    *******************************************************************************************/
    // UPSTREAM message has no acknowledge, just data
    if (pRsp->eReqType != eREQUEST_TYPE_UPSTREAM)
    {
        uint8_t j = 0;
        char cAck[4];

        memcpy(cAck, pui8Buf[i], 3);

        cAck[3]='\0';

        for (; j < 5; j++)
        {
            if (!strcmp(acknowledgeArr[j], cAck))
                break;
        }

        if (j < 5)
            pRsp->eReqAck = (teREQUEST_ACKNOWLEDGE)j;
        else
            return eSCI_ERROR_ACKNOWLEDGE_UNKNOWN;

        // let i correspond to the position of the char after the acknowledge
        i+=4;
    }
    else
        return eSCI_ERROR_NONE;
    // TODO: Support UPSTREAM messages...

    // Get the number after the acknowledge (And leave if there is none)
    {
        uint8_t j = 0;
        tuREQUESTVALUE uNum = {.ui32_hex = 0};
        uint8_t *pui8NumStr;

        while ((i + j) < ui8MsgSize)
        {
            if (pui8Buf[j] == ';')
                break;

            j++;
        }
        
        // Check if there is a number after the acknowledge
        if (j > 0)
        {
            pui8NumStr = (uint8_t*)malloc(j+1);
            memcpy(pui8NumStr,&pui8Buf[i],j);
            pui8NumStr[j] = '\0';

            #ifdef VALUE_MODE_HEX
            if(!strToHex(pui8NumStr, &uNum.ui32_hex))
                return eSCI_ERROR_PARAMETER_CONVERSION_FAILED; 
            #else
                uNum.f_float = atof((char*)pui8NumStr);
            #endif

            free(pui8NumStr);
        }
        // No other parameters are going to follow
        else
            return eSCI_ERROR_NONE;

        // Assign the number to the data field
        switch (pRsp->eReqAck)
        {
            case eREQUEST_ACK_STATUS_SUCCESS_DATA:
            case eREQUEST_ACK_STATUS_SUCCESS_UPSTREAM:
                #ifdef VALUE_MODE_HEX
                pRsp->ui32DataLength = uNum.ui32_hex;
                #else
                pRsp->ui32DataLength = uNum.f_float;
                #endif
                break;

            case eREQUEST_ACK_STATUS_ERROR:
                #ifdef VALUE_MODE_HEX
                pRsp->ui16ErrNum = uNum.ui32_hex;
                #else
                pRsp->ui16ErrNum = uNum.f_float;
                #endif
                break;

            default:
                // Save GetVar result
                if (pRsp->eReqType == eREQUEST_TYPE_GETVAR)
                {
                    pRsp->uValArr[0] = uNum;
                }
                break;
        }

        // let i correspond to the position of the char after the first data number
        i += (j + 1);
    }

    /*******************************************************************************************
     * Variable value conversion (Values that are comma separated)
    *******************************************************************************************/
   // Only if at least 1 parameter has been passed
   if (ui8MsgSize > i)
   {
        uint8_t ui8_valStrLen = ui8MsgSize - i;
        uint8_t j = 0;
        uint8_t ui8_numOfVals = 0;
        uint8_t ui8_valueLen = 0;
        uint8_t *p_valStr = NULL;

        while (ui8_numOfVals <= MAX_NUM_RESPONSE_VALUES)
        {
            ui8_numOfVals++;

            while (j < ui8_valStrLen)
            {
                // Value seperator found
                if (pui8Buf[i + j] == ',')
                    break;
                
                j++;
                ui8_valueLen++;
            }

            p_valStr = (uint8_t*)malloc(ui8_valueLen + 1);

            // copy the number string into new array
            memcpy(p_valStr, &pui8Buf[i + j - ui8_valueLen], ui8_valueLen);

            p_valStr[ui8_valueLen] = '\0';

            #ifdef VALUE_MODE_HEX
            if(!strToHex(p_valStr, &pRsp->uValArr[ui8_numOfVals - 1].ui32_hex))
                return eSCI_ERROR_PARAMETER_CONVERSION_FAILED;
            #else
            pRsp->uValArr[ui8_numOfVals - 1].f_float = atof((char*)p_valStr);
            #endif

            free(p_valStr);

            if (j == ui8_valStrLen)
                break;
            
            ui8_valueLen = 0;
            j++;
        }

        if (ui8_numOfVals != pRsp->ui32DataLength)
            return eSCI_ERROR_EXPECTED_DATALENGTH_NOT_MET;
    }

    return eSCI_ERROR_NONE;
}