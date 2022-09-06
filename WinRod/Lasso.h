
#include <d3d9.h>
#include <d3dx9.h>
#include "structures.h"

class Lasso
{

public:
	Lasso()
	{
		Entry = NULL;
		Start = NULL;
		Last = NULL;
	}

	void	MouseLasso(D3DXVECTOR2*, int, int, int, D3DXMATRIX*, AtomParam*, int, AtomParam*, int, int);
	void	ResetSelection(POINT, AtomParam*, int, AtomParam*, int);

private:
	void	insert_pixel(const int x, const int y);
	void	LineLasso(D3DXVECTOR2 Pos1, D3DXVECTOR2 Pos2, POINT *Lasso, int *count);
	void	BoundaryFill(const int _x, const int _y, int Width, int Height, const bool fill_color, const bool boundary_color);
	bool	*Mask; //two dimensional array represented as a [x+y*width] array

	struct Pixel
	{
		int x;
		int y;

		Pixel *Next;
	};

	Pixel *Entry;
	Pixel *Start;
	Pixel *Last;
};