# Makefile -- makefile for the Knapsack solver program.
# Author: Luis Colorado <luiscoloradourcola@gmail.com>
# Date: Thu Dec 29 13:10:51 EET 2016

RM		  = rm -f

targets   = knapsack
TO_CLEAN  = $(targets)

knapsack_objs = knapsack.o
TO_CLEAN += $(knapsack_objs)
.PHONY: all

all: $(targets)

knapsack: $(knapsack_objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(knapsack_objs) $(knapsack_libs)
TO_CLEAN += knapsack

clean:
	$(RM) $(TO_CLEAN)
