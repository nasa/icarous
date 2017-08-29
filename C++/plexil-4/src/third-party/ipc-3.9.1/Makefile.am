
IPC_LOCATIONS = PUBLIC_BIN_DIR=$(bindir) PUBLIC_LIB_DIR=$(libdir) \
 PUBLIC_INC_DIR=$(includedir) PUBLIC_MAN_DIR=$(mandir) PUBLIC_DOC_DIR=$(docdir) 

IPC_SUBDIRS = SUBDIRS='etc src doc'

IPC_OPTIONS = THREADED=1 MAKE_SHARED_LIBS=1

all-local: ipc-all install-exec-local

ipc-all:
	$(MAKE) -f GNUmakefile.ipc $(AM_MAKEFLAGS) \
 $(IPC_LOCATIONS) $(IPC_OPTIONS) $(IPC_SUBDIRS) all

clean-local:
	$(MAKE) -f GNUmakefile.ipc $(AM_MAKEFLAGS) \
 $(IPC_LOCATIONS) $(IPC_OPTIONS) $(IPC_SUBDIRS) clean

install-exec-local:
	$(MAKE) -f GNUmakefile.ipc $(AM_MAKEFLAGS) \
 $(IPC_LOCATIONS) $(IPC_OPTIONS) $(IPC_SUBDIRS) install_incs install_libs install_bins

install-data-local:
	$(MAKE) -f GNUmakefile.ipc $(AM_MAKEFLAGS) \
 $(IPC_LOCATIONS) $(IPC_OPTIONS) $(IPC_SUBDIRS) install_mans install_docs

uninstall-local:
	$(MAKE) -f GNUmakefile.ipc $(AM_MAKEFLAGS) \
 $(IPC_LOCATIONS) $(IPC_OPTIONS) $(IPC_SUBDIRS) uninstall

# Distribute files from here.
#EXTRA_DIST = subdir/Makefile subdir/program.c ...

EMPTY_AUTOMAKE_TARGETS = dvi pdf ps info html ctags check-local

.PHONY: $(EMPTY_AUTOMAKE_TARGETS)

$(EMPTY_AUTOMAKE_TARGETS):