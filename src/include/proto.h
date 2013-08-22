#ifndef PROTO_H
#define PROTO_H

#include <stdint.h>

#include <site_types.h>

#define	DEFAULT_RCVPORT	"1999"
#define	DEFAULT_MGROUP	"224.2.2.2"

#define	CHNNR	200

#define	LISTCHNID	0
#define	MINCHNID	1
#define	MAXCHNID	(MINCHNID+CHNNR-1)

#define	MSG_CHANNEL_MAX	(512-8)
struct msg_channel_st {
	chnid_t id;	/* MUST BETWEEN [MINCHNID, MAXCHNID] */
	uint8_t data[1];
} __attribute__((packed));

struct msg_listentry_st {
	chnid_t id;     /* MUST BETWEEN [MINCHNID, MAXCHNID] */
	uint16_t len;
	uint8_t descr[1];
} __attribute__((packed));


#define	MSG_LIST_MAX	(65536-20-8)
struct msg_list_st {
	chnid_t id;     /* MUST BE LISTCHNID */
	struct msg_listentry_st entry[1];
} __attribute__((packed));

#endif

