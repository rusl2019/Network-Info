# Cross-platform build configuration for wxWidgets application
TARGET = network_info
WXWIDGETS_DIR = $(PWD)/external
SRC_FILES = $(TARGET).cpp
BUILD_DIR = build

# Linux configuration
LINUX_CXX = g++
LINUX_WXDIR = $(WXWIDGETS_DIR)/wxWidgets-linux
LINUX_WXCONFIG = $(LINUX_WXDIR)/bin/wx-config
LINUX_WXFLAGS = `$(LINUX_WXCONFIG) --cxxflags --libs core,base --static`
LINUX_LDFLAGS = -static-libgcc -static-libstdc++
LINUX_OUTPUT = $(BUILD_DIR)/linux/$(TARGET)

# Windows configuration (cross-compile)
WIN_CXX = x86_64-w64-mingw32-g++
WIN_WXDIR = $(WXWIDGETS_DIR)/wxWidgets-win
WIN_WXCONFIG = $(WIN_WXDIR)/bin/wx-config
WIN_LIBS = -lws2_32 -liphlpapi -lcomctl32 -luser32 -lgdi32 -lole32 -loleaut32 -luuid
WIN_WXFLAGS = `$(WIN_WXCONFIG) --cxxflags --libs core,base --static`
WIN_LDFLAGS = -static-libgcc -static-libstdc++
WIN_OUTPUT = $(BUILD_DIR)/windows/$(TARGET).exe

# Default target builds both platforms
all: directories linux windows

# Create build directories
directories:
	mkdir -p $(BUILD_DIR)/linux $(BUILD_DIR)/windows

# Linux build
linux: directories $(LINUX_OUTPUT)

$(LINUX_OUTPUT): $(SRC_FILES)
	@echo "Building Linux version..."
	$(LINUX_CXX) $^ -o $@ $(LINUX_WXFLAGS) $(LINUX_LDFLAGS)
	@echo "Linux build complete: $@"

# Windows build
windows: directories $(WIN_OUTPUT)

$(WIN_OUTPUT): $(SRC_FILES)
	@echo "Building Windows version..."
	$(WIN_CXX) $^ -o $@ $(WIN_WXFLAGS) $(WIN_LIBS) $(WIN_LDFLAGS)
	@echo "Windows build complete: $@"

# Clean build artifacts
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)

# Deep clean (removes build files and external dependencies)
distclean: clean
	@echo "Removing external dependencies..."
	rm -rf external wxWidgets*

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build for both Linux and Windows (default)"
	@echo "  linux      - Build only for Linux"
	@echo "  windows    - Build only for Windows"
	@echo "  clean      - Remove build files"
	@echo "  distclean  - Remove build files and external dependencies"
	@echo "  help       - Show this help message"

.PHONY: all directories linux windows clean distclean help