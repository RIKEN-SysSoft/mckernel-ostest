# Makefile COPYRIGHT FUJITSU LIMITED 2015-2016

# target name.
TARGET := test_mck
ARCH?=x86

#---------------
# path settings.
#---------------
# directory.
BIN_DIR       := bin
SRC_ROOT_DIR  := src
DEP_ROOT_DIR  := depend
OBJ_ROOT_DIR  := obj
INC_DIRS      := . $(SRC_ROOT_DIR) $(SRC_ROOT_DIR)/arch/$(ARCH)/include

SRC_DIRS := $(shell find $(SRC_ROOT_DIR) -type d | grep -vw $(SRC_ROOT_DIR)/arch) $(shell find $(SRC_ROOT_DIR)/arch/$(ARCH) -type d)
OBJ_DIRS := $(patsubst $(SRC_ROOT_DIR)%, $(OBJ_ROOT_DIR)%, $(SRC_DIRS))
DEP_DIRS := $(patsubst $(SRC_ROOT_DIR)%, $(DEP_ROOT_DIR)%, $(SRC_DIRS))

# file lists.
BIN     := $(TARGET)
PROGRAM := $(BIN_DIR)/$(BIN)
SRCS    := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJS    := $(patsubst $(SRC_ROOT_DIR)%, $(OBJ_ROOT_DIR)%, $(SRCS:.c=.o))
DEPS    := $(patsubst $(SRC_ROOT_DIR)%, $(DEP_ROOT_DIR)%, $(SRCS:.c=.d))
TEST_SUTE_LIST := test_suite.list
TEST_SUTE_LIST_NEW := __test_suite.list
TEST_CASE_LIST := test_case.list
TEST_CASE_LIST_NEW := __test_case.list
CLEAN_FILES := $(TEST_SUTE_LIST) $(TEST_CASE_LIST) $(TEST_SUTE_LIST_NEW) $(TEST_CASE_LIST_NEW)

#--------------
# build otions.
#--------------
# compile.
CFLAGS += -g
CFLAGS += -O0
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -funsigned-char

# preprocess.
CPPFLAGS += $(foreach dir,$(INC_DIRS),-I$(dir))
CPPFLAGS += -D_GNU_SOURCE

# link.
LDFLAGS  += -lpthread -lm
LOADLIBS +=

#--------
# targets
#--------
CC ?= gcc
LD := $(CC)

RM := rm -rf
MV := mv
CP := cp
SED := sed
MKDIR := mkdir -p
DIFF := diff
FIND := find
GREP := grep
SORT := sort

__update_test_sute_list := $(shell $(FIND) $(SRC_ROOT_DIR)/ -type d |\
	 $(GREP) -vw $(SRC_ROOT_DIR)/ |\
	 $(GREP) -vw $(SRC_ROOT_DIR)/arch |\
	 $(SED) "s|$(SRC_ROOT_DIR)/\(.*\)|TEST_SUITE_DEF(\1)|g" >$(TEST_SUTE_LIST_NEW);\
	 if $(DIFF) $(TEST_SUTE_LIST_NEW) $(TEST_SUTE_LIST) >/dev/null 2>&1; then\
	   $(RM) $(TEST_SUTE_LIST_NEW);\
	 else\
	   $(MV) $(TEST_SUTE_LIST_NEW) $(TEST_SUTE_LIST);\
	 fi)

__update_test_case_list := \
	$(foreach dir,\
	  $(shell $(FIND) $(SRC_ROOT_DIR)/ -type d | $(GREP) -vw $(SRC_ROOT_DIR)/ | $(GREP) -vw $(SRC_ROOT_DIR)/arch),\
	  $(shell $(FIND) $(dir)/ -regex .*/[0-9]+.c | $(SORT) |\
		$(SED) "s|$(SRC_ROOT_DIR)/\([^/]*\)/0\{,2\}\([0-9]*\)\.c|TEST_CASE_DEF(\1,\2)|g" >> $(TEST_CASE_LIST_NEW)))\
	$(shell \
	 if $(DIFF) $(TEST_CASE_LIST_NEW) $(TEST_CASE_LIST) >/dev/null 2>&1; then\
	   $(RM) $(TEST_CASE_LIST_NEW);\
	 else\
	   $(MV) $(TEST_CASE_LIST_NEW) $(TEST_CASE_LIST);\
	 fi)

# Default target.
all: $(TARGET)

# Generate Program.
$(TARGET): $(OBJS)
	$(MKDIR) $(BIN_DIR)
	$(LD) $(LDFLAGS) $^ $(LOADLIBS) -o $(PROGRAM)

# Remove any temporary products.
clean:
	$(RM) $(CLEAN_FILES)
	-@if [ "." != $(OBJ_ROOT_DIR) ]; then \
		$(RM) $(OBJ_ROOT_DIR); \
	fi
	-@if [ "." != $(DEP_ROOT_DIR) ]; then \
		$(RM) $(DEP_ROOT_DIR); \
	fi

# Remove any generated file.
clobber: clean
	$(RM) $(PROGRAM)
	find $(SRC_ROOT_DIR) -name \*~ -exec $(RM) {} +;
	-@if [ "." != $(BIN_DIR) ]; then \
		$(RM) $(BIN_DIR); \
	fi

# Generate object file.
$(OBJS):
	-@ $(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $(patsubst $(OBJ_ROOT_DIR)%, $(SRC_ROOT_DIR)%, $(@:.o=.c)) \
		-DTEST_SUITE=$(shell echo $@ | sed 's|$(OBJ_ROOT_DIR)/\([^/]*\).*.o|\1|g') \
		-DTEST_NUMBER=$(shell echo $@ | sed 's|$(OBJ_ROOT_DIR)/[^/]*/[0]\{,2\}\([0-9]*\).o|\1|g' | grep -E ^[0-9]+$)

# Generate dependencies file.
$(DEPS):
	-@ $(MKDIR) $(DEP_DIRS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -M $(filter %/$(patsubst $(DEP_ROOT_DIR)%, $(SRC_ROOT_DIR)%, $(@:.d=.c)), $(SRCS)) |\
	$(SED) 's|$(subst .d,.o,$(notdir $@)) *:|$(filter %/$(patsubst $(DEP_ROOT_DIR)%, $(OBJ_ROOT_DIR)%, $(@:.d=.o)), $(OBJS)) : Makefile|' >$@

.PHONY: all clean clobber
ifneq "$(MAKECMDGOALS)" "clean"
 ifneq "$(MAKECMDGOALS)" "clobber"
  -include $(DEPS)
 endif
endif
