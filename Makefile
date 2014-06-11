LIBS=-lresolv
DESTDIR=/usr/local/

srv: main.c
	$(CC) -o $@ $(LIBS) $^

install: srv
	install srv $(DESTDIR)/bin

test: srv
	./srv _sip._tcp.cisco.com
