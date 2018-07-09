package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"time"
)

type client chan<- string

var (
	entering = make(chan client)
	leaving  = make(chan client)
	messages = make(chan string)
)

func broadcaster() {
	clients := make(map[client]bool)
	for {
		select {
		case msg := <-messages:
			for cli := range clients {
				cli <- msg
			}

		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
}

func handleConn(c net.Conn) {
	ch := make(chan string)
	go clientWriter(c, ch)

	who := c.RemoteAddr().String()
	ch <- "You are " + who
	messages <- who + " has arrived"
	entering <- ch

	input := bufio.NewScanner(c)
	for input.Scan() {
		messages <- who + ": " + input.Text() + "\t" + time.Now().Format("17:02:02")
	}

	leaving <- ch
	messages <- who + " has left"
	c.Close()
}

func clientWriter(c net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(c, msg)
	}
}

func main() {
	listener, err := net.Listen("tcp", ":8000")
	if err != nil {
		log.Fatal(err)
	}

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Fatal(err)
			continue
		}
		go handleConn(conn)
	}
}
