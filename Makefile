CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LIBS = -lssl -lcrypto

TARGET = securevault

SRC = src/main.cpp \
      src/authentication.cpp \
      src/file_manager.cpp \
      src/encryption.cpp \
      src/permissions.cpp \
      src/logger.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
