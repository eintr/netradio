#ifndef THR_CHANNEL_H
#define THR_CHANNEL_H

#include "medialib.h"

int thr_channel_create(struct mlib_listentry_st*);
int thr_channel_destroy(struct mlib_listentry_st*);

int thr_channel_destroyall(void);

#endif

