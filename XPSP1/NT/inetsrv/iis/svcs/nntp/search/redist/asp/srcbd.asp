<% Response.Expires = 0 %>


<HTML>
<HEAD></HEAD>

<% if Instr(Request.ServerVariables("HTTP_USER_AGENT"),"MSIE") then %>

<FRAMESET COLS="190,*" FRAMEBORDER=0 FRAMESPACING=1>
	<FRAME SRC="srcmenu.asp" NAME="menu" FRAMEBORDER=1 FRAMESPACING=0 SCROLLING="no">
	<FRAME SRC="srcdflt.asp" NAME="text" FRAMEBORDER=0 FRAMESPACING=1>

</FRAMESET>

<% else %>

<FRAMESET COLS="190,*" BORDER=0 FRAMESPACING = 0 >
	<FRAME SRC="srcmenu.asp" NAME="menu" BORDER=0 FRAMESPACING=0 MARGINHEIGHT=0 MARGINWIDTH=0 SCROLLING="no">
	<FRAME SRC="srcdflt.asp" NAME="text" FRAMEBORDER=0 FRAMESPACING=1 MARGINHEIGHT=0 MARGINWIDTH=0>
			
</FRAMESET>

<% end if %>

</HTML>