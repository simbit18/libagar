/*	$Csoft: primitive.h,v 1.22 2003/06/08 00:21:05 vedge Exp $	*/
/*	Public domain	*/

#ifndef _AGAR_WIDGET_PRIMITIVE_H_
#define _AGAR_WIDGET_PRIMITIVE_H_
#include "begin_code.h"

struct primitive_ops {
	void	(*box)(void *, int, int, int, int, int, int);
	void	(*frame)(void *, int, int, int, int, int);
	void	(*circle)(void *, int, int, int, int);
	void	(*line)(void *, int, int, int, int, int);
	void	(*line2)(void *, int, int, int, int, int);
	void	(*rect_outlined)(void *, int, int, int, int, int);
	void	(*rect_filled)(void *, int, int, int, int, int);
	void	(*plus)(void *, int, int, int, int, int);
	void	(*minus)(void *, int, int, int, int, int);
};

extern struct primitive_ops primitives;

__BEGIN_DECLS
extern DECLSPEC struct window	*primitive_config_window(void);
extern DECLSPEC void		 primitives_init(void);
__END_DECLS

#include "close_code.h"
#endif	/* _AGAR_WIDGET_PRIMITIVE_H_ */
