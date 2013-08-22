DIRS=doc src
INSTALLDIR=/usr/local

all:
%:
	for i in $(DIRS); do	\
		make -C $$i $@;		\
	done

install:
	for i in $(DIRS); do	\
		make -C $$i install;		\
	done
	cp -r dist $(INSTALLDIR)/netradio

