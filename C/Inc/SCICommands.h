/**************************************************************************//**
 * \file SCICommands.h
 * \author Roman Holderried
 *
 * \brief SCI command processing.
 *
 * <b> History </b>
 * 	- 2022-11-17 - File creation -
 *****************************************************************************/


#ifndef _SCICOMMANDS_H_
#define _SCICOMMANDS_H_

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

#define MAX_NUM_COMMAND_VALUES 10

/******************************************************************************
 * Type definitions
 *****************************************************************************/
typedef enum
{
    eSCI_COMMAND_ERROR_NONE = 0,
}teCOMMAND_ERROR;

typedef union
{
    float           f_float;
    uint32_t        ui32_hex;
}tuCOMMANDVALUE;


/** \brief Command type enumeration.*/
typedef enum 
{
    eCOMMAND_TYPE_NONE          = 0,
    eCOMMAND_TYPE_GETVAR        = 1,
    eCOMMAND_TYPE_SETVAR        = 2,
    eCOMMAND_TYPE_COMMAND       = 3,
    eCOMMAND_TYPE_UPSTREAM      = 4,
    eCOMMAND_TYPE_DOWNSTREAM    = 5
}teCOMMAND_TYPE;


/** \brief Command structure declaration.*/
typedef struct
{
    int16_t         i16_num;            /*!< ID Number.*/
    uint8_t         ui8_valArrLen;      /*!< Length of the value Array.*/
    tuCOMMANDVALUE  valArr[MAX_NUM_COMMAND_VALUES];    /*!< Pointer to the value array.*/
    teCOMMAND_TYPE  e_cmdType;          /*!< Command Type.*/
}COMMAND;

#define COMMAND_DEFAULT         {0, 0, {{.ui32_hex = 0}}, eCOMMAND_TYPE_NONE}

/** \brief Response structure declaration.*/
typedef struct
{
    // bool                b_valid;    /*!< Flags if response is valid and can be sent.*/
    int16_t             i16_num;    /*!< ID Number. (Reflects Command ID number).*/
    union 
    {
        float           f_float;    
        uint32_t        ui32_hex;   
    }val;                           /*!< Response value.*/
    teCOMMAND_TYPE      e_cmdType;  /*!< Response type inherited from Command type.*/
    COMMAND_CB_STATUS   e_cmdStatus;/*!< Status returned by the command callback.*/
    PROCESS_INFO        info;       /*!< Additional command processing info.*/
}RESPONSE;

#define RESPONSE_DEFAULT         {/*false,*/ 0, {.ui32_hex = 0}, eCOMMAND_TYPE_NONE, eCOMMAND_STATUS_UNKNOWN, PROCESS_INFO_DEFAULT}

/******************************************************************************
 * Function declarations
 *****************************************************************************/


#endif //_SCICOMMANDS_H_