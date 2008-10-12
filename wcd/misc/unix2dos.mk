
UNIXVERSION=wcd-4.0.1
DOSVERSION=wcd401

# Convert in order so that no make target is out of date.

unix2dos:
	cd ../.. ; cp -Rp ${UNIXVERSION} ${DOSVERSION}
	cd ../../${DOSVERSION}/src ; unix2dos *.h *.c Makefile
	cd ../../${DOSVERSION}/src/c3po ; unix2dos *
	cd ../../${DOSVERSION}/src/patch ; unix2dos *.*
	cd ../../${DOSVERSION}/src/po ; unix2dos *.po *.pot
	cd ../../${DOSVERSION} ; unix2dos *.txt
	cd ../../${DOSVERSION}/doc ; unix2dos *.txt

