include ../py/mkenv.mk
-include mpconfigport.mk

PREFIX=arm-none-eabi-
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
LD = $(PREFIX)ld 

# define main target
PROG = micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# Disable stripping of the main executable, relocation info is needed by genzehn
NOSTRIP := 1
# DEBUG := 1

# include py core make definitions
include ../py/py.mk

INC =  -I.
INC += -I..
INC += -I../py
INC += -I$(BUILD)

# Debugging/Optimization
ifdef DEBUG
COPT = -O0 -g
else
COPT = -Os -g #-fomit-frame-pointer -flto
endif

# compiler settings
CWARN = -Wall -Werror -Wno-error=cpp -Wno-error=format -Wno-error=unused-variable # -Wno-error=incompatible-pointer-types -Wno-error=missing-braces 
CFLAGS =  -DMICROPY_LIB $(INC) $(CWARN) -ansi -std=gnu99 $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)
CFLAGS +=  -mtune=arm926ej-s -mcpu=arm926ej-s -mlittle-endian -mfloat-abi=soft -fno-strict-aliasing -mthumb -fno-exceptions -DHP39 -fdata-sections -ffunction-sections  # -fpermissive


LDFLAGS = $(LDFLAGS_MOD) -lm $(LDFLAGS_EXTRA) 

# source files
SRC_C = $(shell find . -name \*.c)

USERMODULES_DIR := ulab

CFLAGS_USERMOD += -I$(USERMODULES_DIR)

override CFLAGS_EXTRA += -DMODULE_ULAB_ENABLED=1


SRC_QSTR += $(SRC_C)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

include ../py/mkrules.mk

all: $(PROG)
	ln -sf ../ulab .

distdir:
	/bin/cp *.c *.h *.mk Makefile mklib $(distdir)
