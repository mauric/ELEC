#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_



#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <stddef.h>
#include <unistd.h>  // usleep (unix standard?)
#include "sys/alt_flash.h"
#include "sys/alt_flash_types.h"
#include "io.h"
#include "alt_types.h"  // alt_u32
#include "altera_avalon_pio_regs.h" //IOWR_ALTERA_AVALON_PIO_DATA
#include "sys/alt_irq.h"  // interrupt
#include "sys/alt_alarm.h" // time tick function (alt_nticks(), alt_ticks_per_second())
#include "sys/alt_timestamp.h"
#include "sys/alt_stdio.h"
#include "system.h"
#include "terasic_includes.h"
#include <fcntl.h>
#include "I2C.h"
#include <altera_avalon_spi.h>
#include <altera_avalon_spi_regs.h>


void single_byte_write( alt_u32 data_base,alt_u8 SlaveAdress,alt_u8 reg_adresse,alt_u8 Mask);

void read_single_byte(alt_u32 data_base,alt_u8 SlaveAdress,alt_u8 reg_adresse,alt_u8 *pData);

void multiple_read( alt_u32 data_base,alt_u8 SlaveAdress,alt_u8 reg_adresse,alt_u8 *pData,alt_u8 nb_reg);

void temp(char *);
void printSerial();
void ADC_0 (alt_u16 SS, alt_u16 IN,alt_u16 *Data);


// accelerometer
void Read_Lux(alt_u8 SlaveAdress ,alt_u8 RegLuxLow,alt_u8 RegLuxHigh,alt_u8 *Datalux);
bool write_lux(alt_u8  registre, alt_u8 * data);
bool read_lux(alt_u8 registre, alt_u8 * data);
bool write_ACC(alt_u8  registre, alt_u8  data);
bool read_ACC(alt_u8 registre, alt_u8 * data);
void accel(char * rep);
void light(char * rep);








#endif
