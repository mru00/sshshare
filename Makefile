CXXFLAGS+=-O2
CXXFLAGS+=`pkg-config gtk+-2.0 --cflags`
LDFLAGS+=-lutil -lboost_filesystem -lboost_system -lboost_regex -lboost_date_time -lxerces-c `pkg-config gtk+-2.0 --libs`


CXXFILES=sshshare/config.cxx sshshare/main.cxx sshshare/scp.cxx sshshare/sshshare_config.cxx sshshare/echoprocess.cxx sshshare/password.cxx sshshare/shares.cxx sshshare/sshshare.cxx sshshare/linewiseprocess.cxx sshshare/process.cxx sshshare/sshprocess.cxx 


%.o: %.cxx
	$(COMPILE.cpp) $(OUTPUT_OPTION) -c $<


sshshare/sshshare: $(CXXFILES:.cxx=.o)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@
