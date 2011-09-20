bin_PROGRAMS=sshshare
sshshare_SOURCES=config.cxx echoprocess.cxx linewiseprocess.cxx main.cxx password.cxx process.cxx scp.cxx shares.cxx sshprocess.cxx sshshare_config.cxx sshshare.cxx
noinst_HEADERS=config.hxx echoprocess.hxx linewiseprocess.hxx password.hxx process.hxx scp.hxx shares.hxx sshprocess.hxx sshshare_config.hxx sshshare.hxx

sshshare_CPPFLAGS=$(DEPS_CFLAGS) $(BOOST_CPPFLAGS)
AM_LDFLAGS=$(DEPS_LIBS) -lutil  $(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_REGEX_LDFLAGS) $(BOOST_DATE_TIME_LDFLAGS) $(BOOST_LDFLAGS)

sshshare_LDADD=$(BOOST_FILESYSTEM_LIBS) $(BOOST_REGEX_LIBS) $(BOOST_DATE_TIME_LIBS) $(BOOST_LDADD) $(BOOST_SYSTEM_LIBS)





