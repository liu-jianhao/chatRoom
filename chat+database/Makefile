target:
	gcc -g -Wall -I /usr/include/mysql/ -o server server.c -L /usr/bin/mysql -lmysqlclient
	gcc client.c -o client

clean:
	rm -rf server client
