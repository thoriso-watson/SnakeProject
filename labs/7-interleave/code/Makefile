# list out the driver program source 
TEST_SRC = example-ir-send-recv.c

SRC = measure-overheads.c

# changing this leads to noticeably higher overhead --- perhaps
# gcc struct optimzations only kick in at higher levels?
OPT_LEVEL = -O3

# your source, shared by driver code.
#   if you want to use our staff-hc-sr04.o,
#   comment SRC out and uncomment STAFF_OBJS
# SRC := hc-sr04.c
# STAFF_OBJS = staff-hc-sr04.o

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o)
include $(CS49N_2021_PATH)/libpi/mk/Makefile.template
