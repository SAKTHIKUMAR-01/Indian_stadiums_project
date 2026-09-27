# Indian Stadium Management System - Cross-Platform Makefile
# Target: Linux (Ubuntu/Debian/Fedora/Arch) & Windows (MinGW-w64 / MSYS2)

CC = gcc

# Detect Operating System
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)

ifneq ($(findstring Linux,$(UNAME_S)),)
    # Linux configuration
    CFLAGS = -Wall -Wextra -O2 -Iinclude $(shell pkg-config --cflags libxml-2.0 2>/dev/null || echo "-I/usr/include/libxml2") $(shell pkg-config --cflags mariadb 2>/dev/null || echo "-I/usr/include/mariadb" || echo "-I/usr/include/mysql")
    LDFLAGS = -lcurl -lxml2 -lmariadb -lpthread
    TARGET = bin/stadium_system
    PYTHON_CMD = python3
else
    # Windows (MinGW-w64 / MSYS2) configuration
    CFLAGS = -Wall -Wextra -O2 -Iinclude -I"C:/msys64/ucrt64/include" -I"C:/msys64/ucrt64/include/libxml2" -I"C:/msys64/ucrt64/include/mariadb"
    LDFLAGS = -L"C:/msys64/ucrt64/lib" -lcurl -lxml2 -lmariadb -lpthread -lws2_32
    TARGET = bin/stadium_system.exe
    PYTHON_CMD = python
endif

SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
DATA_DIR = data
OUTPUT_DIR = output

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/curl_download.c \
       $(SRC_DIR)/parser.c \
       $(SRC_DIR)/cleaner.c \
       $(SRC_DIR)/buffer.c \
       $(SRC_DIR)/producer.c \
       $(SRC_DIR)/csv_consumer.c \
       $(SRC_DIR)/mysql_consumer.c \
       $(SRC_DIR)/database.c \
       $(SRC_DIR)/csv.c \
       $(SRC_DIR)/crud.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean run viz directories help

all: directories $(TARGET)

directories:
	@mkdir -p $(BIN_DIR) $(DATA_DIR) $(OUTPUT_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "[Build] Binary created successfully: $(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	@echo "[Exec] Launching Indian Stadium Management System..."
	@./$(TARGET)

viz:
	@echo "[Viz] Running Python analytical visualizations..."
	$(PYTHON_CMD) python/visualize.py

clean:
	@rm -rf $(SRC_DIR)/*.o $(BIN_DIR)/stadium_system $(BIN_DIR)/stadium_system.exe
	@echo "[Clean] Cleaned build artifacts."

help:
	@echo "Available Makefile targets:"
	@echo "  make         - Build project binary"
	@echo "  make run     - Build and run stadium management system"
	@echo "  make viz     - Run Python data visualization script"
	@echo "  make clean   - Remove object files and executables"
