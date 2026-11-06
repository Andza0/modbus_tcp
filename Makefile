SRCS := $(wildcard *.c)
OBJECTS := $(SRCS:.c=.o)
TARGET := mbread
CFLAGS := -std=c11 -D_DEFAULT_SOURCE -Wall -Wextra

all: clean $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
