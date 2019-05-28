CSRC = \
			code_generator.c \
			error_handler.c \
			node.c \
			tokenizer.c \
			9cc.c \

CFLAGS = -g -Wall
CPPFLAGS = \
          -I./linkedlist/src/
LDFLAGS = \
          -L./linkedlist/src/
LDLIBS = \
          -llinkedlist

9cc: $(CSRC)

clean:
	rm -f 9cc *.o *~ tmp*
