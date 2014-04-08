include wcd/src/version.mk

SVNREPO = svn://svn.code.sf.net/p/wcd/code
SVNSSHREPO = svn+ssh://svn.code.sf.net/p/wcd/code

help:
	@echo "${MAKE} dist    : Create source code distribution packages."
	@echo "${MAKE} tag     : Create a tag copy of trunk."


RELEASE_DIR = ../wcd-${VERSION}
RELEASE_DIR_SHORT = ../wcd${VERSION_SHORT}

# Documentation files are created and included in the source
# package for users that don't have the proper tools installed.
# Documentation creation only works properly on Cygwin or
# a modern Linux distribution.


dist:
	rm -rf ${RELEASE_DIR}
	rm -rf ${RELEASE_DIR_SHORT}
	svn export ${SVNREPO}/trunk/wcd ${RELEASE_DIR}
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
	cd .. ; tar cvzf wcd-${VERSION}-src.tar.gz wcd-${VERSION}
	cd .. ; rm -f wcd${VERSION_SHORT}s.zip
	cd .. ; zip -r wcd${VERSION_SHORT}s.zip wcd${VERSION_SHORT}

tag:
	svn copy ${SVNSSHREPO}/trunk \
	         ${SVNSSHREPO}/tags/release-${VERSION} \
	    -m "Tagging release ${VERSION}."


# Branch name (example).
BRANCH=compact_tree

branch:
	svn copy ${SVNSSHREPO}/trunk \
	         ${SVNSSHREPO}/branches/${BRANCH} \
	    -m "Creating branch ${BRANCH} from trunk."


release_from_branch:
	rm -rf ${RELEASE_DIR}
	rm -rf ${RELEASE_DIR_SHORT}
	svn export ${SVNREPO}/branches/${BRANCH}/wcd ${RELEASE_DIR}
	# Include doc files and .mo files, to make it easier to
	# build wcd.
	cd ${RELEASE_DIR}/src ; ${MAKE} doc
	# Create package in DOS text format.
	cd ${RELEASE_DIR}/misc ; ${MAKE} -f unix2dos.mk
	# Touch .mo files, they are already up to date.
	cd ${RELEASE_DIR_SHORT}/src/po ; touch *.mo
	cd .. ; tar cvzf wcd-${VERSION}-src.tar.gz wcd-${VERSION}
	cd .. ; rm -f wcd${VERSION_SHORT}s.zip
	cd .. ; zip -r wcd${VERSION_SHORT}s.zip wcd${VERSION_SHORT}


# Get latest changes of trunk into branch.
merge_from_trunk:
	svn merge ${SVNSSHREPO}/trunk

# Merge branch into trunk.
# After this the branch is unusable for further work.
# Copy a new branch if needed.
merge_to_trunk:
	svn merge --reintegrate ${SVNSSHREPO}/branches/${BRANCH}

# Delete branch
delete_branch:
	svn delete ${SVNSSHREPO}/branches/${BRANCH} -m "Delete branch ${BRANCH}"

