# An IM makefile defines OBJS and GOALS, and the direct rule
# to build GOALS from your OBJS. Other rules are deal in ov.*.mk
IMID=OVKPAbbreviator
SOURCES=OVKPAInterface OVKPAbbreviator OVKPPhrase
LDFLAGS+=-lsqlite3
CFLAGS+=-I../OVKPPhrase
VPATH+=../OVKPPhrase

include ../../Mk/ov.os.mk
include ../../Mk/ov.im.mk
include ../../Mk/ov.vars.mk
include ../../Mk/ov.general.mk


