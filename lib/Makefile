LD		= gcc

CPPFLAGS	= -I. -D_GNU_SOURCE
LDFLAGS		= -lsqlite -lpq -lmysqlclient -lconfig

CFLAGS		= -Wall -Werror -g -O2 -pthread -fPIC

SOLIB		= libbluecherry.so
SOLIBVER	= libbluecherry.so.0
OBJS		= bc-core.o bc-utils.o bc-db-core.o bc-db-sqlite.o \
		  bc-db-psql.o bc-db-mysql.o

all: $(SOLIBVER)

$(SOLIBVER): $(OBJS)
	$(LD) $(LDFLAGS) -pthread -shared -Wl,-soname=$@ -o $@ $(OBJS)
	ln -sf $@ $(SOLIB)

clean:
	rm -f $(OBJS) $(SOLIBVER) $(SOLIB)
