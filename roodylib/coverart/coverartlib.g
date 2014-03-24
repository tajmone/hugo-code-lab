!::
!     CoverArt GRAMMAR V 1.6
!::
#ifclear _COVER_G
#set _COVER_G

#ifset VERSIONS
#message "cover.g Version 1.6"
#endif

xverb "cover","covers"
	*										DoCoverHelp
xverb "cover","covers"
	* "help"								DoCoverHelp
xverb "cover","covers"
	* "always"/"never"/"on"/"off"			DoCoverSettings
!#if defined GAMECOVER
!xverb "cover","covers"
!	*	"once"						DoCoverSettings
!#endif ! GAMECOVER

#endif !_COVER_G