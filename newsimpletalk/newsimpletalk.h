!\-----------------------------------------------------------------------
NewSimpletalk.h version 2.4, based on code written by Robb Sherwin, updated and
turned into an includable library by Roody Yogurt.

This extension does Photopia-style conversation menus.

changelog

	v 2.4 - Added DoQuickTalk routine to allow for commands like "talk <char> 1"
	        so walkthroughs and such can work with the talk system. to use, add
	        grammar like this to your game:
verb "t","talk
	* living "1"/"2"/"3"/"4"        DoQuickTalk
	* number                        DoQuickTalk
	v 2.3 - removed unnecessary word[2] check to allow command shortening
	v 2.2 - fixed message routine bug
	V 2.1 - added can_quit and loop_talk globals. can_quit defaults to true
	but if set false, players can't quit out of conversations. loop_talk
	defaults to false but if set true, loops conversation menus while options are
	available.

	In games with non-disappearing conversation options, authors should be wary
	of loop_talk and no-can_quit combinations.


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
#message "NewSimpletalk.h version 2.4"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj simpletalk_version "NewSimpleTalk Version 2.4"
{
	in included_extensions
	desc_detail
		" by Robb Sherwin and Roody Yogurt";
}
#endif

property quips
property quip alias quips
global loop_talk
global can_quit = true

#if undefined MAX_QUIPS
constant MAX_QUIPS 5
#endif

constant NOT_AVAILABLE 0
constant AVAILABLE 1
constant SPOKEN 2

array quips_array[MAX_QUIPS]

replace DoTalk
{
	speaking = 0
	if (not xobject and word[2] ~= "about") or
		(xobject >= 1 and xobject <= 4)
	{
		if object = player
		{
			Message(&Speakto, 1)    ! "Stop talking to yourself."
			return false
		}
		elseif object is unfriendly or (object is living and not object.#quips)
		{
			if not object.ignore_response
				Message(&Speakto, 4)    ! "X ignores you."
		}
		elseif object is not living
		{
		PhotoMessage(&DoTalk, 1) ! "you can't talk to that"
		return false
		}
		else
		{
			local a,b
			speaking = object
			while true
			{
				b = Phototalk
				a = higher(a,b)
				xobject = 0
				if not loop_talk or (loop_talk and (not MoreTalk or not b))
					break
				""
			}
#ifclear NO_SCRIPTS
			if a
				SkipScript(object)
#endif
!			speaking = 0 ! clear the speaking global as conversation is over
		}
		return true
	}

	PhotoMessage(&DoTalk,2) ! "Just talking to <the object> will suffice."
	return false
}

routine MoreTalk
{
	local x
	for (x=1 ;x<=speaking.#quips ;x++ )
	{
		if speaking.quip #x and (speaking.quip #x & AVAILABLE)
		{
			return true
		}
	}
}

routine Phototalk
{
	local x, z, ok = 0
	local selected

	speaking = object

! Check and make sure you have something to say.
	for (x=1 ;x<=speaking.#quips ;x++ )
	{
		if speaking.quip #x and (speaking.quip #x & AVAILABLE)
		{
			ok++
#ifclear AUTOMATIC_SAY
			if not xobject
				break
#endif
		}
	}

! Write contents to the screen
	if ok
	{
#ifset AUTOMATIC_SAY
		if ok = 1 and not xobject ! and (BeenSpoken(object,t) or not can_quit)
			xobject = 1
#endif
		if not xobject or xobject < 0 or xobject > ok
		{
			PhotoMessage(&PhotoTalk, 1) ! "Please select one:"
			""
		}

! List the player's choices
		for (x=1;  x <= speaking.#quips; x++)
		{
			if QuipOn(speaking, x) and z < MAX_QUIPS
			{
				quips_array[z++] = x
				if not xobject
					PhotoMessage(&PhotoTalk, 2, z) ! "(#)"
!				print "("; number z; ")";
				if not xobject or xobject = z
					SayQ(speaking,x)
			}
		}

		if not xobject
		{
			""
	! Get the choice
			while true
			{
				selected = GetDial(z)
				if can_quit or (not can_quit and selected)
					break
			}
		}
		else
			selected = xobject

		""
   	if (selected ~= 0)
 		{
			Respond(speaking, quips_array[(selected-1)])
		}
		else
			PhotoMessage(&Phototalk,3) ! "Eeeagh! Stage fright! Abort!"
		return selected
	}

	PhotoMessage(&PhotoTalk,4) ! "You really have nothing to say right now."
}

routine DoQuickTalk
{
	local r, x, n
	local i = 1
	while word[(i+1)] ~= ""
	{
		i++
	}
	if i > 2
	{
		r = WordIsNumber(word[i])
	}

	for (x=1 ;x<=object.#quips ;x++ )
	{
		if (object.quip #x & AVAILABLE)
		{
			n++
		}
	}
	if r > n
	{
		"That is not a valid conversation choice."
		return false
	}
	if not r
	{
		if speaking
		{
			r = object
			object = speaking
		}
		else
		{
			"It is unclear whom you are speaking to."
			return
		}
	}
	else
		speaking = object
	return Perform(&DoTalk, speaking, r)
}

routine GetDial(max)
{
	word[1] = ""
	local temp
	PhotoMessage(&GetDial,1) ! "Select a choice or 0 to keep quiet. >> "
	input

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
		PhotoMessage(&GetDial,1) ! "Select a choice or 0 to keep quiet. >> "
		input
		temp = 0
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
	local ret
	ret = NewPhotoMessages(r, num, a, b)
	if ret:  return ret

	select r
		case &PhotoTalk
		{
			select num
				case 1: "Please select one:"
				case 2: print "("; number a; ")";
				case 3: "Eeeagh! Stage fright! Abort!"
				case 4: "You really have nothing to say right now."
		}
		case &GetDial
		{
			select num
				case 1
				{
					if can_quit
						"Select a choice or 0 to keep quiet. >> ";
					else
						"Select a choice >> ";
				}
!\ If you want a custom prompt, make sure &GetDial case 1 is
	{ return "prompt text" } , not just "prompt text"     \!
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

#if undefined WordIsNumber
routine WordisNumber(w)
{
	select w
	case "zero", "0":       return 0
	case "one", "1":        return 1
	case "two", "2":        return 2
	case "three", "3":      return 3
	case "four", "4":       return 4
	case "five", "5":       return 5
	case "six", "6":        return 6
	case "seven", "7":      return 7
	case "eight", "8":      return 8
	case "nine", "9":       return 9
	case "ten", "10":       return 10
	case -1
	{
		local a
		a = StringToNumber(parse$)
		if a
			return a
	}
	return -1
}
#endif  ! WordIsNumber
#endif