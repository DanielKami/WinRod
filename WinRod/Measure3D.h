//Header for measurements in 3D window


class Measure
{

private:

	struct MEASUREMENT
	{
		int measurement; // 0-nothing, 1-distance, 2-angle, 3-dihedral
		double value;
		AtomParam Atom1;
		AtomParam Atom2;
		AtomParam Atom3;
		AtomParam Atom4;
	};

public:

	MEASUREMENT measurement[MAX_MEASUREMENTS];
	int IndexMeasure;

	void	MouseSelect(POINT*, int, int, D3DXMATRIX*, AtomParam*, int, int, bool);
	bool	MeasureDistance(AtomParam*, int, AtomParam*, int);
	bool	MeasureAngle(AtomParam*, int, AtomParam*, int);
	bool	MeasureTorsian(AtomParam*, int, AtomParam*, int);

	void	ResetMeasurement();
	void	ResetAllMeasurements();

};