chip8: main.o chip8.o hardware.o
	$(CXX) $^ -pthread -o $@

main.o: main.cpp chip8.hpp
	$(CXX) -c main.cpp

chip8.o: chip8.cpp chip8.hpp
	$(CXX) -c chip8.cpp

hardware.o: hardware.cpp hardware.hpp
	$(CXX) -c hardware.cpp

chip8.hpp: hardware.hpp

clean:
	rm chip8 *.o
