#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <proto.h>

#include "medialib.h"
#include "server_conf.h"

#include "mytbf.h"

#define	PATHSIZE	1024

struct channel_context_st {
	chnid_t id;
	char *desc;
	glob_t mp3glob;
	int pos;
	off_t offset;
	int fd;
	mytbf_t *tbf;
};

static struct channel_context_st channel[MAXCHNID+1];

static struct channel_context_st *path2entry(const char *path)
{
	static chnid_t curr_id=MINCHNID;
	struct channel_context_st *me;
	FILE *fp;
#define	BUFSIZE	1024
	char pathstr[PATHSIZE], linebuf[BUFSIZE];

	syslog(LOG_DEBUG, "%s is processing path %s.", __FUNCTION__, path);

	strncpy(pathstr, path, PATHSIZE);
	strncat(pathstr, "/desc.text", PATHSIZE);

	fp = fopen(pathstr, "r");
	if (fp==NULL) {
		syslog(LOG_INFO, "%s is not a channel dir (Can't find desc.text) .", path);
		return NULL;
	}

	if (fgets(linebuf, BUFSIZE, fp)==NULL) {
		syslog(LOG_INFO, "%s is not a channel dir (Can't get description text).", path);
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	me = malloc(sizeof(*me));
	if (me==NULL) {
		syslog(LOG_ERR, "malloc(): %s", strerror(errno));
		return NULL;
	}
	me->tbf = mytbf_init(MP3_BITRATE/8, MP3_BITRATE*10/8);
	if (me->tbf==NULL) {
		syslog(LOG_ERR, "mytbf_init(): %s", strerror(errno));
		free(me);
		return NULL;
	}
	me->desc = strdup(linebuf);

        strncpy(pathstr, path, PATHSIZE);
        strncat(pathstr, "/*.mp3", PATHSIZE);

	if (glob(pathstr, 0, NULL, &me->mp3glob)!=0) {
		syslog(LOG_ERR, "%s is not a channel dir (Can't find mp3 files).", path);
		mytbf_destroy(me->tbf);
		free(me);
		return NULL;
	}
	me->pos = 0;
	me->offset = 0;
	me->fd = open(me->mp3glob.gl_pathv[me->pos], O_RDONLY);
	if (me->fd<0) {
		syslog(LOG_WARNING, "%s open failed.",me->mp3glob.gl_pathv[me->pos] );
		mytbf_destroy(me->tbf);
		free(me);
		return NULL;
	}
	me->id = curr_id;
	curr_id++;

	syslog(LOG_DEBUG, "Contructed {%d, \"%s\"}", me->id, me->desc);

	return me;
}

int mlib_getchnlist(struct mlib_listentry_st **result, int *resnum)
{
	char path[PATHSIZE];
	glob_t globres;
	struct mlib_listentry_st *ptr;
	struct channel_context_st *res;
	int num, i;

	for (i=0;i<MAXCHNID+1;++i) {
		channel[i].id=-1;
	}

	snprintf(path, PATHSIZE, "%s/*", server_conf.media_dir);
	if (glob(path, 0, NULL, &globres)) {
		return -1;
	}
	ptr = malloc(globres.gl_pathc*sizeof(struct channel_context_st));
	if (ptr==NULL) {
		perror("malloc()");
		exit(1);
	}
	num = 0;
	for (i=0;i<globres.gl_pathc;++i) {
		res=path2entry(globres.gl_pathv[i]);
		if (res!=NULL) {
			syslog(LOG_DEBUG, "path2entry() returned {%d, \"%s\"}", res->id, res->desc);
			memcpy(channel+res->id, res, sizeof(*res));
			ptr[num].id = res->id;
			ptr[num].desc = strdup(res->desc);
			num++;
		}
	}
	*result = realloc(ptr, num*sizeof(struct mlib_listentry_st));
	*resnum = num;

	return 0;
}

static int open_next(chnid_t id)
{
	int i;

	for (i=0;i<channel[id].mp3glob.gl_pathc;++i) {
		channel[id].pos++;
		if (channel[id].pos==channel[id].mp3glob.gl_pathc) {
			channel[id].pos=0;
		}
		close(channel[id].fd);
		channel[id].fd = open(channel[id].mp3glob.gl_pathv[channel[id].pos], O_RDONLY);
		if (channel[id].fd<0) {
			syslog(LOG_WARNING, "open(%s): %s.", channel[id].mp3glob.gl_pathv[channel[id].pos], strerror(errno));
		} else {
			syslog(LOG_DEBUG, "open(%s): SUCCEEDED.", channel[id].mp3glob.gl_pathv[channel[id].pos]);
			channel[id].offset = 0;
			return 0;
		}
	}
	syslog(LOG_ERR, "None of mp3s in channel %d is available.", id);
	exit(1);
}

ssize_t mlib_readchn(chnid_t id, void *buf, size_t size)
{
	int tbfsize, len;

	tbfsize = mytbf_fetchtoken(channel[id].tbf, size);
	while (1) {
		len = pread(channel[id].fd, buf, tbfsize, channel[id].offset);
		if (len<0) {
			syslog(LOG_WARNING, "media file %s read() failed.", channel[id].mp3glob.gl_pathv[channel[id].pos]);
			open_next(id);
		} else if (len==0) {
			syslog(LOG_DEBUG, "media file %s is over.", channel[id].mp3glob.gl_pathv[channel[id].pos]);
			open_next(id);
		} else {
			channel[id].offset += len;
			break;
		}
	}
	mytbf_returntoken(channel[id].tbf, tbfsize-len);
	return len;
}

