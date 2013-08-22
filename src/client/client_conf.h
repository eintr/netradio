#ifndef CLIENT_CONF_H
#define CLIENT_CONF_H

struct client_conf_st {
	char *rcvport;
	char *mgroup;
	char *player_cmd;
};

extern struct client_conf_st client_conf;

#define	DEFAULT_PLAYERCMD	"/usr/bin/mpg123 -y - > /dev/null"

#endif

