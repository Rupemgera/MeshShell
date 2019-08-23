#include "stress_field.h"

#include <stdio.h>
#include <sstream>
#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>

/*
 hypermesh tensor stress order : 
 XX YY ZZ XY YZ ZX

 tensor matrix:
 | XX XY XZ |
 | YX YY YZ |
 | ZX ZY ZZ |
*/
bool PrincipalStressField::readInStress(std::string filename, VMeshPtr mesh)
{
	int n;
	double sigma[6];

	std::string tmp;
	std::ifstream stress_fin(filename);

	//读入文件头，判断单元类型
	std::getline(stress_fin, tmp);
	if (tmp.find("Node") != tmp.npos)
	{
		element_type = STRESS_ELEMENT_TYPE::Node;
	}
	else if (tmp.find("Element") != tmp.npos)
	{
		element_type = STRESS_ELEMENT_TYPE::Cell;
	}
	else
	{
		// unrecognized element type!
		return false;
	}

	/* lambda function to change stress tensor from vector form to matrix form */
	auto transform = [](double sigma[6]) -> Matrix_3 {
		Matrix_3 stress_tensor;
		stress_tensor << sigma[0], sigma[3], sigma[5],
			sigma[3], sigma[1], sigma[4],
			sigma[5], sigma[4], sigma[2];
		return stress_tensor;
	};

	//读入数据
	if (mesh != nullptr)
	{

		//预分配vector的大小
		if (element_type == STRESS_ELEMENT_TYPE::Node)
		{
			resize(mesh->n_vertices());
		}
		else if (element_type == STRESS_ELEMENT_TYPE::Cell)
		{
			resize(mesh->n_cells());
		}

		while (std::getline(stress_fin, tmp))
		{
			if (tmp == "")
				continue;
				/*
			读取stress的六个独立分量
			*/
#ifdef __linux
			sscanf(tmp.c_str(), "%d , %lf %lf %lf %lf %lf %lf", &n,
				   sigma, sigma + 1, sigma + 2, sigma + 3, sigma + 4, sigma + 5);
#endif // __linux
#ifdef _WIN64
			sscanf_s(tmp.c_str(), "%d , %lf %lf %lf %lf %lf %lf", &n,
					 sigma, sigma + 1, sigma + 2, sigma + 3, sigma + 4, sigma + 5);
#endif // _WIN64

			/*
			向量形式的stress转换为矩阵形式
			*/
			auto stress_tensor = transform(sigma);
			_stress[n - 1] = stress_tensor;
			_frames[n - 1] = cal_principal_direction(stress_tensor);
		}
	}
	else
	{

		while (std::getline(stress_fin, tmp))
		{
			if (tmp == "")
				continue;
				/* 
			读取stress的六个独立分量 
			*/
#ifdef __linux
			sscanf(tmp.c_str(), "%d , %lf %lf %lf %lf %lf %lf", &n,
				   sigma, sigma + 1, sigma + 2, sigma + 3, sigma + 4, sigma + 5);
#endif // __linux
#ifdef _WIN64
			sscanf_s(tmp.c_str(), "%d , %lf %lf %lf %lf %lf %lf", &n,
					 sigma, sigma + 1, sigma + 2, sigma + 3, sigma + 4, sigma + 5);
#endif // _WIN64

			/*
			向量形式的stress转换为矩阵形式
			*/
			auto stress_tensor = transform(sigma);
			//添加到应力张量集
			_stress.push_back(stress_tensor);
			_frames.push_back(cal_principal_direction(stress_tensor));
		}
	}

	_n = _stress.size();

	//calculate tets center poisition
	setCellCenter(mesh);

	//save to another form
	size_t dot_position = filename.find_last_of('.');
	std::string extension = filename.substr(dot_position + 1);
	std::string rest_filename = filename.substr(0, dot_position + 1);
	std::ofstream fout(rest_filename + "txt");
	saveStress(fout);

	return true;
}

bool PrincipalStressField::saveStress(std::ofstream &fout)
{
	for (int i = 0; i < _location.size(); ++i)
	{
		fout << i << ' ';
		fout << _frames[i](0, 0) << ' ' << _frames[i](1, 0) << ' ' << _frames[i](2, 0) << ' ';
		fout << _frames[i](0, 1) << ' ' << _frames[i](1, 1) << ' ' << _frames[i](2, 1) << ' ';
		fout << _frames[i](0, 2) << ' ' << _frames[i](1, 2) << ' ' << _frames[i](2, 2) << ' ';
		fout << _location[i][0] << ' ' << _location[i][1] << ' ' << _location[i][2] << std::endl;
	}
	return true;
}

// calculate the position of center point of tets
bool PrincipalStressField::setCellCenter(VMeshPtr mesh)
{
	_location.resize(mesh->n_cells());
	for (auto citer = mesh->cells_begin(); citer != mesh->cells_end(); ++citer)
	{
		MeshPoint c(0, 0, 0);
		for (auto viter = mesh->cv_iter(*citer); viter.valid(); ++viter)
		{
			MeshPoint p = mesh->vertex(*viter);
			c += p;
		}
		_location[citer->idx()] = c / 4;
	}
	return true;
}

PrincipalStressField::PrincipalStressField()
{
	/* initialize */
	_stress.clear();
	_frames.clear();
	_n = -1;
}

PrincipalStressField::~PrincipalStressField()
{
}

const bool PrincipalStressField::set_mesh(VMeshPtr mesh)
{
	_mesh = mesh;
	return true;
}

void PrincipalStressField::get_locations(std::vector<MeshPoint> &ret)
{
	ret = _location;
}

void PrincipalStressField::get_principal_dirs(std::vector<MeshPoint> &ret, int P)
{
	ret.resize(_frames.size());
	for (int i = 0; i < _frames.size(); ++i)
	{
		ret[i][0] = _frames[i](0, P);
		ret[i][1] = _frames[i](1, P);
		ret[i][2] = _frames[i](2, P);
	}
}

//bool PrincipalStressField::singular_edges_extraction(VMeshPtr mesh)
//{
//	//
//
//	//先将原来的singularities全部清空
//	std::unordered_set<OvmEgH>().swap(singular_ehs);
//	std::unordered_set<OvmEgH>().swap(regular_singular_ehs);
//	std::unordered_set<OvmEgH>().swap(irregular_singular_ehs);
//	std::unordered_set<OvmEgH>().swap(postive_singularities);
//	std::unordered_set<OvmEgH>().swap(negative_singularities);
//
//	//SOM3
//	meshtools::Frame::set_SOM3();
//	const auto SOM3 = meshtools::Frame::SOM3;
//	const auto regular_inx = meshtools::Frame::regular_inx;
//
//	std::unordered_set<int> sin_idx;
//	////将奇异线输出
//	//std::cout<<"Before writing!"<<std::endl;
//	//std::ofstream fout("regular singularities.txt");
//	for (int s_ind = 0; s_ind < 10; s_ind++)
//	{
//		sin_idx.insert(regular_inx[s_ind]);
//		//	std::cout<<regular_inx[s_ind]<<": "<<std::endl;
//		//	fout<<regular_inx[s_ind]<<":"<<std::endl;
//		//	for (int row_id = 0;row_id < 3;row_id++)
//		//	{
//		//		for (int col_id = 0;col_id < 3;col_id++)
//		//		{
//		//			fout<<SOM3[regular_inx[s_ind]][row_id][col_id]<<" ";
//		//			std::cout<<SOM3[regular_inx[s_ind]][row_id][col_id]<<" ";
//		//		}
//		//		std::cout<<std::endl;
//		//		fout<<std::endl;
//		//	}
//	}
//	//std::cout<<"After writting"<<std::endl;
//	//fout.close();
//	//system("pause");
//	//////////////////////////////////////////////////////////////////////////
//	for (auto iter = mesh->edges_begin(); iter != mesh->edges_end(); iter++)
//	{
//		_Matrix_3 fm;
//		if (mesh->is_boundary(*iter))
//		{
//			OvmHaEgH current_heh = mesh->halfedge_handle(*iter, 0);
//			std::vector<OvmHaFaH> adj_hfhs;
//			for (auto hehf_it = mesh->hehf_iter(current_heh); hehf_it; hehf_it++)
//			{
//				adj_hfhs.push_back(*hehf_it);
//			}
//			OvmCeH start_cell = mesh->incident_cell(adj_hfhs[0]);
//			OvmCeH end_cell = mesh->incident_cell(mesh->opposite_halfface_handle(adj_hfhs[adj_hfhs.size() - 1]));
//			OvmVec3d a = frames[start_cell].x_vec; OvmVec3d b = frames[end_cell].x_vec;
//			_Matrix_3 M;
//			if ((a - b).length() < 1e-7)
//			{
//				M[0][0] = 1;
//				M[0][1] = 0;
//				M[0][2] = 0;
//				M[1][0] = 0;
//				M[1][1] = 1;
//				M[1][2] = 0;
//				M[2][0] = 0;
//				M[2][1] = 0;
//				M[2][2] = 1;
//			}
//			else
//			{
//				OvmVec3d u = cross(a, b);
//				u = u.normalize();
//				double cro_len = cross(a, b).length();
//				if (cro_len > 1)
//				{
//					cro_len = 1;
//				}
//				double miu = atan2(cro_len, dot(a, b));
//				M[0][0] = cos(miu) + (1 - cos(miu))*u[0] * u[0];
//				M[0][1] = (1 - cos(miu))*u[0] * u[1] - sin(miu)*u[2];
//				M[0][2] = (1 - cos(miu))*u[0] * u[2] + sin(miu)*u[1];
//				M[1][0] = (1 - cos(miu))*u[1] * u[0] + sin(miu)*u[2];
//				M[1][1] = cos(miu) + (1 - cos(miu))*u[1] * u[1];
//				M[1][2] = (1 - cos(miu))*u[1] * u[2] - sin(miu)*u[0];
//				M[2][0] = (1 - cos(miu))*u[2] * u[0] - sin(miu)*u[1];
//				M[2][1] = (1 - cos(miu))*u[2] * u[1] + sin(miu)*u[0];
//				M[2][2] = cos(miu) + (1 - cos(miu))*u[2] * u[2];
//			}
//			Matrix_3 Re(M);
//			Matrix_3 new_F0 = Re * (frames[start_cell].m().transpose());
//			OvmVec3d x_v = OvmVec3d(new_F0(0,0), new_F0(1,0), new_F0(2,0));
//			OvmVec3d y_v = OvmVec3d(new_F0(0,1), new_F0(1,1), new_F0(2,1));
//			OvmVec3d z_v = OvmVec3d(new_F0(0,2), new_F0(1,2), new_F0(2,2));
//			meshtools::Frame new_f0(frames[start_cell].center, x_v, y_v, z_v);
//			int index = meshtools::Frame::matching_Matrix(new_f0, frames[end_cell]);
//			_Matrix_3 final_m = { SOM3[index][0][0], SOM3[index][0][1], SOM3[index][0][2], SOM3[index][1][0], SOM3[index][1][1], SOM3[index][1][2], SOM3[index][2][0], SOM3[index][2][1], SOM3[index][2][2] };
//			Matrix_3 Final_M(final_m);
//			for (int hf_i = 0; hf_i < adj_hfhs.size(); hf_i++)
//			{
//				index = matching_matrices[adj_hfhs[adj_hfhs.size() - 1 - hf_i]];
//				_Matrix_3 m = { SOM3[index][0][0], SOM3[index][0][1], SOM3[index][0][2], SOM3[index][1][0], SOM3[index][1][1], SOM3[index][1][2], SOM3[index][2][0], SOM3[index][2][1], SOM3[index][2][2] };
//				Final_M = Final_M * Matrix_3(m);
//			}
//			for (int row = 0; row < 3; row++)
//			{
//				for (int col = 0; col < 3; col++)
//				{
//					fm[row][col] = Final_M(row,col);
//				}
//			}
//		}
//		else
//		{
//			std::vector<OvmHaFaH> adj_hfhs;
//			OvmHaEgH current_heh = mesh->halfedge_handle(*iter, 0);
//			for (auto hehf_it = mesh->hehf_iter(current_heh); hehf_it; hehf_it++)
//			{
//				adj_hfhs.push_back(*hehf_it);
//			}
//			Matrix_3 Final_M(1);
//			for (int hf_i = 0; hf_i < adj_hfhs.size(); hf_i++)
//			{
//				int index = matching_matrices[adj_hfhs[adj_hfhs.size() - 1 - hf_i]];
//				_Matrix_3 m = { SOM3[index][0][0], SOM3[index][0][1], SOM3[index][0][2], SOM3[index][1][0], SOM3[index][1][1], SOM3[index][1][2], SOM3[index][2][0], SOM3[index][2][1], SOM3[index][2][2] };
//				Final_M = Final_M * Matrix_3(m);
//			}
//			for (int row = 0; row < 3; row++)
//			{
//				for (int col = 0; col < 3; col++)
//				{
//					fm[row][col] = Final_M.M[row][col];
//				}
//			}
//		}
//		int clo_idx = meshtools::Frame::find_closet_Matrix_index(fm);
//
//		if (clo_idx != regular_inx[0])
//		{
//			singular_ehs.insert(*iter);
//			if (sin_idx.find(clo_idx) != sin_idx.end())
//			{
//				//正常奇异线
//				//std::cout<<"Regular singularity "<<clo_idx<<std::endl;
//				regular_singular_ehs.insert(*iter);
//
//			}
//			else
//			{
//				//std::cout<<"Irrgular singularity"<<std::endl;
//				irregular_singular_ehs.insert(*iter);
//				//非有效奇异线
//			}
//		}
//	}
//}

Matrix_3 PrincipalStressField::cal_principal_direction(Matrix_3 &sigma)
{
	Eigen::SelfAdjointEigenSolver<Matrix_3> solver(sigma);
	auto real_values = solver.eigenvalues();
	double values[] = {real_values(0), real_values(1), real_values(2)};
	int order[] = {0, 1, 2};
	Matrix_3 vectors = solver.eigenvectors();

	//sort eigen values
	for (int i = 0; i < 2; i++)
	{
		for (int j = i + 1; j < 3; j++)
		{
			if (values[i] < values[j])
			{
				std::swap(values[i], values[j]);
				std::swap(order[i], order[j]);
			}
		}
	}

	Matrix_3 ret;
	for (int i = 0; i < 3; i++)
	{
		ret.col(i) = vectors.col(order[i]);
	}

	// check for right-handedness

	Eigen::Vector3d c = ret.col(0).cross(ret.col(1));
	// if not right-handed
	if (c.dot(ret.col(2)) < 0)
	{
		ret.col(2) = c;
	}

	return ret;
}

bool PrincipalStressField::resize(size_t elements_number)
{
	_n = elements_number;
	_stress.resize(elements_number);
	_frames.resize(elements_number);
	return true;
}

inline Matrix_3 PrincipalStressField::vectorFormToMatrixForm(double sigma[6])
{
	Matrix_3 stress_tensor;
	stress_tensor << sigma[0], sigma[3], sigma[5],
		sigma[3], sigma[1], sigma[4],
		sigma[5], sigma[4], sigma[2];
	return stress_tensor;
}
