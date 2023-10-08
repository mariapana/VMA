CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99
TARGETS = main.c vma.c ll.c locators.c menu.c

build: $(TARGETS)
	$(CC) $(CFLAGS) $^ -o vma

run_vma:
	./vma

clean:
	rm -f *.o vma