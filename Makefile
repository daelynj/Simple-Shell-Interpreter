.phony all:
all: sample inf arg

s: shell.c
	gcc shell.c -lreadline -o shell

r: shell
	./shell

# sample: sample.c
# 	gcc sample.c -lreadline -ltermcap -o sample

# inf: inf.c
# 	gcc inf.c -o inf

# arg: arg.c
# 	gcc arg.c -o arg

.PHONY clean:
clean:
	# -rm ./arg ./inf ./sample ./shell
	-rm ./shell
