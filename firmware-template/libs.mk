$(info $$DEFINES [${DEFINES}])

ifeq ($(findstring RDM_RESPONDER,$(DEFINES)),RDM_RESPONDER)
	ifneq ($(findstring rdmresponder,$(LIBS)),rdmresponder)
		LIBS+=rdmresponder
	endif
	ifneq ($(findstring rdmsensor,$(LIBS)),rdmsensor)
		LIBS+=rdmsensor device
	endif
	ifneq ($(findstring rdmsubdevice,$(LIBS)),rdmsubdevice)
		LIBS+=rdmsubdevice
	endif
	ifneq ($(findstring NODE_ARTNET,$(DEFINES)),NODE_ARTNET)
		ifneq ($(findstring dmxreceiver,$(LIBS)),dmxreceiver)
			LIBS+=dmxreceiver
		endif
	endif
	RDM=1
	LIBS+=dmx
endif

ifeq ($(findstring NODE_DMX,$(DEFINES)),NODE_DMX)
	LIBS+=dmxreceiver dmx
endif

LIBS+=rdm

ifeq ($(findstring OUTPUT_DMX_SEND,$(DEFINES)),OUTPUT_DMX_SEND)
	LIBS+=dmxsend dmx
endif

ifeq ($(findstring OUTPUT_DDP_PIXEL_MULTI,$(DEFINES)),OUTPUT_DDP_PIXEL_MULTI)
	LIBS+=ws28xxdmx ws28xx jamstapl
else
	ifeq ($(findstring OUTPUT_DMX_PIXEL_MULTI,$(DEFINES)),OUTPUT_DMX_PIXEL_MULTI)
		LIBS+=ws28xxdmx ws28xx jamstapl
	else
		ifeq ($(findstring OUTPUT_DMX_PIXEL,$(DEFINES)),OUTPUT_DMX_PIXEL)
			LIBS+=ws28xxdmx ws28xx
		endif
	endif
endif

ifeq ($(findstring OUTPUT_DDP_PIXEL,$(DEFINES)),OUTPUT_DDP_PIXEL)
	LIBS+=ws28xx
endif

ifeq ($(findstring OUTPUT_DMX_STEPPER,$(DEFINES)),OUTPUT_DMX_STEPPER)
	LIBS+=l6470dmx l6470
endif

ifeq ($(findstring OUTPUT_DMX_TLC59711,$(DEFINES)),OUTPUT_DMX_TLC59711)
	LIBS+=tlc59711dmx tlc59711
endif

ifeq ($(findstring OUTPUT_DMX_SERIAL,$(DEFINES)),OUTPUT_DMX_SERIAL)
	LIBS+=dmxserial
endif

ifdef COND
	LIBS+=spiflashstore spiflash
endif

ifeq ($(findstring NODE_LTC_SMPTE,$(DEFINES)),NODE_LTC_SMPTE)
	DEFINES+=ENABLE_SSD1311 ENABLE_TC1602 ENABLE_CURSOR_MODE
endif

ifeq ($(findstring DISPLAY_UDF,$(DEFINES)),DISPLAY_UDF)
	LIBS+=displayudf
endif

ifneq ($(findstring network,$(LIBS)),network)
	LIBS+=network
endif

ifneq ($(findstring properties,$(LIBS)),properties)
	LIBS+=properties
endif

LIBS+=lightset display hal

$(info $$LIBS [${LIBS}])