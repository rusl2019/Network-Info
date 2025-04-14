# Variabel umum
TARGET = network_info

# Variabel untuk Linux
LINUX_CXX = g++
LINUX_WXCONFIG = /usr/bin/wx-config
LINUX_WXFLAGS = `$(LINUX_WXCONFIG) --cxxflags --libs core,base`

# Variabel untuk Windows (cross-compile)
WIN_CXX = x86_64-w64-mingw32-g++
WIN_WXCONFIG = /usr/local/bin/wx-config
WIN_LIBS = -lws2_32 -liphlpapi -lcomctl32 -luser32 -lgdi32 -lole32 -loleaut32 -luuid
WIN_WXFLAGS = `$(WIN_WXCONFIG) --cxxflags --libs core,base --static`
WIN_LDFLAGS = -static-libgcc -static-libstdc++

all: linux windows

linux:
	$(LINUX_CXX) $(TARGET).cpp -o $(TARGET) $(LINUX_WXFLAGS)

windows:
	$(WIN_CXX) $(TARGET).cpp -o $(TARGET).exe $(WIN_WXFLAGS) $(WIN_LIBS) $(WIN_LDFLAGS)

clean:
	rm -f $(TARGET) $(TARGET).exe *.o

.PHONY: all linux windows clean
