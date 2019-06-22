#include <stdio.h>
#include <durex.h>
#include <stdlib.h>
#include <unistd.h>

int		main() {
	int			sock;

	log_durex("-------------------- NEW SESSION -----------------------\n");
	dprintf(log_durex(NULL), "Start MAIN on PID : %d\n", getpid());

	sock = create_server(PORT);
	start_daemon();
	while (0x2a)
		listen_socket(sock);
	close(sock);
	exit(EXIT_SUCCESS);
}





/*
 * Server : Listen on port 4242 ->
 * 									- Can read input		->
 * 									- Quit					|	Shell en background * 3
 * 															|
 *
 *
 *
 *
 * */