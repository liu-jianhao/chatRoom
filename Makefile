all:
	gcc -o trans_ser trans_ser.c
	gcc -o trans_cli trans_cli.c


clean:
	rm -rf trans_cli trans_ser ser cli
