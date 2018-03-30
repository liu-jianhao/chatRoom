all:
	gcc -o trans_ser trans_ser.c
	gcc -o trans_cli trans_cli.c


clean:
	rm trans_cli trans_ser
