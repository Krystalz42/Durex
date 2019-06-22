

#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <durex.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <sys/errno.h>
#include <assert.h>

int log_durex(char *str) {
	static int fildes;
	const char *path = "/tmp/log_durex.log";

	if (fildes == 0) {
		fildes = open(path, O_WRONLY | O_CREAT | O_TRUNC, 644);
	}
	if (str)
		write(fildes, str, strlen(str));
	return fildes;
}

void listen_socket(int socket) {
	int tsock;

	if ((tsock = accept(socket, NULL, NULL)) == -1) { //7000
		perror("accept");
		exit(EXIT_FAILURE);
	}

	listen_client(tsock);
	dprintf(log_durex(NULL), "Close socket [%d] pid : %d return : %d\n",socket,getpid(), close(tsock));
}
/*
 * Return true if the pid == father
 */
void start_daemon() {
	pid_t			pid;

	/* Start process */
	pid = fork();
	if (pid < 0) {
		log_durex("error: fork\n");
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		exit(EXIT_SUCCESS);
	} else {
		dprintf(log_durex(NULL), "Child daemon running on pid : %d\n", getpid());
	}

	/* Create the file mode mask */

	umask(0);

	/* Create a new SID for the child process */

	if (setsid() == -1) {
		dprintf(log_durex(NULL),"error: setsid error [%s]\n", strerror(errno));

		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */

	if (chdir("/") < 0) {
		log_durex("error: chdir\n");
		exit(EXIT_FAILURE);
	}

	/* Close the files descriptor */

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

/*
 * Basic creation of server, ready to accept
 * */

int		create_server(uint16_t port) {
	static const int QUEUE = 3;
	int			sock;
	struct protoent *proto;
	struct sockaddr_in sin;

	if ((proto = getprotobyname("tcp")) == NULL) {
		log_durex("error: getprotobyname\n");
		exit(1);
	}

	if ((sock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) == -1) {
		log_durex("error: socket\n");
		exit(1);
	}

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

	sin.sin_port = htons(port);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((bind(sock, (const struct sockaddr *)&sin, sizeof(sin))) == -1) {
		log_durex("error: bind:\n");
		exit(1);
	}

	if (listen(sock, QUEUE) == -1) {
		log_durex("error: listen:\n");
		exit(1);
	}
	return (sock);
}






















