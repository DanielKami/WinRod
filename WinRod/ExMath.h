//Additional mathematical functions
//Created 15.11.2014 by Daniel Kaminski


double	sqr(double);
float	sqr(float);
float	sng(float x);

//vectors
void	VectorNormalisation(double *v);
double	DotProduct(double *v, double *w);
void	CrossProduct(double *v, double *w, double *cross);
void	OrtNormVector(double *vec, double *vref, double *res);
double myRound(double fValue);