SUB_DIRS = http_server http_client

.PHONY: all $(SUB_DIRS) clean install

all : $(SUB_DIRS)

#server : http_server 

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
