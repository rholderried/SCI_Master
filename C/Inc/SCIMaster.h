/**************************************************************************//**
 * \file SCIMaster.h
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


#ifndef _SCIMASTER_H_
#define _SCIMASTER_H_

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "SCIRequests.h"
#include "Buffer.h"
#include "DataLink.h"

/******************************************************************************
 * Defines
 *****************************************************************************/

#define SCI_MASTER
#define SCI_MASTER_VERSION_MAJOR    0
#define SCI_MASTER_VERSION_MINOR    5
#define SCI_MASTER_REVISION         0

#define GETVAR_IDENTIFIER       '?'
#define SETVAR_IDENTIFIER       '!'
#define COMMAND_IDENTIFIER      ':'
#define UPSTREAM_IDENTIFIER     '>'
#define DOWNSTREAM_IDENTIFIER   '<'

/******************************************************************************
 * Type definitions
 *****************************************************************************/

/** @brief SCI version data structure */
typedef struct
{
    uint8_t ui8VersionMajor;
    uint8_t ui8VersionMinor;
    uint8_t ui8Revision;
}tsSCI_MASTER_VERSION;

#define tsSCI_MASTER_VERSION_VALUE {SCI_MASTER_VERSION_MAJOR, SCI_MASTER_VERSION_MINOR, SCI_MASTER_REVISION}

typedef enum
{
    ePROTOCOL_ERROR         = -1,
    ePROTOCOL_IDLE          = 0,
    ePROTOCOL_SENDING       = 1,
    ePROTOCOL_EVALUATING    = 2,
    ePROTOCOL_RECEIVING     = 3,
}tePROTOCOL_STATE;

// typedef enum
// {
//     eSCI_ERROR_NONE = 0,
//     eSCI_ERROR_TRANSMISSION_ERROR,
//     eSCI_ERROR_RECEIVE_ERROR,
//     eSCI_ERROR_DATAFRAME_ERROR

//     // eSCI_ERROR_
// }teSCI_ERROR;


typedef struct
{
    void (*commandCallback)(teREQUEST_TYPE eReqType, int16_t i16ReqNum, tuREQUESTVALUE *uVal, int16_t i16ArgNum);

}tsSCI_MASTER_CALLBACKS;

typedef struct
{
    tsSCI_MASTER_VERSION sVersion;
    tePROTOCOL_STATE eProtocolState;

    uint8_t ui8RxBuffer[RX_PACKET_LENGTH];   /*!< RX buffer space. */ 
    uint8_t ui8TxBuffer[TX_PACKET_LENGTH];   /*!< TX buffer space. */ 

    tsFIFO_BUF sRxFIFO;  /*!< RX buffer management. */ 
    tsFIFO_BUF sTxFIFO;  /*!< TX buffer management. */

    tsDATALINK     sDatalink;
    // SCI_COMMANDS sciCommands;   /*!< Commands variable structure. */

}tsSCI_MASTER;

#define tsSCI_MASTER_DEFAULTS { \
    tsSCI_MASTER_VERSION_VALUE, \
    ePROTOCOL_IDLE, \
    {0},{0}, \
    tsFIFO_BUF_DEFAULTS, \
    tsFIFO_BUF_DEFAULTS, \
    tsDATALINK_DEFAULTS \
}

/******************************************************************************
 * Function declarations
 *****************************************************************************/
/** \brief Main state machine for the SCI master.
 * 
 * Handles the SCI protocol.
*/
void SCIMasterSM (tsSCI_MASTER *psSciMaster);

/** \brief Non blocking SCI data transmission.
 * 
 * Passes protocol handling to the SCIMasterSM state machine.
 * 
 * @param eCmdType  Command type to be sent
 * @param i16Num    Command number
 * @param uVal      Pointer to the values to be sent
*/
void _SCIMasterQueryNonBlocking (tsSCI_MASTER *psSciMaster, teREQUEST_TYPE eCmdType, int16_t i16CmdNum, tuREQUESTVALUE *uVal, int16_t i16ArgNum);

/** \brief Formulates the dataframe of an SCI Request.
 * 
 * @param pui8Buf       Pointer to the message buffer
 * @param pui8Size      Pointer to a variable that holds the actual byte count of the packet
 * @param sReq          Structure of type tsREQUEST holding all the relevant data
 * 
 * @returns Error indicator
*/
teSCI_ERROR _SCIMasterRequestBuilder(uint8_t *pui8Buf, uint8_t *pui8Size, tsREQUEST sReq);

/** \brief Parses the SCI response from the device.
 * 
 * @param pui8Buf       Pointer to the message buffer
 * @param ui8MsgSize    Size of the message to be analyzed 
 * @param pRsp          pointer to the response data structure
 * 
 * @returns Error indicator
*/
teSCI_ERROR _SCIMasterResponseParser(uint8_t* pui8Buf, uint8_t ui8MsgSize, tsRESPONSE *pRsp);



#endif //_SCIMASTER_H_