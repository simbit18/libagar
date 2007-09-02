/*
 * Copyright (c) 2007 Hypertriton, Inc. <http://hypertriton.com/>
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

/*
 * Generic sketch annotation class.
 */ 

#include <config/have_opengl.h>
#ifdef HAVE_OPENGL

#include <core/core.h>

#include "sk.h"

void
SK_AnnotInit(void *p, Uint32 name, const SK_NodeOps *ops)
{
	SK_Annot *ann = p;

	SK_NodeInit(ann, ops, name, SK_NODE_UNCONSTRAINED);
	ann->flags = 0;
}

int
SK_AnnotLoad(SK *sk, void *p, AG_Netbuf *buf)
{
	SK_Annot *ann = p;

	ann->flags = (Uint)AG_ReadUint32(buf);
	return (0);
}

int
SK_AnnotSave(SK *sk, void *p, AG_Netbuf *buf)
{
	SK_Annot *ann = p;

	AG_WriteUint32(buf, (Uint32)ann->flags);
	return (0);
}

int
SK_AnnotDelete(void *p)
{
	SK_Annot *ann = p;
	int rv;

	rv = SK_NodeDel(ann);
	SK_Update(SKNODE(ann)->sk);
	return (rv);
}
#endif /* HAVE_OPENGL */
