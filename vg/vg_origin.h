/*	$Csoft: vg_origin.h,v 1.3 2004/05/24 03:32:22 vedge Exp $	*/
/*	Public domain	*/

#ifndef _AGAR_VG_ORIGIN_H_
#define _AGAR_VG_ORIGIN_H_
#include "begin_code.h"

#define VG_NORIGINS 3

__BEGIN_DECLS
int	VG_AddOrigin(struct vg *, float, float, float, Uint32);
void	VG_Origin(struct vg *, int, float, float);
void	VG_OriginColor(struct vg *, int, int, int, int);
void	VG_OriginRadius(struct vg *, int, float);
void	VG_DrawOrigin(struct vg *);
__END_DECLS

#include "close_code.h"
#endif /* _AGAR_VG_ORIGIN_H_ */
