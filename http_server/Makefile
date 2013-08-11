include config.mak

INSTALL_DIR = $(shell pwd)/bin
export INSTALL_DIR

INCLUDE_DIR += -I$(shell pwd)/src/include
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

uninstall :
	-rm -rf $(INSTALL_DIR)/*
clean :
	@for dir in $(SUB_DIRS); \
	do \
		make -C $$dir clean; \
	done 

