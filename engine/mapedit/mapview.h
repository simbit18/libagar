/*	$Csoft: mapview.h,v 1.34 2003/03/11 01:57:31 vedge Exp $	*/
/*	Public domain	*/

#ifndef _AGAR_MAPEDIT_MAPVIEW_H_
#define _AGAR_MAPEDIT_MAPVIEW_H_

#include <engine/widget/widget.h>

#include <engine/mapedit/nodeedit.h>
#include <engine/mapedit/layedit.h>

/* Construction of source tile maps */
struct mapview_constr {
	int		 x, y;		/* Current position */
	struct window	*win;		/* Source tiles window */
	int		 replace;	/* Replace mode? */
	struct button	*trigger;
};

struct mapview {
	struct widget	wid;

	int	flags;
#define MAPVIEW_EDIT		 0x001	/* Mouse/keyboard edition */
#define MAPVIEW_INDEPENDENT	 0x002	/* Zoom/ss independent from map's */
#define MAPVIEW_TILEMAP		 0x004	/* Map of `source' nodes */
#define MAPVIEW_GRID		 0x008	/* Display a grid */
#define MAPVIEW_PROPS		 0x010	/* Display node properties */
#define MAPVIEW_ZOOMING_IN	 0x020
#define MAPVIEW_ZOOMING_OUT	 0x040
#define MAPVIEW_CENTER		 0x080
#define MAPVIEW_SAVEABLE	 0x100	/* Load/save keys */
#define MAPVIEW_NO_CURSOR	 0x200	/* Hide cursor */

	int	 prop_bg;		/* Background of node attributes */
	int	 prop_style;		/* Style of node attributes */

	/* Mouse scrolling state */
	struct {
		int	scrolling;	/* Currently scrolling? */
		int	x, y;		/* Current mouse position */
	} mouse;

	/* Selections */
	struct {
		int	set;
		int	x, y;		/* Selection origin */
		int	xoffs, yoffs;	/* Displacement from origin */
	} msel;
	struct {
		int		set;
		int		x, y;		/* Selection origin */
		unsigned int	w, h;		/* Displacement from origin */
	} esel;

	/* Zoom and soft-scroll offsets. */
	Uint16		*zoom;		/* Zoom (%) */
	Sint16		*ssx, *ssy;	/* Soft scroll offsets */
	int		*tilew, *tileh;
	SDL_TimerID	 zoom_tm;
	struct {
		Uint16	zoom;
		Sint16	ssx, ssy;
		int	tilew, tileh;
	} izoom;

	struct map	*map;		/* Map to display */
	Uint8		 cur_layer;	/* Layer being edited */
	int		 mx, my;	/* Display offset (nodes). XXX u32? */
	unsigned int	 mw, mh;	/* Display size (nodes) */
	
	int		 cx, cy;	/* Cursor position (nodes) */
	int		 cxrel, cyrel;
	struct node	*cur_node;

	struct mapview_constr	constr;	/* Source tile mapping */
	struct nodeedit		nodeed;	/* Node editor */
	struct layedit		layed;	/* Layer editor */
};

enum mapview_prop_labels {
	MAPVIEW_FRAME_0,
	MAPVIEW_FRAME_1,
	MAPVIEW_FRAME_2,
	MAPVIEW_FRAME_3,
	MAPVIEW_FRAME_4,
	MAPVIEW_FRAME_5,
	MAPVIEW_FRAME_6,
	MAPVIEW_FRAMES_END,
	MAPVIEW_BLOCK,
	MAPVIEW_ORIGIN,
	MAPVIEW_WALK,
	MAPVIEW_CLIMB,
	MAPVIEW_SLIPPERY,
	MAPVIEW_EDGE,
	MAPVIEW_EDGE_N,
	MAPVIEW_EDGE_S,
	MAPVIEW_EDGE_W,
	MAPVIEW_EDGE_E,
	MAPVIEW_EDGE_NW,
	MAPVIEW_EDGE_NE,
	MAPVIEW_EDGE_SW,
	MAPVIEW_EDGE_SE,
	MAPVIEW_BIO,
	MAPVIEW_REGEN,
	MAPVIEW_SLOW,
	MAPVIEW_HASTE
};

struct mapview	*mapview_new(struct region *, struct map *, int, int, int);
void		 mapview_init(struct mapview *, struct map *, int, int, int);
void		 mapview_destroy(void *);

void		 mapview_node_edit_win(struct mapview *);
void		 mapview_draw(void *);
void		 mapview_draw_props(struct mapview *, struct node *, int, int,
		     int, int);
void		 mapview_center(struct mapview *, int, int);
void		 mapview_zoom(struct mapview *, int);
void		 mapview_map_coords(struct mapview *, int *, int *);
void		 mapview_set_selection(struct mapview *, int, int,
		     unsigned int, unsigned int);
int		 mapview_get_selection(struct mapview *, int *, int *,
		     unsigned int *, unsigned int *);

#endif /* _AGAR_MAPEDIT_MAPVIEW_H_ */
