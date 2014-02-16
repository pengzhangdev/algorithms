### Makefile ---
##
## Filename: Makefile
## Description: The target of this makefile is to handle muti target with muti makefile, each containing one target.
## Author: Peng Zhang
## Maintainer: Peng Zhang
## Created: Sun Feb 16 20:52:38 2014 (+0800)
## Version:
## Last-Updated:Sun Feb 16 20:55:09 2014 (+0800)
##           By: Peng Zhang
##     Update #: 1

######################### args define ##############################

CC := gcc
CXX := g++
CFLAGS := -O2 -g -D_DEBUG
CPPFLAGS := -O2 -g -fpermissive -D_DEBUG
DEPEND_DIR := .dep
LDFLAGS :=
COMPILER := $(CC)

LOCAL_C_SRCS := \
        $(wildcard *.c ./*/*.c) \

LOCAL_CPP_SRCS := \
        $(wildcard *.cpp ./*/*.cpp) \

LOCAL_SRCS := \
        $(LOCAL_C_SRCS) \
        $(LOCAL_CPP_SRCS) \


LOCAL_INCLUDES := \
    ./include \
#       $(shell pwd)/../log/ \

LOCAL_MODULE := libalg.so


CFLAGS += $(LOCAL_CFLAGS)

CPPFLAGS += $(LOCAL_CPPFLAGS)

INCLUDES := $(foreach var, $(LOCAL_INCLUDES), -I$(var))


ifneq ($(strip $(LOCAL_CPP_SRCS)),)
        COMPILER := $(CXX)
endif

ifeq ($(strip $(suffix $(LOCAL_MODULE))), .so)
$(warning "build shared library")
        LDFLAGS += -shared
        CFLAGS += -fPIC
        CPPFLAGS += -fPIC
endif

ifeq ($(strip $(suffix $(LOCAL_MODULE))), .a)
$(warning "build static library")
        LDFLAGS += -static
endif


################################### rules start ###################################
all: $(LOCAL_MODULE)

%.o: %.c
	$(CC) -c $(CFLAGS) -I$(INCLUDES) $< -o $@

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) -I$(INCLUDES) $< -o $@

$(DEPEND_DIR)/%.c.d: %.c
	@set -e; rm -f $@; \
	mkdir -p $(DEPEND_DIR)/$(dir $<); \
	$(CC) -MM $(INCLUDES) $< > $@.$$$$; \
	sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	sed -i 's,$(notdir $*).o:,$(dir $<)$(notdir $*).o:,g' $@; \
	rm -f $@.$$$$

sinclude $(foreach var, $(filter %.c.d, $(LOCAL_SRCS:.c=.c.d)), $(DEPEND_DIR)/$(var))

$(DEPEND_DIR)/%.cpp.d: %.cpp
	@set -e; rm -f $@; \
	mkdir -p $(DEPEND_DIR)/$(dir $<); \
	$(CC) -MM $(INCLUDES) $< > $@.$$$$; \
	sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	sed -i 's,$(notdir $*).o:,$(dir $<)$(notdir $*).o:,g' $@; \
	rm -f $@.$$$$

#$(warning $(foreach var, $(filter %.cpp.d, $(LOCAL_SRCS:.cpp=.cpp.d)), $(DEPEND_DIR)/$(var)))
sinclude $(foreach var, $(filter %.cpp.d, $(LOCAL_SRCS:.cpp=.cpp.d)), $(DEPEND_DIR)/$(var))


$(LOCAL_MODULE) : $(filter %.o, $(LOCAL_SRCS:.c=.o)) $(filter %.o, $(LOCAL_SRCS:.cpp=.o))
	$(COMPILER) $(CFLAGS) $(INCLUDES) $(LDFLAGS) $^ -o $@

# add for flymake
.PHONY: check-syntax
ifeq ($(strip $(suffix $(CHK_SOURCES))), .cpp)
        FLYMAKE := $(CXX)
else
        FLYMAKE := $(CC)
endif
check-syntax:
	$(FLYMAKE) -c $(CPPFLAGS) -I$(INCLUDES) -Wall -Wextra -pedantic -fsyntax-only ${CHK_SOURCES}

.PHONY: clean
clean :
	-rm -r $(LOCAL_MODULE) ./*/*.o

.PHONY: distclean
distclean :
	-rm -r $(DEPEND_DIR) ./*/*.o $(LOCAL_MODULE)
	#$(call call-all-subdir-makefiles)


########################## function define ##############################

ifndef call-all-subdir-makefiles
define call-all-subdir-makefiles
	echo "function called"
endef
endif

ifndef clear-all-vars
define clear-all-vars

endef
endif
