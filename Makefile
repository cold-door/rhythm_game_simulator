#
# How to use this Makefile with make command
# make NAME=kadai
#

NAME	= rhythm_game_simulator

all: $(NAME).mb

$(NAME).s: $(NAME).c
	arm-thumb-elf-gcc -S $(NAME).c

$(NAME).o: $(NAME).s
	arm-thumb-elf-as -o $(NAME).o $(NAME).s

$(NAME).out: $(NAME).o
	arm-thumb-elf-ld -T gcc.ls -o $(NAME).out $(NAME).o

$(NAME).mb: $(NAME).out
	arm-thumb-elf-objcopy -O binary $(NAME).out $(NAME).mb

clean:
	rm *.o *.s *.out *.mb *.BAK

