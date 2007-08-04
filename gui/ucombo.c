/*
 * Copyright (c) 2002-2006 CubeSoft Communications, Inc.
 * <http://www.csoft.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <core/core.h>
#include <core/view.h>

#include "ucombo.h"

#include "primitive.h"

static AG_WidgetOps agUComboOps = {
	{
		"AG_Widget:AG_UCombo",
		sizeof(AG_UCombo),
		{ 0,0 },
		NULL,			/* init */
		NULL,			/* reinit */
		AG_UComboDestroy,
		NULL,			/* load */
		NULL,			/* save */
		NULL			/* edit */
	},
	NULL,			/* draw */
	AG_UComboScale
};

AG_UCombo *
AG_UComboNew(void *parent, Uint flags)
{
	AG_UCombo *com;

	com = Malloc(sizeof(AG_UCombo), M_OBJECT);
	AG_UComboInit(com, flags);
	AG_ObjectAttach(parent, com);
	if (flags & AG_UCOMBO_FOCUS) {
		AG_WidgetFocus(com);
	}
	return (com);
}

static void
ucombo_collapse(AG_UCombo *com)
{
	AG_WidgetBinding *stateb;
	int *state;

	if (com->panel == NULL)
		return;

	com->saved_w = AGWIDGET(com->panel)->w;
	com->saved_h = AGWIDGET(com->panel)->h;
	AG_WindowHide(com->panel);
	AG_ObjectDetach(com->list);
	AG_ViewDetach(com->panel);
	com->panel = NULL;
	
	stateb = AG_WidgetGetBinding(com->button, "state", &state);
	*state = 0;
	AG_WidgetBindingChanged(stateb);
	AG_WidgetUnlockBinding(stateb);
}

static void
ucombo_expand(AG_Event *event)
{
	AG_UCombo *com = AG_PTR(1);
	int expand = AG_INT(2);
	AG_Widget *pan;

	if (expand) {
		com->panel = AG_WindowNew(AG_WINDOW_MODAL|AG_WINDOW_NOTITLE|
		                          AG_WINDOW_NOBORDERS);
		AG_WindowSetPadding(com->panel, 0, 0, 0);

		pan = AGWIDGET(com->panel);

		AG_ObjectAttach(com->panel, com->list);
	
		pan->w = com->saved_w > 0 ? com->saved_w : AGWIDGET(com)->w*4;
		pan->h = com->saved_h > 0 ? com->saved_h : AGWIDGET(com)->h*5;
		pan->x = AGWIDGET(com)->cx;
		pan->y = AGWIDGET(com)->cy;

		/* XXX redundant check */
		if (pan->x+pan->w > agView->w)
			pan->w = agView->w - pan->x;
		if (pan->y+pan->h > agView->h)
			pan->h = agView->h - pan->y;
		
		AG_TlistPrescale(com->list, "XXXXXXXXXXXXXXXXXX", 6);
		AGWIDGET_SCALE(com->list, -1, -1);
		AGWIDGET_SCALE(pan, -1, -1);
		AG_WINDOW_UPDATE(pan);
		AG_WindowShow(com->panel);
	} else {
		ucombo_collapse(com);
	}
}

/* Effect a user item selection. */
static void
ucombo_selected(AG_Event *event)
{
	AG_Tlist *tl = AG_SELF();
	AG_UCombo *com = AG_PTR(1);
	AG_TlistItem *it;

	AG_MutexLock(&tl->lock);
	if ((it = AG_TlistSelectedItem(tl)) != NULL) {
		it->selected++;
		AG_ButtonText(com->button, "%s", it->text);
		AG_PostEvent(NULL, com, "ucombo-selected", "%p", it);
	}
	AG_MutexUnlock(&tl->lock);

	ucombo_collapse(com);
}

void
AG_UComboInit(AG_UCombo *com, Uint flags)
{
	Uint wflags = AG_WIDGET_FOCUSABLE|AG_WIDGET_UNFOCUSED_BUTTONUP;

	if (flags & AG_UCOMBO_HFILL) { wflags |= AG_WIDGET_HFILL; }
	if (flags & AG_UCOMBO_VFILL) { wflags |= AG_WIDGET_VFILL; }

	AG_WidgetInit(com, &agUComboOps, wflags);
	com->panel = NULL;
	com->saved_h = 0;

	com->button = AG_ButtonNew(com, AG_BUTTON_STICKY, _("..."));
	AG_ButtonSetPadding(com->button, 1,1,1,1);
	AG_SetEvent(com->button, "button-pushed", ucombo_expand, "%p", com);
	
	com->list = Malloc(sizeof(AG_Tlist), M_OBJECT);
	AG_TlistInit(com->list, AG_TLIST_EXPAND);
	AG_SetEvent(com->list, "tlist-changed", ucombo_selected, "%p", com);
}

void
AG_UComboDestroy(void *p)
{
	AG_UCombo *com = p;

	if (com->panel != NULL) {
		AG_WindowHide(com->panel);
		AG_ObjectDetach(com->list);
		AG_ViewDetach(com->panel);
	}
	AG_ObjectDestroy(com->list);
	Free(com->list, M_OBJECT);
	AG_WidgetDestroy(com);
}

void
AG_UComboScale(void *p, int w, int h)
{
	AG_UCombo *com = p;

	if (w == -1 && h == -1) {
		AGWIDGET_SCALE(com->button, -1, -1);
		AGWIDGET(com)->w = AGWIDGET(com->button)->w;
		AGWIDGET(com)->h = AGWIDGET(com->button)->h;
		return;
	}
	
	AG_WidgetScale(com->button, w, h);
	AGWIDGET(com->button)->x = 0;
	AGWIDGET(com->button)->y = 0;
}

