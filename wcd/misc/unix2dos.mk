include ../src/version.mk

UNIXVERSION=wcd-${VERSION}
DOSVERSION=wcd${VERSION_SHORT}

# Convert in order so that no make target is out of date.
#
# This Makefile requires 'dos2unix' with --keepdate option:
# http://www.xs4all.nl/~waterlan/dos2unix.html
# http://sourceforge.net/projects/dos2unix/

unix2dos:
	cd ../.. ; cp -Rp ${UNIXVERSION} ${DOSVERSION}
	cd ../../${DOSVERSION}/src ; unix2dos --keepdate *.h *.c Makefile *.mk
	cd ../../${DOSVERSION}/src/c3po ; unix2dos --keepdate *
	cd ../../${DOSVERSION}/src/patch ; unix2dos --keepdate *.*
	cd ../../${DOSVERSION}/src/po ; unix2dos --keepdate *.pot
	cd ../../${DOSVERSION}/src/po ; unix2dos --keepdate *.po
	cd ../../${DOSVERSION} ; unix2dos --keepdate *.txt
	cd ../../${DOSVERSION}/doc ; unix2dos --keepdate *.txt
	cd ../../${DOSVERSION} ; unix2dos --keepdate file_id.diz

