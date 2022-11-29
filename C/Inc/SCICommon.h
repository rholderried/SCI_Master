/**************************************************************************//**
 * \file SCICommon.h
 * \author Roman Holderried
 *
 * \brief Common defines and type definitions for the SCI.
 *
 * <b> History </b>
 * 	- 2022-11-17 - File copied from SCI
 *****************************************************************************/

#ifndef SCICOMMON_H_
#define SCICOMMON_H_
/******************************************************************************
 * Includes
 *****************************************************************************/

/******************************************************************************
 * defines
 *****************************************************************************/

/******************************************************************************
 * Type definitions
 *****************************************************************************/

/** \brief SCI Master errors */
typedef enum
{
    eSCI_ERROR_NONE = 0,
    eSCI_ERROR_VAR_NUMBER_INVALID,
    eSCI_ERROR_UNKNOWN_DATATYPE,
    eSCI_ERROR_COMMAND_IDENTIFIER_NOT_FOUND,
    eSCI_ERROR_NUMBER_CONVERSION_FAILED,
    eSCI_ERROR_ACKNOWLEDGE_UNKNOWN,
    eSCI_ERROR_PARAMETER_CONVERSION_FAILED,
    eSCI_ERROR_EXPECTED_DATALENGTH_NOT_MET,
    eSCI_ERROR_MESSAGE_EXCEEDS_TX_BUFFER_SIZE,
    eSCI_ERROR_FEATURE_NOT_IMPLEMENTED
}teSCI_ERROR;

/** \brief Request acknowledge enumeration */
typedef enum
{
    eREQUEST_ACK_STATUS_SUCCESS             = 0,
    eREQUEST_ACK_STATUS_SUCCESS_DATA        = 1,
    eREQUEST_ACK_STATUS_SUCCESS_UPSTREAM    = 2,
    eREQUEST_ACK_STATUS_ERROR               = 3,
    eREQUEST_ACK_STATUS_UNKNOWN             = 4
}teREQUEST_ACKNOWLEDGE;

/** \brief Return value of the Transfer callbacks*/
typedef enum
{
    eTRANSFER_ACK_SUCCESS = 0,
    eTRANSFER_ACK_REPEAT_REQUEST,
    eTRANSFER_ACK_ABORT
}teTRANSFER_ACK;




#endif //SCICOMMON_H_