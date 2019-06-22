#include <sys/socket.h>
#include <durex.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <strings.h>

/*
 * Fork a new child for begin shell or anything else
 */

int start_daemon_child() {
	pid_t			pid;

	/* Start process */
	pid = fork();
	if (pid < 0) {
		log_durex("error: fork\n");
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		return (pid);
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
	return (pid);
}

/*
 * Return a new socket
 */

int create_interactive_socket(int port) {
	static const int QUEUE = 1;
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