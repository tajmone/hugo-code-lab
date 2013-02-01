!::
!  COLORLIB GRAMMAR
!::

#ifset VERSIONS
#message "ColorLib.g Version 1.1"
#endif

xverb "color" "colors"
	*											DoColorHelp
xverb "color" "colors"
	*	"on"/"off"/"default"				DoColorOnOff
xverb "color" "colors"
	* "help"									DoColorHelp
