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