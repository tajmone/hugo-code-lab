!----------------------------------------------------------------------------
!* "OPPORTUNITY" SYSTEM
!----------------------------------------------------------------------------

! Roody's note: Sometimes, it's cute to have special responces to actions or
! words right after something has happened.  This system is just a simple
! way of setting up for games that do a lot of this.

global current_opp ! current opportunity

! A fuse so that each question is only answerable for one turn
! opp_window = short for "opportunity window"
fuse opp_window "(opp_window fuse)"
{}

event opp_window
{
	if not self.tick
		current_opp = 0
}

class Question ! for Ballyhoo "egress" type questions
{
	type question
}

routine NewQuestion(new_question)
{
	current_opp = new_question
	if not new_question.long_desc
		print new_question.name
	Activate (opp_window, 2) ! Starts the new timer
}

routine NewOpp(opportunity)
{
	current_opp = opportunity
	Activate (opp_window, 2)
}

class opp
{}