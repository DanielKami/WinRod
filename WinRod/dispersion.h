#pragma once




class Dispersion
{
public:
	Dispersion();
	virtual ~Dispersion();

	void cromer(int zed, double energy, double *fp, double*fpp);

private:

	//GAUS/
	int icount;
	double d_xsect_barns, d_bind_nrg_au, d_xsect_int[6], d_energy_au;

	//EDGE/
	double d_xsect_edge_au;

	//Constants table
	void bdnrgin();
	void relinc();
	void xnrg_b_d();
	void xsec_com();

	typedef double(Dispersion::*Sigma)(double);

	double sigma0(double x);
	double sigma1(double x);
	double sigma2(double x);
	double sigma3(double x);
	double gauss(Dispersion *obiect, Sigma sigma);

	void lgndr(int index, double *d_bb, double *d_cc);
	double aknint(double d_log_energy, int n, double d_log_nrg[], double d_log_xsect[]);
	void sort(int n, double a[], double b[]);
	void mcm(int zed, double energy, double *fp, double *fpp);

	int max0(int x, int y);
	int min0(int x, int y);
	double sign(double x, double y);
	double sqr(double x);

};

