/*
 * capteurs.c
 *
 *  Created on: 4 mai 2016
 *      Author: a3quirat
 */
#include "terasic_includes.h"
#include "capteurs.h"
#include "I2C.h"
#include "accelerometer_adxl345.h"
#include "stdio.h"
#include "stdlib.h"
#include "zigbee.h"
#define LOW_BYTE_REG 0x04
#define HIGH_BYTE_REG 0x03
#define  ADDR_LUX_I2C_W 0x00
#define  ADDR_LUX_I2C_R 0x01
#define ADDR_LUX_I2C 0x4a
#define DATA_X0	0x33
#define DATA_Y0	0x35
#define DATA_Z0	0x37
#define REG_DATA_FORMAT 0x31
#define REG_POWER_CTL 0x2d
#define ADDR_ACC_I2C 	0x1d
#define ADDR_ACC_I2C_W	0x00
#define ADDR_ACC_I2C_R	0x01

#define REG_DATA_FORMAT 0x31
#define REG_POWER_CTL 0x2d
#define DATA_FORMAT 0x00 | XL345_SPI3WIRE | XL345_INT_HIGH | XL345_FULL_RESOLUTION | XL345_DATA_JUST_RIGHT | XL345_RANGE_2G
#define POWER_CTL 0x00 | XL345_MEASURE | XL345_WAKEUP_4HZ
void i2c_start(alt_u32 clk_base, alt_u32 data_base);
void i2c_stop(alt_u32 clk_base, alt_u32 data_base);
bool i2c_write(alt_u32 clk_base, alt_u32 data_base, alt_u8 Data);
void i2c_read(alt_u32 clk_base, alt_u32 data_base, alt_u8 *pData, bool bAck);

alt_u8 ADDR_ADXL_Write = 0x3A;
alt_u8 ADDR_ADXL_Read = 0x3B;


void temp(char * rep){

  i2c_start(I2C_SCL_BASE, I2C_EXT_SDA_BASE);
  if(i2c_write(I2C_SCL_BASE, I2C_EXT_SDA_BASE,(0x48<<1)+1))
  i2c_read(I2C_SCL_BASE,I2C_EXT_SDA_BASE ,rep ,TRUE);
  i2c_read(I2C_SCL_BASE,I2C_EXT_SDA_BASE ,rep+1 ,FALSE);

  i2c_stop(I2C_SCL_BASE, I2C_EXT_SDA_BASE);
}


////MAX44009

void light(char * rep) {
	alt_u8 init_reg2 = 0x83;
	write_lux(0x02, &init_reg2);
	read_lux(LOW_BYTE_REG, &rep[0]);
	read_lux(HIGH_BYTE_REG, &rep[1]);
}

bool write_lux(alt_u8 registre, alt_u8 * data) {
	i2c_start(I2C_SCL_BASE, I2C_EXT_SDA_BASE);

	if (!i2c_write(I2C_SCL_BASE, I2C_EXT_SDA_BASE,
			ADDR_LUX_I2C << 1 | ADDR_LUX_I2C_W)) {
		return FALSE;
	}

	if (!i2c_write(I2C_SCL_BASE, I2C_EXT_SDA_BASE, registre)) {
		return FALSE;
	}

	if (!i2c_write(I2C_SCL_BASE, I2C_EXT_SDA_BASE, *data)) {
		return FALSE;
	}

	i2c_stop(I2C_SCL_BASE, I2C_EXT_SDA_BASE);
	return TRUE;
}

bool read_lux(alt_u8 registre, alt_u8 * data) {
	i2c_start(I2C_SCL_BASE, I2C_EXT_SDA_BASE);

	// Slave addr write
	if (!i2c_write(I2C_SCL_BASE, I2C_EXT_SDA_BASE,
			ADDR_LUX_I2C << 1 | ADDR_LUX_I2C_W)) {
		return FALSE;
	}
	// Registre addr
	if (!i2c_write(I2C_SCL_BASE, I2C_EXT_SDA_BASE, registre)) {
		return FALSE;
	}
	i2c_start(I2C_SCL_BASE, I2C_EXT_SDA_BASE);
	// Slave addr read
	if (!i2c_write(I2C_SCL_BASE, I2C_EXT_SDA_BASE,
			ADDR_LUX_I2C << 1 | ADDR_LUX_I2C_R)) {
		return FALSE;
	}
	// Read
	i2c_read(I2C_SCL_BASE, I2C_EXT_SDA_BASE, data, FALSE);

	// Bit stop :
	i2c_stop(I2C_SCL_BASE, I2C_EXT_SDA_BASE);
	return TRUE;
}

void accel(char * rep){

	write_ACC(REG_DATA_FORMAT,DATA_FORMAT);
	write_ACC(REG_POWER_CTL,POWER_CTL);
	read_ACC(DATA_X0, &rep[0]);
	read_ACC(DATA_Y0, &rep[1]);
	read_ACC(DATA_Z0, &rep[2]);

}

bool write_ACC(alt_u8  registre, alt_u8  data)
{
	i2c_start(I2C_SCL_BASE, I2C_SDA_BASE);

	if(!i2c_write(I2C_SCL_BASE, I2C_SDA_BASE,ADDR_ACC_I2C<< 1 | ADDR_ACC_I2C_W)){
		return FALSE;
	}

	if(!i2c_write(I2C_SCL_BASE, I2C_SDA_BASE,registre)){
		return FALSE;
	}

	if(!i2c_write(I2C_SCL_BASE, I2C_SDA_BASE, data)){
		return FALSE;
	}

	i2c_stop(I2C_SCL_BASE, I2C_SDA_BASE);
	return TRUE;
}


bool read_ACC(alt_u8 registre, alt_u8 * data)
{
	i2c_start(I2C_SCL_BASE, I2C_SDA_BASE);

		// Slave addr write
		if(!i2c_write(I2C_SCL_BASE, I2C_SDA_BASE,ADDR_ACC_I2C<< 1 | ADDR_ACC_I2C_W)){
			return FALSE;
		}
		// Registre addr
		if(!i2c_write(I2C_SCL_BASE, I2C_SDA_BASE,registre)){
			return FALSE;
		}
		i2c_start(I2C_SCL_BASE, I2C_SDA_BASE);
		// Slave addr read
		if(!i2c_write(I2C_SCL_BASE, I2C_SDA_BASE,ADDR_ACC_I2C<< 1 | ADDR_ACC_I2C_R)){
				return FALSE;
		}
		// Read
		i2c_read(I2C_SCL_BASE, I2C_SDA_BASE, data, FALSE);

	// Bit stop :
	i2c_stop(I2C_SCL_BASE, I2C_SDA_BASE);
return TRUE;
}



/// ADC
void ADC_0 (alt_u16 SS, alt_u16 IN,alt_u16 *Data)
{			

	alt_u32 status=0;

	IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SPI_0_BASE, SS); 

	status = IORD_ALTERA_AVALON_SPI_STATUS(SPI_0_BASE);

	while ((status & ALTERA_AVALON_SPI_STATUS_TRDY_MSK) == 0);

	if ((status & ALTERA_AVALON_SPI_STATUS_TRDY_MSK))
	{
		IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_0_BASE,IN);
		IOWR_ALTERA_AVALON_SPI_TXDATA(SPI_0_BASE,IN);

		*Data = IORD_ALTERA_AVALON_SPI_RXDATA(SPI_0_BASE);


	}
	else {printf("Error\n");}
}


