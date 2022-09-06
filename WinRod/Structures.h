//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header with structure definitions
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "definitions.h"
#include <d3dx9.h>
//--------------------------------------------------------------------------------------------------------------------------------
struct AtomParam
{
	float	x, y, z;
	float	DW_in_plane;
	float	DW_out_of_plane;
	float	occupancy;
	int		AtomicNumber;
	int		Selected;			// The number higher than 0 indicate that atom is selected, the number indicate the order of sselection important for angle calculations
	int		Z_Depth;			//For transparency drawing order
	int		AtomInTheFile;
	int		display_mode;
	float   charge;
	WCHAR    Name[5];            //Name of atom form file
	D3DXMATRIX mShift;
};


//--------------------------------------------------------------------------------------------------------------------------------
struct BondParam
{
	//For bond creation
	int		NrBondedAtoms;
	int		BondedAtom[MAX_BONDED_ATOMS];
	float	AngleZ[MAX_BONDED_ATOMS];
	float	AngleXY[MAX_BONDED_ATOMS];
	float	Distance[MAX_BONDED_ATOMS];
	D3DXMATRIX mRotate[MAX_BONDED_ATOMS];	
};


//--------------------------------------------------------------------------------------------------------------------------------
struct ModelStruct
{
	AtomParam	Atom[MAX_ATOMS];
	int			nr_of_atoms;
};

//--------------------------------------------------------------------------------------------------------------------------------
struct DataStruct
{
	char	Header[HEADER];
	WCHAR	THeader[HEADER];
};


struct FLAG
{
	bool			plot_bulk_labels;	//Plot labels or not
	bool			plot_surf_labels;
	bool			g_compare;
	bool			g_spring;
	bool			g_light_follows_cam;
	bool            g_perspective;
	bool			g_cell;
	bool			g_termal_rings;
	bool			g_planeOnOff;
	bool			g_plot_shadows;
	bool			g_transparency;
	bool			g_bulk;
	bool			g_surface;
	bool			g_texture;
	bool			g_liquid_profile;
	bool            g_individual_color;
	float           g_scale_color;
	double			DWscale;
	float			BallSize;
	float			RodThickness;
	int				display_mode;//When atoms are change in properities 3D set the style for all atoms
	bool            display_surface1;
	float			g_specular;
	float			g_specular_intensty;
	int			    g_text_size;
//	WCHAR			g_text_font;
	//Object colors
	DWORD			rgbCurrent;        // initial color selection
	DWORD			rgbLight;
	DWORD			rgbText;


	void Init()
	{
		plot_surf_labels = false;
		plot_bulk_labels = false;

		g_bulk = true;
		g_surface = true;
		g_compare = false;
		g_termal_rings = true;
		g_cell = true;
		g_perspective = true;
		display_surface1 = true;
		g_planeOnOff = true;
		g_plot_shadows = false;
		g_transparency = true;
		g_spring = false;
		BallSize = 0.1f;
		RodThickness = .05f;

		DWscale = 25. / PIPI16;
		g_light_follows_cam = true;
		g_specular = 25;
		g_specular_intensty = 0.6f;
		g_individual_color = false;
		g_scale_color = 1.f;
		g_text_size = 10;
	}
};

struct ATOM_PROPERITIES
{
	float	AtomColor[99][4];
	float	AtomRadius[99];
};