#ifndef SERVER_CONF_H
#define SERVER_CONF_H

enum {
	run_daemon,
	run_foreground
};

struct server_conf_st {
	char *rcvport;
	char *mgroup;
	char *media_dir;
	char runmode;
	char *ifname;
};

extern struct server_conf_st server_conf;

#define	DEFAULT_MEDIADIR	"/var/media"
#define	DEFAULT_IF	"eth0"

extern int serversd;
extern struct sockaddr_in sndaddr;

#endif

