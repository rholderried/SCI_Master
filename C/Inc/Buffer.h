/**************************************************************************//**
 * \file Buffer.h
 * \author Roman Holderried
 *
 * \brief Functions for controlling data traffic from and into a memory space.
 * 
 * Needs an externally defined buffer space (array), to which the address must
 * be passed to the constructor.
 *
 * <b> History </b>
 * 	- 2022-11-17 - Copy from SCI
 *****************************************************************************/

#ifndef _BUFFER_H_
#define _BUFFER_H_

/******************************************************************************
 * Includes
 *****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/******************************************************************************
 * Type definitions
 *****************************************************************************/
    
typedef struct
{
    uint8_t     *pui8_bufPtr;   /*!< Pointer to the external buffer. */
    int16_t     i16_bufIdx;     /*!< Actual buffer index (Last written index). */
    uint8_t     ui8_bufLen;     /*!< Length of the buffer array. */
    uint8_t     ui8_bufSpace;   /*!< Actual remaining buffer space. */
    bool        b_ovfl;         /*!< Overflow indicator. */
}tsFIFO_BUF;

#define FIFO_BUF_DEFAULT {NULL, 0, 0, 0, false}

/******************************************************************************
 * Function declaration
 *****************************************************************************/
/** \brief Initializes the buffer structure
 *
 * @param *p_inst   Pointer to the buffer data structure
 * @param *pui8_buf Pointer to the start of the buffer space
 * @param ui8_bufLen Desired length of the buffer
 */
void fifoBufInit(tsFIFO_BUF* p_inst, uint8_t *pui8_buf, uint8_t ui8_bufLen);

/** \brief Puts one byte into the buffer
 *
 * @param data Data byte
 */
void putElem(tsFIFO_BUF* p_inst, uint8_t ui8_data);

/** \brief Buffer read operation
 *
 * This routine receives the address of a pointer variable, which gets moved
 * to the start of the buffer.
 * 
 * @param   **pui8_target Pointer address.
 * @returns Size of the stored data in bytes.
 */
uint8_t readBuf (tsFIFO_BUF* p_inst, uint8_t **pui8_target);

/** \brief Empties the buffer
 *
 * Sets the buffer index to -1 (start value) and the actual buffer Space
 * to the buffer length. The buffer contents hence are "invalidated".
 */
void flushBuf (tsFIFO_BUF* p_inst);

/** \brief Sets the input pointer to the next free buffer address
 * 
 * @param   **pui8_target Pointer address.
 * @returns True if there was free buffer space available, false otherwise
 */
bool getNextFreeBufSpace(tsFIFO_BUF* p_inst, uint8_t **pui8_target);

/** \brief Increases the buffer index.
 *
 * @param   ui8_size counts about which to increase the index.
 * @returns True if the operation was successful, false otherwise.
 */
bool increaseBufIdx(tsFIFO_BUF* p_inst, uint8_t ui8_size);

/** \brief Returns the actual (last written) buffer index.
 *
 * @returns actual buffer index.
 */
int16_t getActualIdx(tsFIFO_BUF* p_inst);

#endif