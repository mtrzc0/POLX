include ../conf.mk

DIRS = ./terminal \
	./keyboard \
	./timer

all: 
	for dir in $(DIRS); do \
		make -C $$dir; \
	done
