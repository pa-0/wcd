include wcd/src/version.mk

help:
	@echo "${MAKE} release : Create source code packages."


RELEASE_DIR = ../wcd-${VERSION}
RELEASE_DIR_SHORT = ../wcd${VERSION_SHORT}

release:
	rm -rf ${RELEASE_DIR}
	rm -rf ${RELEASE_DIR_SHORT}
	svn export https://wcd.svn.sourceforge.net/svnroot/wcd/trunk/wcd ${RELEASE_DIR}
	# Include doc files and .mo files, to make it easier to
	# build wcd.
	cd ${RELEASE_DIR}/src ; ${MAKE} docfiles mofiles
	# Create package in DOS text format.
	cd ${RELEASE_DIR}/misc ; ${MAKE} -f unix2dos.mk
	# Touch .mo files, they are already up to date.
	cd ${RELEASE_DIR_SHORT}/src/po ; touch *.mo
	cd .. ; tar cvzf wcd-${VERSION}-src.tar.gz wcd-${VERSION}
	cd .. ; rm -f wcd${VERSION_SHORT}s.zip
	cd .. ; zip -r wcd${VERSION_SHORT}s.zip wcd${VERSION_SHORT}
