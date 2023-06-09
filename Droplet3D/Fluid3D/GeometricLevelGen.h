﻿#ifndef _geo_level_gen_h_
#define _geo_level_gen_h_


#include <iostream>
#include <vector>
#include <unordered_map>
#include "tbb/tbb.h"
#include "util.h"
#include "vec.h"
#include <math.h>
#include "sparse_matrix.h"
#include "blas_wrapper.h"

 
//using namespace LosTopos;
template<class T>
struct levelGen{
	void generateRP(const FixedSparseMatrix<T> &A,
		FixedSparseMatrix<T> &R,
		FixedSparseMatrix<T> &P,
		int ni, int nj, int nk);



	void generateLevelsGalerkinCoarsening(std::vector<FixedSparseMatrix<T> *> &A_L,
		std::vector<FixedSparseMatrix<T> *> &R_L,
		std::vector<FixedSparseMatrix<T> *> &P_L,
		std::vector<Vec3i>                 &S_L,
		int & total_level,
		//given
		FixedSparseMatrix<T> &A,
		int ni,int nj,int nk);

	void generateRPCompressed(const FixedSparseMatrix<T> &A,
		FixedSparseMatrix<T> &R,
		FixedSparseMatrix<T> &P,
		std::vector<bool>         &pattern,
		std::vector<char>         &mask,
		std::vector<int>          &index_table,
		std::vector<char>         &mask_this_level,
		std::vector<int>          &idxTable_this_level,
		int ni, int nj, int nk)
	{
		generatePattern(mask,index_table,pattern,ni,nj,nk);
		int nni = ceil((float)ni/2.0);
		int nnj = ceil((float)nj/2.0);
		int nnk = ceil((float)nk/2.0);
		SparseMatrix<T> r(0, 0);
		SparseMatrix<T> p(0, 0);
		p.resize(A.n, A.n);
		p.zero();
		//generate index table for this level
		mask_this_level.resize(nni*nnj*nnk);
		idxTable_this_level.resize(nni*nnj*nnk);
		mask_this_level.assign(mask_this_level.size(),0);
		idxTable_this_level.assign(idxTable_this_level.size(),0);
		int compute_num = ni*nj*nk;
		int slice = ni*nj;
		tbb::parallel_for(0,compute_num,1,[&](int thread_idx)
		{
			int k = thread_idx/slice;
			int j = (thread_idx%slice)/ni;
			int i = thread_idx%ni;

			{
				int ii = i/2, jj = j/2, kk = k/2;
				int index = ii + nni*jj + nni*nnj*kk;
				int upper_idx = i + j*ni + ni*nj*k;
				if(mask[upper_idx] == 1) 
				{
					mask_this_level[index]=1;
				}
			}
		});
		idxTable_this_level[0] = mask_this_level[0]-1;
		for (int i=1;i<idxTable_this_level.size();i++)
		{
			idxTable_this_level[i] = idxTable_this_level[i-1]+mask_this_level[i];
		}
		int unknowns = idxTable_this_level[idxTable_this_level.size()-1]+1;
		assert(unknowns>0);
		r.resize(unknowns, unknowns);
		r.zero();


		for(int k=0;k<nnk;k++)for(int j=0;j<nnj;j++)for (int i=0;i<nni;i++)
		{
			unsigned int index = (k*nnj +j)*nni + i;
			for(int kk=0;kk<=1;kk++)for(int jj=0;jj<=1;jj++)for(int ii=0;ii<=1;ii++)
			{
				int iii = i*2 + ii;
				int jjj = j*2 + jj;
				int kkk = k*2 + kk;
				if(iii<ni && jjj<nj && kkk<nk)
				{
					unsigned int index2 = (kkk*nj + jjj)*ni + iii;
					if (mask[index2]==1 && mask_this_level[index]==1)
					{
						r.set_element(idxTable_this_level[index], 
							index_table[index2], (T)0.125);
						p.set_element(index_table[index2],
							idxTable_this_level[index], 1.0);
					}

				}
			}
		}


		R.construct_from_matrix(r);
		P.construct_from_matrix(p);
		r.clear();
		p.clear();

	}

	void generatePattern(std::vector<char>         &mask,
		std::vector<int>          &index_table,
		std::vector<bool>         &pattern,
		int ni, int nj, int nk)
	{
		pattern.resize(index_table[index_table.size()-1]+1);
		pattern.assign(pattern.size(),false);
		int compute_num = ni*nj*nk;
		int slice = ni*nj;
		tbb::parallel_for(0,compute_num,1,[&](int thread_idx)
		{
			int k = thread_idx/slice;
			int j = (thread_idx%slice)/ni;
			int i = thread_idx%ni;
			if((i+j+k)%2 == 0)
			{
				int index = k*nj*ni + j*ni + i;
				if(mask[index]==1)
				{
					pattern[index_table[index]] = true;
				}
			}


		});
	}

	void generateLevelsGalerkinCoarseningCompressed(std::vector<FixedSparseMatrix<T> *> &A_L,
		std::vector<FixedSparseMatrix<T> *> &R_L,
		std::vector<FixedSparseMatrix<T> *> &P_L,
		std::vector<std::vector<bool>*>           &b_L,
		int & total_level,
		//given
		FixedSparseMatrix<T> &A,
		std::vector<char>         &mask,
		std::vector<int>          &index_table,
		int ni,int nj,int nk){
        std::cout<<"building levels ...... "<< std::endl;
			std::vector<Vec3i> S_L;
			std::vector<char>          mask0;
			std::vector<int>           idxTable0;
			A_L.resize(0);
			R_L.resize(0);
			P_L.resize(0);
			b_L.resize(0);
			total_level = 1;
			A_L.push_back(&A);
			mask0 = mask;
			idxTable0 = index_table;
			S_L.push_back(Vec3i(ni,nj,nk));
			b_L.push_back(new std::vector<bool>);
			int nni = ni, nnj = nj, nnk = nk;
			unsigned int unknowns = A.n;
			while (unknowns > 4096)
			{
				A_L.push_back(new FixedSparseMatrix<T>);
				R_L.push_back(new FixedSparseMatrix<T>);
				P_L.push_back(new FixedSparseMatrix<T>);
				nni = ceil((float)nni/2.0);
				nnj = ceil((float)nnj/2.0);
				nnk = ceil((float)nnk/2.0);

				S_L.push_back(Vec3i(nni,nnj,nnk));
				b_L.push_back(new std::vector<bool>);

				int i = total_level - 1;
				std::vector<char> mask_temp;
				std::vector<int> idxTable_temp;
				//printf("generating R and P\n");
				generateRPCompressed(*(A_L[i]), *(R_L[i]),*(P_L[i]), *(b_L[i]),
					mask0, idxTable0,
					mask_temp, idxTable_temp,
					S_L[i].u[0], S_L[i].u[1],S_L[i].u[2]);
				//printf("generating R and P done!\n");
				//printf("%d,%d,%d\n",A_L[i]->n, P_L[i]->n, R_L[i]->n);
				FixedSparseMatrix<T> temp;
				multiplyMat(*(A_L[i]),*(P_L[i]),temp,1.0);
				multiplyMat(*(R_L[i]),temp, *(A_L[i+1]),0.5);
				//printf("multiply matrix done\n");
				temp.resize(0, 0);
				temp.clear();
				mask0 = mask_temp;
				idxTable0 = idxTable_temp;
				idxTable_temp.resize(0);idxTable_temp.shrink_to_fit();
				mask_temp.resize(0);    mask_temp.shrink_to_fit();


				unknowns = A_L[i+1]->n;
				total_level++;
			}


			generatePattern(mask0, idxTable0,
				*(b_L[total_level-1]), 
				S_L[total_level-1].u[0], 
				S_L[total_level-1].u[1], 
				S_L[total_level-1].u[2]);




            std::cout<<"build levels done"<< std::endl;
	}







	void generateRPSparse(const FixedSparseMatrix<T> &A,
		FixedSparseMatrix<T> &R,
		FixedSparseMatrix<T> &P,
		std::vector<bool>         &pattern,
		std::vector<Vec3i>		 &Dof_ijk_fine,
		std::vector<Vec3i>	     &Dof_ijk_coarse,
		int ni, int nj, int nk)
	{
		generatePatternSparse(Dof_ijk_fine,pattern);
		int nni = ceil((float)ni/2.0);
		int nnj = ceil((float)nj/2.0);
		int nnk = ceil((float)nk/2.0);
		SparseMatrix<T> r(0, 0);
		SparseMatrix<T> p(0, 0);
		p.resize(A.n, A.n);
		p.zero();
		Dof_ijk_coarse.resize(0);
		std::unordered_map<unsigned int, unsigned int> index_mapping;
		//generate index table for this level
		for (unsigned int idx_f=0;idx_f<Dof_ijk_fine.size();idx_f++)
		{
			Vec3i ijk = Dof_ijk_fine[idx_f];
			int coarse_i = ijk[0]/2, coarse_j = ijk[1]/2, coarse_k = ijk[2]/2;
			unsigned int idx = coarse_i + coarse_j*nni + coarse_k*nni*nnj;
			if(index_mapping.find(idx)==index_mapping.end())
			{
				unsigned int idx_c = Dof_ijk_coarse.size();
				index_mapping[idx] = idx_c;
				p.set_element(idx_f,idx_c,1.0f);
				//r.set_element(idx_c,idx_f,0.25f);
				Dof_ijk_coarse.push_back(Vec3i(coarse_i,coarse_j,coarse_k));
			}
			else
			{
				unsigned int idx_c = index_mapping[idx];
				p.set_element(idx_f,idx_c,1.0f);
				//r.set_element(idx_c,idx_f,0.25f);
			}
		}
		r.resize(Dof_ijk_coarse.size(), Dof_ijk_coarse.size());

		for (unsigned int idx_f=0;idx_f<Dof_ijk_fine.size();idx_f++)
		{
			Vec3i ijk = Dof_ijk_fine[idx_f];
			int coarse_i = ijk[0]/2, coarse_j = ijk[1]/2, coarse_k = ijk[2]/2;
			unsigned int idx = coarse_i + coarse_j*nni + coarse_k*nni*nnj;
			unsigned int idx_c = index_mapping[idx];
			r.set_element(idx_c,idx_f,0.125f);
		}



		R.construct_from_matrix(r);
		P.construct_from_matrix(p);
		r.clear();
		p.clear();

	}

	void generatePatternSparse
		(std::vector<Vec3i>       &Dof_ijk,
		std::vector<bool>         &pattern)
	{
		pattern.resize(Dof_ijk.size());
		tbb::parallel_for((size_t)0,
			(size_t)Dof_ijk.size(),
			(size_t)1,
			[&](size_t index)
		{
			Vec3i ijk = Dof_ijk[index];
			if ((ijk[0]+ijk[1]+ijk[2])%2==1)
			{
				pattern[index] = true;
			}
			else
			{
				pattern[index] = false;
			}
		});
	}

	void generateLevelsGalerkinCoarseningSparse
		(std::vector<FixedSparseMatrix<T> *> &A_L,
		std::vector<FixedSparseMatrix<T> *> &R_L,
		std::vector<FixedSparseMatrix<T> *> &P_L,
		std::vector<std::vector<bool>*>           &b_L,
		int & total_level,
		//given
		FixedSparseMatrix<T> &A,
		std::vector<Vec3i> & Dof_ijk,
		int ni, int nj, int nk){
        std::cout<<"building levels ...... "<< std::endl;
			std::vector<Vec3i> Dof_ijk_fine;
			std::vector<Vec3i> Dof_ijk_coarse;
			std::vector<Vec3i> S_L;
			Dof_ijk_fine = Dof_ijk;
			A_L.resize(0);
			R_L.resize(0);
			P_L.resize(0);
			b_L.resize(0);
			total_level = 1;
			A_L.push_back(&A);
			S_L.push_back(Vec3i(ni,nj,nk));
			b_L.push_back(new std::vector<bool>);
			int nni = ni, nnj = nj, nnk = nk;
			unsigned int unknowns = A.n;
			while (unknowns > 4096)
			{
				A_L.push_back(new FixedSparseMatrix<T>);
				R_L.push_back(new FixedSparseMatrix<T>);
				P_L.push_back(new FixedSparseMatrix<T>);
				nni = ceil((float)nni/2.0);
				nnj = ceil((float)nnj/2.0);
				nnk = ceil((float)nnk/2.0);

				S_L.push_back(Vec3i(nni,nnj,nnk));
				b_L.push_back(new std::vector<bool>);

				int i = total_level - 1;
				//printf("generating R and P\n");
				generateRPSparse(*(A_L[i]), *(R_L[i]),*(P_L[i]), *(b_L[i]),
					Dof_ijk_fine,Dof_ijk_coarse,
					S_L[i].u[0], S_L[i].u[1],S_L[i].u[2]);
				Dof_ijk_fine.resize(0); Dof_ijk_fine.shrink_to_fit();
				Dof_ijk_fine = Dof_ijk_coarse;
				//printf("generating R and P done!\n");
				//printf("%d,%d,%d\n",A_L[i]->n, P_L[i]->n, R_L[i]->n);
				FixedSparseMatrix<T> temp;
				multiplyMat(*(A_L[i]),*(P_L[i]),temp,1.0);
				multiplyMat(*(R_L[i]),temp, *(A_L[i+1]),0.5);
				//printf("multiply matrix done\n");
				temp.resize(0, 0);
				temp.clear();


				unknowns = A_L[i+1]->n;
				total_level++;
			}


			generatePatternSparse(Dof_ijk_fine,
				*(b_L[total_level-1]));

			Dof_ijk_fine.resize(0);
			Dof_ijk_fine.shrink_to_fit();


			std::cout<<"build levels done"<< std::endl;
	}




};




#endif