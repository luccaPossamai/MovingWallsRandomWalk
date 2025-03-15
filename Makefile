INCLUDES = includes
TARGET = program

comp: $(TARGET).c
	gcc -I$(INCLUDES) -O3 -o run $(TARGET).c -Wall -lm
comprun: $(TARGET).c
	gcc -I$(INCLUDES) -O3 -o run $(TARGET).c -Wall -lm
	./run
clean:
	rm -f $(TARGET)
