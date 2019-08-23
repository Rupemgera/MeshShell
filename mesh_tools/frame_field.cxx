#include "frame_field.h"

namespace meshtools {

	Frame::Frame()
	{
		center = OvmVec3d(0, 0, 0);
		x_vec = OvmVec3d(1, 0, 0);
		y_vec = OvmVec3d(0, 1, 0);
		z_vec = OvmVec3d(0, 0, 1); 
		tag = -2;
		ch = (OvmCeH)-1;
	}

	Matrix_3 Frame::m()
	{
		//_Matrix_3 temp = { this->x_vec[0], this->x_vec[1], this->x_vec[2], this->y_vec[0], this->y_vec[1], this->y_vec[2], this->z_vec[0], this->z_vec[1], this->z_vec[2] };
		//return Matrix_3(temp);
		Matrix_3 tmp;
		tmp << this->x_vec[0], this->x_vec[1], this->x_vec[2],
			this->y_vec[0], this->y_vec[1], this->y_vec[2],
			this->z_vec[0], this->z_vec[1], this->z_vec[2];
		return tmp;
		
	}

	void Frame::print_Frame()
	{
		for (int row_i = 0; row_i < 3; row_i++)
		{
			std::cout << x_vec[row_i] << " " << y_vec[row_i] << " " << z_vec[row_i] << std::endl;
		}
	}

	int matching_Matrix(Frame &f1, Frame &f2)
	{
		Matrix_3 m1(f1.m().transpose()), m2(f2.m().transpose());
		std::vector<double> all_dif;
		for (int n = 0; n < 24; n++) {
			/*_Matrix_3 t = { SOM3[n][0][0], SOM3[n][1][0], SOM3[n][2][0], SOM3[n][0][1], SOM3[n][1][1], SOM3[n][2][1], SOM3[n][0][2], SOM3[n][1][2], SOM3[n][2][2] };
			Matrix_3 temp(t);*/
			Matrix_3 temp;
			temp << SOM3[n][0][0], SOM3[n][1][0], SOM3[n][2][0],
				SOM3[n][0][1], SOM3[n][1][1], SOM3[n][2][1],
				SOM3[n][0][2], SOM3[n][1][2], SOM3[n][2][2];
			Matrix_3 transform_matrix = m2 * temp;
			double dif(0.0);
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					dif += (m1(i,j) - transform_matrix(i,j)) * (m1(i,j) - transform_matrix(i,j));
				}
			}
			all_dif.push_back(dif);
		}

		double min_dif(1000);
		int min_index;
		for (int n = 0; n < 24; n++)
			if (all_dif[n] < min_dif) {
				min_dif = all_dif[n];
				min_index = n;
			}
		return min_index;
	}

	void set_SOM3()
	{
		int count = 0;
		for (int i = 0; i < 3; i++) {
			for (int p = 0; p < 2; p++) {
				for (int j = 0; j < 3; j++) {
					if (j != i) {
						for (int q = 0; q < 2; q++) {
							SOM3[count][0][i] = pow(-1.0, p + 1);
							SOM3[count][1][j] = pow(-1.0, q);
							SOM3[count][2][0] = SOM3[count][0][1] * SOM3[count][1][2] - SOM3[count][0][2] * SOM3[count][1][1];
							SOM3[count][2][1] = SOM3[count][0][2] * SOM3[count][1][0] - SOM3[count][0][0] * SOM3[count][1][2];
							SOM3[count][2][2] = SOM3[count][0][0] * SOM3[count][1][1] - SOM3[count][0][1] * SOM3[count][1][0];
							count++;
						}
					}
				}
			}
		}
		//ֽ�淨�����Ź̶��������⣬��1��6��7Ϊ˳ʱ��ת90�� 3 4 9Ϊ��ʱ��90�� 2 5 8Ϊת180��
		for (int m_ind = 0; m_ind < 24; m_ind++)
		{
			if (SOM3[m_ind][0][0] == 1)
			{
				if (SOM3[m_ind][1][1] == 1 && SOM3[m_ind][2][2] == 1)
				{
					regular_inx[0] = m_ind;
				}
				else if (SOM3[m_ind][1][2] == -1 && SOM3[m_ind][2][1] == 1)
				{
					regular_inx[1] = m_ind;
				}
				else if (SOM3[m_ind][1][1] == -1 && SOM3[m_ind][2][2] == -1)
				{
					regular_inx[2] = m_ind;
				}
				else if (SOM3[m_ind][1][2] == 1 && SOM3[m_ind][2][1] == -1)
				{
					regular_inx[3] = m_ind;
				}
			}
			else if (SOM3[m_ind][1][1] == 1)
			{
				if (SOM3[m_ind][0][2] == -1 && SOM3[m_ind][2][0] == 1)
				{
					regular_inx[4] = m_ind;
				}
				else if (SOM3[m_ind][0][0] == -1 && SOM3[m_ind][2][2] == -1)
				{
					regular_inx[5] = m_ind;
				}
				else if (SOM3[m_ind][0][2] == 1 && SOM3[m_ind][2][0] == -1)
				{
					regular_inx[6] = m_ind;
				}
			}
			else if (SOM3[m_ind][2][2] == 1)
			{
				if (SOM3[m_ind][0][1] == -1 && SOM3[m_ind][1][0] == 1)
				{
					regular_inx[7] = m_ind;
				}
				else if (SOM3[m_ind][0][0] == -1 && SOM3[m_ind][1][1] == -1)
				{
					regular_inx[8] = m_ind;
				}
				else if (SOM3[m_ind][0][1] == 1 && SOM3[m_ind][1][0] == -1)
				{
					regular_inx[9] = m_ind;
				}
			}
		}
	}

	int find_closet_Matrix_index(_Matrix_3 M)
	{
		double closet_dis = 18;
		double current_dis;
		int closet_idx = -1;
		for (int m_ind = 0; m_ind < 24; m_ind++)
		{
			current_dis = 0;
			for (int row_ind = 0; row_ind < 3; row_ind++)
			{
				for (int col_ind = 0; col_ind < 3; col_ind++)
				{
					current_dis += abs(M[row_ind][col_ind] - SOM3[m_ind][row_ind][col_ind]);
				}
			}
			if (current_dis < closet_dis)
			{
				closet_dis = current_dis;
				closet_idx = m_ind;
			}
		}
		return closet_idx;
	}
}