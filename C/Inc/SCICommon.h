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
#define EEPROM_BYTE_ADDRESSABLE      1
#define EEPROM_WORD_ADDRESSABLE      2
#define EEPROM_LONG_ADDRESSABLE      4

#define SCI_RECEIVE_MODE_TRANSFER   0
#define SCI_RECEIVE_MODE_STREAM     1

/******************************************************************************
 * Type definitions
 *****************************************************************************/
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




#endif //SCICOMMON_H_