!::
!     ROODYLIB GRAMMAR
!::

#ifset VERSIONS
#message "roodylib.g Grammar Version 1.8"
#endif


verb "push","pull","press","move","roll"
	* object "to" xobject			DoPushDirTo

verb "push","pull","press","move","roll", "shove", "yank", "tug"
	*			DoVague
	* object "to" xobject			DoPushDir
	* object "north"/ "n"/ "south"/ "s"/ "east"/ "e"/ "west"/ "w"/ \
	"southeast"/ "se"/ "southwest"/ "sw"/ "northwest"/ "nw"/ \
	"northeast"/ "ne"/ "up"/ "u"/ "down"/ "d"/ "in"/ "out"/ \
	"inside"/ "outside"		        DoPushDir
	* "on" object		DoMove
	* object		 DoMove

#ifclear _VERBSTUB_G
verb "smell", "sniff", "inhale", "breathe"
	*                                       DoSmell
	* object                                DoSmell

verb "search"
	*                                       DoVague
	* "for" object                          DoVague
	* "under"/"underneath"/"beneath"/"below" object	DoLookUnder
	* "in" object                           DoSearch
	* object                                DoSearch
#endif

#ifset HUGOFIX
verb "scope"
	*                 DoScope
	* "rooms"         DoScopeRooms

xverb "verbtest"
	*	object         DoVerbTest
#endif

! Defining the no-object version of DoEnter before hugolib.g to make use of
! some enter-object-detecting code in DoEnter
verb "enter"
	*				DoEnter

! Roody's note: I've found that if you support "swim", players will also try
! "swim in [object]"
#ifset _VERBSTUB_G
verb "swim", "dive"
	*                                       DoSwim
	* "in"/"into"                           DoSwim

verb "wave"
	*                                       DoWaveHands
	* "hands"                               DoWaveHands
	* "to"/"at" object                           DoWaveHands
	* held                                  DoWave
#endif
!----------------------------------------------------------------------------
! NON-ACTION VERBS:
!----------------------------------------------------------------------------
#ifclear NO_XYZZY
xverb "xyzzy", "plugh"
	*	               DoXYZZY
#endif
!\ Roody's note: To get around a bug in Hugo where only the firxst line of
a multi-line xverb definition is understood to be an xverb, I've split all
of the multi-line ones into single lines. \!

#ifclear NO_XVERBS

xverb "restore", "resume"
	*                                                       DoRestore
xverb "restore", "resume"
	* "game"/"story"                                        DoRestore

xverb "save", "suspend"
	*                                                       DoSave
xverb "save", "suspend"
	* "game"/"story"                                        DoSave

xverb "script", "transcript", "transcription"
	*                                                       DoScriptOnOff
xverb "script", "transcript", "transcription"
	* "on"/"off"                                            DoScriptOnOff

#ifclear NO_RECORDING
xverb "record"
	*                                                       DoRecordOnOff
xverb "record"
	* "on"/"off"                                            DoRecordOnOff

#endif

xverb "quit", "q"
	*                                                       DoQuit
xverb "quit", "q"
	* "game"                                                DoQuit

xverb "restart"
	*                                                       DoRestart
xverb "restart"
	* "game"                                                DoRestart

xverb "display"
	*                                                       DoDisplay
xverb "display"
	* "wide"/"tall"                                         DoDisplay

xverb "wide", "tall"
	*                                                       DoDisplay

#endif  ! ifclear NO_XVERBS

#ifset HUGOFIX
#ifclear _HUGOFIX_G
#set _HUGOFIX_G

xverb "$", "$ca", "$fd", "$on", "$pm", "$pr", "$sc"
	*                                       DoHugoFix

xverb "$ac"
	* anything                              DoHugoFix
xverb "$ac"
	* anything number                       DoHugoFix
xverb "$ac"
	* number                                DoHugoFix
xverb "$ac"
	* number number                         DoHugoFix

verb "$at"
	* anything "is" number			DoHugoFix
verb "$at"
	* number "is" number			DoHugoFix
verb "$at"
	* anything "is" "not" number		DoHugoFix
verb "$at"
	* number "is" "not" number		DoHugoFix

xverb "$au"
	*					HugoFixAudit

xverb "$de"
	* anything                              DoHugoFix
xverb "$de"
	* number                                DoHugoFix

#ifclear NO_OBJLIB
xverb "$di"
	*                                       DoHugoFix
xverb "$di"
	* number                                DoHugoFix
xverb "$di"
	* anything                              DoHugoFix
#endif

#ifclear NO_FUSES
xverb "$fd"
	*                                       DoHugoFix
#endif

xverb "$fi"
	*					DoHugoFix

xverb "$fo"
	*					DoHugoFix
xverb "$fo"
	* anything                              DoHugoFix
xverb "$fo"
	* number                                DoHugoFix

xverb "$kn"
	*					DoHugoFix
xverb "$kn"
	* number				DoHugoFix

xverb "$mo"
	* number "to" "$loc"			DoHugoFix
xverb "$mo"
	* anything "to" "$loc"			DoHugoFix
xverb "$mo"
	* number "to" number                    DoHugoFix
xverb "$mo"
	* anything "to" number			DoHugoFix
xverb "$mo"
	* number "to" anything			DoHugoFix
xverb "$mo"
	* anything "to" anything                DoHugoFix

xverb "$mp"
	* anything                              DoHugoFix
xverb "$mp"
	* number                                DoHugoFix

xverb "$na"
	* number                                DoHugoFix

xverb "$nr"
	*					DoHugoFix

xverb "$nu"
	* anything                              DoHugoFix

xverb "$on"
	*                                       DoHugoFix

xverb "$ot"
	*                                       DoHugoFix
xverb "$ot"
	* number                                DoHugoFix
xverb "$ot"
	* anything                              DoHugoFix
xverb "$ot"
	* "$loc"				DoHugoFix

xverb "$pc"
	*					DoHugoFix
xverb "$pc"
	* "$all"				DoHugoFix
xverb "$pc"
	* "$all" anything			DoHugoFix
xverb "$pc"
	* anything				DoHugoFix
xverb "$pc"
	* "$all" number				DoHugoFix
xverb "$pc"
	* number				DoHugoFix

xverb "$pm"
	*                                       DoHugoFix

xverb "$rr"
	*					DoHugoFix

#ifclear NO_SCRIPTS
xverb "$sc"
	*                                       DoHugoFix
#endif

xverb "$uk"
	* number				DoHugoFix
xverb "$uk"
	* anything				DoHugoFix

xverb "$wn"
	* word					DoHugoFix

xverb "$wo"
	* number                                DoHugoFix

#endif	! _HUGOFIX_G
#endif   ! ifset HUGOFIX
