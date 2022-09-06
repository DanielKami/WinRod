//Additional mathematical functions for better code clarity
//Created 15.11.2014 by Daniel Kaminski

#include <stdafx.h>
#include "ExMath.h"
#include "Math.h"

double sqr(double x){
	return (x*x);
}
float sqr(float x){
	return (x*x);
}

float sng(float x)
{
	if (x < 0) return (-1);
	return (1);
}

void VectorNormalisation(double *v)
{
	double lenght = sqrt(sqr(v[0]) + sqr(v[1] + sqr(v[2])));
	v[0] /= lenght;
	v[1] /= lenght;
	v[2] /= lenght;
}

double DotProduct(double *v, double *w)
{
	return (v[0] * w[0] + v[1] * w[1] + v[2] * w[2]);
}

void CrossProduct(double *v, double *w, double *cross)
{
	//<Wy*Vz - Wz*Vy, Wz*Vx - Wx*Vz, Wx*Vy - Wy*Vx>

	cross[0] = w[1] * v[2] - w[2] * v[1];
	cross[1] = w[2] * v[0] - w[0] * v[2];
	cross[2] = w[0] * v[1] - w[1] * v[0];
}
void OrtNormVector(double *vec, double *vref, double *res)
{
	double dot = DotProduct(vec, vref);

	res[0] = vec[0] - vref[0] * dot;
	res[1] = vec[1] - vref[1] * dot;
	res[2] = vec[2] - vref[2] * dot;

	VectorNormalisation(res);
}

double myRound(double fValue)
{
	return fValue < 0 ? ceil(fValue - 0.5)
		: floor(fValue + 0.5);
}