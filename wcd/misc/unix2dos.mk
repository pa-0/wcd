include ../src/version.mk

UNIXVERSION=wcd-${VERSION}
DOSVERSION=wcd${VERSION_SHORT}

# Convert in order so that no make target is out of date.

unix2dos:
	cd ../.. ; cp -Rp ${UNIXVERSION} ${DOSVERSION}
	cd ../../${DOSVERSION}/src ; unix2dos *.h *.c Makefile *.mk
	cd ../../${DOSVERSION}/src/c3po ; unix2dos *
	cd ../../${DOSVERSION}/src/patch ; unix2dos *.*
	sleep 2
	cd ../../${DOSVERSION}/src/po ; unix2dos *.pot
	sleep 2
	cd ../../${DOSVERSION}/src/po ; unix2dos *.po
	cd ../../${DOSVERSION} ; unix2dos *.txt
	cd ../../${DOSVERSION}/doc ; unix2dos *.txt

