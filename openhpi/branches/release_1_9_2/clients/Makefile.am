MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES        = @TEST_CLEAN@

INCLUDES                = -I$(top_srcdir)/include -I$(top_srcdir)/utils

COMMONLIBS              = $(top_builddir)/src/libopenhpi.la @STATIC_PLUGIN_LIBS@ @STATIC_PLUGIN_EXTRA_LIBS@

AM_CFLAGS               = @CFLAGS@

bin_PROGRAMS = \
	hpigetevents \
        hpiinv \
	hpiel \
	hpisensor \
        hpipower \
	hpitree \
	hpialarmpanel \
	hpifan \
	hpireset \
	hpireset2 \
	hpiwdt \
	hpipower2 \
	hpisettime
	

#bin_PROGRAMS           = hpithres \
#                         hpifru \
#                         hpievent

include_HEADERS         =

hpialarmpanel_SOURCES   = hpialarmpanel.c
hpialarmpanel_LDADD     = $(COMMONLIBS)
hpialarmpanel_LDFLAGS   =

#hpithres_SOURCES        = hpithres.c
#hpithres_LDADD          = $(COMMONLIBS)
#hpithres_LDFLAGS        =

hpifan_SOURCES          = hpifan.c
hpifan_LDADD            = $(COMMONLIBS)
hpifan_LDFLAGS          =

#hpifru_SOURCES          = hpifru.c
#hpifru_LDADD            = $(COMMONLIBS)
#hpifru_LDFLAGS          =

hpireset_SOURCES        = hpireset.c
hpireset_LDADD          = $(COMMONLIBS)
hpireset_LDFLAGS        =

hpireset2_SOURCES       = hpireset2.c
hpireset2_LDADD         = $(COMMONLIBS)
hpireset2_LDFLAGS       =

hpiel_SOURCES           = hpiel.c
hpiel_LDADD             = $(COMMONLIBS)
hpiel_LDFLAGS           =

hpisensor_SOURCES       = hpisensor.c
hpisensor_LDADD         = $(COMMONLIBS)
hpisensor_LDFLAGS       =

hpiwdt_SOURCES          = hpiwdt.c
hpiwdt_LDADD            = $(COMMONLIBS)
hpiwdt_LDFLAGS          =

hpiinv_SOURCES          = hpiinv.c
hpiinv_LDADD            = $(COMMONLIBS)
hpiinv_LDFLAGS          =

hpisettime_SOURCES      = hpisettime.c
hpisettime_LDADD        = $(COMMONLIBS)
hpisettime_LDFLAGS      =

hpigetevents_SOURCES      = hpigetevents.c
hpigetevents_LDADD        = $(COMMONLIBS)
hpigetevents_LDFLAGS      =

hpipower_SOURCES      = hpipower.c
hpipower_LDADD        = $(COMMONLIBS)
hpipower_LDFLAGS      =

hpipower2_SOURCES      = hpipower2.c
hpipower2_LDADD        = $(COMMONLIBS)
hpipower2_LDFLAGS      =

#hpievent_SOURCES      = hpievent.c
#hpievent_LDADD        = $(COMMONLIBS)
#hpievent_LDFLAGS      =

hpitree_SOURCES      = hpitree.c
hpitree_LDADD        = $(COMMONLIBS)
hpitree_LDFLAGS      =

clean-local:
	rm -f *~ *.o

