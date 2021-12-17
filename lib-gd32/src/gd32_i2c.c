/**
 * @file gd32_i2c.h
 *
 */
/* Copyright (C) 2021 by Arjan van Vught mailto:info@gd32-dmx.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>

#include "gd32_i2c.h"
#include "gd32.h"

#define TIMEOUT			0xfff

static uint8_t s_address;

static int32_t _sendstart(void) {
	int32_t timeout = TIMEOUT;

	/* wait until I2C bus is idle */
	while (i2c_flag_get(I2C_PERIPH, I2C_FLAG_I2CBSY)) {
		if (--timeout <= 0) {
			return -GD32_I2C_NOK_TOUT;
		}
	}

	timeout = TIMEOUT;

	/* send a start condition to I2C bus */
	i2c_start_on_bus(I2C_PERIPH);

	/* wait until SBSEND bit is set */
	while (!i2c_flag_get(I2C_PERIPH, I2C_FLAG_SBSEND)) {
		if (--timeout <= 0) {
			return -GD32_I2C_NOK_TOUT;
		}
	}

	return GD32_I2C_OK;
}

static int32_t _sendslaveaddr(void) {
	int32_t timeout = TIMEOUT;

	/* send slave address to I2C bus */
	i2c_master_addressing(I2C_PERIPH, s_address, I2C_TRANSMITTER);

	/* wait until ADDSEND bit is set */
	while (!i2c_flag_get(I2C_PERIPH, I2C_FLAG_ADDSEND)) {
		if (--timeout <= 0) {
			return -GD32_I2C_NOK_TOUT;
		}
	}

	/* clear the ADDSEND bit */
	i2c_flag_clear(I2C_PERIPH, I2C_FLAG_ADDSEND);

	timeout = TIMEOUT;

	/* wait until the transmit data buffer is empty */
	while (SET != i2c_flag_get(I2C_PERIPH, I2C_FLAG_TBE)) {
		if (--timeout <= 0) {
			return -GD32_I2C_NOK_TOUT;
		}
	}

	return GD32_I2C_OK;
}

int32_t _stop(void) {
	int32_t timeout = TIMEOUT;

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C_PERIPH);

    /* wait until the stop condition is finished */
    while(I2C_CTL0(I2C_PERIPH)&0x0200) {
		if (--timeout <= 0) {
			return -GD32_I2C_NOK_TOUT;
		}
    }

	return GD32_I2C_OK;
}

static int32_t _senddata(uint8_t *data_addr, uint32_t data_count) {
	int32_t timeout;
	uint32_t i;

	for (i = 0; i < data_count; i++) {
		i2c_data_transmit(I2C_PERIPH, *data_addr);

		/* point to the next byte to be written */
		data_addr++;

		timeout = TIMEOUT;

		/* wait until BTC bit is set */
		while (!i2c_flag_get(I2C_PERIPH, I2C_FLAG_BTC)) {
			if (--timeout <= 0) {
				return -GD32_I2C_NOK_TOUT;
			}
		}
	}

	return GD32_I2C_OK;
}

static int _write(char *buffer, int len) {
	int ret, ret0 = -1;

	ret = _sendstart();

	if (ret != GD32_I2C_OK) {
		goto i2c_write_err_occur;
	}

	ret = _sendslaveaddr();

	if (ret) {
		goto i2c_write_err_occur;
	}

	ret = _senddata((uint8_t*) buffer, (uint32_t) len);

	if (ret) {
		goto i2c_write_err_occur;
	}

	ret0 = 0;

i2c_write_err_occur: _stop();

	return ret0;
}

/*
 * Public API's
 */

void gd32_i2c_begin(void) {
	rcu_periph_clock_enable(I2C_RCU_CLK);
	rcu_periph_clock_enable(I2C_GPIO_SCL_CLK);
	rcu_periph_clock_enable(I2C_GPIO_SDA_CLK);
	gpio_init(I2C_GPIO_SCL_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
	gpio_init(I2C_GPIO_SDA_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
	i2c_clock_config(I2C_PERIPH, GD32_I2C_FULL_SPEED, I2C_DTCY_2);
	i2c_enable(I2C_PERIPH);
	i2c_ack_config(I2C_PERIPH, I2C_ACK_ENABLE);
}

void gd32_i2c_set_baudrate(uint32_t baudrate) {
	i2c_clock_config(I2C_PERIPH, baudrate, I2C_DTCY_2);
}

void gd32_i2c_set_address(uint8_t address) {
	s_address = address << 1;
}

uint8_t gd32_i2c_write(const char *buffer, uint32_t data_length) {
	const int32_t ret = _write((char *)buffer, (int) data_length);

	return (uint8_t)-ret;;
}

uint8_t gd32_i2c_read(char *buffer, uint32_t data_length) {
	int32_t timeout = TIMEOUT;

	/* wait until I2C bus is idle */
	while (i2c_flag_get(I2C_PERIPH, I2C_FLAG_I2CBSY)) {
		if (--timeout <= 0) {
			goto i2c_read_err_occur;
		}
	}

	if (2 == data_length) {
		i2c_ackpos_config(I2C_PERIPH, I2C_ACKPOS_NEXT);
	}

	/* send a start condition to I2C bus */
	i2c_start_on_bus(I2C_PERIPH);

	timeout = TIMEOUT;

	/* wait until SBSEND bit is set */
	while (!i2c_flag_get(I2C_PERIPH, I2C_FLAG_SBSEND)) {
		if (--timeout <= 0) {
			goto i2c_read_err_occur;
		}
	}

	/* send slave address to I2C bus */
	i2c_master_addressing(I2C_PERIPH, s_address, I2C_RECEIVER);

	if (data_length < 3) {
		/* disable acknowledge */
		i2c_ack_config(I2C_PERIPH, I2C_ACK_DISABLE);
	}

	timeout = TIMEOUT;

	/* wait until ADDSEND bit is set */
	while (!i2c_flag_get(I2C_PERIPH, I2C_FLAG_ADDSEND)) {
		if (--timeout <= 0) {
			goto i2c_read_err_occur;
		}
	}

	/* clear the ADDSEND bit */
	i2c_flag_clear(I2C_PERIPH, I2C_FLAG_ADDSEND);

	if (1 == data_length) {
		/* send a stop condition to I2C bus */
		i2c_stop_on_bus(I2C_PERIPH);
	}

	int32_t timeout_loop = TIMEOUT;

	/* while there is data to be read */
	while (data_length) {
		if (3 == data_length) {
			timeout = TIMEOUT;
			/* wait until BTC bit is set */
			while (!i2c_flag_get(I2C_PERIPH, I2C_FLAG_BTC)) {
				if (--timeout <= 0) {
					goto i2c_read_err_occur;
				}
			}

			/* disable acknowledge */
			i2c_ack_config(I2C_PERIPH, I2C_ACK_DISABLE);
		}

		if (2 == data_length) {
			timeout = TIMEOUT;

			/* wait until BTC bit is set */
			while (!i2c_flag_get(I2C_PERIPH, I2C_FLAG_BTC)) {
				if (--timeout <= 0) {
					goto i2c_read_err_occur;
				}
			}

			/* send a stop condition to I2C bus */
			i2c_stop_on_bus(I2C_PERIPH);
		}

		/* wait until the RBNE bit is set and clear it */
		if (i2c_flag_get(I2C_PERIPH, I2C_FLAG_RBNE)) {
			*buffer = i2c_data_receive(I2C_PERIPH);
			buffer++;
			data_length--;
			timeout_loop = TIMEOUT;
		}

		if (--timeout_loop <= 0) {
			goto i2c_read_err_occur;
		}
	}

	timeout = TIMEOUT;

	/* wait until the stop condition is finished */
	while (I2C_CTL0(I2C_PERIPH) & 0x0200) {
		if (--timeout <= 0) {
			return GD32_I2C_NOK_TOUT;
		}
	}

	/* enable acknowledge */
	i2c_ack_config(I2C_PERIPH, I2C_ACK_ENABLE);

	i2c_ackpos_config(I2C_PERIPH, I2C_ACKPOS_CURRENT);

	return GD32_I2C_OK;

i2c_read_err_occur: _stop();

	return GD32_I2C_NOK_TOUT;
}
