CFLAGS 	:= 	-I /usr/include/glib-2.0/ 	\
		   	-I /usr/include/dbus-1.0	\
			-I /usr/lib64/dbus-1.0/include
CFLAGS  += `pkg-config --cflags glib-2.0`
LDFLAGS += `pkg-config --libs glib-2.0`
LIBS	:= -ldbus-1
all:listen send listen_shutdown
listen:listen.c
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

send:send.c
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

listen_shutdown:listen_shutdown.c
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)
