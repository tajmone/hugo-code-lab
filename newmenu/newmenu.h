!::
! NEWMENU.H  Version 3.5 by Roody Yogurt
!::
!\

	For a nice overview of this contribution, check out:
	http://hugo.gerynarsabode.org/index.php?title=NewMenu.h

   version 3.5 - cleaned up some code, added automatic mouseclick support
	version 3.4 - got rid of the array usage for setting up menus (a legacy
	              from the original menu routines). now it's completely based
                 on object hierarchy. added some stuff to sort menus by priority
                 property the first time they are opened
	version 3.3 - changed some "press a key" stuff
	version 3.2 - Got rid of extra screen clears and other code cleaning
	version 3.1 - added alt_title property for option objects. if provided,
	              the name property is still used as a menu choice, but alt_title
	              is used to title the actual page.
	              added NEW_FUSE code
	              fixed ShowPage "returning to game" printed text
	              removed DESCFORM_I code
	              changed cheap code to be keypress-based, not input-based
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
	              Changed the default printing behavior of MenuPause
	              Changed text in the Special Commands page
	              MenuPause with top pause message now always one line window
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
	New in version 1.2- added "options_gap" property for menucategory objects.
	Controls spaces between "[N]ext item..." text and menu options, giving more
	configurability.

	New in version 1.1- added "title_gap" property to menucategory object. Put in
	a true value if you want that menu to have a blank line between the title and
	"[N]ext item..." text

This is an update to Kent Tessman's menu routine in the Hugo Library.
Differences in this version:

- Adds some "hidden" text so transcripts look better.
- Adds asterisk-highlighting when used with glk interpreters
- Adds support to cheapglk interpreters
- Has built-in support for Guilty Bastards style hints

As it is based on the original menu routine and Guilty Bastards hint routine,
this extension draws on those two things a fair amount so this thing couldn't
have been written without Kent's original work.

First things first-
Include this file after "hugolib.h".

This extension *expects* a menu_category object named main_menu. One is
provided by newmenu.h, but if you need to replace it, do the following:

replace main_menu "TITLE OF YOUR MAIN MENU"
{
	inherits menu_category
}

Any additional menus are of the menu_category class.

menu_category how_play "How To Play Interactive Fiction"
{
	in main_menu
	priority 8
}

Now, calling MakeMenu from a DoAbout or DoHelp routine will automaticaly bring
up that menu. (if you have several different menus that can be called by
different commands, call MakeMenu([other_menu_object]).

Now let's fill that menu with choices. For this, we use option objects.

option contact_choice "Contact"
{
	in main_menu    ! which menu it is
	menu_text
	{
		""
		"\_ Feel free to send me your thoughts and suggestions at
		roody.yogurt@gmail.com! What an amazing discourse we will have!"
		! MenuPause will be called automatically
	}
}

The menu_text property holds the text you want on the page. After the menu_text
property is printed, the MenuPause routine automatically prints a "press a key"
message and waits for a pause.  Change the MenuPause messages in NewMenuMessages
if you'd like to change the text.

You can change when an option is available by changing its option_available
value.
Of course, return true when you want it available:

option helicopter_choice "Whoa, a Helicopter!"
{
	in main_menu
	option_available
	{
		if helicopter is known
			return true
		else
			return false
	}
	menu_text
	{
		! helicopter text
	}
}

Since menu options are listed by object hierarchy, it's possible that you'll
occasionally want to rearrange them.  For this reason, options are given a
"priority" property.  Higher priority values are listed first.

If you want an option to lead to *another* menu, just put a menu_category
object in that menu_object:

menu_category submenu_choice "CHOICE LEADING TO SUBMENU"
{
	in main_menu
}

And then put option objects in that menu just as you would normally do.

You can even give that submenu a different title than its link.  The name
property automatically is used as the link text, but you can add an alt_name
propety:

menu_category submenu_choice "NAME OF LINK TO SUBMENU"
{
	alt_name "ACTUAL NAME OF SUBMENU"
	in main_menu
}

Hints!

So yeah, this thing also supports hints. Just make a hint menucategory object,
and fill it with hint_options like this:

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
#message "NewMenu.h Version 3.5"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include
it first!"
#endif
version_obj newmenu_version "NewMenu Version 3.5"
{
	in included_extensions
	desc_detail
		" by Kent Tessman and Roody Yogurt";
}
#endif

#ifset _ROODYLIB_H

object menulib "menu"
{
! if roodylib.h has been included before newmenu.h, nothing needs to be
! added to the init routine
	type settings
	in init_instructions
	execute
	{
		local i
#ifset USE_DEFAULT_MENU
		if word[LAST_TURN] ~= "undo"
		{
			for i in init_instructions
			{
				if &i.usage_desc
					break
			}
			if i
				special_choice.option_available = true
			else
				special_choice.option_available = false
		}
#endif
#ifset DEBUG
		if word[LAST_TURN] ~= "undo","restore"
		{
			for (i=(menu_category + 1);i<=objects ;i++ )
			{
				if i.type = menu_category and parent(i) = nothing and
				i ~= menu_category
					move i to menu_pages
			}
		}
#endif
		return i ! just to avoid a warning if DEBUG and DEFAULT_MENU are
		         ! turned off
	}
}
#endif ! _ROODYLIB_H

#ifset USE_DEFAULT_MENU
#if undefined usage_desc
property usage_desc alias short_desc ! some text describing what commands turn
                                     ! a library's function on/off
                                     ! (ex: CHEAPMODE ON)
#endif

#if undefined TOTAL_COMMANDS
constant TOTAL_COMMANDS 16
#endif

#ifset _ROODYLIB_H
array verblib_commands[TOTAL_COMMANDS] = "OPEN","CLOSE","LOCK","UNLOCK", \
	"WEAR", "REMOVE","TURN ON","TURN OFF", "DRINK",  "EAT", \
	"SIT" , "MOVE", "INSERT", "SEARCH"
#ifset _VERBSTUB_G
array verbstub_commands[TOTAL_COMMANDS] = "PUSH", "PULL", "YELL", "JUMP", \
	"THROW","SLEEP", "KISS", "WAVE", "CLIMB", "WAKE", "SWIM", \
	"DIG", "TIE", "BURN", "CUT"
#endif
#else
array verblib_commands[TOTAL_COMMANDS] = "OPEN","CLOSE","LOCK","UNLOCK", \
	"WEAR", "REMOVE","TURN ON","TURN OFF", "DRINK",  "EAT", \
	"SIT" , "MOVE", "INSERT"
#ifset _VERBSTUB_G
array verbstub_commands[TOTAL_COMMANDS] = "SEARCH","PUSH", "PULL", "YELL", \
"JUMP", "THROW","SLEEP", "KISS", "WAVE", "CLIMB", "WAKE", "SWIM", \
	"DIG", "TIE", "BURN", "CUT"
#endif
#endif

#endif ! USE_DEFAULT_MENU

attribute sorted alias special

! menu_category properties
property title_gap alias initial_desc
property options_gap alias reach
property page_text_color alias holding
property page_bg_color alias exclude_from_all
property title_color alias list_contents
property title_bg  alias capacity

! option/hint_option properties

property menu_text alias long_desc
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
property alt_title alias misc
property alt_name alias misc ! just because I think I'm going to forget the
                             ! name of the property from time to time

property option_available alias in_to
property hints_available alias in_to
property hints_revealed alias out_to
property priority alias parse_rank

#ifset DEBUG
object menu_pages
{}
#endif

class menu_category
{
	type menu_category
	title_gap  0 ! lines between menu title and "[N]ext key"
	options_gap 1 ! lines between "[N]ext key" and menu options
	option_available true
	page_bg_color 0
	page_text_color 0
	title_color 0
	title_bg 0
	priority 0
}

class option
{
	type option
	option_available true
	priority 0
}

option hint_option "(hint_option)"
{
	type hint_option
	menu_text
		Help_Hints(self)
	hints_available 1
	hints_revealed 0
}

routine MakeMenu(menu_title, width, recurse)
{
	local category, old_category, cos
 	local simple_port !, glktest

	cos = CheaporSimple
	if not menu_title
		menu_title = main_menu

	if not (display.windowlines > (display.screenheight + 100)) and system(61)
		simple_port = true

	if not recurse
	{
		if verbroutine = &EndGame
			""
		MenuOpen(menu_title)
	}

!	local revisit
   while true
	{
		if not cos ! and category ! revisit
			SetPageColors(menu_title)

		for category in menu_title
		{
			if category.option_available
			{
				if category.priority and menu_title is not sorted
					break
			}
		}

		if category ! c
			SortMenu(menu_title)

		if not (cos = 2 or simple_port)
		{
			window 0
			cls
		}
		category = Menu(menu_title, width, old_category)
		old_category = category
		if category
		{
			if category.type = menu_category
				MakeMenu(category,width,(recurse + 1))
			else
			{
				if not cos
				{
					SetPageColors(category)
					color MENU_TEXTCOLOR, MENU_BGCOLOR
				}
				do
				{
					if display.needs_repaint and cos ~= 2
					{
						color MENU_BGCOLOR, MENU_BGCOLOR
						MenuMessage(&MakeMenu,4) ! "[WINDOW RESIZED]"
						color TEXTCOLOR, BGCOLOR, INPUTCOLOR
						""
					}
					display.needs_repaint = false
					if not cos
						color MENU_BGCOLOR, MENU_BGCOLOR
					if cos ~= 2
						MenuMessage(&MakeMenu,3,category.name)
					color TEXTCOLOR, BGCOLOR, INPUTCOLOR
					if not (cos = 2 or simple_port)! simple_port
					{
						if category.alt_title
							CenterTitle(category.alt_title)
						else
							CenterTitle(category.name)
					}
					else
					{
						Indent
						print "\_  ";
						Font(BOLD_ON)
						if category.alt_title
							print category.alt_title
						else
							print category.name
						Font(BOLD_OFF)
						""
					}
					if not cos and display.hasvideo
						locate 1,TopPageMargin
					run category.menu_text
					print newline
					""
					if category.type ~= hint_option
						MenuPause(category)
				}
				while (display.needs_repaint = true  )
			}
		}
		else
		{
			if not recurse
			{
				if not (cos = 2 or simple_port)
				{
#ifset _ROODYLIB_H
					InitScreen
#else
					color TEXTCOLOR, BGCOLOR, INPUTCOLOR
					if not cos ! system(61)
						window 0
					cls
#endif
				}
				if not verbroutine ~= &EndGame
					AfterMenu
				else
					PrintStatusLine
			}
			return
		}
	}
}

routine MenuOpen(menu_title)
{
	if not (system(61) or CheapOrSimple = 2)
		color BGCOLOR, BGCOLOR
	MenuMessage(&MenuOpen,1,menu_title) ! "Opening menu..."
	""
	color TEXTCOLOR, BGCOLOR, INPUTCOLOR
}

routine AfterMenu
{
#ifset _ROODYLIB_H
#ifclear NO_ACCESSIBILITY
	if not (cheap & CHEAP_ON)
#endif
#endif
		PrintStatusline
	Font(DEFAULT_FONT) ! just in case
	MenuMessage(&AfterMenu,1) ! "...and now returning to the story."
#ifset _ROODYLIB_H
	if (FORMAT & DESCFORM_I)
		""
#endif
	DescribePlace(location, true)
#ifset NEW_FUSE
	fake_runevents
#endif
}

routine TopPageMargin
{
	if not display.hasvideo
		return display.windowlines
	else
		return 2
}

!replace Menu(menu_par,num, width, selection)
replace Menu(menu_par, width, selection)
{
	local i, column, cos, mouseclick, titlegap, optionsgap, num
	local simple_port, glktest,n
	local sel = 1

	glktest = display.windowlines > (display.screenheight + 100)
	simple_port = not glktest and system(61)
	cos = CheapOrSimple
	titlegap = menu_par.title_gap
	optionsgap = menu_par.options_gap

	if system(61)
	{
		titlegap = 0
		optionsgap = 1
	}
	if width = 0:  width = 20
	for i in menu_par
	{
		if i.option_available
		{
			num++
			n = string(_temp_string,i.name)
			if n > width : width = n
		}
	}

	if width > (display.linelength-1):  width = display.linelength-1

	! Default selection is first child if not otherwise given
	if selection = 0:  selection = child(menu_par)

	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
	while true
	{
		if not cos ! system(61)
		{
			color MENU_BGCOLOR, MENU_BGCOLOR
			if display.needs_repaint
			{
				window 0
				display.needs_repaint = false
			}
			cls
		}
		if cos = 2 or
		( glktest and
		( num + 5 + titlegap + optionsgap ) >= (display.screenheight/3*2))
			return CheapMenu(menu_par,num,cos,simple_port)
		column = display.linelength/2 - width/2
		window ( num + 5 + titlegap + optionsgap )
		{
			if glktest ! system(61) and not simple_port
				cls

			! This section "fakes" CenterTitle, so it appears to be
			! a regular title window, when in actuality, the whole menu
			! is in the status window
			if not simple_port
				locate 1,1
			Font(PROP_OFF)
			n = string(_temp_string, menu_par.name)
			color MENU_SELECTCOLOR, MENU_SELECTBGCOLOR ! shouldn't affect glk
			print to (display.linelength/2 - n/2);
			print menu_par.name;
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

			for (i=0; i<optionsgap;i++)
			{
				""
			}

			sel = 1
			i = child(menu_par)
			while i
			{
				if i.option_available
				{
					if i = selection
					{
						if system(61) ! glk or minimum port
							print to (column - 2);"* ";
						else
							locate (column + 1), display.cursor_row
						color MENU_SELECTCOLOR, MENU_SELECTBGCOLOR  ! shouldn't affect
																				  ! glk?
						print i.name; to (column+width)
						color MENU_TEXTCOLOR, MENU_BGCOLOR
					}
					else
					{
						locate (column + 1), display.cursor_row !print to column;
						print i.name ; to (column+width)
					}
					sel++
				}
				i = younger(i)
			}
		}
		word[0] = PressKey
		if word[0] = MOUSE_CLICK and display.pointer_x >= column and
					display.pointer_x <= column+width
		{
			mouseclick = (num + 1) + display.pointer_y
			i = PickNthChild(mouseclick, menu_par, &IsOptionAvailable )
			if i.name
			{
					selection = i
					word[0] = ENTER_KEY
			}
			mouseclick = 0
		}
		elseif word[0] = MOUSE_CLICK
		{
			if (display.pointer_y + num + 5 + titlegap + optionsgap ) = 3 + titlegap
			{
				if display.pointer_x >= (display.screenwidth - 10)
					word[0] = ESCAPE_KEY
				elseif display.pointer_x <= 11
					word[0] = 'n'
			}
			elseif (display.pointer_y + num + 5 + titlegap + optionsgap ) = 4 + titlegap
			{
				if display.pointer_x >= (display.screenwidth - 16)
					word[0] = ENTER_KEY
				elseif display.pointer_x <= 15
					word[0] = 'p'
			}
		}
		if word[0] >= '0' and word[0] <= '9'
		{
			i = word[0] - '0'
			if i = 0:  i = 10
			if i <= num
			{
				i = PickNthChild(i, menu_par, &IsOptionAvailable )
				if i.name
					selection = i
			}
		}
		select word[0]
			case 'N', 'n', DOWN_ARROW, RIGHT_ARROW
			{
				while true
				{
					if not younger(selection)
						selection = child(menu_par)
					else
						selection = younger(selection)
					if selection.option_available and selection.name ~= ""
						break
				}
			}
			case 'P', 'p', UP_ARROW, LEFT_ARROW
			{
				while true
				{
					if not elder(selection)
						selection = youngest(menu_par)
					else
						selection = elder(selection)
					if selection.option_available and selection.name ~= ""
						break
				}
			}
			case 'Q', 'q', ESCAPE_KEY
			{
				if not cos ! system(61)
				{
					window !0
					{cls}
				}
				return 0
			}
			case ENTER_KEY
			{
				if not cos ! system(61)
				{
					window !0
					{cls}
				}
				return selection
			}
	}
!	}
}
#if undefined PickNthChild
routine PickNthChild(num, par, qualify_routine)
{
	local x, n, qual
	x = child(par)
	while x
	{
		if qualify_routine
		{
			qual = call qualify_routine(x)
			if qual
				n++
		}
		else
			n++
		if n = num
			return x
		x = younger(x)
	}
	return false
}
#endif

routine IsOptionAvailable(obj)
{
	if obj.option_available
		return true
	return false
}

routine CheapMenu(menu_par,num,cos,simple_port)
{
	Font(PROP_ON)
	local sel,i
	while true
	{
		if not (cos = 2 or simple_port)
			cls
		if not simple_port
			CenterTitle(menu_par.name)
		if display.needs_repaint
			display.needs_repaint = false
		print newline
		Indent
		print "\_  ";
		Font(BOLD_ON)
		print menu_par.name
		Font(BOLD_OFF)
		""
		sel = 1
		i = child(menu_par)
		while i
		{
			if i.option_available
			{
				Indent
				print number sel++;". ";
				print i.name
			}
			i = younger(i)
		}
		print ""
		MenuMessage(&Menu, 2)		! "Select the number of your choice"
		if num > 9
		{
			GetInput
			if word[1] = "q"
			{
				""
				return 0
			}
			if word[1]
				sel = StringToNumber(word[1])
			else
				sel = StringToNumber(parse$)
		}
		else
		{
			pause
			if word[0] = 'q','Q', '0', ESCAPE_KEY
			{
				printchar word[0]
				print newline
				if not (cos = 2 or simple_port)
					cls
				""
				return 0
			}
			else
				sel = word[0] - 48
		}
		if num <= 9
		{
			printchar word[0]
			print newline
		}
		""
		if sel > 0 and (sel <= num) ! sel)
		{
			if not (cos = 2 or simple_port)
				cls
			return PickNthChild(sel, menu_par, &IsOptionAvailable)
		}
		else
			"Not a valid option.\n"
	}
}

routine MenuPause(page,no_newline)
{
	local simple_port, cos
	simple_port = not (display.windowlines > (display.screenheight + 100)) and system(61)
	cos = CheapOrSimple
	Indent
	if (cos = 2 or simple_port)
		MenuMessage(&MenuPause,1,page) ! "[PRESS A KEY TO CONTINUE]"
	else
		MenuMessage(&MenuPause,2,page) ! "\Ipress a key to continue\i"
	PressKey
	Font(DEFAULT_FONT)
	if not no_newline
		""
}

routine CheaporSimple
{
#ifset _ROODYLIB_H
#ifclear NO_ACCESSIBILITY
	if cheap
		return 2
#endif
#endif
	if system(61)
		return 3
}

routine Help_Hints(obj)
{
	local i
	for (; i<=obj.hints_revealed; i++)
	{
		if i
			""
		run obj.(hint1+i)
		print newline
	}
	""
	while &obj.(hint1+i) ~= 0 ! i.e., no more topics
	{
		Font(BOLD_ON)
		MenuMessage(&Help_Hints,1) ! "[Press 'H' for another hint, or 'Q' to
										!	quit]";
		Font(BOLD_OFF)
		print newline
		word[0] = PressKey
		if word[0] = 'Q', 'q', ESCAPE_KEY
			return
		if word[0] = 'H', 'h'
		{
			obj.hints_revealed++
			run obj.(hint1+i++)
			print newline
			""
		}
	}
	Font(BOLD_ON)
	if CheapOrSimple ~= 2
	{
		MenuMessage(&Help_Hints,2) ! "[No more hints.  Press any
	                           !  key...]";
	Font(BOLD_OFF)
	PressKey
	}
	else
	{
		MenuMessage(&Help_Hints,3) ! "[No more hints.  Leaving page.]";
		Font(BOLD_OFF)
	}

	return
}


routine PressKey
{
	local key
	key = system(11) ! READ_KEY
	if not system_status
		system(32) ! PAUSE_100TH_SECOND
	if system_status or CheaporSimple ! system(61) ! MINIMAL_INTERFACE
	{
		pause
		key = word[0]
	}
	else
	{
		while true
		{
			key = system(11) ! READ_KEY
			system(32) ! PAUSE_100TH_SECOND
			if key:break
		}
	}

	return key
}


routine ShowPage(page)
{
 	local simple_port,glktest, reset_indent, cos
	if display.windowlines > (display.screenheight + 100)
		glktest = true
	cos = CheapOrSimple ! so we only call it once
	if not glktest and system(61)
		simple_port = true
	if not cos ! (CheaporSimple = 2 or system(61))
		window 0
	if verbroutine = &EndGame
		""
	if not (cos = 2 or simple_port)
	{
		color BGCOLOR, BGCOLOR
		MenuMessage(&ShowPage,1,page) ! "[OPENING PAGE]"
		""
		cls
	}
	do
	{
		if display.needs_repaint and cos ~= 2
		{
			color BGCOLOR, BGCOLOR
			MenuMessage(&MakeMenu,4) ! "[WINDOW RESIZED]"
			""
		}
		display.needs_repaint = false
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		if not (cos = 2 or simple_port)
		{
			if page.alt_title
				CenterTitle(page.alt_title)
			else
				CenterTitle(page.name)
		}
		if not cos and display.hasvideo
			locate 1,TopPageMargin
		elseif cos = 2
		{
			if not (FORMAT & NOINDENT_F)
			{
				reset_indent = true
				FORMAT = FORMAT | NOINDENT_F
			}
		}
		run page.menu_text
		if reset_indent
			FORMAT = FORMAT & ~NOINDENT_F
		print newline
		if cos ~= 2 and
			not (page.type = hint_option and verbroutine = &EndGame) ! or page.type = hint_option)
			""
		if page.type ~= hint_option and not (CheaporSimple = 2 or simple_port)
			MenuPause(page,(verbroutine = &EndGame))
	}
	while (display.needs_repaint = true  )

	if not (cos = 2 or simple_port)
	{
#ifset _ROODYLIB_H
		InitScreen
#else
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		if not cos ! system(61)
			window 0
		cls
#endif
	}
	if verbroutine ~= &EndGame and
		not (cos = 2 or simple_port) ! and page.type ~= hint_option)
		AfterMenu
	else
		PrintStatusLine
}

routine SetPageColors(page)
{
	if not (MENU_TEXTCOLOR or MENU_BGCOLOR) and ! must not have been set
		not (main_menu.page_text_color or main_menu.page_bg_color)
	{
		MENU_TEXTCOLOR = TEXTCOLOR
		MENU_BGCOLOR = BGCOLOR
	}

	if not( MENU_SELECTCOLOR or MENU_SELECTBGCOLOR) and
		not (main_menu.title_color or main_menu.title_bg)
	{
		MENU_SELECTCOLOR = SL_TEXTCOLOR
		MENU_SELECTBGCOLOR = SL_BGCOLOR
	}
	if not (main_menu.page_text_color or main_menu.page_bg_color)
	{
		main_menu.page_text_color = MENU_TEXTCOLOR
		main_menu.page_bg_color = MENU_BGCOLOR
	}
	if not (main_menu.title_color or main_menu.title_bg)
	{
		main_menu.title_color = MENU_SELECTCOLOR
		main_menu.title_bg = MENU_SELECTBGCOLOR
	}
	if page.page_text_color or page.page_bg_color
	{
		MENU_TEXTCOLOR = page.page_text_color
		MENU_BGCOLOR = page.page_bg_color
	}
	else
	{
		MENU_TEXTCOLOR = main_menu.page_text_color
		MENU_BGCOLOR = main_menu.page_bg_color
	}
	if page.title_color or page.title_bg
	{
		MENU_SELECTCOLOR = page.title_color
		MENU_SELECTBGCOLOR = page.title_bg
	}
	else
	{
		MENU_SELECTCOLOR = main_menu.title_color
		MENU_SELECTBGCOLOR = main_menu.title_bg
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
				case 3
				{
!					local l,i
!					l = string(_temp_string, a)
!					print to (40 - l/2-1);
!					print "+";
!					for (i=1;i<=l ;i++ )
!					{
!						print "-";
!					}
!					print "+"
!					print to (40 - l/2-1);
!					print "|";
!					print a;
!					print "|"
!					print to (40 - l/2 - 1);
!					print "+";
!					for (i=1;i<=l ;i++ )
!					{
!						print "-";
!					}
!					print "+"
!\ The previous code is this experimental thing I wrote to make transcripts
look nicer, but it looks kind of crappy in scrollback (which is shown in
proportional font). Out for now. \!
!					print "[";a;"]"
					Indent
					print "\_ "; a
					""
				}
				case 4 : "Window resized. Redrawing screen."
				case 5 : "[LEAVING MENU]"
				case 6 : "Opening the menu..."
		}
		case &Menu
		{
			select num
				case 1
				{
					if system(61)
						locate 2, display.cursor_row
					else
						locate 1, display.cursor_row
					print "[N]ext item"; to (display.linelength - 11); \
		    				"[Q]uit menu"
					if system(61)
						locate 2, display.cursor_row
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
		case &MenuOpen
		{
			select num
				case 1:"Opening menu..."
		}
		case &MenuPause
		{
			select num
				case 1  ! default cheapglk "press a key"
					"[PRESS A KEY TO CONTINUE]" ! ;
				case 2  ! default normal "press a key"
					"\Ipress a key to continue\i" ! ;
		}
		case &Help_Hints
		{
			select num
				case 1
					"[Press 'H' for another hint, or 'Q' to quit]"; !;
				case 2
					print "[No more hints.  Press any key...]";
				case 3
					print "[No more hints.  Leaving page.]";
		}
		case &ShowPage
		{
			select num
				case 1
				{
!					"[OPENING PAGE]"
					if a.alt_name
						print "(Opening \""; a.alt_name;"\")"
					else
						print "(Opening \""; a.name;"\")"
				}
				case 2
					"[CLOSING PAGE]"
		}
		case &AfterMenu
		{
			select num
				case 1: print "\I...and now returning to the story.\i"
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

!object menu_bowl
!{}

routine SortMenu(menu_title)
{
	local resort,x, prio, old_prio = 200
	x = child(menu_title)
	while x
	{
		if x.priority
			prio = x.priority
		if prio > old_prio
		{
			resort = true
			break
		}
		old_prio = prio
		x = younger(x)
	}
	if not resort
		return
	while child(menu_title)
	{
		move child(menu_title) to display
	}
	while child(display)
	{
		prio = 0
		x = child(display)
		while x
		{
			if x.priority and x.priority > prio
				 prio = x.priority
			x = younger(x)
		}
		x = child(display)
		if prio
		{
			while x
			{
				if prio = x.priority
				{
					move x to menu_title
					break
				}
				x = younger(x)
			}
		}
		else
		{
			while x
			{
				move child(display) to menu_title
				x = child(display)
			}
		}
	}
	menu_title is sorted
	return true ! means we resorted
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
	priority 10
	menu_text
	{
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
		Indent
		"Also! If you are a screenwriter or would like to support
		Kent Tessman in his current endeavors, check out his fantastic
		program, Fade In Professional Screenwriting Software ®
		(\Bhttp://www.fadeinpro.com\b)!"
	}
}

menu_category how_play "How To Play Interactive Fiction"
{
	in main_menu
	priority 8
}

option where_choice "Where To Get More IF"
{
	in main_menu
	priority 6
	menu_text
	{
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
		Indent
		"The IF Database (\Ihttp://ifdb.tads.org/\i)"
		Indent
		"IF Reviews (\Ihttp://www.ifreviews.org/\i)"
		Indent
		"The IF Archive (\Ihttp://ifarchive.org/\i)"

!		"\Ihttp://www.joltcountry.com/phpBB2/viewforum.php?f=8\i\n"
	}
}

#ifset _ROODYLIB_H ! Only roodylib-using games can do Special Commands
option special_choice "Special Commands"
{
	in main_menu ! (should be added to main_menu by menulib object
	menu_text
	{
#if set NO_ACCESSIBILITY
		local cheap
#endif
		if not cheap
		{
			Indent
			"\BAdditional commands:\b\n"
		}
		SpecialCommands
	}
}

routine SpecialCommands
{
	local i, sum, nl, skip
#ifclear NO_ACCESSIBILITY
	skip = cheaplib
#endif
	i = child(init_instructions)
	while i
	{
		if &i.usage_desc and i ~= skip
		{
			if nl
			{
				""
				nl = false
			}
			if i.usage_desc
			{
				nl = true
!				if i ~= youngest(init_instructions)
!					""
				sum++
			}
		}
		i = younger(i)
	}
	if skip
	{
!		Indent
!		Font(BOLD_ON)
!		if sum
!		{
!			print "\nAdditionally, if you are playing with a screen reader or
!			simple console window interpreter, the following commands may help:"
!		}
!		else
!			print "The following commands exist to help gameplay with screen
!			readers and simple console window interpreters:"
!		Font(BOLD_OFF)
		if nl
			""
		run skip.usage_desc
		sum++
	}
	if not sum
	{
		Indent
		"Sorry, it appears that there are no special features supported by your
		interpreter. Try the official Hugo interpreter or Hugor TODAY!"
	}
}
#endif

option what_is "What is \"interactive fiction\"?"
{
	in how_play
	priority 10
	menu_text
	{
		Indent
		"In interactive fiction, you, the player, will be appraised of
		your surroundings and happenings in the game by largely textual means.
		You get the chance to interact with these surroundings by typing
		commands (read the other entries in this menu for examples of the kind
		of commands that are accepted). Hopefully, you are rewarded for your
		efforts with engaging puzzles and entertaining prose!"
	}
}

option move_around "Moving around"
{
	in how_play
	priority 8
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

	}
}

option look_around "Looking around"
{
	in how_play
	priority 6
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
	}
}

option talk_chars "Talking to characters"
{
	in how_play
	priority 4
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
	}
}

option manip_objects "Manipulating objects"
{
	in how_play
	priority 2
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

		local a ! ,b
		while true ! checking how many possible columns we can write
		{          ! (although we end up using, at max, 5 since 5 looks nice
			if (a*12 + INDENT_SIZE) >= display.linelength
			{
				break
			}
			a++
		}
		PrintCommands(verblib_commands,a)
!		b++
#ifset _VERBSTUB_G
		Indent
		"Less commonly, you might use the following:\n"
		PrintCommands(verbstub_commands,a)
#endif  ! VERBSTUB_G
		Indent
		"Hopefully, that gives you some ideas!"
	}
}

routine PrintCommands(arr,columns)
{
	local i, n
	local max = 5 ! -1
	if CheaporSimple ~= 2
		Font(BOLD_ON|PROP_OFF)

	if display.windowlines < (display.screenheight + 100)
		columns--
	while array arr[n] ~= 0 and n < (array arr[])
	{
		print to (INDENT_SIZE + i++ * 12);
		print array arr[n++];
		if i >= columns or array arr[n] = 0 or i = max
		{
			i = 0
			""
		}
	}
	""
	Font(BOLD_OFF|PROP_ON)
!	return start
}

option do_nothing "Doing nothing at all"
{
	in how_play
	priority 0
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
	}
}

#ifclear NO_XVERBS
option saving_loading "Saving, restoring, and other \"meta\" commands"
{
	in how_play
	priority -2
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
		"So, how about that?"
	}
}
#endif ! ifclear NO_XVERBS
#endif ! USE_DEFAULT_MENU

#endif ! _NEWMENU_H