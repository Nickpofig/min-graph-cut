# 					----[ REFERNCIES ]-----
# web: https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html#Automatic-Variables
# $@ - target's name
# $^ - all prerequisites
# $< - first prerequisite
#
# pattern:
# <target>: <prerequistes>
# \t <recipe>
#
#
# 					----[ DEBUGGING ]----
# to use gdb gcc debugging tool withing visual studio code
# you should to specify <make command> with "-g" compiler option
# and compile <sources> including <headers> into appropriate <binary>
# after that just create <debug configuration> in the launch.json
# with <task> in the task.json that call this appropriate <make command>

CC := gcc

BUILD_DIRECTORY := builds
SOURCE_DIRECTORY := sources
OBJECT_DIRECTORY := objects
INCLUDE_DIRECTORY := includes
TEST_DIRECTORY := tests

PROGRAM := $(BUILD_DIRECTORY)/program
SOURCES := $(wildcard $(SOURCE_DIRECTORY)/*.c)
OBJECTS := $(SOURCES:$(SOURCE_DIRECTORY)/%.c=$(OBJECT_DIRECTORY)/%.o)
INCLUDES := $(wildcard $(INCLUDE_DIRECTORY)/*.h)
TESTS := $(wildcard $(TEST_DIRECTORY)/%.c)

INCLUDE := -I$(INCLUDE_DIRECTORY)
CFLAGS  := -Wall -std=c99
LDFLAGS := -Llib
LDLIBS  := -lm
DEFINES := 

.PHONY: all clean

help:
	@echo "available commands:\n * sequential\n * help\n * parallel\n * test-info\n * test_<any>\n * clean"

sequential: clean $(PROGRAM)

parallel: DEFINES += -D__include_mpi
parallel: CC := mpicc 
parallel: CFLAGS += -fopenmp 
parallel: clean $(PROGRAM)

test-info: 
	@echo "$(subst $(TEST_DIRECTORY)/, * ,$(subst $()  $(),\n,$(subst .c, ,$(wildcard $(TEST_DIRECTORY)/*.c))))"

test_%: $(filter-out %main.o,$(OBJECTS))
	$(CC) $(INCLUDE) $(LDFLAGS) $^ $(TEST_DIRECTORY)/$@.c $(LDLIBS) -o $(BUILD_DIRECTORY)/$@

$(PROGRAM): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) $(CFLAGS) -o $@

$(OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.c $(INCLUDES) | $(OBJECT_DIRECTORY)
	$(CC) $(INCLUDE) $(CFLAGS) $(DEFINES) -c $< -o $@

$(OBJECT_DIRECTORY):
	mkdir $@

clean:
	$(RM) $(wildcard $(OBJECT_DIRECTORY)/*.o) $(wildcard $(BUILD_DIRECTORY)/*)
