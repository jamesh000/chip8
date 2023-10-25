Chip8: Main.o Chip8.o Hardware.o
	$(CXX) $^ -pthread -o $@

Main.o: Main.cpp Chip8.h
	$(CXX) -c Main.cpp

Chip8.o: Chip8.cpp Chip8.h
	$(CXX) -c Chip8.cpp

Hardware.o: Hardware.cpp Chip8.h
	$(CXX) -c Hardware.cpp
