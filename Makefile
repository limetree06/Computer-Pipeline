TARGET = simulator
SOURCES = a.cpp a.h b.cpp b.h

$(TARGET): $(SOURCES)
	g++ -o $(TARGET) -O2 -Wall $(SOURCES)

test1: $(TARGET)
	./$(TARGET) p1e1.txt 5 > out-p1e1.txt && diff p1e1-output.txt out-p1e1.txt

# This is commented out but you can add more tests if you want to
#test: test1 test2 test3 test4

test: test1 
