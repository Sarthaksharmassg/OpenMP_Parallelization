CC = gcc
CFLAGS = -fopenmp
TARGET = modified_jacobi
SRC = modified_jacobi.c

$(TARGET): $(SRC)
    $(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
    rm -f $(TARGET)