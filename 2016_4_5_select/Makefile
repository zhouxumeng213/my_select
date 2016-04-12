.PHONY:all
all:my_select client
my_select:my_select.c
	gcc -o $@ $^
client:client.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm -rf my_select client
