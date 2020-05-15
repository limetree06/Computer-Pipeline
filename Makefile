TARGET = simulator
SOURCES = simulator.cpp simulator.h

$(TARGET): $(SOURCES)
	g++ -o $(TARGET) -O2 -Wall $(SOURCES)

test1: $(TARGET)
	./$(TARGET) End.txt 100 00 > out-End.txt
	./$(TARGET) Logical.txt 100 00 > out-Logical.txt
	./$(TARGET) Memory.txt 100 00 > out-Memory.txt
	./$(TARGET) SU.txt 100 00 > out-SU.txt
	./$(TARGET) Sum.txt 100 00 > out-Sum.txt
	./$(TARGET) Swap.txt 100 00 > out-Swap.txt
	./$(TARGET) Fibonacci.txt 100 00 > out-Fibonacci.txt



# This is commented out but you can add more tests if you want to
#test: test1 test2 test3 test4

test: test1
