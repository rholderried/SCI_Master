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
#include "SCITransfer.h"
#include "Buffer.h"
#include "SCIDataLink.h"

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

    tsSCI_TRANSFER sSCITransfer;

}tsSCI_MASTER;

#define tsSCI_MASTER_DEFAULTS { \
    tsSCI_MASTER_VERSION_VALUE, \
    ePROTOCOL_IDLE, \
    {0},{0}, \
    tsFIFO_BUF_DEFAULTS, \
    tsFIFO_BUF_DEFAULTS, \
    tsDATALINK_DEFAULTS, \
    tsSCI_TRANSFER_DEFAULTS \
}

/******************************************************************************
 * Function declarations
 *****************************************************************************/
/** \brief Main state machine for the SCI master.
 * 
 * Handles the SCI protocol.
*/
void SCIMasterSM (void);

/** \brief Non blocking SCI data transmission.
 * 
 * Passes protocol handling to the SCIMasterSM state machine.
 * 
 * @param eCmdType  Command type to be sent
 * @param i16Num    Command number
 * @param uVal      Pointer to the values to be sent
*/
void _SCIMasterQueryNonBlocking (teREQUEST_TYPE eCmdType, int16_t i16CmdNum, tuREQUESTVALUE *uVal, int16_t i16ArgNum);

/** \brief High level receive routine.
 * 
 * @param pui8RecBuf    Pointer to the receive buffer or FIFO
 * @param ui8ByteCount  Number of bytes to process
*/
void SCIReceive (uint8_t *pui8RecBuf, uint8_t ui8ByteCount);

#endif //_SCIMASTER_H_