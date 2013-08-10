TARGET = transfer.exe

C_SOURCES = $(wildcard *.c)
CPP_SOURCES = $(wildcard *.cpp)
OBJECTS = $(C_SOURCES:%.c=%.o) $(CPP_SOURCES:%.cpp=%.o)

ifeq ($(CONFIG_TRANSFER_DEBUG),y)
	CPPFLAGS += -DTRANSFER_DEBUG
endif

ifeq ($(CONFIG_GDB),y)
	CFLAGS += -g
	CXXFLAGS += -g
endif

CFLAGS += -Wall
CXXFLAGS += -Wall
CPPFLAGS += $(INCLUDE_DIR)
LDFLAGS += $(LIB_DIR) -L ../share_mem
LIBS += -lpthread -lshare_mem

all : $(TARGET)

install : $(TARGET)
	@if [ "$(INSTALL_DIR)" -a -d $(INSTALL_DIR) ]; then \
		$(STRIP) $(TARGET); \
		cp $(TARGET) $(INSTALL_DIR); \
	fi

$(TARGET) : $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJECTS:%.o=%.d)
endif

%.d : %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d : %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: clean
clean:
	-rm -f $(TARGET) $(C_SOURCES:%.c=%.d*) $(CPP_SOURCES:%.cpp=%.d*) $(OBJECTS)
