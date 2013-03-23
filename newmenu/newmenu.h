!::
! NEWMENU.H  Version 3.1 by Roody Yogurt
!::
!\

	For a nice overview of this contribution, check out:
	http://hugo.gerynarsabode.org/index.php?title=NewMenu.h

	version 3.1 - added NEW_FUSE code
	version 3.0 - Added USE_EXTENSION_CREDITING object stuff
	version 2.9 - Fixed some flags so the default menu works without Roodylib
	              Cleaned up command-printing page in default menu; added
					  PrintCommands routine
	version 2.8 - Rewrote how "cheap"-mode works and looks. Now glk menus that
	              are over a certain size are automatically drawn as cheap
					  menus. Other little changes.
	version 2.7 - Added an argument to ShowPage so it can be called at the
	              end of a game and won't expect to go back to gameplay
					  Added it to MakeMenu so authors have the option of calling
					  up menus after the game, too
	version 2.6 - common_commands array was being overwritten by MenuInit
	              Changed the default printing behavior of CoolPause
					  Changed text in the Special Commands page
					  CoolPause with top pause message now always one line window
					     (instead of current display.status_window height)
	version 2.5 - added some code for dealing with changing window sizes
	mid-menu

	version 2.4 - changed it so newmenu always compiles with a main_menu object,
	default menu still only displays with #USE_DEFAULT_MENU switch. you can
	change the name of the main_menu object easily with:
	main_menu.name = "Your New Name"

	version 2.3 - fixed some cheaplib/color stuff. added a #USE_DEFAULT_MENU
	switch so the built-in menu only shows up when wanted. got rid of extra
	array.

	version 2.2 - added a "priority" property for the option objects. higher
	numbers are listed first in menu lists (only works when newmenu is used in
	conjunction with "roodylib". Fixed some clearscreen stuff; tidied up the
	messages to be shown during transcripts.

	version 2.0 - took out jumps, hid a cursor, fixed some pauses, made Glk.h
	no longer necessary for menus to work with interpreters like Gargoyle

	version 1.9 - fixed a bug in non-glk games, added Kent Tessman's
	cursor-hiding pause code

	version 1.8 - fixed something where hidden transcript notes were
	being printed in BGCOLOR, BGCOLOR instead of MENU_BGCOLOR, MENU_BGCOLOR

	version 1.7 - better support for menu colors. Now choosing specific
	menu colors results in the colors being used for the whole page, instead
	of a colored menu option "island" on a normal TEXTCOLOR/BGCOLOR background
	in some circumstances.

	Also, I added some color properties for menu_category options, so different
	menu pages can have different color schemes. The new properties are:
	page_text_color
	page_bg_color
	title_color
	title_bg

   version 1.5 - fixed message routines
	New in version 1.2- added "options_gap" property for menucategory objects. Controls spaces
	between "[N]ext item..." text and menu options, giving more configurability.

	New in version 1.1- added "title_gap" property to menucategory object. Put in a true value
	if you want that menu to have a blank line between the title and "[N]ext item..." text

This is an update to Kent Tessman's menu routine in the Hugo Library.
Differences in this version:

- Adds some "hidden" text so transcripts look better.
- Adds asterisk-highlighting when used with glk interpreters (requires glk.h and possibly cheapglk.h)
- Adds support to cheapglk interpreters (requires cheapglk.h and glk.h)
- Has built-in support for Guilty Bastards style hints

As it is based on the original menu routine and Guilty Bastards hint routine, this
extension draws on those two things a fair amount, despite having been worked around, so
this thing couldn't have been written without Kent's original work.

First things first-
Include this file after "hugolib.h".

This extension *expects* a menu_category object named main_menu. One is
provided by newmenu.h, but if you need to replace it, do the following:

replace main_menu "TITLE OF YOUR MAIN MENU"
{
	inherits menu_category
}

Now, calling MakeMenu from a DoAbout or DoHelp routine will automaticaly bring up that menu. (if
you have several different menus that can be called by different commands, call MakeMenu([other_menu_object]).

Now let's fill that menu with choices. For this, we use option objects.

option contact_choice "Contact"
{
	in main_menu    ! which menu it is
	menu_text	{
					""
					"\_ Feel free to send me your thoughts and suggestions at roody.yogurt@gmail.com! What an amazing discourse we will have!\n"
					CoolPause(1)
					}
}

The menu_text property holds the text you want on the page. Personally, I believe every time the game is paused,
there should be some "press key to continue," so I've included a CoolPause routine for doing that quickly.

CoolPause([true_if_bottom_of_screen], [optional text])

CoolPause can also do "press a key" in the status bar by putting a 0 in the first value, but as far as menu choices go,
you'll probably want the text at the bottom. If you want something different than the default "press a key" text, put your
string in the optional text field.

Of course, if you are in love with the old way, you could just put a regular pause there.

You can make change when an option is available by changing its option_available value.
Of course, return true when you want it available:

option helicopter_choice "Whoa, a Helicopter!"
{
	in main_menu
	option_available {
							if helicopter is known
								return true
							else
								return false
							}
	menu_text	{
					! helicopter text
					}
}

If you want an option to lead to *another* menu, give it a menu_link property:

option submenu_choice "CHOICE LEADING TO SUBMENU"
{
	in main_menu
	menu_link	[submenu menucategory object]
}

Hints!

So yeah, this thing also supports hints. Just make a hint menucategory object, and
fill it with hint_options like this:

hint_option studiopass_hints "How do I get on the studio lot?"
{
	in hint_menu
	hints_available
		{
		if studio_lot is visited
			return true
		else
			return false
		}
	hint1
	{
		"Movie studio security is on par with Area 51.  To get on
		the lot, you'll need a pass."
	}
	hint2
	{
		"There's a studio pass in the envelope that was slipped
		under the door of your hotel room."
	}
	hint3
	{
		"Drive to the studio lot.  Make sure you've got the pass
		handy.  Get out of the car and go north."
	}
}
	Notice the hints_available property (which does pretty the same
	thing as the option_available property). Now, the player will
	have to type 'H' for each additional hint on this hint page.

	Hmmm, that should be everything! E-mail me if you have any questions!

											Roody Yogurt
											roody.yogurt@gmail.com
\!
#ifclear _NEWMENU_H
#set _NEWMENU_H

#ifset VERSIONS
#message "NewMenu.h Version 3.1"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj newmenu_version "NewMenu Version 3.1"
{
	in included_extensions
	desc_detail
		" by Kent Tessman and Roody Yogurt";
}
#endif

#ifclear _SYSTEM_H
#include "system.h"
#endif

#ifset USE_DEFAULT_MENU

#ifset _ROODYLIB_H

#ifclear USE_SORTING
#message warning "#set USE_SORTING if you want to make use of the menu
priority system!"
#endif

object menulib "menu"
{
! if roodylib.h has been included before newmenu.h, nothing needs to be
! added to the init routine
	type settings
	in init_instructions
	execute
		{
		if not CheckWordSetting("undo")
			{
			if not CheckWordSetting("restore")
				MenuInit
			}
		}
}

property usage_desc ! some text describing what commands turn a library's
						  ! function on/off (ex: CHEAPMODE ON)

#endif ! _ROODYLIB_H

#if undefined TOTAL_COMMANDS
constant TOTAL_COMMANDS 35
#endif

array common_commands[TOTAL_COMMANDS]

routine MenuInit
{
	common_commands[0] = "OPEN","CLOSE","LOCK","UNLOCK",
	"WEAR", "REMOVE","TURN ON","TURN OFF", "DRINK",  ! "EAT",
	"SIT" , "MOVE"

! the 0s below show where commands switch from regular to verbstub
#ifset _ROODYLIB_H
	common_commands[11] = "SEARCH"
#ifset _VERBSTUB_G
	common_commands[12] = 0, "PUSH", "PULL", "YELL", "JUMP", "THROW",
	"SLEEP", "KISS", "WAVE", "CLIMB", "WAKE", "SWIM",
	"DIG", "TIE", "BURN", "CUT"
#endif ! _VERBSTUB_G
#elseif set _VERBSTUB_G  !VERBSTUB_G *without* ROODYLIB
	common_commands[11] = "SEARCH", 0, "PUSH", "PULL", "YELL", "JUMP",
	"THROW", "SLEEP", "KISS", "WAVE", "CLIMB", "WAKE",
	"SWIM", "DIG", "TIE", "BURN", "CUT"
		! Changed Throw  and Throw At listings to just Throw
#endif

#ifset _ROODYLIB_H
	local i
	for i in init_instructions
		{
		if &i.usage_desc
			{
			move special_choice to main_menu
			break
			}
		}
#endif ! _ROODYLIB_H
}
#endif ! USE_DEFAULT_MENU

! menu_category properties
property title_gap alias initial_desc
property options_gap alias reach
property page_text_color alias holding
property page_bg_color alias exclude_from_all
property title_color alias list_contents
property title_bg alias capacity

! option/hint_option properties

property menu_text alias long_desc
property menu_link alias door_to
property hint1 alias n_to
property hint2 alias ne_to
property hint3 alias e_to
property hint4 alias se_to
property hint5 alias s_to
property hint6 alias sw_to
property hint7 alias w_to
property hint8 alias nw_to
property hint9 alias u_to
property hint10 alias d_to

property option_available alias in_to
property hints_available alias in_to
property hints_revealed alias out_to
property priority alias parse_rank

class menu_category
{
	title_gap 0 ! lines between menu title and "[N]ext key"
	options_gap 1 ! lines between "[N]ext key" and menu options
}

class option
{
	option_available true
#ifset _ROODYLIB_H
	priority 0
#endif
}

option hint_option
{
	menu_text
		{
		Help_Hints(self)
		}
	hints_available 1
	hints_revealed 0
}

routine MakeMenu(menu_title,end_o_game, recurse)
{
	local glktest, count, category, old_category
 	local h,r, simple_port
	if not glktest
		{
		if display.windowlines > (display.screenheight + 100)
			{
			glktest = true
			}
		}
	if not recurse
		{
		if not CheaporSimple
			{
			color BGCOLOR, BGCOLOR
			MenuMessage(&MakeMenu,1) ! "[OPENING MENU]"
			}
		else
			{
			""
			MenuMessage(&MakeMenu,7) ! "Opening the menu..."
			}
!		""
		}
	if not glktest and system(61)
		simple_port = true
	if not (CheaporSimple = 2 or simple_port)
		cls
	r = recurse + 1
	if not menu_title
		h = main_menu
	else
		h = menu_title

   while true
		{
		if h.page_text_color
			MENU_TEXTCOLOR = h.page_text_color
		else
			MENU_TEXTCOLOR = 0
		if h.page_bg_color
			MENU_BGCOLOR = h.page_bg_color
		else
			MENU_BGCOLOR = 0
		if h.title_color
			MENU_SELECTCOLOR = h.title_color
		else
			MENU_SELECTCOLOR = 0
		if h.title_bg
			MENU_SELECTBGCOLOR = h.title_bg
		else
			MENU_SELECTBGCOLOR = 0

		if MENU_TEXTCOLOR=0 and MENU_BGCOLOR=0  ! must not have been set
		{
			MENU_TEXTCOLOR = TEXTCOLOR
			MENU_BGCOLOR = BGCOLOR
		}

		if MENU_SELECTCOLOR= 0 AND MENU_SELECTBGCOLOR = 0
			{
			MENU_SELECTCOLOR = SL_TEXTCOLOR
			MENU_SELECTBGCOLOR = SL_BGCOLOR
			}
		if not (CheaporSimple = 2 or simple_port)
			color MENU_TEXTCOLOR, MENU_BGCOLOR

		count = 0
		local c
		for category in h
			{
				if category.option_available
				{
					menuitem[++count] = category !.name
					if category.priority
						c = true
				}
			}
#ifset _ROODYLIB_H
#ifset USE_SORTING
			if c
				{
				SortArray(menuitem,count,&MenuPriority,1)
				}
#endif
#endif

	#ifset SHOW_NAVIGATE
	#ifset CHEAP
				if not cheap
					{
	#endif
					color MENU_BGCOLOR, MENU_BGCOLOR
					if not simple_port ! non-glk minimal port
						MenuMessage(&MakeMenu,2,recurse,h.name) ! "[MENU NAME]"
					color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	#ifset CHEAP
					}
				else
					{
					Font(PROP_OFF)
					MenuMessage(&MakeMenu,2,0,h.name) ! "[MENU NAME]"
					""
					}
	#endif
	#endif ! SHOW_NAVIGATE

		if not (CheaporSimple = 2 or simple_port)
			{
			window 0
			cls
			}
		menuitem[0] = h
		category = Menu(count, 0, old_category,h.title_gap,h.options_gap)
		old_category = category
		if not (CheaporSimple = 2 or simple_port)
			cls
		if category
			{
			if menuitem[category].menu_link
				{
				MakeMenu(menuitem[category].menu_link,0,r)
				}
			else
				{
				do
					{
					if display.needs_repaint and CheaporSimple ~= 2
						{
						color MENU_BGCOLOR, MENU_BGCOLOR
						MenuMessage(&MakeMenu,4) ! "[WINDOW RESIZED]"
						color TEXTCOLOR, BGCOLOR, INPUTCOLOR
!						""
						cls
						locate 1,1
						}
					display.needs_repaint = false
					if not (CheaporSimple = 2 or simple_port)
						color MENU_BGCOLOR, MENU_BGCOLOR
					if CheaporSimple ~= 2
						MenuMessage(&MakeMenu,3,menuitem[category].name)
					color TEXTCOLOR, BGCOLOR, INPUTCOLOR
					if not simple_port
						CenterTitle(menuitem[category].name)
#ifset CHEAP
					if not cheap
						{
#endif
					if not simple_port
						cls
					if not system(61) ! glk or minimum port
						locate 1,1
#ifset CHEAP
						}
#endif
					run menuitem[category].menu_text
					if not (CheaporSimple = 2 or simple_port)
						""
					}
				while (display.needs_repaint = true  )
				window 0 ! only to draw a line in simple interpreters
				if not (CheaporSimple = 2 or simple_port)
					cls
				}
			}
		else
			{
			if not recurse
			{
#ifset CHEAP
				if not cheap
					{
#endif
					color BGCOLOR, BGCOLOR
					MenuMessage(&MakeMenu,5) ! "[LEAVING MENU]"
#ifset _ROODYLIB_H
					if not simple_port
						InitScreen
#else
					color TEXTCOLOR, BGCOLOR, INPUTCOLOR
					if not system(61)
						window 0
					if not simple_port
						cls
					if not system(61) ! glk or minimum port
						locate 1, (display.statusline_height + 1)
#endif
#ifset CHEAP
					}
#endif
				if not end_o_game
				{
					MenuMessage(&MakeMenu,6) ! "Returning to the game..."
#if defined DESCFORM_I
					if (FORMAT & DESCFORM_I)
						""
#endif
#ifset CHEAP
					if not cheap
#endif
						PrintStatusline
					Font(DEFAULT_FONT) ! just in case
					DescribePlace(location, true)
#ifset NEW_FUSE
					runevents
#endif
				}
			}
			return false
		}
	}
}

#ifset _ROODYLIB_H
routine MenuPriority(obj1,obj2)
{
	return (obj1.priority < obj2.priority)
}
#endif

replace Menu(num, width, selection,titlegap,optionsgap)
{
	local i, column, oldselection
	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
	local simple_port, glktest
	simple_port = not (display.windowlines > (display.screenheight + 100)) and system(61)
	if width = 0:  width = 20
	for (i=1; i<=num; i++)          ! determine appropriate width
#ifclear NO_AUX_MATH
		width = higher(width, string(_temp_string, menuitem[i].name))
#else
	{
		local n
		n = string(_temp_string, menuitem[i].name)
		if n > width:  width = n
	}
#endif

	if width > display.linelength-1:  width = display.linelength-1

	! Default selection is 1 if not otherwise given
	if selection = 0:  selection = 1
	if selection > num:  selection = num

	if CheaporSimple = 2 ! or simple_port
		column = 5
	else
		column = display.linelength/2 - width/2
	glktest = display.windowlines > (display.screenheight + 100)
	if CheaporSimple = 2 or
	( glktest and
	( num + 5 + titlegap + optionsgap ) >= (display.screenheight/3*2))
	{
		while true
			{
				if glktest
					cls
				else
					""
				if not simple_port
					CenterTitle(menuitem[0].name)
				if display.needs_repaint
					display.needs_repaint = false
				print newline
				Font(PROP_OFF|BOLD_OFF|ITALIC_OFF)
		!		MenuMessage(&Menu, 2)		! print key commands
		!		""
				local sel = 1
				Indent
				print "\_  ";
				Font(BOLD_ON)
				print menuitem[0].name
				Font(BOLD_OFF)
				""
				while menuitem[sel]
				{
					Indent
					print number sel; ". ";
					print menuitem[sel++].name
				}
				print ""
				MenuMessage(&Menu, 2)		! "Select the number of your choice"
				input
	!			select word[0]

				if word[1] = "q", "exit", "quit", "0"! ESCAPE_KEY
					{
					""
					return 0
					}
				local numb
				numb = StringToNumber(word[1])
				if not numb
				{
					numb = StringToNumber(parse$)
				}
				if numb and numb < sel
				{
					""
					return numb
				}

			}
		}
	else
		{
		while true
			{
			if not system(61)
				{
				color MENU_BGCOLOR, MENU_BGCOLOR
				cls
				}
			window ( num + 5 + titlegap + optionsgap )
				{
				if not simple_port
					cls
				local m
				! This section "fakes" CenterTitle, so it appears to be
				! a regular title window, when in actuality, the whole menu
				! is in the status window
				if not simple_port
					locate 1,1
				if MENU_SELECTCOLOR = 0 and MENU_SELECTBGCOLOR = 0 ! not set yet
					{         ! this section shouldn't affect a glk interpreter game
					MENU_SELECTCOLOR = SL_TEXTCOLOR
					MENU_SELECTBGCOLOR = SL_BGCOLOR
					}
				Font(PROP_OFF)
				m = string(_temp_string, menuitem[0].name)
				color MENU_SELECTCOLOR, MENU_SELECTBGCOLOR ! shouldn't affect glk
				print to (display.linelength/2 - m/2);
				print menuitem[0].name;
				print to display.linelength ! make sure we color the line completely
				for (i=0; i<titlegap;i++)
					{
					""
					}
				color MENU_TEXTCOLOR, MENU_BGCOLOR
				! end of fake CenterTitle
				Font(BOLD_OFF|ITALIC_OFF|UNDERLINE_OFF|PROP_OFF) ! shouldn't affect
																				 ! glk
				MenuMessage(&Menu, 1)	! print key commands

				if system(61) ! is minimal port
					""
				else
				{
				for (i=0; i<optionsgap;i++)
					{
					""
					}
				}

				if selection ~= oldselection		!	glk code that *shouldn't*
					{										!  affect normal execution
					if oldselection ~= 0				!  (hopefully)
						selection = oldselection
					}
				for (i=1; i<=num ; i++)
					{
					if i = selection
						{
						if system(61) ! glk or minimum port
							print to (column - 2);
						else
							print to column;
						if system(61) ! is minimal port
							print "* ";
						color MENU_SELECTCOLOR, MENU_SELECTBGCOLOR  ! shouldn't affect
																				  ! glk?
						print menuitem[selection].name; to (column+width);
						color MENU_TEXTCOLOR, MENU_BGCOLOR
						""
						}
					else
						{
						print to column;
						print menuitem[i].name; to (column+width)
						}
					}
				print ""
				}
				Font(DEFAULT_FONT)
				word[0] = PauseForKey
				if not system(61)
					{
					if display.needs_repaint
						{
						window 0
						display.needs_repaint = 0
						}
					color MENU_BGCOLOR, MENU_BGCOLOR
					cls
					}
				select word[0]
					case 'N', 'n', DOWN_ARROW, RIGHT_ARROW
					{
						if menuitem[++selection].name = ""
							++selection
						if selection > num : selection = 1
					}
					case 'P', 'p', UP_ARROW, LEFT_ARROW
					{
						if menuitem[--selection].name = ""
							--selection
						if selection < 1 : selection = num
					}
					case 'Q', 'q', ESCAPE_KEY
					{
					if not system(61)
						{
						window 0
						}
					if not simple_port
						{
						cls
						Font(DEFAULT_FONT)
						}
					return 0
					}
					case ENTER_KEY
					{
					color MENU_BGCOLOR, MENU_BGCOLOR
					if not system(61)
						{
						window 1, (3+ optionsgap + titlegap), display.screenwidth, ( num + 5 + titlegap + optionsgap )
							{
							cls
							}
						window 0
						}
					if not simple_port
						Font(DEFAULT_FONT)

					oldselection = selection

					color MENU_TEXTCOLOR, MENU_BGCOLOR
					return selection
					}

				if word[0] >= '0' and word[0] <= '9'
					{
						i = word[0] - '0'
						if i = 0:  i = 10

						selection = 1
						while --i
						{
							selection++
							if menuitem[selection].name = ""
								selection++
						}
						if selection > num or menuitem[selection].name = ""
							selection = oldselection
					}
		}
	}
}

routine CoolPause(bottom,pausetext)
{
	local a, m
	local simple_port
	simple_port = not (display.windowlines > (display.screenheight + 100)) and system(61)
	if pausetext
	{
		m = string(_temp_string, pausetext)
		if display.screenwidth > m
			a = pausetext

	}

#ifset CHEAP
	if cheap
		bottom = true
#endif
	if not bottom
	{
		Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
		if not system(61)
		{
			window display.statusline_height
			{
				cls
			}
		}
		window  1 ! display.statusline_height
			{
			local y
			y = display.linelength
			color SL_TEXTCOLOR, SL_BGCOLOR
			cls			! make sure we've drawn the entire status bar in the
							!  proper colors
			locate 1,1
			text to _temp_string
			if a
				print a;
			else
				MenuMessage(&CoolPause,1) ! "PRESS A KEY TO CONTINUE";
			text to 0

			local alength
			alength = StringLength(_temp_string)
			print to (display.linelength/2 - alength/2);
			StringPrint(_temp_string)
!			print to display.linelength;
			}
			color TEXTCOLOR, BGCOLOR, INPUTCOLOR
			Font(DEFAULT_FONT)
		PauseForKey
		""
		}
	else
		{
		Font(DEFAULT_FONT)
#ifset CHEAP
		if cheap
			{
			Indent
			if a
				print a
			else
				MenuMessage(&CoolPause,2) ! "[PRESS A KEY TO CONTINUE]";
			pause
			""
			}
		else
			{
#endif
			Indent
			Font(ITALIC_ON)
			if a
				print a;
			else
				MenuMessage(&CoolPause,3) ! "press a key to continue";
			Font(ITALIC_OFF)
			PauseForKey
#ifset CHEAP
			}
#endif
		}
	Font(DEFAULT_FONT)
}

routine CheaporSimple
{
#ifset CHEAP
if cheap
	return 2
#endif
if system(61)
	return 3
}

routine Help_Hints(obj)
{
local i
#ifset CHEAP
if not cheap
#endif
	""
for (; i<=obj.hints_revealed; i++)
	{
		run obj.(hint1+i)
		""
	}
while &obj.(hint1+i) ~= 0 ! i.e., no more topics
	{
	Font(BOLD_ON)
	MenuMessage(&Help_Hints,1) ! "[Press 'H' for another hint, or 'Q' to
										!	quit]";
	Font(BOLD_OFF)
	word[0] = PauseForKey
!	""
	if word[0] = 'Q', 'q', ESCAPE_KEY
		{
#ifset CHEAP
		if cheap
			""
#endif
		return
		}
	if word[0] = 'H', 'h'
		{
		obj.hints_revealed++
		print newline
		}
	if word[0] = 'H', 'h'
		{
		""
		run obj.(hint1+i++)
		""
		}
	}

	Font(BOLD_ON)
	MenuMessage(&Help_Hints,2) ! "[No more hints.  Press any
										!  key...]";
	Font(BOLD_OFF)
	PauseForKey
#ifset CHEAP
		if cheap
			""
#endif
	return
}


routine PauseForKey(p)	! Where p is a prompt, if it ends up being used
{
	local key

	key = system(READ_KEY)
	if system_status or system(MINIMAL_INTERFACE)
	{
		! If READ_KEY isn't available, we have to use the
		! regular pause-with-cursor (and maybe a prompt)
		if p
		{
			if not system(MINIMAL_INTERFACE)
			! If we give a prompt, it always goes at the bottom
				locate (display.screenwidth-20), display.screenheight
			Font(PROP_ON | ITALIC_ON | BOLD_OFF)
			print p;
			Font(DEFAULT_FONT | ITALIC_OFF)
		}
		pause
		key = word[0]
	}
	else
	{
		while true
		{
			key = system(READ_KEY)
			system(PAUSE_100TH_SECOND)
			if key:break
		}
	}

	return key
}

routine ShowPage(page,end_o_game)
{
 	local simple_port,glktest
	if display.windowlines > (display.screenheight + 100)
		{
		glktest = true
		}

	if not glktest and system(61)
		simple_port = true
	color BGCOLOR, BGCOLOR
	if not (CheaporSimple = 2 or system(61))
		{
		window 0
		}
	if not CheaporSimple = 2 or simple_port
		{
		MenuMessage(&ShowPage,1) ! "[OPENING PAGE]"
		cls
		}
	do
		{
		if display.needs_repaint and CheaporSimple ~= 2
			{
			color BGCOLOR, BGCOLOR
			MenuMessage(&MakeMenu,4) ! "[WINDOW RESIZED]"
			color TEXTCOLOR, BGCOLOR, INPUTCOLOR
!			""
			cls
			locate 1,1
			}
		display.needs_repaint = false
		if not (CheaporSimple = 2 or simple_port)
			color BGCOLOR, BGCOLOR
		if CheaporSimple ~= 2
			MenuMessage(&MakeMenu,3,page.name)
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		if not simple_port
			CenterTitle(page.name)
#ifset CHEAP
		if not cheap
			{
#endif
		if not simple_port
			cls
		if not system(61) ! glk or minimum port
			locate 1,1
#ifset CHEAP
			}
#endif
		run page.menu_text
		if not (CheaporSimple = 2 or simple_port)
			""
		}
	while (display.needs_repaint = true  )
	window 0 ! only to draw a line in simple interpreters
	if not (CheaporSimple = 2 or simple_port)
		cls
#ifset CHEAP
	if not cheap
		{
#endif
		color BGCOLOR, BGCOLOR
		MenuMessage(&ShowPage,2) ! "[CLOSING PAGE]"
#ifset _ROODYLIB_H
		if not simple_port
			InitScreen
#else
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		if not system(61)
			window 0
		if not simple_port
			cls
		if not system(61) ! glk or minimum port
			locate 1, (display.statusline_height + 1)
#endif
#ifset CHEAP
		}
#endif
	if not end_o_game
	{
		MenuMessage(&Menu,6) ! "Returning to the game..."
#ifset CHEAP
		if not cheap
#endif
			PrintStatusline
		Font(DEFAULT_FONT) ! just in case
		DescribePlace(location, true)
#ifset NEW_FUSE
		fake_runevents
#endif
	}
}

routine MenuMessage(r, num, a, b)
{
	if NewMenuMessages(r, num, a, b):  return

	select r
		case &MakeMenu
			{
			select num
				case 1 : "[OPENING MENU]"
				case 2
					{
					if a
						""
					print "[MENU NAME: \""; b ; "\"]"
					}
	!  some other options
	!			case 3: print "[MENU CHOICE: \""; menuitem[a];"\"]"
	!			case 3: print "[\""; menuitem[a];"\"]"
				case 3
					{
					local l
					l = string(_temp_string, a)
					print to (40 - l/2);
					print a
					}
				case 4 : "[WINDOW RESIZED - REDRAWING]"
				case 5 : "[LEAVING MENU]"
				case 6
					{
					print "\IReturning to the story...\i"
					}
				case 7 : "Opening the menu..."
			}
		case &Menu
			{
			select num
				case 1
					{
					print "[N]ext item"; to (display.linelength - 11); \
		    				"[Q]uit menu"
					print "[P]revious item"; to (display.linelength - 17);
					print "[Enter] to select"
					}
				case 2
					{
					! The CheapGlk version now works off numbers to make
					! navigation as easy as possible in simple terps
					print "Select the number of your choice or \"Q\" to exit: ";
					}
			}
		case &CoolPause
			{
			select num
				case 1  ! default top "press a key"
	!				"\_ [PRESS A KEY TO CONTINUE]";
					print "[PRESS A KEY TO CONTINUE]";
				case 2  ! default cheapglk "press a key"
!					"\_ [PRESS A KEY TO CONTINUE]";
					"[PRESS A KEY TO CONTINUE]";
				case 3  ! default normal "press a key"
!					"\_\B Press a key to continue...\b"
					"press a key to continue";
			}

		case &Help_Hints
			{
			select num
				case 1
					"[Press 'H' for another hint, or 'Q' to quit]" !;
				case 2
					"[No more hints.  Press any key...]"
			}
		case &ShowPage
			{
			select num
				case 1
					"[OPENING PAGE]"
				case 2
					"[CLOSING PAGE]"
			}
}

!\ The NewMenuMessages routine may be REPLACED and should return
true if a replacement message exists for routine <r> \!

routine NewMenuMessages(r, num, a, b)
{
	select r

!	case
!	{
!		select num
!		case 1:
!	}

	case else : return false

	return true ! this line is only reached if we replaced something
}

menu_category main_menu
{
name
	{
#if defined GAME_TITLE
	return GAME_TITLE
#endif
#if undefined GAME_TITLE
	return "Help Menu"
#endif
	}
}

#ifset USE_DEFAULT_MENU
option hugo_choice "About Hugo"
{
	in main_menu
#ifset _ROODYLIB_H
	priority 10
#endif
	menu_text	{
					Indent
					"Hugo is a system created by Kent Tessman for developing and playing sophisticated computer adventure games (or \"interactive fiction\").\n"
					Indent
					"The complete system comprises a high-level programming language, world-simulation library, compiler, runtime engine, debugger, and other tools. Games can be designed for player input keyboard and/or mouse in addition to using Hugo's full-sentence natural-language parsing capabilities, and can make extensive use of graphics, sound, music, and video playback.\n"
					Indent
					"Hugo is powerful and versatile enough to have been used not only for games such as ";
					Font(ITALIC_ON)
#if defined GAME_TITLE
					print GAME_TITLE;
#else
					print "Future Boy!";
#endif
					Font(ITALIC_OFF)
					", but also for presentations and tutorials, prototyping, and other applications.\n"
					Indent
					"Hugo's cross-platform support is broad, including official implementations for Windows, Macintosh, Linux/Unix, Windows Mobile/Pocket PC, and Palm, with additional versions available for other platforms. The source code for all components is available.\n"
					Indent
					"\BHugo's official website:\b"
					Indent
					"\Ihttp://www.generalcoffee.com/index_noflash.php?content=hugo\i"
					""
					Indent
					"\BHugo By Example, a Hugo code repository:\b"
					Indent
					"\Ihttp://hugo.gerynarsabode.org\i\n"
					Indent
					"\BThe joltcountry Hugo forum:\b"
					Indent
					"\Ihttp://www.joltcountry.com/phpBB2/viewforum.php?f=8\i\n"
					CoolPause(1)
					}
}

option if_choice "How To Play Interactive Fiction"
{
	in main_menu
#ifset _ROODYLIB_H
	priority 9
#endif
	menu_link	how_play
}

option where_choice "Where To Get More IF"
{
	in main_menu
#ifset _ROODYLIB_H
	priority 8
#endif
	menu_text	{
					Indent
					"\BMore Hugo games can be found at:\b"
					Indent
					"\Ihttp://ifarchive.org/indexes/if-archiveXgamesXhugo.html\i"
					Indent(true)
					"\BOther sites of interest:\b"
					Indent
					"The Brass Lantern (\Ihttp://brasslantern.org/\i)"
					Indent
					"The People's Republic of Interactive Fiction
					(\Ihttp://pr-if.org/play/\i)"
	!				Indent
	!				"Brass Lantern (\Ihttp://brasslantern.org/\i)"
					Indent
					"The IF Database (\Ihttp://ifdb.tads.org/\i)"
					Indent
					"IF Reviews (\Ihttp://www.ifreviews.org/\i)"
					Indent
					"The IF Archive (\Ihttp://ifarchive.org/\i)\n"

			!		"\Ihttp://www.joltcountry.com/phpBB2/viewforum.php?f=8\i\n"
					CoolPause(1)
					}
}

#ifset _ROODYLIB_H ! Only roodylib-using games can do Special Commands
option special_choice "Special Commands"
{
!	in main_menu (should be added to main_menu by menulib object
	menu_text	{
					Indent
!					"Additional commands that \Imay\i work with this game,
!					depending on your interpreter, include:\n"
					"\BAdditional commands:\b\n"
					SpecialCommands
					CoolPause(1)
					}
}

routine SpecialCommands
{
local i, sum
for i in init_instructions
	{
	if &i.usage_desc
		{
		if i.usage_desc
			{
			""
			sum++
			}
		}
	}
if not sum
	{
	Indent
	"Sorry, it appears that there are no special features supported by your
	interpreter. Try the official Hugo interpreter or Hugor TODAY!\n"
	}
}
#endif

menu_category how_play "How To Play Interactive Fiction"
{}

option what_is "What is \"interactive fiction\"?"
{
	in how_play
#ifset _ROODYLIB_H
	priority 10
#endif
	menu_text
		{
!		Indent
!		"Exactly \Iwhat is\i interactive fiction is very much a source of debate.
!		Some say interactive fiction is a combination of storytelling and world
!		simulation, often with the inclusion of game elements. Other
!		descriptions may be wildly more or less stringent.\n"
		Indent
		"In interactive fiction, you, the player, will be appraised of
		your surroundings and happenings in the game by largely textual means.
		You get the chance to interact with these surroundings by typing
		commands (read the other entries in this menu for examples of the kind
		of commands that are accepted). Hopefully, you are rewarded for your
		efforts with engaging puzzles and entertaining prose!"
		""
		CoolPause(1)
		}
}

option move_around "Moving around"
{
	in how_play
#ifset _ROODYLIB_H
	priority 9
#endif
	menu_text
		{
		Indent
		"Everything described in your current location should be available to
		interact with (unless it is explicitly out-of-reach), so rest assured, you
		should almost never have to \B>WALK OVER TO THING\b. This is a common
		misunderstanding.\n"
		Indent
		"That said, you \Ido\i need to move from room to room. Most of the time,
		this will involve going in compass directions (\BGO SOUTH. GO NORTHWEST.\b). Such directions can be abbreviated further (\BS. NW.\b).\n"
		Indent
		"All in all, eight compass directions are usually accepted (\BNORTH\b, \BNORTHEAST\b, \BEAST\b, \BSOUTHEAST\b, \BSOUTH\b, \BSOUTHWEST\b, \BWEST\b, AND \BNORTHWEST\b), along with the directions \"\BUP\b\",\"\BDOWN\b\",\"\BIN\b\", and \"\BOUT\b\".\n"
		Indent
		"Some games will demand shipboard directions like \"\BAFT\b\",\"\BFORE\b\",\"\BSTARBOARD\b\", and \"\BPORT\b\" for certain areas, while some
		games \Iwill\i accept a simple \B>GO TO <ROOM>\b. In both cases, it is
		expected that the game will adequately inform you of your options.\n"

		"Sometimes, you'll want to \BENTER\b, \BCLIMB\b, or \BGET ON\b something.
		Later on, you may want to \BGET OFF\b, \BGET UP\b, or \BEXIT\b \Ithat
		same object\i!"

		""
		CoolPause(1)
		}
}

option look_around "Looking around"
{
	in how_play
#ifset _ROODYLIB_H
	priority 8
#endif
	menu_text
		{
		Indent
		"Getting a good look at things is important in interactive fiction. If
		you've forgotten what the room looks like, you can always \B>LOOK\b or
		\B>LOOK AROUND\b again to get another peek.\n"
		Indent
		"To get a closer look at objects (including yourself), you'll want to
		\BEXAMINE\b or \BLOOK AT\b the object, like:\n"
		Indent
		"\B>EXAMINE FOOTPRINT\b\n"
		Indent
		"(To save time, \BEXAMINE\b can be shortened down to \BX\b.)\n"
		Indent
		"Some things can be \BSEARCH\bed or \BLOOK\b \BIN\b, too. In tougher
		games, you may even need to \BLOOK BEHIND\b or \BLOOK UNDER\b something."

		""
		CoolPause(1)
		}
}

option talk_chars "Talking to characters"
{
	in how_play
#ifset _ROODYLIB_H
	priority 7
#endif
	menu_text
		{
		Indent
		"In \"traditional\" IF, charactes can be interacted with using four major
		commands:\n"
		Indent
		"\BASK <CHARACTER> ABOUT <SUBJECT>\b"
		Indent
		"\BTELL <CHARACTER> ABOUT <SUBJECT>\b"
		Indent
		"\BSHOW <OBJECT> TO <CHARACTER>\b"
		Indent
		"\BGIVE <OBJECT> TO <CHARACTER>\b\n"
		Indent
		"Additionally, sometimes characters can be given commands, using this
		syntax:\n"
		Indent
		"\BDETECTIVE, FOLLOW ME\b"
		Indent
		"\BCLOWN, JUMP THROUGH HOOP\b\n"
		Indent
		"In newer games, though, it is common that a simple \"\BTALK TO
		<CHARACTER>\b\" will handle all of your character-interacting needs."
		""
		CoolPause(1)
		}
}

option manip_objects "Manipulating objects"
{
	in how_play
#ifset _ROODYLIB_H
	priority 6
#endif
	menu_text
		{
		Indent
		"Knowing how to work with object is very important to IF. To begin with,
		sometimes your character will start off with objects in his or her
		possession. To see what items you possess, try \B>TAKE INVENTORY\b. This
		can be shortened down to \"\BINVENTORY\b\", \"\BINV\b\", or just
		\"\BI\b\".\n"
		Indent
		"Now that you know what items you've got, you want MORE, right? To acquire
		objects, try \BPICK UP <object>\b or \BGET <object>\b. To let it go, try
		\BDROP <object>\b. Sometimes you'll even want to \BPUT <object> IN
		<another object>\b or \BPUT <object> ON <another object>\b. Conversely,
		you may want to \BTAKE <object> FROM\b or \BEMPTY\b objects.\n"
		Indent
		"A well-implemented game might cover lots of sensory verbs (like
		\BSMELL\b, \BTASTE\b, \BLISTEN TO\b, or even \BTOUCH\b), but that can't
		always be expected.\n"
		Indent
		"Here are some other common commands:\n"

		local a,b
		while true ! checking how many possible columns we can write
		{          ! (although we end up using, at max, 5 since 5 looks nice
			if (a*12 + INDENT_SIZE) >= display.linelength
			{
				break
			}
			a++
		}
		b = PrintCommands(a,b)
		b++
#ifset _VERBSTUB_G
		Indent
		"Less commonly, you might use the following:\n"
		PrintCommands(a,b)
#endif  ! VERBSTUB_G
		Indent
		"Hopefully, that gives you some ideas!"
		""
		CoolPause(1)
		}
}

routine PrintCommands(columns,start)
{
	local i
	local max = 5 ! -1
	if CheaporSimple ~= 2
		Font(BOLD_ON|PROP_OFF)

	if display.windowlines < (display.screenheight + 100)
		columns--
	while common_commands[start] ~= 0
	{
		print to (INDENT_SIZE + i++ * 12);
		print common_commands[start++];
		if i >= columns or common_commands[start] = 0 or i = max
		{
			i = 0
			""
		}
	}
		! old glk version code
!		while common_commands[start] ~= 0
!		{
!			print common_commands[start++];
!			if common_commands[start] ~= 0
!				print "\_      ";
!		}
	""
	Font(BOLD_OFF|PROP_ON)
	return start
}

option do_nothing "Doing nothing at all"
{
	in how_play
#ifset _ROODYLIB_H
	priority 5
#endif
	menu_text
		{
		Indent
		"Sometimes the best thing to do is to do nothing and watch how things
		play out. Since game time doesn't pass when one isn't typing commands,
		you have to type \BWAIT\b. This will automatically make 1-3 turns pass
		by, depending on what the game defaults to.\n"
		Indent
		"Alternatively, you can type \BWAIT <some number>\b to wait a specified
		number of turns. \BWAIT\b can be shortened to \"\BZ\b\"."
		""
		CoolPause(1)
		}
}

option saving_loading "Saving, restoring, and other \"meta\" commands"
{
	in how_play
#ifset _ROODYLIB_H
	priority 4
#endif
	menu_text
		{
		Indent
		"Some commands don't affect the game world itself. They exist solely for
		playing convenience. Some of them include-\n"
		Indent
		"\BSCORE\b- In a score-tracking game, this prints the current score."
		Indent
		"\BSAVE\b- Saves the game state to a file."
		Indent
		"\BRESTORE\b- Restores a saved game."
		Indent
		"\BQUIT\b- Ends a play session."
		Indent
		"\BRESTART\b- Restarts the game."
		Indent
		"\BUNDO\b- Undoes the previous turn."
		Indent
		"\BSCRIPT ON/OFF\b- Writes the prose (with commands) of your game to a
		text file."
		Indent
		"\BRECORD ON/OFF\b- Writes only \Icommands\i to a file."
		Indent
		"\BPLAYBACK\b- Inserts commands from a recording (see previous command)
		into the game."
		Indent
		"\BVERBOSE\b- Gives full description every time a room is entered."
		Indent
		"\BBRIEF\b- Full room description is only printed on the first visit (or
		when LOOK is called)."
		Indent
		"\BSUPERBRIEF\b- Short room descriptions every time."
		Indent
		"\BDISPLAY WIDE/TALL\b- Forces listings (such as inventory) to be wide
		or tall, depending.\n"
		Indent
		"Lastly, if you happen to misspell a word in a command, you can use the
		\BOOPS\b command to fix it:\n"
		Font(BOLD_ON)
		">X MEF"
		"You don't need to use the word \"mef.\"\n"
		">OOPS ME"
		"Looking good.\n"
		Font(BOLD_OFF)
		Indent
		"So, how about that?\n"
		CoolPause(1)
		}
}
#endif ! USE_DEFAULT_MENU

#endif ! _NEWMENU_H