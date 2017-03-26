!----------------------------------------------------------------------------
!* SCORE NOTIFY
!----------------------------------------------------------------------------
!\
Provides text like "You score has gone up by [x] points!"
\!

attribute notify_on alias special

property points alias e_to

object scorenotifylib "scorenotify"
{
	points 0
	is notify_on
	save_info
	{
		select self is notify_on
			case 0 : SaveWordSetting("score_off")
			case 1 : SaveWordSetting("score_on")
		return true
	}
	type settings
	in init_instructions
	execute
	{
		local a
		a = CheckWordSetting("scorenotify")
		if a
		{
			select word[(a+1)]
				case "score_off": self is not notify_on
				case "score_on": self is notify_on
		}
	}
	usage_desc
	{
		"\BSCORE NOTIFICATION ON\b- Be notified when you score points."
		Indent
		"\BSCORE NOTIFICATION OFF\b- Play without score notifications."
	}
}

object scorenotifymain
{
	type settings
	in main_instructions
	execute
	{
		ScoreNotify
	}
}

!\ Set the NOTIFY_FONT global to whatever font you want the notification to be
    in. You can even combine fonts with sometime like:
	NOTIFY_FONT = BOLD_ON | ITALIC_ON

	(any font styles other than BOLD_ON, ITALIC_ON, or UNDERLINE_ON will
	probably cause trouble} \!
global NOTIFY_FONT = BOLD_ON

routine ScoreNotify
{
	if scorenotifylib.points and scorenotifylib is notify_on
	{
		""
		Font(NOTIFY_FONT)
		local a
		a = scorenotifylib.points
		if scorenotifylib.points > 0
			print "[Your score has gone up by "; number a; " points.]"
		else
			print "[Your score has gone down by "; number -a; " points.]"
		Font((NOTIFY_FONT*2))
	}
	if scorenotifylib.points
	{
		scorenotifylib.points = 0    ! reset the point counter
!		PrintStatusLine  ! update status bar with new score
	}

}

! call this routine to add to the game score
routine AddScore(a)
{
	scorenotifylib.points += a
	score += scorenotifylib.points   ! add the points to the score
}

routine SubtractScore(a)
{
	scorenotifylib.points -= a
	score += scorenotifylib.points   ! add the points to the score
}

routine DoScoreNotifyOnOff
{
	if scorenotifylib is notify_on
		Perform(&DoScoreNotifyOff)
	else
		Perform(&DoScoreNotifyOn)
}

routine DoScoreNotifyOn
{
	if scorenotifylib is notify_on
		"[Score notification already on.]"
	else
	{
		"[Score notification on.]"
		scorenotifylib is notify_on
	}
}

routine DoScoreNotifyOff
{
	if scorenotifylib is not notify_on
		"[Score notification already off.]"
	else
	{
		"[Score notification off.]"
		scorenotifylib is not notify_on
	}
}