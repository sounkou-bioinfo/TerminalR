// ===============================================================
//
// TerminalR: R package for text-based user interfaces
// Copyright (C) 2016-2017   Xiuwen Zheng
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>
#include <curses.h>


// -----------------------------------------------------------------------

#define MAX_WINDOW_NUM 256

static WINDOW* WindowList[MAX_WINDOW_NUM];
static int has_init = FALSE;


static WINDOW *get_win(SEXP win)
{
	int iw = Rf_asInteger(win);
	if ((iw < 0) || (iw >= MAX_WINDOW_NUM))
		error("Invalid window ID.");
	WINDOW *w = WindowList[iw];
	if (w == NULL)
		error("Uninitialized window.");
	return w;
}


static int as_logical(SEXP fail)
{
	int v = Rf_asLogical(fail);
	if (v == NA_LOGICAL)
		error("'fail' must be TRUE or FALSE.");
	return v;
}


// -----------------------------------------------------------------------

#define COLOR_GRAY     0x0008
#define COLOR_BR_RED   0x000A

extern SEXP nc_initcolor();

SEXP nc_initscr()
{
	if (!has_init)
	{
		WINDOW *win = initscr();
		if (win == NULL)
			error("Initializing screen fails.");
		has_init = TRUE;
		WindowList[0] = win;
		keypad(win, TRUE);
		noecho();
		cbreak();
		scrollok(win, FALSE);

		// initialize colors
		if (has_colors())
		{
			start_color();
			use_default_colors();
			// color pair assignment
			init_pair(1, COLOR_RED,     -1);
			init_pair(2, COLOR_GREEN,   -1);
			init_pair(3, COLOR_YELLOW,  -1);
			init_pair(4, COLOR_BLUE,    -1);
			init_pair(5, COLOR_CYAN,    -1);
			init_pair(6, COLOR_MAGENTA, -1);
			init_pair(7, COLOR_GRAY,    -1);

			// init_pair(1, COLOR_RED,     COLOR_BLACK);
			// init_pair(2, COLOR_GREEN,   COLOR_BLACK);
			// init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
			// init_pair(4, COLOR_BLUE,    COLOR_BLACK);
			// init_pair(5, COLOR_CYAN,    COLOR_BLACK);
			// init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
			// init_pair(7, COLOR_GRAY,    COLOR_BLACK);
		}
	}
	return R_NilValue;
}


SEXP nc_endwin()
{
	if (has_init)
	{
		// WindowList[0] is stdscr, no need to be deleted
		for (size_t i=1; i < MAX_WINDOW_NUM; i++)
		{
			WINDOW *win = WindowList[i];
			if (win)
			{
				delwin(win);
				WindowList[i] = NULL;
			}
		}
		endwin();
		has_init = FALSE;
	}
	return R_NilValue;
}


SEXP nc_get_used()
{
	return ScalarLogical(has_init ? TRUE : FALSE);
}


SEXP nc_setscrollok(SEXP win, SEXP ok)
{
	WINDOW *w = get_win(win);
	int vok = Rf_asLogical(ok);
	if (vok == NA_LOGICAL)
		error("'ok' must be TRUE or FALSE.");
	scrollok(w, vok);
	return R_NilValue;
}


SEXP nc_getmaxyx(SEXP win)
{
	int x, y;
	WINDOW *w = get_win(win);
	getmaxyx(w, y, x);
	SEXP rv = NEW_INTEGER(2);
	INTEGER(rv)[0] = y;
	INTEGER(rv)[1] = x;
	return rv;
}


SEXP nc_wmove(SEXP win, SEXP y, SEXP x, SEXP fail)
{
	int allow_fail = as_logical(fail);
	WINDOW *w = get_win(win);
	if ((wmove(w, Rf_asInteger(y), Rf_asInteger(x)) == ERR) && allow_fail)
		error("wmove fails.");
	return R_NilValue;
}


SEXP nc_wprintw(SEXP win, SEXP txt, SEXP col, SEXP fail)
{
	int allow_fail = as_logical(fail);
	WINDOW *w = get_win(win);

	col = AS_INTEGER(col);
	int ncol = length(col), icol = 0;
	int init_col = (ncol > 0) ? INTEGER(col)[0] : -1;
	int cur_col = init_col;

	for (size_t i=0; i < length(txt); i++)
	{
		// color
		if (cur_col != -1) wattrset(w, cur_col);
		icol ++;
		if (icol >= ncol)
		{
			icol = 0; cur_col = init_col;
		} else {
			cur_col = INTEGER(col)[icol];
		}
		// text
		const char *s = CHAR(STRING_ELT(txt, i));
		if ((wprintw(w, "%s", s) == ERR) && allow_fail)
			error("wprintw fails.");
	}

	return R_NilValue;
}


SEXP nc_erase(SEXP win)
{
	werase(get_win(win));
	return R_NilValue;
}


SEXP nc_refresh(SEXP win)
{
	wrefresh(get_win(win));
	return R_NilValue;
}


SEXP nc_setcolor(SEXP win, SEXP col)
{
	wattrset(get_win(win), Rf_asInteger(col));
	return R_NilValue;
}


SEXP nc_getch()
{
	return ScalarInteger(getch());
}


SEXP nc_col_attr(SEXP col, SEXP attr)
{
	int nc = length(col);
	int zc = (nc > 0) ? INTEGER(col)[0] : 0, vc=zc;
	int ic = 0;

	int na = length(attr);
	int za = (na > 0) ? INTEGER(attr)[0] : 0, va=za;
	int ia = 0;

	int n  = (nc > na) ? nc : na;
	SEXP rv = NEW_INTEGER(n);
	for (int i=0; i < n; i++)
	{
		INTEGER(rv)[i] = COLOR_PAIR(vc & 0x07) | va;
		if ((++ic) >= nc)
		{
			ic = 0; vc = zc;
		} else {
			vc = INTEGER(col)[ic];
		}
		if ((++ia) >= na)
		{
			ia = 0; va = za;
		} else {
			va = INTEGER(attr)[ia];
		}
	}
	return rv;
}


// -----------------------------------------------------------------------

SEXP nc_setint(SEXP obj, SEXP idx, SEXP val)
{
	if (!IS_INTEGER(obj)) error("'obj' should be an integer object.");
	int i = asInteger(idx);
	if ((i < 1) || (i > length(obj))) error("'idx' is out of range.");
	INTEGER(obj)[i-1] = asInteger(val);
	return R_NilValue;
}

SEXP nc_setlogical(SEXP obj, SEXP idx, SEXP val)
{
	if (!IS_LOGICAL(obj)) error("'obj' should be a logical object.");
	int i = asInteger(idx);
	if ((i < 1) || (i > length(obj))) error("'idx' is out of range.");
	LOGICAL(obj)[i-1] = asLogical(val);
	return R_NilValue;
}

SEXP nc_setreal(SEXP obj, SEXP idx, SEXP val)
{
	if (!IS_NUMERIC(obj)) error("'obj' should be a real object.");
	int i = asInteger(idx);
	if ((i < 1) || (i > length(obj))) error("'idx' is out of range.");
	REAL(obj)[i-1] = asReal(val);
	return R_NilValue;
}

SEXP nc_setlist(SEXP obj, SEXP idx, SEXP val)
{
	if (!isVectorList(obj)) error("'obj' should be a list object.");
	int i = asInteger(idx);
	if ((i < 1) || (i > length(obj))) error("'idx' is out of range.");
	SET_ELEMENT(obj, i-1, val);
	return R_NilValue;
}


// -----------------------------------------------------------------------

SEXP nc_init(SEXP col_attr, SEXP col, SEXP key)
{
	// color attributes
	INTEGER(VECTOR_ELT(col_attr, 0))[0] = A_NORMAL;
	INTEGER(VECTOR_ELT(col_attr, 1))[0] = A_STANDOUT;
	INTEGER(VECTOR_ELT(col_attr, 2))[0] = A_UNDERLINE;
	INTEGER(VECTOR_ELT(col_attr, 3))[0] = A_REVERSE;
	INTEGER(VECTOR_ELT(col_attr, 4))[0] = A_BLINK;
	INTEGER(VECTOR_ELT(col_attr, 5))[0] = A_DIM;
	INTEGER(VECTOR_ELT(col_attr, 6))[0] = A_BOLD;
	INTEGER(VECTOR_ELT(col_attr, 7))[0] = A_PROTECT;
	INTEGER(VECTOR_ELT(col_attr, 8))[0] = A_INVIS;
	// color
	INTEGER(VECTOR_ELT(col, 0))[0] = COLOR_PAIR(0);
	INTEGER(VECTOR_ELT(col, 1))[0] = COLOR_PAIR(1);
	INTEGER(VECTOR_ELT(col, 2))[0] = COLOR_PAIR(2);
	INTEGER(VECTOR_ELT(col, 3))[0] = COLOR_PAIR(3);
	INTEGER(VECTOR_ELT(col, 4))[0] = COLOR_PAIR(4);
	INTEGER(VECTOR_ELT(col, 5))[0] = COLOR_PAIR(5);
	INTEGER(VECTOR_ELT(col, 6))[0] = COLOR_PAIR(6);
	INTEGER(VECTOR_ELT(col, 7))[0] = COLOR_PAIR(7);
	// keyboard
	INTEGER(VECTOR_ELT(key, 0))[0] = KEY_DOWN;
	INTEGER(VECTOR_ELT(key, 1))[0] = KEY_UP;
	INTEGER(VECTOR_ELT(key, 2))[0] = KEY_LEFT;
	INTEGER(VECTOR_ELT(key, 3))[0] = KEY_RIGHT;
	INTEGER(VECTOR_ELT(key, 4))[0] = KEY_RESIZE;
	// return
	return R_NilValue;
}



// -----------------------------------------------------------------------

/// Initialize the package
void R_init_TerminalR(DllInfo *info)
{
	#define CALL(name, num)    { #name, (DL_FUNC)&name, num }

	static R_CallMethodDef callMethods[] =
	{
		CALL(nc_initscr, 0),
		{ NULL, NULL, 0 }
	};
	R_registerRoutines(info, NULL, callMethods, NULL, NULL);

	// initialize
	memset((void*)WindowList, 0, sizeof(WindowList));
	has_init = FALSE;
}

/// Finalize the package
void R_unload_TerminalR(DllInfo *info)
{
	nc_endwin();
}
