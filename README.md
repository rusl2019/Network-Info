# Network Info

A cross-platform GUI application built with wxWidgets to display network interface information, specifically for Ethernet interfaces. It shows the interface type, MAC address, and IPv4 address in a simple, user-friendly interface. The application runs on both **Linux** and **Windows**, with support for cross-compilation from Linux to Windows using MinGW-w64.

## Features
- Displays network interface details:
  - **Type**: Ethernet interfaces only (e.g., `eth0` on Linux, `Ethernet 1` on Windows).
  - **MAC Address**: Hardware address of the interface.
  - **IPv4 Address**: Assigned IP address.
- Cross-platform support:
  - Linux (using GTK backend).
  - Windows (using MSW backend).
- Simple GUI with a "Refresh" button to update network information.
- Lightweight, using minimal wxWidgets libraries (`core` and `base`).

## Prerequisites
- **Linux**:
  - `g++` (GCC compiler).
  - wxWidgets 3.2 or later with GTK backend (`libwxgtk3.0-gtk3-dev` or equivalent).
- **Windows (cross-compilation)**:
  - `mingw-w64` (for cross-compiling from Linux).
  - wxWidgets 3.2 or later, built for Windows with MinGW-w64.
- **Optional**:
  - Git for cloning the repository.
  - Make for building with the provided Makefile.

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/rusl2019/Network-Info.git
   cd Network-Info
   ```

2. Ensure wxWidgets is compilled:
   Build wxWidgets for Linux and Windows using `tools/build_wxwidgets.sh`:
   ```bash
   ./tools/build_wxwidgets.sh
   ```

## Building
The project includes a `Makefile` for building on Linux and cross-compiling for Windows.

- **Build for Linux**:
  ```bash
  make clean
  make linux
  ```

- **Build for Windows (from Linux)**:
  ```bash
  make windows
  ```

- **Clean build artifacts**:
  ```bash
  make clean
  ```

## Running
- **Linux**:
  ```bash
  ./network_info
  ```

- **Windows**:
  - Copy `network_info.exe` to a Windows machine.
  - If using static libraries (recommended), run directly:
    ```
    .\network_info.exe
    ```
  - If using dynamic libraries, ensure wxWidgets DLLs (e.g., `wxmsw32u_core_gcc_custom.dll`, `wxbase32u_gcc_custom.dll`) are in the same directory as `network_info.exe`.

## Code Structure
- `network_info.cpp`: Main source file containing:
  - Cross-platform network interface enumeration (using `getifaddrs` on Linux and `GetAdaptersAddresses` on Windows).
  - wxWidgets GUI with a listbox to display Ethernet interface details.
- `Makefile`: Build script for Linux and Windows targets.

## Dependencies
- **wxWidgets**: For the GUI framework (version 3.2 or later).
- **Linux**:
  - Standard networking headers (`<ifaddrs.h>`, `<linux/if_arp.h>`, etc.).
- **Windows**:
  - Winsock (`ws2_32`) and IP Helper (`iphlpapi`) libraries.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing
Feel free to submit issues or pull requests for improvements, such as:
- Adding support for IPv6 addresses.
- Enhancing the GUI (e.g., using a table format).
- Supporting additional interface types (e.g., Wi-Fi).

## Acknowledgments
- Built with [wxWidgets](https://www.wxwidgets.org/) for cross-platform GUI.
- Inspired by the need for a simple network information tool.