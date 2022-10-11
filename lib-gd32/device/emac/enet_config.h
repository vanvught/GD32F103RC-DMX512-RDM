/**
 * enet_config.h
 *
 */
/* Copyright (C) 2022 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef ENET_CONFIG_H_
#define ENET_CONFIG_H_

#if !defined (GD32_H_)
# error gd32.h should be included first
#endif

#if(PHY_TYPE == LAN8700)

#elif(PHY_TYPE == DP83848)
# define PHY_REG_MICR				0x11U
# define PHY_REG_MISR				0x12U
# define PHY_INT_AND_OUTPUT_ENABLE	0x03U
# define PHY_LINK_INT_ENABLE		0x20U
#elif(PHY_TYPE == RTL8201F)
# define PHY_REG_IER				0x13
# define PHY_REG_IER_INT_ENABLE		BIT(13)
# define PHY_REG_ISR				0x1e
# define PHY_REG_ISR_LINK			BIT(11)
# define PHY_REG_PAGE_SELECT		0x1f
#else
#error PHY_TYPE is not set
#endif

#if defined (BOARD_GD32F103RC)
# include "enet_config_common.h"
#elif defined (BOARD_GD32F107RC)
# include "enet_config_common.h"
#elif defined (BOARD_GD32F207RG)
# include "enet_config_common.h"
#elif defined (BOARD_GD32F207VC)
# include "enet_config_common.h"
#elif defined (BOARD_GD32F303RC)
# include "enet_config_common.h"
#elif defined (BOARD_GD32F407RE)
# include "enet_config_common.h"
#elif defined (BOARD_GD32F450VE)
# include "enet_config_common.h"
#elif defined (BOARD_GD32F450VI)
# include "enet_config_common.h"
#elif defined (BOARD_GD32F207C_EVAL)
# include "enet_config_common.h"
#elif defined (BOARD_BW_OPIDMX4)
# include "enet_config_common.h"
#else
# error Board is unknown / not defined
#endif

#endif /* ENET_CONFIG_H_ */
