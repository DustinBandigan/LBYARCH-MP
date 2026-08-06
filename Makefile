CC = gcc
ASM = nasm

CFLAGS = -Wall -Wextra -O2 -std=c11
ASMFLAGS = -f elf64

TARGET = grayscale
C_OBJECT = main.o
ASM_OBJECT = imgCvtGrayIntToDouble.o

all: $(TARGET)

$(TARGET): $(C_OBJECT) $(ASM_OBJECT)
	$(CC) $(C_OBJECT) $(ASM_OBJECT) -o $(TARGET) -lm -no-pie

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

imgCvtGrayIntToDouble.o: imgCvtGrayIntToDouble.asm
	$(ASM) $(ASMFLAGS) imgCvtGrayIntToDouble.asm -o imgCvtGrayIntToDouble.o

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(C_OBJECT) $(ASM_OBJECT) $(TARGET)
