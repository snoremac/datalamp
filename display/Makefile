export DATALAMP_HOME = .
export DATALAMP_SRC = $(DATALAMP_HOME)/datalamp
export AVRLAUNCH_HOME = $(HOME)/software/avrlaunch/avrlaunch
export AVRLAUNCH_SRC = $(AVRLAUNCH_HOME)/avrlaunch

.PHONY: all debug sim sim-debug
all : datalamp
debug : datalamp-debug
sim : datalamp-sim
debug-sim : datalamp-debug-sim

INCLUDES += -I. -I$(DATALAMP_HOME) -I$(DATALAMP_SRC) -I$(AVRLAUNCH_HOME)
CFLAGS += -DAVRLAUNCH_OVERRIDE_DEFS

include $(AVRLAUNCH_HOME)/mk/defs.mk
include $(AVRLAUNCH_HOME)/mk/common.mk

.PHONY: datalamp datalamp.hex
datalamp datalamp.hex : $(DATALAMP_SRC)/datalamp.hex

datalamp_c_src = $(wildcard $(DATALAMP_SRC)/*.c)
datalamp_asm_src = $(wildcard $(DATALAMP_SRC)/*.S)
datalamp_obj = $(datalamp_c_src:.c=.o) $(datalamp_asm_src:.S=.o)

$(DATALAMP_SRC)/datalamp.elf : $(datalamp_obj) $(AVRLAUNCH_SRC)/avrlaunch.a
	$(CC) -o $@ $^ $(DEFAULT_LDFLAGS) $(LDFLAGS)

.PHONY: $(AVRLAUNCH_SRC)/avrlaunch.a
$(AVRLAUNCH_SRC)/avrlaunch.a : 
	make -C $(AVRLAUNCH_HOME) avrlaunch INCLUDES=-I$(realpath $(DATALAMP_SRC)) CFLAGS="$(CFLAGS)"

.PHONY: clean
clean:
	rm -f $(addprefix $(DATALAMP_HOME)/, $(ARTIFACTS))
	rm -f $(addprefix $(DATALAMP_SRC)/, $(ARTIFACTS))
	make -C $(AVRLAUNCH_HOME) clean
