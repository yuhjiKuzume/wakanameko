mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_COBJS += LineTracer.o PidControl.o ColorControl.o DriveControl.o LineTracerR.o 

APPL_CXXOBJS += LineTracerService.o \
	ColorCheckService.o \
	TurnService.o \
	ColorLogic.o \
  	localizer.o \
	odometory.o \
	position.o \
	Sapporo.o

APPL_DIRS += $(mkfile_path)app $(mkfile_path)LineTracer $(mkfile_path)
APPL_DIRS += $(mkfile_path)app $(mkfile_path)Block/logic
APPL_DIRS += $(mkfile_path)app $(mkfile_path)Block/service
APPL_DIRS += $(mkfile_path)app $(mkfile_path)Block/controller
APPL_DIRS += $(mkfile_path)app $(mkfile_path)Block/localizer

INCLUDES += -I$(mkfile_path)LineTracer\
	-I$(mkfile_path)Block/controller\
	-I$(mkfile_path)Block/logic \
	-I$(mkfile_path)Block/service \
	-I$(mkfile_path)Block/controller \
	-I$(mkfile_path)Block/localizer

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-ev3/Makefile

endif


SRCLANG := c++
APPL_LIBS += -lm