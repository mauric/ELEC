

#ifndef I2C_H_
#define I2C_H_

#include <stdio.h>
#include <stdlib.h> //
#include <string.h>
#include <stddef.h>
#include <unistd.h>  //
#include "terasic_includes.h"
#include <fcntl.h>

//#include "debug.h"


void i2c_start(alt_u32 clk_base, alt_u32 data_base);
void i2c_stop(alt_u32 clk_base, alt_u32 data_base);
bool i2c_write(alt_u32 clk_base, alt_u32 data_base, alt_u8 Data);
void i2c_read(alt_u32 clk_base, alt_u32 data_base, alt_u8 *pData, bool bAck);

#endif /*TERASIC_INCLUDES_H_*/
