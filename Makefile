include wcd/src/version.mk

help:
	@echo "${MAKE} release : Create source code packages."


RELEASE_DIR = ../wcd-${VERSION}
RELEASE_DIR_SHORT = ../wcd${VERSION_SHORT}

release:
	rm -rf ${RELEASE_DIR}
	rm -rf ${RELEASE_DIR_SHORT}
	svn export https://wcd.svn.sourceforge.net/svnroot/wcd/trunk/wcd ${RELEASE_DIR}
	cd ${RELEASE_DIR}/src ; ${MAKE} docfiles
	cd ${RELEASE_DIR}/misc ; ${MAKE} -f unix2dos.mk
	cd .. ; tar cvzf wcd-${VERSION}-src.tar.gz wcd-${VERSION}
	cd .. ; zip -r wcd${VERSION_SHORT}.zip wcd${VERSION_SHORT}
