!::
!     SCORE NOTIFICATION GRAMMAR V. 1.0
!::

#ifset VERSIONS
#message "ScoreNotify.g Version 1.0"
#endif

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