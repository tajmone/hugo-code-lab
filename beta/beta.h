!\-----------------------------------------------------------------------
Beta.h version 2.7, based on code written by Bert Byfield, updated by Mike
Snyder, and turned into this by Jon Blask.

If you are including "roodylib" and have an init routine that calls Init_Calls,
you don't need to do anything. Otherwise, add this to your init routine before
any text is printed:

#ifset BETA
	BetaInit
#endif

That, combined with the replaced DoRestart routine, will reset the
transcript_is_on property to true if the game is restarted while scripting.

Suggestion- add the following line to the top of your newshell.hug file:
!#set BETA

After the inclusion of hugolib.h, add:
#ifset BETA
#include "beta.h"
#endif

Doing it that way will also let you do something like this:
routine Version
{
	color LIGHT_RED
	"\BGuilty Bastards\b"
	color TEXTCOLOR
	"Copyright (c) 1998 by Kent Tessman"
	print "\nRelease "; RELEASE; " "; serial$;
#ifset BETA
	" \B(beta-test version)\b"
#endif
	print newline
	"Written using the Hugo Compiler ";
	print HUGO_VERSION
	"\B(New players should type \"help\".)\b"
#ifset BETA
	"\n\B(NOTE:  This beta release is intended for testing only, not for
	distribution to the general public.  Please report any errors,
	bugs, etc. to the author.)\n\n
        To make a comment, type \"* COMMENT TEXT\" at the prompt.\b"
#endif
#ifset DEBUG
	"\n\B(Compiled with DEBUG set)\b"
#endif
}
-----------------------------------------------------------------------\!

#ifclear _BETA_H
#set _BETA_H

#ifset VERSIONS
#message "Beta.h Version 2.7"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj beta_version "Beta Version 2.7"
{
	in included_extensions
	desc_detail
		" by Bert Byfield, Mike Snyder, and Roody Yogurt";
}
#endif

property transcript_is_on alias long_desc


object betalib "beta"
{
	transcript_is_on false
! if roodylib.h has been included before beta.h, nothing needs to be
! added to the init routine
#ifset _ROODYLIB_H
	type settings
	in init_instructions
	did_print 0
#ifset _NEWMENU_H
	usage_desc
		{
		Indent
		"Betatesters: When a transcript is on (";
		if system(61) ! simple
			print "\"";
		else
		 print "\#147";
		 print "\BSCRIPT ON\b\"), preface your
		notes with an asterisk (";
		if system(61) ! simple
			print "\"";
		else
		 print "\#147";
		 "\B*\b\") to have your comments accepted."
		}
#endif
	save_info
		{
		if self.transcript_is_on
			return true
		else
			return false
		}
	execute
		{
		BetaInit
		}
#endif  ! _ROODYLIB_H
}


routine BetaInit
{
#ifclear _ROODYLIB_H
	if word[10] = "beta"
		betalib.transcript_is_on = true
	else
#else
	if CheckWordSetting("beta")
		betalib.transcript_is_on = true
	elseif not CheckWordSetting("restore") and not CheckWordSetting("undo")
#endif
		{
		BetaMessage(&BetaInit,1) ! Would you like to start a transcript?
!: fancy pause stuff below
		local key
		if system_status or system(61) ! if simple port
			{
			pause
			}
		else
			{
				while true
				{
					key = system(11) ! READ_KEY
					if key
						{
						word[0] = key
						break
						}
					system(32) ! PAUSE_100TH_SECOND
				}
			}
		if word[0] = 'b','B'
			{
			if (not scripton)
				{
				""
				BetaMessage(&DoScriptOnOff, 1, betalib.transcript_is_on)
				}
		!\
			"Transcription is already on."
			or "Unable to begin transcription.", depending
		\!
			else
				{
				""
				betalib.transcript_is_on = true
				BetaMessage(&DoScriptOnOff, 2) ! "Transcription on."
				}
			}
		else
			{
			""
			BetaMessage(&BetaInit,2) ! "No transcript started."
			}
		""
		BetaMessage(&BetaInit,3) ! "[press a key]"
#ifclear _ROODYLIB_H
		pause
		"\n"
		cls
#else
		HiddenPause
		"\n"
		betalib.did_print = true
#endif
		}
}

replace DoScriptOnOff
{
	if word[2] = "on" or words = 1
	{
		if (betalib.transcript_is_on) or (not scripton)
			BetaMessage(&DoScriptOnOff, 1, betalib.transcript_is_on)
	!\
		"Transcription is already on."
		or "Unable to begin transcription.", depending
   \!
		else
			{
			betalib.transcript_is_on = true
			BetaMessage(&DoScriptOnOff, 2) ! "Transcription on."
			}
	}
	elseif word[2] = "off"
	{
		if (not betalib.transcript_is_on) or (not scriptoff)
			BetaMessage(&DoScriptOnOff, 3, betalib.transcript_is_on)
	!\
		"Transcription is not currently on."
		or "Unable to end transcription.", depending
   \!
		else
			{
			betalib.transcript_is_on = false
			BetaMessage(&DoScriptOnOff, 4) ! "Transcription off."
			}
	}
}

#ifclear _ROODYLIB_H
! The NewParseError replacement routine has been changed for better
! coexisting with other NewParseError code if the game calls for it
replace NewParseError(errornumber, obj)
{
     ! changed some unnecessary string-matching code
	if word[1] = "*"
		{
		if betalib.transcript_is_on
			BetaMessage(&DoScriptOnOff, 5) ! Comment recorded!
		else
			BetaMessage(&DoScriptOnOff, 6) ! Comment not recorded!
		return true ! we'll just return true if someone tried to do a comment
		}
	select errornumber
		!    if you have code hijacking any other error messages, it'd go here:
		!	select errornumber
		!		case 1: (etc)
		case else : return false ! if a specific case was NOT mentioned, return
										 ! false
	return true ! this line is only reached if an error message was replaced
}
#else
replace PreParseError
{
     ! changed some unnecessary string-matching code
	if word[1] = "*"
		{
		if betalib.transcript_is_on
			BetaMessage(&DoScriptOnOff, 5) ! Comment recorded!
		else
			BetaMessage(&DoScriptOnOff, 6) ! Comment not recorded!
		return true ! we'll just return true if someone tried to do a comment
		}
	else
		return false
}
#endif

#ifclear _ROODYLIB_H
! We replace DoRestart so we can reset the transcript_is_on global to true if
! the game is restarted while scripting
replace DoRestart
{
   VMessage(&DoRestart, 1)                  ! "Are you sure?"
	GetInput
	if YesorNo = true
	{
		if betalib.transcript_is_on
			word[10] = "beta"
		if not restart
			VMessage(&DoRestart, 2)  ! "Unable to restart."
	}
}

replace DoRestore
{
	if betalib.transcript_is_on
		word[10] = "beta"
	if restore
	{
		if word[10] = "beta"
			betalib.transcript_is_on = true
		VMessage(&DoRestore, 1)         ! "Restored."
		PrintStatusline
		DescribePlace(location, true)
		return true
	}
	else:  VMessage(&DoRestore, 2)           ! "Unable to restore."
}
#endif

routine BetaMessage(r, num, a, b)
{
	if NewBetaMessages(r, num, a, b):  return

	select r
	case &BetaInit
		{
		select num
			case 1 : "This is a beta release! If you'd like to start a transcript
			right away, press \"B\". Otherwise, push any other key to begin
			without starting a transcript."
			case 2 : "No transcript started."
			case 3 : print "[ press a key ]";
		}
	case &DoScriptOnOff
		{
		select num
			case 1
				{
				if a ! a = betalib.transcript_is_on
					print "Transcription is already on."
				else
					print "Unable to begin transcription."
				}
			case 2:  print "Transcription on."
			case 3
				{
				if not a ! a = betalib.transcript_is_on
					print "Transcription is not currently on."
				else
					print "Unable to end transcription."
				}
			case 4: print "Transcription off."
			case 5: print "Comment recorded!"
			case 6: print "Comment not recorded!"
		}
}

!\ The NewBetaMessages routine may be REPLACED and should return
true if a replacement message exists for routine <r> \!

routine NewBetaMessages(r, num, a, b)
{
	return false
}
#endif