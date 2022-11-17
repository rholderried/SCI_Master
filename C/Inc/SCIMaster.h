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
#include "SCICommands.h"
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
}tSCI_MASTER_VERSION;

typedef enum
{
    ePROTOCOL_ERROR         = -1,
    ePROTOCOL_IDLE          = 0,
    ePROTOCOL_SENDING       = 1,
    ePROTOCOL_EVALUATING    = 2,
    ePROTOCOL_RECEIVING     = 3,
}tePROTOCOL_STATE;

typedef enum
{
    eSCI_ERROR_NONE = 0,
    eSCI_ERROR_TRANSMISSION_ERROR,
    eSCI_ERROR_RECEIVE_ERROR,
    eSCI_ERROR_DATAFRAME_ERROR

    // eSCI_ERROR_
}teSCI_ERROR;


typedef struct
{
    void (*commandCallback)(teCOMMAND_TYPE eCmdType, int16_t i16CmdNum, tuCOMMANDVALUE *uVal, int16_t i16ArgNum);

}tsSCI_MASTER_CALLBACKS;

typedef struct
{
    tSCI_MASTER_VERSION sVersion;
    tePROTOCOL_STATE eProtocolState;

    uint8_t ui8RxBuffer[RX_PACKET_LENGTH];   /*!< RX buffer space. */ 
    uint8_t ui8TxBuffer[TX_PACKET_LENGTH];   /*!< TX buffer space. */ 

    tsFIFO_BUF sRxFIFO;  /*!< RX buffer management. */ 
    tsFIFO_BUF sTxFIFO;  /*!< TX buffer management. */

    tsDATALINK     sDatalink;
    // SCI_COMMANDS sciCommands;   /*!< Commands variable structure. */

}tsSCI_MASTER;

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
void SCIMasterQueryNonBlocking (teCOMMAND_TYPE eCmdType, int16_t i16CmdNum, tuCOMMANDVALUE *uVal, int16_t i16ArgNum);


#endif //_SCIMASTER_H_