include ../src/version.mk

UNIXVERSION=wcd-${VERSION}
DOSVERSION=wcd${VERSION_SHORT}

# Keep dates so that no make target is out of date.
#
# This Makefile requires 'dos2unix' with --keepdate option:
# http://waterlan.home.xs4all.nl/dos2unix.html
# http://sourceforge.net/projects/dos2unix/

unix2dos:
	cd ../.. ; cp -Rp ${UNIXVERSION} ${DOSVERSION}
	cd ../../${DOSVERSION}/src ; unix2dos --keepdate *.h *.c *.mk
	cd ../../${DOSVERSION} ; unix2dos --keepdate */Makefile
	cd ../../${DOSVERSION}/src/c3po ; unix2dos --keepdate *
	cd ../../${DOSVERSION}/src/patch ; unix2dos --keepdate *.*
	cd ../../${DOSVERSION} ; unix2dos --keepdate *.txt
	cd ../../${DOSVERSION}/doc ; unix2dos --keepdate *.txt
	cd ../../${DOSVERSION}/doc ; unix2dos --keepdate --add-bom README.txt
	cd ../../${DOSVERSION} ; unix2dos --keepdate file_id.diz
	cd ../../${DOSVERSION}/src/man ; unix2dos --keepdate man1/*.txt
	cd ../../${DOSVERSION}/src/man ; unix2dos --keepdate --add-bom */man1/*.txt
	cd ../../${DOSVERSION}/src/man ; unix2dos --keepdate man1/*.htm */man1/*.htm

