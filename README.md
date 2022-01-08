![GitHub](https://img.shields.io/github/license/vanvught/GD32F103RC-DMX512-RDM)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://img.shields.io/badge/C%2B%2B-11%-blue.svg)
![GitHub issues](https://img.shields.io/github/issues-raw/vanvught/GD32F103RC-DMX512-RDM)
![GitHub contributors](https://img.shields.io/github/contributors/vanvught/GD32F103RC-DMX512-RDM)
![GitHub Sponsors](https://img.shields.io/github/sponsors/vanvught)
![Main](https://github.com/vanvught/GD32F103RC-DMX512-RDM/actions/workflows/c-cpp.yml/badge.svg?branch=main)

# GD32F103RC-DMX512-RDM
The master source code is available here -> [https://github.com/vanvught/rpidmx512](https://github.com/vanvught/rpidmx512)

A development board is available here -> [https://github.com/vanvught/GD32F103R-GD32F107R-GD32F207R-dev-board](https://github.com/vanvught/GD32F103R-GD32F107R-GD32F207R-dev-board)

Full documentation will be available here -> [https://www.gd32-dmx.org](https://www.gd32-dmx.org)

[PayPal.Me Donate](https://paypal.me/AvanVught?locale.x=nl_NL)

### DMX USB Pro
Open source GD32F103RC RDM Controller with USB, DMX512 isolated board and compatible with software that supports Enttec USB Pro.

A DMX512 RDM isolated with USB (FT245RL) extenstion board can be ordered here [https://www.bitwizard.nl/shop/raspberry-pi?product_id=154](https://www.bitwizard.nl/shop/raspberry-pi?product_id=154) -> Option: Model Orange Pi for use with the development board.

### RDM Responder Pixel Controller
[Supported PIDs](RDM.md)

Personalities :

1. Pixel Controller
2. Config mode

Slot description in `Config mode`:

	$ ola_rdm_get --universe 1 --uid 5000:30133331 SLOT_DESCRIPTION 0
	Slot Number: 0
	Name: Type
	$ ola_rdm_get --universe 1 --uid 5000:30133331 SLOT_DESCRIPTION 1
	Slot Number: 1
	Name: Count
	$ ola_rdm_get --universe 1 --uid 5000:30133331 SLOT_DESCRIPTION 2
	Slot Number: 2
	Name: Grouping Count
	$ ola_rdm_get --universe 1 --uid 5000:30133331 SLOT_DESCRIPTION 3
	Slot Number: 3
	Name: Map
	$ ola_rdm_get --universe 1 --uid 5000:30133331 SLOT_DESCRIPTION 4
	Slot Number: 4
	Name: Test Pattern
	$ ola_rdm_get --universe 1 --uid 5000:30133331 SLOT_DESCRIPTION 5
	Slot Number: 5
	Name: Program

The configuration is stored in the flash ROM when the `Slot Number 5` value changes from 0 into 255.

The `Config mode` is also activated when GPIOB Pin 14 (GPIO Header pin 15) is connected to GND on reset.