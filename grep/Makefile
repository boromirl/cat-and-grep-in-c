.PHONY: all clean rebuild s21_grep

CC = gcc
CFLAGS = -pedantic -Wall -Werror -Wextra -std=c11

TARGET1 = s21_grep
EXECUTABLE1 = s21_grep
SRCS1 = s21_grep.c

all: $(TARGET1)

$(TARGET1):
	$(CC) $(CFLAGS) $(SRCS1) -o $(EXECUTABLE1)

clean:
	rm -f $(EXECUTABLE1)

rebuild: clean all