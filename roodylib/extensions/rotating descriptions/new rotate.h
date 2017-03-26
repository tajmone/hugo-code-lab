! Examples:
!door red_door "red door"
!{
!	noun "door"
!	adjective "red"
!	article "the"
!	rotations 0
!	long_desc
!		Rotate
!	rotate_desc "It's a door." "It's still a door." "Stop looking at it." \
!		"I mean it." "I really do."
!	between startlocation east_room
!	!is recycle  ! uncomment if you want descs to cycle
!}
!
!object rock "rock"
!{
!	article "a"
!	noun "rock"
!	in STARTLOCATION
!	long_desc
!		RandomDesc
!	random_desc "A rock." "2nd rock desc." "3rd rock." "4th rock."
!	rotations 0
!}

property rotations ! objects with rotating descriptions need a property to hold
                   ! a value
attribute recycle ! give this attribute to cycle back to the beginning when end
                  ! is reached
property rotate_desc ! holds descriptions to rotate
property random_desc alias rotate_desc

routine Rotate(obj)
{
	if not obj
		obj = object
	local n
	obj.rotations += 1
	n = obj.rotations
	print obj.rotate_desc #n
	if n = obj.#rotate_desc
	{
		if obj is recycle
			obj.rotations = 0
		else
			obj.rotations -= 1
	}
}

!\ I sometimes like random descriptions, too, so I threw in a random description
   version \!
routine RandomDesc(obj)
{
	local x,n
	if not obj
		obj = object
	x = obj.#random_desc
	do
		n = random(x)
	while (n = object.rotations)
	object.rotations = n
	print obj.random_desc #n
}