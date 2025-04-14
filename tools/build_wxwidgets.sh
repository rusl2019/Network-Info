#!/bin/bash

set -e

function log_info() {
	echo -e "\n\033[1;34m[INFO]\033[0m $1"
}

function log_success() {
	echo -e "\033[1;32m[SUCCESS]\033[0m $1"
}

function log_error() {
	echo -e "\033[1;31m[ERROR]\033[0m $1"
	exit 1
}

WX_VERSION="3.2.5"
WX_URL="https://github.com/wxWidgets/wxWidgets/releases/download/v${WX_VERSION}/wxWidgets-${WX_VERSION}.tar.bz2"
WX_TARBALL="wxWidgets-${WX_VERSION}.tar.bz2"
WX_DIR="wxWidgets-${WX_VERSION}"
WORK_DIR="$(pwd)"
EXTERNAL_DIR="$WORK_DIR/external"
LINUX_PREFIX="$EXTERNAL_DIR/wxWidgets-linux"
WIN_PREFIX="$EXTERNAL_DIR/wxWidgets-win"

mkdir -p "$EXTERNAL_DIR"

log_info "Installing dependencies..."
sudo apt-get update || log_error "Failed to update apt repositories"
sudo apt-get install -y g++ make mingw-w64 libgtk-3-dev libpng-dev libjpeg-dev libtiff-dev libexpat-dev || log_error "Failed to install dependencies"

if [ ! -f "$WX_TARBALL" ]; then
	log_info "Downloading wxWidgets ${WX_VERSION}..."
	wget "$WX_URL" || log_error "Failed to download wxWidgets"
else
	log_info "wxWidgets archive already exists, skipping download"
fi

if [ ! -d "$WX_DIR" ]; then
	log_info "Extracting wxWidgets..."
	tar -xjf "$WX_TARBALL" || log_error "Failed to extract wxWidgets"
else
	log_info "wxWidgets directory already exists, skipping extraction"
fi

cd "$WX_DIR" || log_error "Failed to change to wxWidgets directory"

log_info "Building wxWidgets for Linux..."
mkdir -p build-linux
cd build-linux || log_error "Failed to create Linux build directory"

if [ -d "$LINUX_PREFIX" ]; then
	log_info "Linux directory already exists, removing previous installation..."
	rm -rf "$LINUX_PREFIX"
fi
mkdir -p "$LINUX_PREFIX"

../configure --with-gtk=3 --enable-unicode --disable-shared --prefix="$LINUX_PREFIX" || log_error "wxWidgets configuration for Linux failed"
make -j$(nproc) || log_error "wxWidgets compilation for Linux failed"
make install || log_error "wxWidgets installation for Linux failed"
cd ..

log_info "Building wxWidgets for Windows..."
mkdir -p build-win
cd build-win || log_error "Failed to create Windows build directory"

if [ -d "$WIN_PREFIX" ]; then
	log_info "Windows directory already exists, removing previous installation..."
	rm -rf "$WIN_PREFIX"
fi
mkdir -p "$WIN_PREFIX"

../configure --host=x86_64-w64-mingw32 --build=x86_64-linux-gnu --disable-shared --enable-unicode --prefix="$WIN_PREFIX" || log_error "wxWidgets configuration for Windows failed"
make -j$(nproc) || log_error "wxWidgets compilation for Windows failed"
make install || log_error "wxWidgets installation for Windows failed"

cd "$WORK_DIR" || log_error "Failed to return to original directory"

read -p "Remove wxWidgets source files? (y/n): " cleanup_choice
if [[ "$cleanup_choice" == "y" || "$cleanup_choice" == "Y" ]]; then
	log_info "Cleaning up sources..."
	rm -rf "$WX_DIR" "$WX_TARBALL"
	log_success "Sources cleaned up"
fi

log_success "wxWidgets built successfully!"
echo "Installation locations:"
echo "  Linux: $LINUX_PREFIX"
echo "  Windows: $WIN_PREFIX"
