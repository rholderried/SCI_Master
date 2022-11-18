/**************************************************************************//**
 * \file SCIRequests.h
 * \author Roman Holderried
 *
 * \brief SCI command processing.
 *
 * <b> History </b>
 * 	- 2022-11-17 - File creation -
 *****************************************************************************/


#ifndef _SCIREQUESTS_H_
#define _SCIREQUESTS_H_

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "SCIMasterConfig.h"
#include "SCICommon.h"
#include "CommandStucture.h"

/******************************************************************************
 * Defines
 *****************************************************************************/

#define MAX_NUM_REQUEST_VALUES  10
#define MAX_NUM_RESPONSE_VALUES 10

/******************************************************************************
 * Type definitions
 *****************************************************************************/
typedef enum
{
    eSCI_REQUEST_ERROR_NONE = 0,
}teREQUEST_ERROR;

typedef union
{
    float           f_float;
    uint32_t        ui32_hex;
}tuREQUESTVALUE;

typedef tuREQUESTVALUE tuRESPONSEVALUE;


/** \brief Request type enumeration.*/
typedef enum 
{
    eREQUEST_TYPE_NONE          = 0,
    eREQUEST_TYPE_GETVAR        = 1,
    eREQUEST_TYPE_SETVAR        = 2,
    eREQUEST_TYPE_COMMAND       = 3,
    eREQUEST_TYPE_UPSTREAM      = 4,
    eREQUEST_TYPE_DOWNSTREAM    = 5
}teREQUEST_TYPE;

/** \brief Request acknowledge enumeration */
typedef enum
{
    eREQUEST_ACK_STATUS_SUCCESS             = 0,
    eREQUEST_ACK_STATUS_SUCCESS_DATA        = 1,
    eREQUEST_ACK_STATUS_SUCCESS_UPSTREAM    = 2,
    eREQUEST_ACK_STATUS_ERROR               = 3,
    eREQUEST_ACK_STATUS_UNKNOWN             = 4
}teREQUEST_ACKNOWLEDGE;

/** \brief REQUEST structure declaration.*/
typedef struct
{
    int16_t         i16Num;                            /*!< ID Number.*/
    teREQUEST_TYPE  eReqType;                          /*!< REQUEST Type.*/
    tuREQUESTVALUE  uValArr[MAX_NUM_REQUEST_VALUES];   /*!< Pointer to the value array.*/
    uint8_t         ui8ValArrLen;                      /*!< Length of the value Array.*/
}tsREQUEST;

#define tsREQUEST_DEFAULT         {0, 0, {.ui32_hex = 0}, eREQUEST_TYPE_NONE}

/** \brief Response structure declaration.*/
typedef struct
{
    int16_t                 i16Num;                             /*!< ID Number. (Reflects REQUEST ID number).*/
    teREQUEST_TYPE          eReqType;                           /*!< Response type inherited from REQUEST type.*/
    teREQUEST_ACKNOWLEDGE   eReqAck;                            /*!< Acknowledge returned by the REQUEST callback.*/
    tuRESPONSEVALUE         uValArr[MAX_NUM_RESPONSE_VALUES];   /*!< Pointer to the value array.*/                           /*!< Response value.*/
    uint16_t                ui16ErrNum;                         /*!< Returned error number */
    uint32_t                ui32DataLength;                     /*!< Length of the data to follow */
    
}tsRESPONSE;

#define tsRESPONSE_DEFAULT         {0, eREQUEST_TYPE_NONE, eREQUEST_ACK_STATUS_UNKNOWN, {.ui32_hex = 0}, 0, 0}

/******************************************************************************
 * Function declarations
 *****************************************************************************/


#endif //_SCIREQUESTS_H_