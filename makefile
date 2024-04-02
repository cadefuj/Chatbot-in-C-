all: chat.c
			gcc -g -Wall -o chat chat.c

clean:
	 	$(RM) chat
