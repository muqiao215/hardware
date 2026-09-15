SDCC ?= sdcc
PACKIHX ?= packihx

BUILD_DIR := build
SRC_DIR := src
TARGET := main

SRCS := $(SRC_DIR)/main.c

CPPFLAGS ?=
CFLAGS ?=

.PHONY: all clean flash check-tools

all: $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/$(TARGET).ihx: $(SRCS) | $(BUILD_DIR)
	$(SDCC) -mmcs51 --model-small --out-fmt-ihx $(CPPFLAGS) $(CFLAGS) -o $(BUILD_DIR)/ $<

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).ihx
	$(PACKIHX) $< > $@

clean:
	rm -rf $(BUILD_DIR)

check-tools:
	@command -v $(SDCC) >/dev/null || (echo "missing tool: $(SDCC)"; exit 1)
	@command -v $(PACKIHX) >/dev/null || (echo "missing tool: $(PACKIHX)"; exit 1)

flash: $(BUILD_DIR)/$(TARGET).hex
	@echo "Flashing AT89S52 is not supported via avrdude (AVR-only)."
	@echo "Build output: $(BUILD_DIR)/$(TARGET).hex"
	@echo "Use a 51 programmer tool on Windows (e.g. ProgISP / vendor tool) to program AT89S52."
	@false
