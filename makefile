cube: wizard.o cube.o
	cc -g cube.o wizard.o -lreadline -lhistory -lncurses -lpthread -o cube

wizard.o: wizard.c wizard.h
	cc -c wizard.c

cube.o: cube.c cube.h
	cc -c cube.c

clean: 
	rm cube wizard.o cube.o