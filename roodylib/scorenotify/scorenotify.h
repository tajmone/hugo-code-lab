!::
! Hugo Score Notification V. 1.2
!::

!\
Provides text like "You score has gone up by [x] points!"
\!

#ifclear _SCORENOTIFY_H
#SET _SCORENOTIFY_H

#ifset VERSIONS
#message "ScoreNotify.h Version 1.2"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj scorenotify_version "ScoreNotify Version 1.2"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

property score_notify alias d_to

property points alias e_to

object scorenotifylib "scorenotify"
{
	score_notify true
	points 0
#ifset _ROODYLIB_H
	save_info
	{
		select self.score_notify
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
			select word[(a-1)]
				case "score_off": self.score_notify = 0
				case "score_on": self.score_notify = 1
		}
	}
#endif
#ifset _NEWMENU_H
	usage_desc
	{
		"\BSCORE NOTIFICATION ON\b- Be notified when you score points."
		Indent
		"\BSCORE NOTIFICATION OFF\b- Play without score notifications."
	}
#endif ! NEWMENU
}

#ifset _ROODYLIB_H
object scorenotifymain
{
	type settings
	in main_instructions
	execute
	{
		ScoreNotify
	}
}
#endif  ! _ROODYLIB_H

!\ Set the NOTIFY_FONT global to whatever font you want the notification to be
    in. You can even combine fonts with sometime like:
	NOTIFY_FONT = BOLD_ON | ITALIC_ON

	(any font styles other than BOLD_ON, ITALIC_ON, or UNDERLINE_ON will
	probably cause trouble} \!
global NOTIFY_FONT = BOLD_ON

routine ScoreNotify
{
	if scorenotifylib.points and scorenotifylib.score_notify
	{
		""
		Font(NOTIFY_FONT)
		if scorenotifylib.points > 0
			ScoreNotificationMessage(&ScoreNotify, 1, scorenotifylib.points )
			! "[Your score has gone up.]"
		else
			ScoreNotificationMessage(&ScoreNotify, 2, -scorenotifylib.points )
		! "[Your score has gone down.]"
		Font((NOTIFY_FONT*2))
	}
	if scorenotifylib.points
	{
		score += scorenotifylib.points   ! add the points to the score
		scorenotifylib.points = 0    ! reset the point counter
		PrintStatusLine  ! update status bar with new score
	}

}

! routine to call for the last score of a game (after the winning move), as
! main is not called again
routine LastScore(a)
{
	score += a
}

! otherwise, call this routine to add to the game score
routine AddScore(a)
{
	scorenotifylib.points += a
}

routine SubtractScore(a)
{
	scorenotifylib.points -= a
}

routine DoScoreNotifyOnOff
{
	if scorenotifylib.score_notify
		Perform(&DoScoreNotifyOff)
	else
		Perform(&DoScoreNotifyOn)
}

routine DoScoreNotifyOn
{
	if scorenotifylib.score_notify
		ScoreNotificationMessage(&DoScoreNotifyOn, 1 )
		! "[Score notification already on.]"
	else
	{
		ScoreNotificationMessage(&DoScoreNotifyOn, 2 )
		! "[Score notification on.]"
		scorenotifylib.score_notify = 1
	}
}

routine DoScoreNotifyOff
{
	if not scorenotifylib.score_notify
		ScoreNotificationMessage(&DoScoreNotifyOff, 1 )
		! "[Score notification already off.]"
	else
	{
		ScoreNotificationMessage(&DoScoreNotifyOff, 2 )
		! "[Score notification off.]"
		scorenotifylib.score_notify = 0
	}
}

routine ScoreNotificationMessage(r, num, a, b)
{
	if NewScoreNotificationMessages(r, num, a, b):  return

	select r
		case &DoScoreNotifyOn
		{
			select num
				case 1:  "[Score notification already on.]"
				case 2: "[Score notification on.]"
		}
		case &DoScoreNotifyOff
		{
			select num
				case 1:  "[Score notification already off.]"
				case 2: "[Score notification off.]"
		}
		case &ScoreNotify
		{
			select num
				case 1 : "[Your score has gone up.]"
				case 2 : "[Your score has gone down.]"
		}
}

!\ The NewScoreNotificationMessages routine may be REPLACED and should return
true if a replacement message exists for routine <r> \!

routine NewScoreNotificationMessages(r, num, a, b)
{
   select r
!		case &ScoreNotify
!		{
!			select num
!				case 1
!				{
!					print "[Your score has gone up by "; number a; " points.]"
!				}
!		}
		case else : return false
   return true ! this line is only reached if we replaced something
}

#endif _SCORENOTIFY_H