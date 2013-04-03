!::
! "CheapLib" An extension for ugly 'terps
! Version 1.2
!::

#ifclear _CHEAPLIB_H
#set _CHEAPLIB_H

#ifclear CHEAP
#message error "Must \"#set CHEAP\" before \"roodylib.h\" or \"cheaplib.h\" are
					included."
#endif

#ifclear _NEWMENU_H
#message warning "Must include newmenu.h first if your game is using menus."
#endif

#ifclear _ROODYLIB_H
#message error "CheapLib requires \"roodylib.h\". Be sure to include it first!"
#endif

#ifset VERSIONS
#message "CheapLib.h Version 1.2"
#endif

#ifset USE_EXTENSION_CREDITING
version_obj cheap_version "CheapLib Version 1.2"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

global cheap

object cheaplib "cheap"
{
	type settings
	in init_instructions
	save_info
		{
		if cheap
			return true
		else
			return false
		}
	execute
		{
		if CheckWordSetting("cheap") and not cheap
			{
			cheap = true
			DrawCheap
			}
		}
#ifset _NEWMENU_H
#ifset USE_DEFAULT_MENU
	usage_desc
		{
		Indent
		"\BCHEAP/CHEAPMODE ON\b- Turns \"cheap mode\" on. \"Cheap mode\" is a
		no-frills layout meant for interpreters with limited display
		capabilities."
		Indent
		"\BCHEAP/CHEAPMODE OFF\b- Turns \"cheap mode\" off."
		}
#endif ! USE_DEFAULT_MENU
#endif ! _NEWMENU_H
}

routine DoCheapOnOff
{
	local a = 2
	while a < 4
		{
	select word[a]
		case "on"
			{
			if not cheap
				{
				cheap = true
				CheapMessage(&DoCheapOnOff, 1) ! "Cheap mode on."
				DrawCheap
				}
			else
				CheapMessage(&DoCheapOnOff, 2) ! "Cheap mode already on."
			}
		case "off"
			{
			if cheap
				{
				cheap = false
				CheapMessage(&DoCheapOnOff, 3) ! "Cheap mode off."
				PrintStatusLine
				}
			else
				CheapMessage(&DoCheapOnOff, 4) ! "Cheap mode already off."
			}
		a++
		}
}

routine DoCheapHelp
{
	CheapMessage(&DoCheapHelp,1) ! "'Cheap' help spiel."
}

routine DoCheapToggle
{
	if cheap
		{
		cheap = 0
		CheapMessage(&DoCheapToggle,1) ! "'Cheap' mode toggled off."
		PrintStatusLine
		}
	else
		{
		cheap = true
		CheapMessage(&DoCheapToggle,2) ! "'Cheap' mode toggled on."
		DrawCheap(true)
		}
}

routine DrawCheap ! this is basically InitScreen, slightly modified
{
	if not system(61)
	{
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		Font(DEFAULT_FONT)
		window 0
	}
	CenterTitle(CheapTitle,0,1) ! See RoodyLib's CheapTitle routine
}

routine CheapMessage(r, num, a, b)
{
	! Check first to see if the NewMessages routine provides a
	! replacement message:
	if NewCheapMessages(r, num, a, b):  return

	select r
		case &DoCheapHelp
			{
			select num
				case 1
					{
					"To turn on \"cheap mode\", type \BCHEAP ON\b or
					\BCHEAPMODE ON\b. This is the default behavior. To turn cheap
					mode off, type \BCHEAP OFF\b or \BCHEAPMODE OFF\b."
					}
			}
		case &DoCheapOnOff
			{
			select num
				case 1
					"\"Cheap\" mode on."
				case 2
					"\Cheap\" mode already on."
				case 3
					"\"Cheap\" mode off."
				case 4
					"\Cheap\" mode already off."
			}
		case &DoCheapToggle
			{
			select num
				case 1: "\"Cheap\" mode toggled off."
				case 2: "\"Cheap\" mode toggled on."
			}
}

routine NewCheapMessages(r, num, a, b)
{
	select r

!	case :
!	{
!		select num
!		case 1: return false
!		case 2: return false
!	}

        case else : return false

       return true ! this line is only reached if we replaced something
}


#endif