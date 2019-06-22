#ifndef DUREX_DUREX_H
#define DUREX_DUREX_H

#include <stdint.h>
#include <stdbool.h>

/*
 * MACRO
 */

#define PORT 7000

/*
 * STRUCTURE
 */

typedef struct	s_cmd {
	char		*line;
	void		(*function)(int);
}				t_cmd;

/*
 * PROTOTYPE
 */

void			start_daemon();
int				create_server(uint16_t socket);
void			listen_socket(int socket);
void			listen_client(int csocket);
int				create_interactive_socket(int port);
int				start_daemon_child();
int				*files_descriptor();


int				log_durex(char *str);
#endif //DUREX_DUREX_H
