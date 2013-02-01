!::
!     CHEAPLIB GRAMMAR
!::

#ifset VERSIONS
#message "CheapLib.g Version 1.2"
#endif

xverb "cheap", "cheapmode"
	*								DoCheapToggle
xverb "cheap", "cheapmode"
	* "help"						DoCheapHelp
xverb "cheap", "cheapmode"
	* "on"/"off"				DoCheapOnOff
xverb "cheap", "cheapmode"
	* "mode" "on"/"off"		DoCheapOnOff
