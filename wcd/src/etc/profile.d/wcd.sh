#
#  Copyright (C) 2004-2011 Jari Aalto; Licensed under GPL v2 or later
#
#  Define a shell alias for wcd(1), which itself change
#  directories, so it must be done in a shell alias.
#
#  This file is in /etc/profile.d/ and it should be sourced from
#  a shell startup file: ~/.<shell>rc

#export WCDHOME="$HOME/.wcd"

if [ -x "BINDIR/wcd" ] && [[ -n ${WCDHOME:-${HOME}} ]]
then

    wcd ()
    {
	go="${WCDHOME:-${HOME}}/bin/wcd.go"

	rm -f "$go" 2> /dev/null

	BINDIR/wcd "$@"

	[ -f "$go" ] && . "$go"

	unset go
    }

fi

# End of file
