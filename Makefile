TARGET = simulator
<<<<<<< HEAD
SOURCES = a.cpp a.h
=======
SOURCES = a.cpp a.h b.cpp b.h
>>>>>>> a1acfe7e747e93aae55cea8e2c16d145042472b4

$(TARGET): $(SOURCES)
	g++ -o $(TARGET) -O2 -Wall $(SOURCES)

test1: $(TARGET)
<<<<<<< HEAD
	./$(TARGET) p1e1.txt 5 > out-p1e1.txt
	./$(TARGET) p1e2.txt 10 > out-p1e2.txt
	./$(TARGET) p1e3.txt 20 > out-p1e2.txt
	./$(TARGET) p1e4.txt 70 > out-p1e4.txt

# #test: test1 test2 test3 test4

test: test1

=======
	./$(TARGET) p1e1.txt 5 > out-p1e1.txt && diff p1e1-output.txt out-p1e1.txt

# This is commented out but you can add more tests if you want to
#test: test1 test2 test3 test4

test: test1 
>>>>>>> a1acfe7e747e93aae55cea8e2c16d145042472b4
