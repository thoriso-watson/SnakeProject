# setup useful variables that can be used by make.

# this must be defined in your shell's startup file.
ifndef CS49N_2021_PATH
$(error CS49N_2021_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
AR = $(ARM)-ar
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
CS49N_2021_LIBPI_PATH = $(CS49N_2021_PATH)/libpi
LPP = $(CS49N_2021_LIBPI_PATH)
LPI = $(CS49N_2021_LIBPI_PATH)/libpi.a

START ?= $(LPP)/staff-start.o
MEMMAP ?= $(LPP)/memmap

# include path: user can override
INC ?= -I$(LPP)/include -I$(LPP)/ -I$(LPP)/src -I.
# optimization level: user can override
OPT_LEVEL ?= -Og

# various warnings.  any warning = error, so you must fix it.
CFLAGS += $(OPT_LEVEL) -Wall -nostdlib -nostartfiles -ffreestanding -mcpu=arm1176jzf-s -mtune=arm1176jzf-s  -std=gnu99 $(INC) -ggdb -Wno-pointer-sign  $(CFLAGS_EXTRA) -Werror  -Wno-unused-function -Wno-unused-variable

# for assembly file compilation.
ASFLAGS = --warn --fatal-warnings  -mcpu=arm1176jzf-s -march=armv6zk $(INC)

# for .S compilation so we can use the preprocessor.
CPP_ASFLAGS =  -nostdlib -nostartfiles -ffreestanding   -Wa,--warn -Wa,--fatal-warnings -Wa,-mcpu=arm1176jzf-s -Wa,-march=armv6zk   $(INC)
