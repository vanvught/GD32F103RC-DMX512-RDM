$(info $$DEFINES [${DEFINES}])

ifeq ($(findstring RDM_RESPONDER,$(DEFINES)),RDM_RESPONDER)
	ifneq ($(findstring rdmsensor,$(LIBS)),rdmsensor)
		LIBS+=rdmsensor device
	endif
	ifneq ($(findstring NODE_ARTNET,$(DEFINES)),NODE_ARTNET)
		ifneq ($(findstring ,$(LIBS)),)
			LIBS+=
		endif
	endif
endif

ifeq ($(findstring OUTPUT_DMX_SEND,$(DEFINES)),OUTPUT_DMX_SEND)
	LIBS+=
endif

LIBS+=rdm dmx

ifeq ($(findstring ENABLE_RDM_SUBDEVICES,$(DEFINES)),ENABLE_RDM_SUBDEVICES)
	LIBS+=rdmsubdevice
endif

ifeq ($(findstring OUTPUT_DMX_PIXEL,$(DEFINES)),OUTPUT_DMX_PIXEL)
	LIBS+=dmxled pixeldmx pixel
endif

ifeq ($(findstring DISPLAY_UDF,$(DEFINES)),DISPLAY_UDF)
	LIBS+=displayudf
endif

LIBS+=configstore flashcode display hal

$(info $$LIBS [${LIBS}])