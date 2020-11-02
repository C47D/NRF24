COMPONENT_NAME = nrf24

ifeq "$(CPPUTEST_HOME)" ""
$(error The environment variable CPPUTEST_HOME is not set.)
endif

# --- SRC_FILES and SRC_DIRS ---
# Production code files are compiled and put into
# a library to link with the test runner.
#
# Test code of the same name overrides
# production code at link time.
#
# SRC_FILES specifies individual production
# code files.
#
# SRC_DIRS specifies directories containing
# production code C and CPP files.

SRC_FILES += src/NRF24.c
SRC_FILES += src/NRF24_INTERFACE.c
SRC_FILES += src/NRF24_COMMANDS.c

# --- TEST_SRC_FILES and TEST_SRC_DIRS ---
# Test files are always included in the build.
# Production code is pulled into the build unless
# it is overriden by code of the same name in the
# test code.
#
# TEST_SRC_FILES specifies individual test files to build.
# TEST_SRC_DIRS, builds everything in the directory

TEST_SRC_FILES +=
TEST_SRC_DIRS += tests

# INCLUDE_DIRS are searched in order after the included file's
# containing directory

INCLUDE_DIRS += $(CPPUTEST_HOME)/include
INCLUDE_DIRS += $(CPPUTEST_HOME)/include/Platforms/Gcc

INCLUDE_DIRS += inc

include $(CPPUTEST_HOME)/build/MakefileWorker.mk
