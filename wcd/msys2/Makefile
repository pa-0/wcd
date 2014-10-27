
PREFIX = /usr
prefix = $(PREFIX)
exec_prefix = $(prefix)
MKDIR  = mkdir

ENABLE_NLS = 1

bindir    = ${exec_prefix}/bin
PROGRAM = wcdmsys.exe
MACHINE := $(subst -pc-msys,,$(shell gcc -dumpmachine))
VERSIONSUFFIX = -msys2-$(MACHINE)


all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} prefix=${prefix} MKDIR=${MKDIR} INSTALL_OBJS_DOC_INSTALL="../doc/INST_DOS.txt ../doc/INSTALL.txt" ENABLE_NLS=${ENABLE_NLS}

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS}


dist-zip :
	${MAKE} -C ../src dist-zip PROGRAM=${PROGRAM} prefix=${prefix} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

dist-tgz :
	${MAKE} -C ../src dist-tgz PROGRAM=${PROGRAM} prefix=${prefix} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

dist : dist-tgz

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

mostlyclean :
	${MAKE} -C ../src mostlyclean

clean :
	${MAKE} -C ../src clean

