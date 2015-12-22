include wcd/src/version.mk

GITREPO = git://git.code.sf.net/p/wcd/wcd.git

help:
	@echo "${MAKE} dist    : Create source code distribution packages."
	@echo "${MAKE} tag     : Create a tag."


RELEASE_DIR = ../wcd-${VERSION}
RELEASE_DIR_SHORT = ../wcd${VERSION_SHORT}

# Documentation files are created and included in the source
# package for users that don't have the proper tools installed.
# Documentation creation only works properly on Cygwin or
# a modern Linux distribution.


dist:
	rm -rf ../clone
	rm -rf ${RELEASE_DIR}
	rm -rf ${RELEASE_DIR_SHORT}
	git clone ${GITREPO} ../clone
	mv ../clone/wcd ${RELEASE_DIR}
	rm -rf ../clone
	# Include doc files, to make it easier to build wcd.
	cd ${RELEASE_DIR}/src ; ${MAKE} man txt html
	# Make sure .po files are up to date.
	cd ${RELEASE_DIR}/src ; ${MAKE} merge
	# cleanup.
	cd ${RELEASE_DIR}/src ; ${MAKE} mostlyclean
	# fix some file mods.
	cd ${RELEASE_DIR} ; chmod -x */Makefile */*.txt */*.c */*.h src/po/*.* src/man/man1/*.*
	# Create package in DOS text format.
	cd ${RELEASE_DIR}/misc ; ${MAKE} -f unix2dos.mk
	# Remove generated doc files from Unix package.
	cd ${RELEASE_DIR}/src ; ${MAKE} clean
	# Create the packages.
	cd .. ; tar cvzf wcd-${VERSION}.tar.gz wcd-${VERSION}
	cd .. ; rm -f wcd${VERSION_SHORT}.zip
	cd .. ; zip -r wcd${VERSION_SHORT}.zip wcd${VERSION_SHORT}

# Create pgp signature. Required for Debian Linux.
# See http://narfation.org/2013/06/23/signed-upstream-tarballs-in-debian
pgpsign:
	cd ..; gpg --detach-sign --armor ${RELEASE_DIR}.tar.gz

# Send the key tp HKP/SKS keyserver.
# You might want to add this to receive kes easily:
#
#    # .gnupg/gpg.conf
#    keyserver  hkp://pool.sks-keyservers.net
pgpsend:
	cd ..; gpg --keyserver pool.sks-keyservers.net --send-keys B12725BE

tag:
	git tag -a release-${VERSION} -m "Tagging release ${VERSION}."


