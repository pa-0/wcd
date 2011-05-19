#setenv WCDHOME "$HOME/.wcd"

if ( ${?WCDHOME} ) then
  alias wcd "rm -f $WCDHOME/bin/wcd.go ; BINDIR/wcd \!* ; source $WCDHOME/bin/wcd.go"
else
  alias wcd "rm -f $HOME/bin/wcd.go ; BINDIR/wcd \!* ; source $HOME/bin/wcd.go"
endif

