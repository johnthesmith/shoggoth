CC=g++ -c
OBJ=./obj/
LIB=./lib/


point3i.o: $(LIB)point3i.cpp
	$(CC) $(LIB)point3i.cpp -o $(OBJ)point3i.o

