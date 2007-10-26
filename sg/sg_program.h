/*	Public domain	*/

#ifndef _AGAR_SG_PROGRAM_H_
#define _AGAR_SG_PROGRAM_H_

typedef struct sg_program_ops {
	struct ag_object_ops ops;
	int  (*install)(void *, SG_View *);
	void (*deinstall)(void *, SG_View *);
	void (*bind)(void *, SG_View *);
	void (*unbind)(void *, SG_View *);
} SG_ProgramOps;

typedef struct sg_program {
	struct ag_object obj;
	Uint flags;
} SG_Program;

#define SG_PROGRAM(sp)		((SG_Program *)(sp))
#define SG_PROGRAM_OPS(sp)	((SG_ProgramOps *)AGOBJECT(sp)->ops)

__BEGIN_DECLS
extern const AG_ObjectOps sgProgramOps;

void	 SG_ProgramInit(void *, const char *);
int	 SG_ProgramLoad(void *, AG_DataSource *);
int	 SG_ProgramSave(void *, AG_DataSource *);

void	 SG_ProgramInstall(SG_Program *, SG_View *);
void	 SG_ProgramDeinstall(SG_Program *, SG_View *);
void	 SG_ProgramBind(SG_Program *, SG_View *);
void	 SG_ProgramUnbind(SG_Program *, SG_View *);
__END_DECLS

#endif /* _AGAR_SG_PROGRAM_H_ */
