! converse.g - grammar for topic-based conversation system
! Version 1.1, Copyright 2001 by Christopher Tate <ctate@acm.org>
!
! Also needs converse.h for supplying the verb routines


#ifset VERSIONS
#message "NewConverse.g V 1.1"
#endif

verb "topic", "subject", "t"
	*								DoTopic
	* anything					DoTopic

verb "1", "2", "3", "4", "5"
	*								DoConverse


verb "end"
	*						DoVague

verb "stop", "end"
	* "talking"				DoGoodbye
	* "conversation"			DoStopTalk

verb "shut"
	* "up"					DoGoodBye

verb "goodbye", "farewell"
	*                                                       DoGoodbye
	* "~and" object                                         DoGoodbye
	* living                                                DoGoodbye

verb "0"
	*						DoStopTalk

verb "topics"
	*								DoTopics

verb "greet", "greetings"
	*                                                       DoHello
	* "~and" object                                         DoHello
	* living                                                DoHello

verb "ask", "question", "consult"
	*                                                       DoAsk
	* "about" anything                                      DoAskQuestion
	* (PrintConverseUsage) "about" anything                               DoAsk
	* (PrintConverseUsage)                                                DoAsk

verb "talk", "speak"
	*                                                       DoTalk
	* "to" living                                          DoTalk
	* "to" (PrintConverseUsage) "about" anything                          DoTalk
	* "about" anything                                      DoAskQuestion

verb "tell"
	* "me" "about" anything                                 DoAskQuestion
	* (PrintConverseUsage) "about" anything                               DoTell

verb "explain"
	* "to" "me" "about" anything                            DoAskQuestion
	* "to" (PrintConverseUsage) "about" anything                          DoTell

verb "what"
	* "is"/"about" anything                                 DoAskQuestion

verb "who"
	* "is" anything                                         DoAskQuestion
