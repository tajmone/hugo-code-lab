!----------------------------------------------------------------------------
!* EXTRA STRING MANIPULATION ROUTINES
!----------------------------------------------------------------------------

! Some string manipulation routines

! CapitalizeEveryFirstLetter - Capitalizes every first letter of a string array
! (specify a "newarray" array if you want the results copied to a different
!  array than the source)
routine CapitalizeEveryFirstLetter(stringarray,newarray)
{
	local len, i, new, finalarray
	finalarray = stringarray

	if newarray
		finalarray = newarray

	len = StringLength(array stringarray)
	for (i=0; i<len; i=i+1) ! the actual case-conversion loop
	{
		if new or i=0
		{
			if array stringarray[i] >= 'a' and array stringarray[i] <= 'z'
				array finalarray[i] = array stringarray[i] - ('a'-'A')
			new = 0
		}
		elseif array stringarray[i] >= 'A' and array stringarray[i] <= 'Z'
			array finalarray[i] = array stringarray[i] + ('a'-'A')
		else
			array finalarray[i] = array stringarray[i]
		if array stringarray[i] = ' '          ! a space
			new = 1
	}
	array finalarray[i] = 0
}

! Convert To All Caps - Capitalizes every letter of a string array
! (specify a "newarray" array if you want the results copied to a different
!  array than the source)
routine ConvertToAllCaps(stringarray,newarray)
{
	local len, i, finalarray
	finalarray = stringarray
	if newarray
		finalarray = newarray

	len = StringLength(array stringarray)
	for (i=0; i<len; i=i+1) ! the actual case-conversion loop
	{
		if array stringarray[i] >= 'a' and array stringarray[i] <= 'z'
			array finalarray[i] = array stringarray[i] - ('a'-'A')
		else
			array finalarray[i] = array stringarray[i]
	}
	array finalarray[i] = 0
}

! Lowercase- a routine for changing every letter in a string array to
! lowercase
! (specify a "newarray" array if you want the results copied to a different
!  array than the source)
routine Lowercase(stringarray,newarray)
{
	local len, i, finalarray
	finalarray = stringarray
	if newarray
		finalarray = newarray

	len = StringLength(array stringarray)
	for (i=0; i<len; i=i+1) ! the actual case-conversion loop
	{
		if array stringarray[i] >= 'A' and array stringarray[i] <= 'Z'
			array finalarray[i] = array stringarray[i] + ('a'-'A')
		else
			array finalarray[i] = array stringarray[i]
	}
	array finalarray[i] = 0
}

! StringAddTo - Adds second string array to first string array, with optional
! space between them
! (specify a "newarray" array if you want the results copied to a different
!  array than the original array)
routine StringAddTo(original_array, addition_array, add_space,newarray)
{
	local f
	if newarray
	{
		text to (array newarray)
		StringPrint(original_array)
	}
	else
	{
		f = StringLength(array original_array)
		text to (array original_array + f)
	}
	if (add_space)
		print " ";
	StringPrint(array addition_array)
	text to 0
}
