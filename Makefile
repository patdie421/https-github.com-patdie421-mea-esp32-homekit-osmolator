#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
PROJECT_NAME := mea-osmolator
EXTRA_COMPONENT_DIRS := /Users/patrice/esp/esp-idf-lib/components /Users/patrice/esp/mea-esp32-homekit-toolbox/components

CFLAGS += -I$(abspath ../..) -DHOMEKIT_SHORT_APPLE_UUIDS

include $(IDF_PATH)/make/project.mk
