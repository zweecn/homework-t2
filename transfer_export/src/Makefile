SUB_DIRS = client read_proc storage_server share_mem shm_app transfer

.PHONY: all $(SUB_DIRS) clean install

all : $(SUB_DIRS)

shm_app : share_mem

read_proc : share_mem

$(SUB_DIRS):
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
	done
