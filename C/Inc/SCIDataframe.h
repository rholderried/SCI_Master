/**************************************************************************//**
 * \file SCIDataframe.h
 * \author Roman Holderried
 *
 * \brief SCI Dataframe parser declarations and definitions.
 *
 * <b> History </b>
 * 	- 2022-11-21 - File creation -
 *****************************************************************************/


#ifndef _SCIDATAFRAME_H_
#define _SCIDATAFRAME_H_

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "SCICommon.h"
#include "SCITransfer.h"

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

#define REQUEST_ACKNOWLEDGE_NOT_FOUND   -1
// #define REQUEST_ACKNOWLEDGE_SUCCESS     0
// #define REQUEST_ACKNOWLEDGE_DATA        1
// #define REQUEST_ACKNOWLEDGE_UPSTREAM    2
// #define REQUEST_ACKNOWLEDGE_ERROR       3
// #define REQUEST_ACKNOWLEDGE_UNKNOWN     4

/******************************************************************************
 * Type definitions
 *****************************************************************************/

/******************************************************************************
 * Function declarations
 *****************************************************************************/

/** \brief Formulates the dataframe of an SCI Request.
 * 
 * @param pui8Buf       Pointer to the message buffer
 * @param pui8Size      Pointer to a variable that holds the actual byte count of the packet
 * @param sReq          Structure of type tsREQUEST holding all the relevant data
 * 
 * @returns Error indicator
*/
teSCI_ERROR SCIMasterRequestBuilder(uint8_t *pui8Buf, uint8_t *pui8Size, tsREQUEST sReq);

/** \brief Parses the SCI response from the device.
 * 
 * @param pui8Buf       Pointer to the message buffer
 * @param ui8MsgSize    Size of the message to be analyzed 
 * @param pRsp          pointer to the response data structure
 * 
 * @returns Error indicator
*/
teSCI_ERROR SCIMasterResponseParser(uint8_t* pui8Buf, uint8_t ui8MsgSize, tsRESPONSE *pRsp);

int16_t _CheckAcknowledge (uint8_t *pui8Buf, uint8_t ui8BytesToGo);



#endif //_SCIDATAFRAME_H_