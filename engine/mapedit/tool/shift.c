/*	$Csoft: shift.c,v 1.30 2004/01/03 04:25:10 vedge Exp $	*/

/*
 * Copyright (c) 2002, 2003, 2004 CubeSoft Communications, Inc.
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

#include <engine/engine.h>
#include <engine/mapedit/mapedit.h>

#include <engine/widget/vbox.h>
#include <engine/widget/radio.h>
#include <engine/widget/checkbox.h>

static void shift_init(void *);
void shift_mouse(void *, struct mapview *, Sint16, Sint16);

const struct tool shift_tool = {
	N_("Shift tool"),
	N_("Displace a tile with the mouse."),
	MAPEDIT_TOOL_SHIFT,
	-1,
	shift_init,
	NULL,			/* destroy */
	NULL,			/* load */
	NULL,			/* save */
	NULL,			/* effect */
	NULL,			/* cursor */
	NULL			/* mouse */
};

static enum shift_mode {
	SHIFT_HIGHEST,			/* Highest reference */
	SHIFT_ALL			/* All references on layer */
} mode = SHIFT_HIGHEST;

static int multi = 0;			/* Apply to whole selection? */

static void
shift_init(void *p)
{
	struct window *win;
	struct vbox *vb;

	win = tool_window(p, "mapedit-tool-shift");

	vb = vbox_new(win, 0);
	{
		static const char *modes[] = {
			N_("Highest"),
			N_("All"),
			NULL
		};
		struct radio *rad;
		struct checkbox *cb;

		rad = radio_new(vb, modes);
		widget_bind(rad, "value", WIDGET_INT, &mode);
		
		cb = checkbox_new(vb, _("Multi"));
		widget_bind(cb, "state", WIDGET_INT, &multi);
	}
}

void
shift_mouse(void *p, struct mapview *mv, Sint16 relx, Sint16 rely)
{
	struct map *m = mv->map;
	int selx = mv->mx + mv->mouse.x;
	int sely = mv->my + mv->mouse.y;
	int w = 1;
	int h = 1;
	int x, y;

	if (!multi ||
	    mapview_get_selection(mv, &selx, &sely, &w, &h) == -1) {
		if (selx < 0 || selx >= m->mapw ||
		    sely < 0 || sely >= m->maph)
			return;
	}

	for (y = sely; y < sely+h; y++) {
		for (x = selx; x < selx+w; x++) {
			struct node *node = &m->map[y][x];
			struct noderef *nref;

			TAILQ_FOREACH(nref, &node->nrefs, nrefs) {
				if (nref->layer != m->cur_layer)
					continue;

				noderef_set_center(nref,
				    nref->r_gfx.xcenter+relx,
				    nref->r_gfx.ycenter+rely);

				if (mode == SHIFT_HIGHEST)
					break;
			}
		}
	}
}

