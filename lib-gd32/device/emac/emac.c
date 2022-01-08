/**
 * emac.c
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

//TODO This file needs some more cleanup

#include "gd32.h"

#include "debug.h"

#if(PHY_TYPE == RTL8201F)
# define ENET_MEDIAMODE		ENET_100M_FULLDUPLEX
#else
# define ENET_MEDIAMODE		ENET_AUTO_NEGOTIATION
#endif

extern enet_descriptors_struct  txdesc_tab[ENET_TXBUF_NUM];
extern enet_descriptors_struct  *dma_current_rxdesc;

static __IO uint32_t enet_init_status = 0;

static void enet_gpio_config(void) {
	DEBUG_ENTRY

	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

    /* enable SYSCFG clock */
    rcu_periph_clock_enable(RCU_AF);

    rcu_pll2_config(RCU_PLL2_MUL10);
    rcu_osci_on(RCU_PLL2_CK);
    rcu_osci_stab_wait(RCU_PLL2_CK);
    /* get 50MHz from CK_PLL2 on CKOUT0 pin (PA8) to clock the PHY */
#if defined (GD32F10X_CL)
    rcu_ckout0_config(RCU_CKOUT0SRC_CKPLL2);
#else
    rcu_ckout0_config(RCU_CKOUT0SRC_CKPLL2,RCU_CKOUT0_DIV1);
#endif
    gpio_ethernet_phy_select(GPIO_ENET_PHY_RMII);


    /* PA1: ETH_RMII_REF_CLK */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    /* PA2: ETH_MDIO */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    /* PA7: ETH_RMII_CRS_DV */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* PC1: ETH_MDC */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    /* PC4: ETH_RMII_RXD0 */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    /* PC5: ETH_RMII_RXD1 */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

    /* PB11: ETH_RMII_TX_EN */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    /* PB12: ETH_RMII_TXD0 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    /* PB13: ETH_RMII_TXD1 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);

    DEBUG_EXIT
}

static void enet_mac_dma_config(void) {
	DEBUG_ENTRY

	ErrStatus reval_state = ERROR;

	rcu_periph_clock_enable(RCU_ENET);
	rcu_periph_clock_enable(RCU_ENETTX);
	rcu_periph_clock_enable(RCU_ENETRX);

	enet_deinit();

	reval_state = enet_software_reset();

	if (reval_state == ERROR) {
		while (1) {
		}
	}

    enet_init_status = enet_init(ENET_MEDIAMODE, ENET_AUTOCHECKSUM_DROP_FAILFRAMES, ENET_RECEIVEALL);
}

static void enet_system_setup(void) {
	enet_gpio_config();

	enet_mac_dma_config();

	if (enet_init_status == 0) {
		while (1) {
		}
	}
}

int emac_start(uint8_t mac_address[]) {
	DEBUG_ENTRY
#if(PHY_TYPE == LAN8700)
	DEBUG_PUTS("LAN8700");
#elif(PHY_TYPE == DP83848)
	DEBUG_PUTS("DP83848");
#elif(PHY_TYPE == RTL8201F)
	DEBUG_PUTS("RTL8201F");
#else
#error PHY_TYPE is not set
#endif
	DEBUG_PRINTF("ENET_RXBUF_NUM=%u, ENET_TXBUF_NUM=%u", ENET_RXBUF_NUM, ENET_TXBUF_NUM);

	enet_system_setup();

	const uint32_t mac_lo = *(volatile uint32_t *)(0x1FFFF7EC);
	const uint32_t mac_hi = *(volatile uint32_t *)(0x1FFFF7F0);

	mac_address[0] = 2;
	mac_address[1] = (mac_lo >> 8) & 0xff;
	mac_address[2] = (mac_lo >> 16) & 0xff;
	mac_address[3] = (mac_lo >> 24) & 0xff;
	mac_address[4] = (mac_hi >> 0) & 0xff;
	mac_address[5] = (mac_hi >> 8) & 0xff;

#ifndef NDEBUG
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);
#endif

	enet_mac_address_set(ENET_MAC_ADDRESS0, mac_address);

	enet_descriptors_chain_init(ENET_DMA_TX);
	enet_descriptors_chain_init(ENET_DMA_RX);

	int i;
	for (i = 0; i < ENET_TXBUF_NUM; i++) {
		enet_transmit_checksum_config(&txdesc_tab[i], ENET_CHECKSUM_TCPUDPICMP_FULL);
	}

	enet_enable();

	DEBUG_EXIT
	return 1;
}

int emac_eth_recv(uint8_t **packet) {
	const uint32_t size = enet_rxframe_size_get();

	if (size > 0) {
		*packet = (uint8_t*) (enet_desc_information_get(dma_current_rxdesc, RXDESC_BUFFER_1_ADDR));
		return size;

	}

	return -1;
}

void emac_free_pkt(void) {
	ENET_NOCOPY_FRAME_RECEIVE();
}

void emac_eth_send(void *packet, int len) {
	enet_frame_transmit (packet, len);
}
