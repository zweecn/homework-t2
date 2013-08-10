include config.mak

INSTALL_DIR = $(shell pwd)/bin
export INSTALL_DIR

INCLUDE_DIR += -I$(shell pwd)/include
INCLUDE_DIR += -I$(shell pwd)/include/share_mem
export INCLUDE_DIR

SUB_DIRS = src

.PHONY : all $(SUB_DIRS) install clean

all : $(SUB_DIRS)

$(SUB_DIRS) :
	make -C $@

install :
	@for dir in $(SUB_DIRS); \
	do \
		make -C $$dir install; \
	done

clean :
	@for dir in $(SUB_DIRS); \
	do \
		make -C $$dir clean; \
	done \
