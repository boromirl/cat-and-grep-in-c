.PHONY: all clean rebuild s21_cat

CC = gcc
CFLAGS = -pedantic -Wall -Werror -Wextra -std=c11

TARGET1 = cat
EXECUTABLE1 = s21_cat
SRCS1 = cat.c

all: $(TARGET1)

$(TARGET1):
	$(CC) $(CFLAGS) $(SRCS1) -o $(EXECUTABLE1)

clean:
	rm -f $(OBJS1) $(EXECUTABLE1)

rebuild: clean all