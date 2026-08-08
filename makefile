# Makefile

BLD_DIR=./bld
DIST_DIR=./dist

# Protob codegen
RINEX_PROTO_DIR = ./proto
RINEX_PROTO = $(RINEX_PROTO_DIR)/rinex_record.proto
RINEX_PROTO_CC = rinex_record.pb.cc
GENERATED_PROTO= $(BLD_DIR)/rinex_record.pb.cc \
                 $(BLD_DIR)/rinex_record.pb.h \
				 $(BLD_DIR)/rinex_record_pb2.py \
				 $(BLD_DIR)/rinex_record_pb2.pyi


# Shared library compilation
CXX = g++
CXXFLAGS = -O0 -Wall -shared -std=c++17 -fPIC $(shell .venv/bin/python -m pybind11 --includes) -I$(BLD_DIR) -I$(CPP_SRC_DIR)
LDFLAGS = -lprotobuf -L$(BLD_DIR) -lrinex_manager -Wl,-rpath,'$$ORIGIN'
CPP_SRC_DIR=./src
CPP_SRCS = $(CPP_SRC_DIR)/nav_utils.cpp \
           $(CPP_SRC_DIR)/word_serializer.cpp \
           $(CPP_SRC_DIR)/word_definition.cpp \
           $(CPP_SRC_DIR)/encoder.cpp \
           $(CPP_SRC_DIR)/crc.cpp \
           $(CPP_SRC_DIR)/conversions.cpp
RINEX_SRC_DIR=./src/rinex_manager
RINEX_SRCS = $(RINEX_SRC_DIR)/rinex_reader.cpp \
            $(RINEX_SRC_DIR)/rinex_manager.cpp \
            $(BLD_DIR)/$(RINEX_PROTO_CC)
SHARED_LIB = $(BLD_DIR)/nav_utils$(shell python3-config --extension-suffix)
RINEX_SHARED_LIB = $(BLD_DIR)/librinex_manager.so

# Python interface
PY_I_DIR=py_i
PY_I_FILE=nav_utils.pyi

# Rules
all: $(SHARED_LIB) $(RINEX_SHARED_LIB) $(GENERATED_PROTO) $(BLD_DIR)/$(PY_I_FILE)

wheel: all 
	mkdir -p $(DIST_DIR)
	python3 -m pip wheel . --no-deps -w $(DIST_DIR)
	rm -rf build

$(GENERATED_PROTO): $(RINEX_PROTO)
	mkdir -p $(BLD_DIR)
	protoc -I$(RINEX_PROTO_DIR) --cpp_out=$(BLD_DIR) --python_out=$(BLD_DIR) --pyi_out=$(BLD_DIR) $(RINEX_PROTO)

$(RINEX_SHARED_LIB): $(RINEX_SRCS)
	mkdir -p $(BLD_DIR)
	$(CXX) $(CXXFLAGS) $(RINEX_SRCS) -o $(RINEX_SHARED_LIB) $(LDFLAGS)

$(SHARED_LIB): $(CPP_SRCS) $(RINEX_SHARED_LIB)
	$(CXX) $(CXXFLAGS) $(CPP_SRCS) -o $(SHARED_LIB) $(LDFLAGS)

$(BLD_DIR)/$(PY_I_FILE): $(PY_I_DIR)/$(PY_I_FILE)
	mkdir -p $(BLD_DIR)
	cp $< $@



clean:
	rm -rf $(BLD_DIR) $(DIST_DIR) build *.egg-info

.PHONY: all clean test wheel

test:
	for test_file in ./tst/test_*.py; do \
		PYTHONPATH=./bld .venv/bin/python "$$test_file"; \
	done
	@echo "\n passed!"
