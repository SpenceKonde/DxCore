/******************************************************************************
* © 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
* PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL,
* PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY
* KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
* HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
 *****************************************************************************/
#ifndef TWI_DRIVER_H
#define TWI1_DRIVER_H

#include "avr/io.h"

/*! Transaction status defines. */
#ifndef TWIM_STATUS_READY
#define TWIM_STATUS_READY              0
#endif
#ifndef TWIM_STATUS_BUSY
#define TWIM_STATUS_BUSY               1
#endif

/* Transaction status defines.*/
#ifndef TWIS_STATUS_READY
#define TWIS_STATUS_READY                0
#endif
#ifndef TWIS_STATUS_BUSY
#define TWIS_STATUS_BUSY                 1
#endif

/*! Transaction result enumeration. */
typedef enum __attribute__((packed)) TWIM_RESULT_enum {
  TWIM_RESULT_UNKNOWN          = (0x00<<0),
  TWIM_RESULT_OK               = (0x01<<0),
  TWIM_RESULT_BUFFER_OVERFLOW  = (0x02<<0),
  TWIM_RESULT_ARBITRATION_LOST = (0x03<<0),
  TWIM_RESULT_BUS_ERROR        = (0x04<<0),
  TWIM_RESULT_NACK_RECEIVED    = (0x05<<0),
  TWIM_RESULT_FAIL             = (0x06<<0),
} TWIM_RESULT_t;

/* Transaction result enumeration */
typedef enum __attribute__((packed)) TWIS_RESULT_enum {
  TWIS_RESULT_UNKNOWN            = (0x00<<0),
  TWIS_RESULT_OK                 = (0x01<<0),
  TWIS_RESULT_BUFFER_OVERFLOW    = (0x02<<0),
  TWIS_RESULT_TRANSMIT_COLLISION = (0x03<<0),
  TWIS_RESULT_BUS_ERROR          = (0x04<<0),
  TWIS_RESULT_FAIL               = (0x05<<0),
  TWIS_RESULT_ABORTED            = (0x06<<0),
} TWIS_RESULT_t;

/*! TWI Modes */
typedef enum __attribute__((packed)) TWI_MODE_enum {
  TWI_MODE_UNKNOWN = 0,
  TWI_MODE_MASTER = 1,
  TWI_MODE_SLAVE = 2,
  TWI_MODE_MASTER_TRANSMIT = 3,
  TWI_MODE_MASTER_RECEIVE = 4,
  TWI_MODE_SLAVE_TRANSMIT = 5,
  TWI_MODE_SLAVE_RECEIVE = 6
} TWI_MODE_t;

/*! For adding R/_W bit to address */
#ifndef ADD_READ_BIT
#define ADD_READ_BIT(address) (address | 0x01)
#endif
#ifndef ADD_WRITE_BIT
#define ADD_WRITE_BIT(address)  (address & ~0x01)
#endif

void TWI1_MasterInit(uint32_t frequency);
void TWI1_SlaveInit(uint8_t address,uint8_t receive_broadcast,uint8_t second_address);
void TWI1_Flush(void);
void TWI1_Disable(void);
TWI_BUSSTATE_t TWI1_MasterState(void);
uint8_t TWI1_MasterReady(void);
void TWI1_MasterSetBaud(uint32_t frequency);
uint8_t TWI1_MasterWrite(uint8_t slave_address,
                     uint8_t *write_data,
                     uint8_t bytes_to_write,
           uint8_t send_stop);
uint8_t TWI1_MasterRead(uint8_t slave_address,
                    uint8_t* read_data,
                    uint8_t bytes_to_read,
          uint8_t send_stop);
uint8_t TWI1_MasterWriteRead(uint8_t slave_address,
                         uint8_t *write_data,
                         uint8_t bytes_to_write,
                         uint8_t bytes_to_read,
             uint8_t send_stop);
void TWI1_MasterInterruptHandler(void);
void TWI1_MasterArbitrationLostBusErrorHandler(void);
void TWI1_MasterWriteHandler(void);
void TWI1_MasterReadHandler(void);
void TWI1_MasterTransactionFinished(uint8_t result);

void TWI1_SlaveInterruptHandler(void);
void TWI1_SlaveAddressMatchHandler(void);
void TWI1_SlaveStopHandler(void);
void TWI1_SlaveDataHandler(void);
void TWI1_SlaveWriteHandler(void);
void TWI1_SlaveReadHandler(void);
void TWI1_attachSlaveRxEvent( void (*function)(int), uint8_t *read_data, uint8_t bytes_to_read );
void TWI1_attachSlaveTxEvent( uint8_t (*function)(void), uint8_t *write_data );
void TWI1_SlaveTransactionFinished(uint8_t result);
/*! TWI master interrupt service routine.
 *
 *  Interrupt service routine for the TWI master. Copy the needed vectors
 *  into your code.
 *


  ISR(TWI1_TWIM_vect){
    TWI1_MasterInterruptHandler();
  }

  ISR(TWI1_TWIS_vect){
    TWI1_SlaveInterruptHandler();
  }

 *
 */


#endif /* TWI1_DRIVER_H */
