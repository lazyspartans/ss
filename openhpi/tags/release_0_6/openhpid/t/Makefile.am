#
# Copyright (c) 2004 by FORCE Computers.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Authors:
#    Thomas Kanngieser <thomas.kanngieser@fci.com>
#

REMOTE_SOURCES		= simple_config.c connection.c marshal.c 

MOSTLYCLEANFILES 	= $(REMOTE_SOURCES) $(shell ls *.bb*) $(shell ls *.da) $(shell ls *.gcov)

INCLUDES		= -I..

AM_CFLAGS		= @CFLAGS@ -ftest-coverage -fprofile-arcs


noinst_PROGRAMS = float_format

float_format_SOURCES = float_format.c

CLEANFILES=float32.bin float64.bin *~

$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -s ../$@; \
	fi

TESTS = \
	config_000 \
	connection_000 \
	connection_001 \
	marshal_000 \
	marshal_001 \
	marshal_002 \
	marshal_003 \
	marshal_004 \
	marshal_005 \
	marshal_006 \
	marshal_007 \
	marshal_008 \
	marshal_009 \
	marshal_010 \
	marshal_011 \
	marshal_012 \
	marshal_013 \
	marshal_014 \
	marshal_015 \
	marshal_016 \
	marshal_017 \
	marshal_018 \
	marshal_019 \
	marshal_020 \
	marshal_021 \
	marshal_022 \
	marshal_023 \
	marshal_024 \
	marshal_025 \
	marshal_026

check_PROGRAMS = $(TESTS)

config_000_SOURCES = config_000.c $(REMOTE_SOURCES)

connection_000_SOURCES = connection_000.c $(REMOTE_SOURCES)
connection_001_SOURCES = connection_001.c $(REMOTE_SOURCES)

marshal_000_SOURCES = marshal_000.c $(REMOTE_SOURCES)
marshal_001_SOURCES = marshal_001.c $(REMOTE_SOURCES)
marshal_002_SOURCES = marshal_002.c $(REMOTE_SOURCES)
marshal_003_SOURCES = marshal_003.c $(REMOTE_SOURCES)
marshal_004_SOURCES = marshal_004.c $(REMOTE_SOURCES)
marshal_005_SOURCES = marshal_005.c $(REMOTE_SOURCES)
marshal_006_SOURCES = marshal_006.c $(REMOTE_SOURCES)
marshal_007_SOURCES = marshal_007.c $(REMOTE_SOURCES)
marshal_008_SOURCES = marshal_008.c $(REMOTE_SOURCES)
marshal_009_SOURCES = marshal_009.c $(REMOTE_SOURCES)
marshal_010_SOURCES = marshal_010.c $(REMOTE_SOURCES)
marshal_011_SOURCES = marshal_011.c $(REMOTE_SOURCES)
marshal_012_SOURCES = marshal_012.c $(REMOTE_SOURCES)
marshal_013_SOURCES = marshal_013.c $(REMOTE_SOURCES)
marshal_014_SOURCES = marshal_014.c $(REMOTE_SOURCES)
marshal_015_SOURCES = marshal_015.c $(REMOTE_SOURCES)
marshal_016_SOURCES = marshal_016.c $(REMOTE_SOURCES)
marshal_017_SOURCES = marshal_017.c $(REMOTE_SOURCES)
marshal_018_SOURCES = marshal_018.c $(REMOTE_SOURCES)
marshal_019_SOURCES = marshal_019.c $(REMOTE_SOURCES)
marshal_020_SOURCES = marshal_020.c $(REMOTE_SOURCES)
marshal_021_SOURCES = marshal_021.c $(REMOTE_SOURCES)
marshal_022_SOURCES = marshal_022.c $(REMOTE_SOURCES)
marshal_023_SOURCES = marshal_023.c $(REMOTE_SOURCES)
marshal_024_SOURCES = marshal_024.c $(REMOTE_SOURCES)
marshal_025_SOURCES = marshal_025.c $(REMOTE_SOURCES)
marshal_026_SOURCES = marshal_026.c $(REMOTE_SOURCES)
