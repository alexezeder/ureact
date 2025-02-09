##########################################################################
# source files
##########################################################################

# the list of CMakeLists.txt
CMAKE_FILES_TO_FORMAT=$(shell find . -type f -name CMakeLists.txt -o -name '*.cmake' | grep -v './cmake-build' | sort)

INCLUDE_FILES_TO_FORMAT=$(shell find ./include -type f -name '*.hpp' | sort)
TEST_FILES_TO_FORMAT=$(shell find ./tests/src -type f -name '*.h' -or -name '*.hpp' -or -name '*.c' -or -name '*.cpp' | sort)
CPP_FILES_TO_FORMAT=${INCLUDE_FILES_TO_FORMAT} ${TEST_FILES_TO_FORMAT}

FILES_TO_FORMAT=${CMAKE_FILES_TO_FORMAT} ${CPP_FILES_TO_FORMAT}

##########################################################################
# documentation of the Makefile's targets
##########################################################################

# main target
all:
	@echo "pretty_version - output version of formatting tools"
	@echo "pretty_check - check if all cmake and c++ files are properly formatted"
	@echo "pretty - prettify all cmake and c++ files"

##########################################################################
# Prettify
##########################################################################

# output version of formatting tools
pretty_version:
	@./support/pretty.sh --version

# check if all cmake and c++ files are properly formatted
pretty_check:
	@./support/pretty.sh --check $(FILES_TO_FORMAT)

# prettify all cmake and c++ files
pretty:
	@./support/pretty.sh $(FILES_TO_FORMAT)
