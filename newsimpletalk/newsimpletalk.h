!\-----------------------------------------------------------------------
NewSimpletalk.h version 2.0, based on code written by Robb Sherwin, updated and
turned into an includable library by Roody Yogurt.

This extension does Photopia-style conversation menus.

Usage notes:

Of course, #include this from your main file.

When you define your characters, give them a quips property and proceed to fill
it in with elements of 1 or 0.

character fred "Fred"
{
	quips 0 1 1 0
}

	In the above, Fred's line #1 is not available. 2 and 3 are, though.

Then, add the following routines:

routine SayQ (char, line)
{
	select char
	case fred
		{
		select line
		case 1: {">This is the first thing you can say to Fred."}
		case 2: {">This is the second thing you can say to Fred."}
		}
	case babboon
		{
		select line
		case 1: {">This is the first thing you can say to the babboon."}
		case 2: {">This is the second thing you can say to the babboon."}
		}
}

routine Respond (char, line)
{
	local dont_remove ! set to true in cases where you want dialogue repeatable
	select char
	case fred
		{
		select line
		case 1: "This is the first response from Fred."
		case 2: "This is the second response from Fred."
		}
	 case babboon
		{
		select line
		case 1: "This is the first response from the babboon."
		case 2: "This is the second response from the babboon."
		}
! Let's turn off whichever quip we selected unless we set dont_remove to true
	if not dont_remove
		SetQuip(char,line,0,1)
	else
		SetQuip(char,line,1,1)
}

	And fill them to suit your game.

	In the rest of your code, if you need to turn a quip on, call SetQuip, like:

	SetQuip(fred,3, 1) ! this sets his line three quip on
-----------------------------------------------------------------------\!

#ifclear _SIMPLETALK_H
#set _SIMPLETALK_H

#ifset VERSIONS
#message "NewSimpletalk.h version 1.9"
#endif

#ifset USE_EXTENSION_CREDITING
version_obj simpletalk_version "NewSimpleTalk Version 1.9"
{
	in included_extensions
	desc_detail
		" by Robb Sherwin and Roody Yogurt";
}
#endif

property quips
property quip alias quips

#if undefined MAX_QUIPS
constant MAX_QUIPS 5
#endif

constant NOT_AVAILABLE 0
constant AVAILABLE 1
constant SPOKEN 2

array quips_array[MAX_QUIPS]

replace DoTalk
{
	if word[2] = "to" and not xobject
	{
		if object = player
		{
			Message(&Speakto, 1)    ! "Stop talking to yourself."
			speaking = 0
			return false
		}
		elseif object is unfriendly or
		(object is living and not object.#quips)
		{
			if not object.ignore_response
				Message(&Speakto, 4)    ! "X ignores you."
			speaking = 0
		}
		elseif object is not living
		{
		PhotoMessage(&DoTalk, 1) ! "you can't talk to that"
		speaking = 0
		return false
		}
		else
		{
			local a
			a = Perform(&DoPhototalk, object)
#ifclear NO_SCRIPTS
			if a
				SkipScript(object)
#endif
		}
		return true
	}

	PhotoMessage(&DoTalk,2) ! "Just talking to <the object> will suffice."
	return false
}

routine DoPhototalk
{
	local x, z, ok = 0
	local selected

	speaking = object

! Check and make sure you have something to say.
	for (x=1 ;x<=speaking.#quips ;x++ )
	{
		if speaking.quip #x and not (speaking.quip #x & SPOKEN)
		{
			ok++
			break
		}
	}

! Write contents to the screen
	if ok
		{
		PhotoMessage(&DoPhotoTalk, 1) ! "Please select one:"
		""

! List the player's choices
		for (x=1;  x <= speaking.#quips; x++)
			{
			if QuipOn(speaking, x) and z < MAX_QUIPS
				{
				quips_array[z++] = x
				print "("; number z; ")";
				SayQ(speaking,x)
				}
			}

! Get the choice
		""
		selected = GetDial(z)
   	if (selected ~= 0)
 		{
			""
			selected--
			Respond(speaking, quips_array[selected])
			return true
		}
		else
			return
	}

	PhotoMessage(&DoPhotoTalk,2) ! "You really have nothing to say right now."
}


routine GetDial(max)
{
	word[1] = ""
	local temp,prompttext
	prompttext = PhotoMessage(&GetDial,1)
	! "Select a choice or 0 to keep quiet. >> "
	GetInput( prompttext )

	while true
	{
		if word[1] = "0" or words = 0
			break
		if word[1]
			temp = StringToNumber(word[1])
		else
			temp = StringToNumber(parse$)
		if temp ~= 0 and temp <= max
			break
		GetInput( prompttext )
		temp = 0
	}

	if not temp
		{
		""
		PhotoMessage(&GetDial,2) ! "Eeeagh! Stage fright! Abort!"
		}
	return temp
}

routine SetQuip (char, line, onoff,markused)
{
	if markused
	{
		char.quips #line = onoff | SPOKEN
	}
	else : char.quips #line = onoff
}

routine QuipOn (char, line)
{
	if (char.quips #line & AVAILABLE)
		return true
}

routine BeenSpoken (char,line)
{
	if (char.quips #line) & SPOKEN
		return true
}

replace DoAsk
{
	return PrintConverseUsage
}

replace DoAskQuestion
{
	return PrintConverseUsage
}

replace DoTell
{
	return PrintConverseUsage
}

routine PrintConverseUsage
{
	"Try \B>TALK TO CHARACTER\b instead."
	return false
}

routine PhotoMessage(r, num, a, b)
{
	if NewPhotoMessages(r, num, a, b):  return

	select r
		case &DoPhotoTalk
			{
			select num
				case 1: "Please select one:"
				case 2: "You really have nothing to say right now."
			}
		case &GetDial
			{
			select num
				case 1
					return "Select a choice or 0 to keep quiet. >> "
!\ If you want a custom prompt, make sure &GetDial case 1 is
	{ return "prompt text" } , not just "prompt text"     \!
				case 2
					"Eeeagh! Stage fright! Abort!"
			}
		case &DoTalk
			{
			select num
				case 1: "You can't talk to that!"
				case 2: 	print "Just talking to "; art(object); " will suffice."
			}
}

!\ The NewPhotoMessages routine may be REPLACED and should return
true if a replacement message exists for routine <r> \!

routine NewPhotoMessages(r, num, a, b)
{
	return false
}

#endif