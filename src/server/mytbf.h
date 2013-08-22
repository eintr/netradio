#ifndef MYTBF_H
#define MYTBF_H

#define	TBFMAX	1024

typedef void mytbf_t;

mytbf_t *mytbf_init(int cps, int burst);

int mytbf_destroy(mytbf_t *);

int mytbf_fetchtoken(mytbf_t *, int n);

int mytbf_returntoken(mytbf_t *, int n);

#endif

