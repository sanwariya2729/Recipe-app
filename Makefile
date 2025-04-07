CXX = g++
WX_FLAGS = `wx-config --cxxflags --libs`
MYSQL_FLAGS = -lmysqlcppconn
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Wno-deprecated-copy $(WX_FLAGS) -I/usr/include/cppconn

LDFLAGS = $(WX_FLAGS) $(MYSQL_FLAGS)

SRC = App.cpp 
OBJ = $(SRC:.cpp=.o)
TARGET = recipeapp

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

