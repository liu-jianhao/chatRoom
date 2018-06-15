all:
	gcc -g -Wall -I /usr/include/mysql/ -o connect connect.c -L /usr/bin/mysql -lmysqlclient
	gcc -g -Wall -I /usr/include/mysql/ -o select select.c -L /usr/bin/mysql -lmysqlclient
	gcc -g -Wall -I /usr/include/mysql/ -o update update.c -L /usr/bin/mysql -lmysqlclient


clean:
	rm -rf select connect update
