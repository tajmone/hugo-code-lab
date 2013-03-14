!::

! CONFIGURATION FILE MANAGER
!::

!\
	Include this file in your game before other configuration-file-writing
	extensions. It's also good to #set USE_CONFIG_SYSTEM, although this file
	will do that automatically.

	Also declare a DATA_FILE constant. This is the data file that will be
	created:

	constant DATA_FILE "nddata" ! creates a "nddata file"

\!
#ifclear CONFIG_SYS_H
#set CONFIG_SYS_H

#ifclear USE_CONFIG_SYSTEM
#set USE_CONFIG_SYSTEM
#endif

#ifset VERSIONS
#message "Config_sys.h Version 1.0"
#endif

#ifclear _ROODYLIB_H
#message error "Config_Sys requires \"roodylib.h\". Be sure to include it first!"
#endif

#ifset USE_EXTENSION_CREDITING
version_obj config_sys_version "Config_Sys.h Version 1.0"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

!\ FILE_CHECK_START - a "file start" version of FILE_CHECK. Is the first thing
written to a data file, so LoadSettings can check for it and cut out early if
it's not there.
\!
constant FILE_CHECK_START     4659

!\ configlib is an init_instructions object that calls each configuration file's
setup property. This allows pre-game questions like, "Do you want color?" and so
on. Have the setup property return true if InitScreen should be called
afterward.

	It also goes through all of the configuration objects and determines their
	names' StringSum values, saving them in their name_sum properties.
\!

object configlib
{
	in init_instructions
	execute
	{
		local i, l
		if not CheckWordSetting("undo") and not CheckWordSetting("restore")
		{
			for i in config_instructions
			{
				l = string(_temp_string, i.name)
				i.name_sum = StringSum(_temp_string)
			}
		}
		if not CheckWordSetting("undo")
		{
			if not system(62)
			{
				LoadSettings
				if not CheckWordSetting("restore")
				{
					for i in config_instructions
					{
						if i.setup
						{
							InitScreen
						}
					}
					SaveSettings
				}
			}
		}
	}
}

!\ config_instructions is an object for holding all of the configuration file
objects \!

object config_instructions
{}

routine LoadSettings
{
	readfile DATA_FILE
	{
		local a, i
		while true
		{
			a = readval
			if a++ ~= FILE_CHECK_START
			{
				break
			}
			for i in config_instructions
			{
				a = readval
				if a ~= i.name_sum
				{
					break
				}
				else
					run i.load_info
			}
			a = readval
			break
		}
		if a ~= FILE_CHECK
		{
			return false
		}
		else
			return true
	}
}

routine SaveSettings
{
	writefile DATA_FILE
	{
		local i
		writeval FILE_CHECK_START
		for i in config_instructions
		{
			writeval i.name_sum
			run i.save_info
		}
		writeval FILE_CHECK
	}
}

property name_sum alias n_to
property load_info alias ne_to
! A save_info property already exists, thanks to RoodyLib
!property save_info alias e_to
property setup alias se_to
property first_time alias s_to

!\ StringSum adds up the ASCII value of a string. This extension uses
it to differentiate one configuration file object from another. \!

routine StringSum(arr)
{
	local i, sum

	while (array arr[i] and i < array arr[])
	{
		sum += array arr[i]
		i++
	}

	return sum
}

#endif ! CONFIG_SYS_H