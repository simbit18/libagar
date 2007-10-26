/*	Public domain	*/

__BEGIN_DECLS
SG_Line2	SG_LineFromPts2(SG_Vector2, SG_Vector2);
SG_Line		SG_LineFromPts(SG_Vector, SG_Vector);

SG_Plane	SG_PlaneFromNormal(SG_Vector, SG_Real);
SG_Plane	SG_PlaneFrom3Pts(SG_Vector, SG_Vector, SG_Vector);
SG_Plane	SG_PlaneAtDistance(SG_Plane, SG_Real);
SG_Plane	SG_PlaneRead(AG_DataSource *);
void		SG_PlaneWrite(AG_DataSource *, SG_Plane *);
int	 	SG_PlaneIsValid(SG_Plane);
SG_Vector	SG_PlaneNorm(SG_Plane);
SG_Vector	SG_PlaneNormp(const SG_Plane *);
SG_Real		SG_VectorPlaneAngle(SG_Vector, SG_Plane);

SG_Intersect2	SG_IntersectLineLine2(SG_Line2, SG_Line2);
SG_Real		SG_PointLineDistance2(SG_Vector2, SG_Line2);
SG_Real		SG_LineLineAngle(SG_Line, SG_Line);
SG_Real		SG_LineLineAngle2(SG_Line2, SG_Line2);
__END_DECLS
