/*	$Csoft: button.c,v 1.16 2002/05/22 02:03:01 vedge Exp $	*/

/*
 * Copyright (c) 2002 CubeSoft Communications, Inc.
 * <http://www.csoft.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistribution of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistribution in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of CubeSoft Communications, nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
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

#include <sys/types.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <engine/engine.h>
#include <engine/queue.h>
#include <engine/version.h>

#include "primitive.h"
#include "text.h"
#include "widget.h"
#include "window.h"
#include "button.h"

extern TTF_Font *font;		/* text */

static const struct widget_ops button_ops = {
	{
		button_destroy,
		NULL,		/* load */
		NULL,		/* save */
		NULL,		/* onattach */
		NULL,		/* ondetach */
		NULL,		/* attach */
		NULL		/* detach */
	},
	button_draw,
	button_event
};

struct button *
button_new(struct region *reg, char *caption, int flags, int rw, int rh)
{
	struct button *button;

	button = emalloc(sizeof(struct button));
	button_init(button, caption, flags, rw, rh);

	pthread_mutex_lock(&reg->win->lock);
	region_attach(reg, button);
	pthread_mutex_unlock(&reg->win->lock);

	return (button);
}

void
button_init(struct button *b, char *caption, int flags, int rw, int rh)
{
	widget_init(&b->wid, "button", "widget", &button_ops, rw, rh);

	b->caption = strdup(caption);
	b->flags = flags;
	b->justify = BUTTON_CENTER;
	b->xmargin = 6;
	b->ymargin = 6;
}

void
button_destroy(void *p)
{
	struct button *b = p;

	OBJECT_ASSERT(p, "widget");

	free(b->caption);
}

void
button_draw(void *p)
{
	static SDL_Color white = { 255, 255, 255 }; /* XXX fgcolor */
	struct button *b = p;
	SDL_Surface *s, *bg;
	int x = 0, y = 0;

	OBJECT_ASSERT(p, "widget");

	/* Button */
	bg = primitive_box(b, WIDGET(b)->w, WIDGET(b)->h,
	    (b->flags & BUTTON_PRESSED) ? -1 : 1);
	WIDGET_DRAW(b, bg, 0, 0);

	/* Label */
	s = TTF_RenderText_Solid(font, b->caption, white);
	if (s == NULL) {
		fatal("TTF_RenderTextSolid: %s\n", SDL_GetError());
	}

	switch (b->justify) {
	case BUTTON_LEFT:
		x = b->xmargin;
		y = b->ymargin;
		break;
	case BUTTON_CENTER:
		x = ((bg->w - s->w) >> 1) + (b->xmargin>>1);
		y = ((bg->h - s->h) >> 1);
		break;
	case BUTTON_RIGHT:
		x = bg->w - s->w - b->xmargin;
		y = bg->h - s->h - b->ymargin;
		break;
	}
	if (b->flags & BUTTON_PRESSED) {
		x++;
		y++;
	}

	WIDGET_DRAW(b, s, x, y);
	SDL_FreeSurface(s);
}

void
button_event(void *p, SDL_Event *ev, int flags)
{
	struct button *b = p;
	int pushed = 0;
	
	OBJECT_ASSERT(p, "widget");

	switch (ev->type) {
	case SDL_MOUSEBUTTONDOWN:
		if (ev->button.button == 1) {
			b->flags |= BUTTON_PRESSED;
		} else {
			WIDGET_FOCUS(b);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (ev->button.button == 1) {
			b->flags &= ~(BUTTON_PRESSED);
			pushed++;
		}
		break;
	case SDL_KEYDOWN:
		if (ev->key.keysym.sym == SDLK_RETURN ||
		    ev->key.keysym.sym == SDLK_SPACE) {
			b->flags |= BUTTON_PRESSED;
		}
		break;
	case SDL_KEYUP:
		if (ev->key.keysym.sym == SDLK_RETURN ||
		    ev->key.keysym.sym == SDLK_SPACE) {
			b->flags &= ~(BUTTON_PRESSED);
			pushed++;
		}
		break;
	}
	
	if (pushed) {
		WIDGET(b)->win->redraw++;

		if (b->push != NULL) {
			b->push(b);
		}
	}
}

