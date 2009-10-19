include wcd/src/version.mk

help:
	@echo "${MAKE} release : Create source code packages."
	@echo "${MAKE} tag     : Create a tag copy of trunk."


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
	# Unix2dos does not keep original dates.
	# Touch .pot file, this is already up to date.
	sleep 2
	cd ${RELEASE_DIR_SHORT}/src/po ; touch *.pot
	# Touch .po files, they are already up to date.
	sleep 2
	cd ${RELEASE_DIR_SHORT}/src/po ; touch *.po
	# Touch .mo files, they are already up to date.
	sleep 2
	cd ${RELEASE_DIR_SHORT}/src/po ; touch *.mo
	cd .. ; tar cvzf wcd-${VERSION}-src.tar.gz wcd-${VERSION}
	cd .. ; rm -f wcd${VERSION_SHORT}s.zip
	cd .. ; zip -r wcd${VERSION_SHORT}s.zip wcd${VERSION_SHORT}

tag:
	svn copy https://wcd.svn.sourceforge.net/svnroot/wcd/trunk \
	         https://wcd.svn.sourceforge.net/svnroot/wcd/tags/release-${VERSION} \
	    -m "Tagging release ${VERSION}."


BRANCH=wcd-utf16

release_from_branch:
	rm -rf ${RELEASE_DIR}
	rm -rf ${RELEASE_DIR_SHORT}
	svn export https://wcd.svn.sourceforge.net/svnroot/wcd/branches/${BRANCH}/wcd ${RELEASE_DIR}
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


# Get latest changes of trunk into branch.
merge_from_trunk:
	svn merge https://wcd.svn.sourceforge.net/svnroot/wcd/trunk

# Merge branch into trunk.
# After this the branch is unusable for further work.
# Copy a new branch if needed.
merge_to_trunk:
	svn merge --reintegrate https://wcd.svn.sourceforge.net/svnroot/wcd/branches/${BRANCH}

# Delete branch
delete_branch:
	svn delete https://wcd.svn.sourceforge.net/svnroot/wcd/branches/${BRANCH} -m "Delete branch ${BRANCH}"

