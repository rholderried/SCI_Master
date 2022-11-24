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

#define GETVAR_IDENTIFIER       '?'
#define SETVAR_IDENTIFIER       '!'
#define COMMAND_IDENTIFIER      ':'
#define UPSTREAM_IDENTIFIER     '>'
#define DOWNSTREAM_IDENTIFIER   '<'

#define REQUEST_ACKNOWLEDGE_NOT_FOUND   -1

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

/** \brief Parses the SCI response from the device (transfer).
 * 
 * @param pui8Buf       Pointer to the message buffer
 * @param ui8MsgSize    Size of the message to be analyzed 
 * @param pRsp          pointer to the response data structure
 * 
 * @returns Error indicator
*/
teSCI_ERROR SCIMasterResponseParser(uint8_t* pui8Buf, uint8_t ui8MsgSize, tsRESPONSE *pRsp);

/** \brief Parses the SCI response from the device (stream).
 * 
 * @param pui8Buf       Pointer to the message buffer
 * @param ui8MsgSize    Size of the message to be analyzed 
 * @param pRsp          pointer to the response data structure
 * 
 * @returns Error indicator
*/
teSCI_ERROR SCIMasterStreamParser(uint8_t* pui8Buf, uint8_t ui8MsgSize, tsRESPONSE *pRsp);

/** \brief Internal function to check the acknowledge string of the messages
 * 
 * The function expects the acknowledge string at the beginning of the buffer segment.
 * 
 * @param pui8Buf       Pointer to the Buffer segment to analyse
 * @param ui8BytesToGo  Remaining characters to the end of the buffer segment
 * 
 * @returns Acknowledge identificator (-1 if no Acknowledge was found). 
 * */
int16_t _CheckAcknowledge (uint8_t *pui8Buf, uint8_t ui8BytesToGo);



#endif //_SCIDATAFRAME_H_