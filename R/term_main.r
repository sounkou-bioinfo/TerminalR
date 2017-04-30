#######################################################################
#
# Package Name: TerminalR
# Description:
#   R Interface to the ncurses library for terminal user interfaces
#
# TerminalR R package, R package for text-based user interfaces
# Copyright (C) 2016-2017   Xiuwen Zheng
# All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#


##########################################################################
#
#
#

crtInitScreen <- function()
{
	.Call(nc_initscr)
	invisible()
}

crtDoneScreen <- function()
{
	.Call(nc_endwin)
	invisible()
}

crtUsed <- function()
{
	.Call(nc_get_used)
}

crtGetWinMax <- function(win=0L)
{
	.Call(nc_getmaxyx, win)
}

crtSetScroll <- function(win=0L, ok=FALSE)
{
	.Call(nc_setscrollok, win, ok)
	invisible()
}

crtMove <- function(win=0L, y, x, fail=FALSE)
{
	.Call(nc_wmove, win, y, x, fail)
	invisible()
}

crtPutText <- function(win=0L, txt, col=integer(), fail=FALSE)
{
	stopifnot(is.character(txt))
	stopifnot(is.numeric(col) | is.character(col))
	if (is.character(col))
		col <- crtColor(col)
	.Call(nc_wprintw, win, txt, col, fail)
	invisible()
}

crtErase <- function(win=0L)
{
	.Call(nc_erase, win)
	invisible()
}

crtRefresh <- function(win=0L)
{
	.Call(nc_refresh, win)
	invisible()
}

crtSetColor <- function(win=0L, col)
{
	stopifnot(is.numeric(col), length(col)==1L)
	.Call(nc_setcolor, win, col)
	invisible()
}



##########################################################################

COL_ATTR_NORMAL     <- 0L
COL_ATTR_STANDOUT   <- 0L
COL_ATTR_UNDERSCORE <- 0L
COL_ATTR_REVERSE    <- 0L
COL_ATTR_BLINK      <- 0L
COL_ATTR_DIM        <- 0L
COL_ATTR_BOLD       <- 0L
COL_ATTR_PROTECT    <- 0L
COL_ATTR_INVIS      <- 0L

COL_WHITE      <- 0L
COL_RED        <- 0L
COL_GREEN      <- 0L
COL_YELLOW     <- 0L
COL_BLUE       <- 0L
COL_CYAN       <- 0L
COL_MAGENTA    <- 0L
COL_GRAY       <- 0L


crtColor <- function(col, attr=COL_ATTR_NORMAL)
{
	stopifnot(is.character(col) | is.numeric(col))
	stopifnot(is.numeric(attr))
	if (is.character(col))
	{
		col <- match(col, c("white", "red", "green", "yellow", "blue",
			"cyan", "magenta", "gray")) - 1L
	}
	.Call(nc_col_attr, as.integer(col), as.integer(attr))
}



##########################################################################

KEY_TAB   <- 0x0009
KEY_ENTER <- 0x000A
KEY_ESC   <- 0x001B
KEY_SPACE <- 0x0020

KEY_DOWN  <- 0L
KEY_UP    <- 0L
KEY_LEFT  <- 0L
KEY_RIGHT <- 0L

KEY_RESIZE <- 0L

crtGetKey <- function()
{
	.Call(nc_getch)
}



#######################################################################

set_int <- function(obj, idx, val)
{
	.Call(nc_setint, obj, idx, val)
}

set_logical <- function(obj, idx, val)
{
	.Call(nc_setlogical, obj, idx, val)
}

set_real <- function(obj, idx, val)
{
	.Call(nc_setreal, obj, idx, val)
}

set_list <- function(obj, idx, val)
{
	.Call(nc_setlist, obj, idx, val)
}



#######################################################################
# Export stardard R library function(s)
#######################################################################

.onAttach <- function(lib, pkg)
{
	.Call(nc_init,
		list(COL_ATTR_NORMAL, COL_ATTR_STANDOUT, COL_ATTR_UNDERSCORE,
			COL_ATTR_REVERSE, COL_ATTR_BLINK, COL_ATTR_DIM, COL_ATTR_BOLD,
			COL_ATTR_PROTECT, COL_ATTR_INVIS),
		list(COL_WHITE, COL_RED, COL_GREEN, COL_YELLOW, COL_BLUE, COL_CYAN,
			COL_MAGENTA, COL_GRAY),
		list(KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT, KEY_RESIZE)
		)
    TRUE
}
