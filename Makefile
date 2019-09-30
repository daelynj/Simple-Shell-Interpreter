.phony all:
all: shell

shell: shell.c
	gcc shell.c -lreadline -o shell

run: shell
	./shell

.PHONY clean:
clean:
	-rm ./shell
