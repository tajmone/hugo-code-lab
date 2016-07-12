!::
!     ROODYLIB GRAMMAR
!::

#ifset VERSIONS
#message "roodylib.g Grammar Version 4.1.1"
#endif

#ifclear _ROODYLIB_G
#set _ROODYLIB_G

#ifclear NO_VERBS
! just used by CHARACTER, AGAIN
verb "g" "again"
	*             DoAgain

!\ Roody's note: Redefined "go" so that the somewhat ingrammatical phrase
"go off of <object>" can be supported. We'll see if this causes any problems.
Also, "go to" now directs to DoGo, not DoEnter.   \!
verb "go", "walk"
	*							DoGo
	* "in"/"into"/"inside"/"through" object            DoEnter
	* "off" platform										DoExit
	* "out"/"to"/"toward"/"towards" object            DoGo
	* "to"/"toward"/"towards" object            DoGo
	* "out"/"outside"                                       DoExit
	* object                                                DoGo

! Changed "look in"'s grammar to allow transparent, non-container objects
verb "look", "l", "examine", "x", "watch"
	*                                                       DoLookAround
	* "around"						DoLookAround
	* "in"/"inside" object                               DoLookIn
	* "on" platform                                         DoLookIn
	* "at"/"to" object                                      DoLook
	* "out"/"through" object                                DoLookThrough
	* "under"/"underneath"/"beneath"/"below" object         DoLookUnder
	* "beside"/"behind"/"around" object                     DoLookUnder
	* object                                                DoLook

!#ifclear NO_OBJLIB
!verb "push","pull","press","move","roll"
!	* object "to" xobject			DoPushDirTo
!#endif

verb "push","pull","press","move","roll", "shove", "yank", "tug"
	*			DoVague
#ifclear NO_OBJLIB
	* object "to" xobject			DoPushDirTo
	* object "north"/ "n"/ "south"/ "s"/ "east"/ "e"/ "west"/ "w"/ \
	"southeast"/ "se"/ "southwest"/ "sw"/ "northwest"/ "nw"/ \
	"northeast"/ "ne"/ "up"/ "u"/ "down"/ "d"/ "in"/ "out"/ \
	"inside"/ "outside"		        DoPushDir
#endif
	* "on" object		DoMove
	* object		 DoMove

! Roody's note: Added to make use of some DoPutIn code where an xobject is not
! provided.
verb "put", "place", "set"
	*                                                       DoVague
#ifset USE_CHECKHELD
	* "down" multi                                          DoDrop_CheckHeld
	* multi "on"/"onto" "ground"/"floor"                    DoPutonGround_CheckHeld
	* multi "outside" xobject                               DoPutonGround_CheckHeld
	* multi "down"                                          DoDrop_CheckHeld
	* multi "in"/"into"/"inside" container                  DoPutIn_CheckHeld
	* multi "on"/"onto" platform                            DoPutIn_CheckHeld
	* held                                                  DoPutIn_CheckHeld
#else
	* "down" multiheld                                      DoDrop
	* multiheld "on"/"onto" "ground"/"floor"                DoPutonGround
	* multiheld "outside" xobject                           DoPutonGround
	* multiheld "down"                                      DoDrop
	* multiheld "in"/"into"/"inside" container              DoPutIn
	* multiheld "on"/"onto" platform                        DoPutIn
	* held                                                  DoPutIn
#endif

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

! updated "wear" and "remove" to use new checkheld system
! also, the no-checkheld "remove" behavior now defaults to held
! objects unless a parent is spec
verb "wear"
	*                                DoVague
#ifset USE_CHECKHELD
	* multi                          DoWear_Checkheld
#else
	* multiheld                      DoWear
#endif

verb "remove"
	*                                DoVague
	* multi "from"/"outof"/"offof" parent       DoGet
#ifset USE_CHECKHELD
	* multi                          DoTakeOff_Checkheld
#else
	* object                      DoTakeOff
#endif

verb "take"
	*                                                       DoVague
	* "inventory"                                           DoInventory
#ifset USE_CHECKHELD
	* "off" multiheld                                       DoTakeOff_Checkheld
	* multiheld "off"                                       DoTakeOff_Checkheld
#elseif set AIF
	* "off" multiheld                                       DoTakeOff
	* multiheld "off"                                       DoTakeOff
#else
	* "off" held                                       DoTakeOff
	* held "off"                                       DoTakeOff
#endif

! Roody's note: Pre-defining "empty" before verblib.g. Alterred to not
! execute DoEmptyOrGet, which I find troublesome. Also, if used with
! NEW_EMPTY switch, more emptying rules are allowed (see NEW_EMPTY section
! in roodylib.h.
verb "empty", "unload"
	*                                                       DoVague
#ifset NEW_EMPTY
	* (CheckEmpty) "on"/"onto" "ground"/"floor"             DoEmptyGround
#else
	* object "on"/"onto" "ground"/"floor"                   DoEmptyGround
#endif
	* multi "from"/"off"/"on"/"in" parent                    DoGet
	* multi "offof"/"outof" parent                         DoGet
	* multi "from" "offof"/"outof"/"on"/"in" parent         DoGet
! Send >UNLOAD OBJECT to DoEmptyoOrGet, which dispatches to DoEmpty or DoGet
!	* object                                                DoEmpty
#ifset NEW_EMPTY
	* (CheckEmpty)						DoEmpty ! DoEmptyOrGet
#else
	* object                DoEmpty
#endif

! Defining the no-object version of DoEnter before hugolib.g to make use of
! some enter-object-detecting code in DoEnter
verb "enter"
	*				DoEnter
	* object		DoEnter

!\ Roody's note - Mike Snyder pointed out that it's not optimal that "kick", by
default, points to DoHit.  Sending the command to its own verb routine makes
it easier to replace or catch with before routines.
\!
verb "kick"
	*			  DoVague
	* object   DoKick

! Roody's note: Defining DoHit verbs to include "smash"
verb "hit", "strike", "break", "attack", "whack", "beat", \
	"punch", "smash", "clobber"
	*                                                       DoVague
	* object "with"/"using" held                            DoHit
	* object                                                DoHit

#ifclear NO_XYZZY
verb "xyzzy", "plugh"
	*	               DoXYZZY
#endif  ! NO_XYZZY

! Roody's note: I've found that if you support "swim", players will also try
! "swim in [object]"
#ifset _VERBSTUB_G
verb "swim", "dive"
	*                                       DoSwim
	* "in"/"into"                           DoSwim

verb "wave"
	*                                       DoWaveHands
	* "hands"                               DoWaveHands
	* "to"/"at" object                      DoWaveHands
	* held                                  DoWave
#endif

#endif  ! NO_VERBS

!----------------------------------------------------------------------------
! NON-ACTION VERBS:
!----------------------------------------------------------------------------
#ifclear NO_XVERBS

!\ Roody's note: To get around a bug in Hugo where only the first line of
a multi-line xverb definition is understood to be an xverb, I've split all
of the multi-line ones into single lines. \!

#ifset NO_MODE_CHANGE
xverb "brief", "normal","superbrief", "short","verbose","long"
	*                                                 	DoFakeRefuse
#endif  ! NO_MODE_CHANGE

#ifclear NO_ACCESSIBILITY
xverb "cheap", "cheapmode"
	*								DoCheapToggle
xverb "cheap", "cheapmode"
	* "help"						DoCheapHelp
xverb "cheap", "cheapmode"
	* "on"/"off"				DoCheapOnOff
xverb "cheap", "cheapmode"
	* "mode" "on"/"off"		DoCheapOnOff
#ifset NEW_MENU
xverb "cheap","simple"
	* "menu"/"menus"		DoCheapOnOff
#endif

xverb "normal"
	*	              DoAccessibility

xverb "clear","clearscreen"
	* "never"/"off"/"normal"           DoAccessibility


xverb "info"
	*	DoInfo

xverb "prompt"
	*	             DoPrompt

xverb "status"
	*					DoStatus
#endif

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
xverb "playback"
	*                                                       DoRecordOnOff
xverb "playback"
	*  "on"/"off"                                           DoRecordOnOff
xverb "record"
	*                                                       DoRecordOnOff
xverb "record"
	* "on"/"off"                                            DoRecordOnOff

#endif  ! NO_RECORDING

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

#ifclear NO_VERSION
#if defined GAME_TITLE
xverb "version"
	*                                                       DoVersion
#endif ! if defined GAME_TITLE
#endif ! ifclear NO_VERSION
#endif  ! ifclear NO_XVERBS

#ifset USE_FOOTNOTES
verb "footnote","note","fn","f"
*							DoFootnote
*	number				DoFootnote

xverb "help"
	* "footnotes"						DoFootnoteMode

xverb "footnotes", "notes"
	*										DoFootnoteMode
	* "help"                      DoFootnoteMode
	* "always"							DoFootnoteMode
	* "on"/"normal"					DoFootnoteMode
	* "off"/"never"					DoFootnoteMode
#endif ! ifset USE_FOOTNOTES

#ifset SCORE_NOTIFY
xverb "notify"
	*        DoScoreNotifyOnOff
xverb "score"
* "notify"/"notification"			DoScoreNotifyOnOff

xverb "score", "notify"
* "on"				DoScoreNotifyOn
xverb "score"
* "notify"/"notification" "on" DoScoreNotifyOn

xverb "score", "notify"
* "off"				DoScoreNotifyOff
xverb "score"
* "notify"/"notification" "off" DoScoreNotifyOff
#endif ! ifset SCORE_NOTIFY

! We'll allow HugoFix commands to work even in NO_XVERBS games as it is
! assumed they'll only be used for betatesting.

#ifset HUGOFIX

!#ifclear _HUGOFIX_G
!#set _HUGOFIX_G
!#endif	! _HUGOFIX_G

xverb "scope"
	*                 DoScope

#ifclear NO_OBJLIB
xverb "scope"
	* "rooms"         DoScopeRooms
#endif

xverb "verbtest"
	*	object         DoVerbTest

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

xverb "$om"
	*                                       DoHugoFix

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
xverb "$pn"
	*				DoHugoFix

xverb "$rp"
	*					DoHugoFix
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
#endif   ! ifset HUGOFIX

#endif ! _ROODYLIB_G