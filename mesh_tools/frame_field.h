#pragma once

#include "meshDefs.h"

namespace meshtools
{
class Frame
{
public:
	Frame();
	Frame(OvmVec3d c, OvmVec3d x, OvmVec3d y, OvmVec3d z) : center(c), x_vec(x), y_vec(y), z_vec(z) {}
	//Frame(OvmVec3d c, OvmVec3d x, OvmVec3d y, OvmVec3d z, int tag1): center(c), x_vec(x), y_vec(y), z_vec(z), tag(tag1){}
	~Frame() {}
	Frame &operator=(Frame &f1)
	{
		center = f1.center;
		x_vec = f1.x_vec;
		y_vec = f1.y_vec;
		z_vec = f1.z_vec;
		tag = f1.tag;
		ch = f1.ch;
		return *this;
	}
	void set_tag(int t) { tag = t; }
	void set_ch(OvmCeH c) { ch = c; }
	Matrix_3 m();
	void print_Frame();
	OvmVec3d center;
	OvmVec3d x_vec;
	OvmVec3d y_vec;
	OvmVec3d z_vec;
	/***************************************************************************
		//若tag=-1，说明frame在优化过程中可以变化；
		//若tag=0， 说明x_vec在优化过程中保持不变；
		//若tag=1，说明y_vec在优化过程中保持不变；
		//若tag=2，说明z_vec在优化过程中保持不变；
		//若tag=3，说明x_vec在优化过程中保持不变，且该单元有两个面邻接几何面，为2-3面和4-5面
		//若tag=4，说明y_vec在优化过程中保持不变，且该单元有两个面邻接几何面，为0-1面和4-5面
		//若tag=5，说明z_vec在优化过程中保持不变，且该单元有两个面邻接几何面，为0-1面和2-3面
		//若tag=6，说明该frame不能变动，该单元有三个面邻接几何面
		***************************************************************************/
	int tag;

	OvmCeH ch;
	//HoopsView *hoopsview;

	/*
		
		*/

	//
};

/*
	static variables & functions
	*/

static _Matrix_3 SOM3[24];

static int regular_inx[10];

static int matching_Matrix(Frame &f1, Frame &f2);

static void set_SOM3();

static int find_closet_Matrix_index(_Matrix_3 M);
} // namespace meshtools
