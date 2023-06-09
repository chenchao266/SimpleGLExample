/*
  FLUIDS v.3 - SPH Fluid Simulator for CPU and GPU
  Copyright (C) 2012. Rama Hoetzlein, http://fluids3.com

  Fluids-ZLib license (* see part 1 below)
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. Acknowledgement of the
	 original author is required if you publish this in a paper, or use it
	 in a product. (See fluids3.com for details)
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/********************************************************************************************/
/*   PCISPH is integrated by Xiao Nie for NVIDIA’s 2013 CUDA Campus Programming Contest    */
/*                     https://github.com/Gfans/ISPH_NVIDIA_CUDA_CONTEST                    */
/*   For the PCISPH, please refer to the paper "Predictive-Corrective Incompressible SPH"   */
/********************************************************************************************/

#ifndef DEF_FLUID_SYS
#define DEF_FLUID_SYS

#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//#include "vector.h"
//#include "gl_helper.h"
//#include "xml_settings.h"
#include "global.h"
#include "vector_functions.h"
#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/geometry.h"
#include "Bullet3Common/b3Matrix3x3.h"
#include "fluid.h"
//#include "camera3d.h"

enum RUN_FLAG_E{
    RUN_CPU_SPH = 0,
    RUN_CUDA_INDEX_SPH = 1,
    RUN_CUDA_FULL_SPH = 2,
    RUN_CPU_PCISPH = 3,
    RUN_CUDA_INDEX_PCISPH = 4,
    RUN_CUDA_FULL_PCISPH = 5,
};

enum P_FLAG_E {
    // =标量参数,
    PRUN_MODE = 0,
    PMAXNUM = 1,
    PEXAMPLE = 2,
    PSIMSIZE = 3,
    PSIMSCALE = 4,
    PGRID_DENSITY = 5,
    PGRIDSIZEREALSCALE = 6,
    PVISC = 7,
    PRESTDENSITY = 8,
    PMASS = 9,
    PCOLLISIONRADIUS = 10,
    PSPACINGREALWORLD = 11,
    PSMOOTHRADIUS = 12,
    PGASCONSTANT = 13,
    PBOUNDARYSTIFF = 14,
    PBOUNDARYDAMP = 15,
    PACCEL_LIMIT = 16,
    PVEL_LIMIT = 17,
    PSPACINGGRAPHICSWORLD = 18,
    PGROUND_SLOPE = 19,
    PFORCE_MIN = 20,
    PFORCE_MAX = 21,
    PMAX_FRAC = 22,
    PDRAWMODE = 23,
    PDRAWSIZE = 24,
    PDRAWTEXT = 26,
    PCLR_MODE = 27,
    PPOINT_GRAV_AMT = 28,
    PSTAT_OCCUPANCY = 29,
    PSTAT_GRIDCOUNT = 30,
    PSTAT_NEIGHCNT = 31,
    PSTAT_NEIGHCNTMAX = 32,
    PSTAT_SEARCHCNT = 33,
    PSTAT_SEARCHCNTMAX = 34,
    PSTAT_PMEM = 35,
    PSTAT_GMEM = 36,
    PTIME_INSERT = 37,
    PTIME_SORT = 38,
    PTIME_COUNT = 39,
    PTIME_PRESS = 40,
    PTIME_FORCE = 41,
    PTIME_ADVANCE = 42,
    PTIME_RECORD = 43,
    PTIME_RENDER = 44,
    PTIME_TOGPU = 45,
    PTIME_FROMGPU = 46,
    PFORCE_FREQ = 47,
    PTIME_OTHER_FORCE = 48,
    PTIME_PCI_STEP = 49,
    PDENSITYERRORFACTOR = 50,
    PMINLOOPPCISPH = 51,
    PMAXLOOPPCISPH = 52,
    PMAXDENSITYERRORALLOWED = 53,
    PKERNELSELF = 54,
    PINITIALIZEDENSITY = 55,
};
enum G_FLAG_E {
    // =向量参数,
    PGRIDVOLUMEMIN = 0,
    PGRIDVOLUMEMAX = 1,
    PBOUNDARYMIN = 2,
    PBOUNDARYMAX = 3,
    PINITPARTICLEMIN = 4,
    PINITPARTICLEMAX = 5,
    PEMIT_POS = 6,
    PEMIT_ANG = 7,
    PEMIT_DANG = 8,
    PEMIT_SPREAD = 9,
    PEMIT_RATE = 10,
    PPOINT_GRAV_POS = 11,
    PPLANE_GRAV_DIR = 12,
};
enum B_FLAG_E {
    // =布尔参数,
    PPAUSE = 0,
    PDEBUG = 1,
    PUSE_CUDA = 2,
    PUSE_GRID = 3,
    PWRAP_X = 4,
    PWALL_BARRIER = 5,
    PLEVY_BARRIER = 6,
    PDRAIN_BARRIER = 7,
    PPLANE_GRAV_ON = 11,
    PPROFILE = 12,
    PCAPTURE = 13,
    PDRAWGRIDCELLS = 14,
    PPRINTDEBUGGINGINFO = 15,
    PDRAWDOMAIN = 16,
    PDRAWGRIDBOUND = 17,
    PUSELOADEDSCENE = 18,
};
const int max_num_adj_grid_cells_cpu = 27;

// 使用SOA 代替 AOS 结构体Particle只是用来计算粒子所占用内存大小的辅助数据结构
struct Particle 
{															// offset - TOTAL: 120 (must be multiple of 12 = sizeof(b3Vector3) )
	b3Vector3		fpos;									// 0
	b3Vector3		fpredicted_pos;							// 12
	b3Vector3		fvel;									// 24
	b3Vector3		fvel_eval;								// 36
	b3Vector3		fcorrection_pressure_force;				// 48
	b3Vector3		fforce;									// 60
	b3Vector3		fsum_gradw;								// 72
	float			fsum_gradw_dot;							// 84
	float			fpressure;								// 88
	float			fcorrection_pressure;					// 92
	float			fdensity_reciprocal;					// 96
	float			fdensityError;							// 100
	int				fparticle_grid_cell_index;				// 104
	int				fnext_particle_index_in_the_same_cell;	// 108			
	unsigned			fclr;									// 112
	int				fpadding1;								// 116
	int				fpadding2;								// 120	填充字节 成员对齐，参见：《高质量程序设计指南C/C++语言》（第三版）P147， 8.1.4 成员对齐
};
class   Timer;
class ParticleSystem {
public:
	ParticleSystem ();
	~ParticleSystem();

	// 渲染函数
	void		Draw ( b3Matrix3x3& cam, float rad );
	void		DrawDomain(b3Vector3& domain_min, b3Vector3& domain_max);
	void		DrawGrid ();
	void		DrawText ();
	void		DrawCell ( int gx, int gy, int gz );
	void		DrawParticle ( int p, int r1, int r2, b3Vector3 clr2 );
	void		DrawParticleInfo ();
	void		DrawParticleInfo ( int p );
	void		DrawNeighbors ( int p );
	void		DrawCircle ( b3Vector3 pos, float r, b3Vector3 clr, b3Matrix3x3& cam );

	// 实用工具函数
	void		AllocateTemporalParticlesMemory ( uint num_particles );
	void		AllocateParticlesMemory ( int cnt );
	int			AddParticleToBuf ();
	int			num_points ();
	void		DeallocateTemporalParticleMemory();

	// 设置函数
	void		Setup ( bool bStart );
	void		SetupRender ();
	void		SetupKernels ();
	void		SetupDefaultParams ();				
	void		SetupExampleParams ( bool bStart );
	void		SetupSpacing ();
	void		SetupBoundaryParams();
	void		SetupSampleParticleVolumePCISPH ( const b3Vector3 & minCorner, const b3Vector3 & maxCorner, const float particleSpacing, const float jitter);
	void		SetupInitParticleVolume ( const b3Vector3 &minCorner, const b3Vector3 &maxCorner, const float particleSpacing, const float jitter );
	void		SetupAdditonalParticleVolume ( const b3Vector3 &minCorner, const b3Vector3 &maxCorner, const float particleSpacing, const float jitter ,const int numParticles);
	void		SetupInitParticleVolumeLoad(const b3Vector3& minVec, const b3Vector3& maxVec);
	void		SetupInitParticleVolumeFromFile(const b3Vector3& minVec, const b3Vector3& maxVec);
	void		SetupSampleGridAllocatePCISPH ( const b3Vector3 &domain_volume_min, const b3Vector3 &domain_volume_max, const float sim_scale, const float cell_size, const float border );
	void		SetupGridAllocate ( const b3Vector3 &grid_volume_min, const b3Vector3 &grid_volume_max, const float sim_scale, const float cell_size, const float border );
	void		ParseXML ( std::string name, int id, bool bStart );

	//邻居查找函数
	void		Search ();
	void		InsertParticlesCPU (uint num_particle);
	void		FindNeighborsGrid (uint num_particle);

	// 运行
	void		Run(int w, int h);
	void		RunCPUSPH();
	void		RunCUDAIndexSPH();
	void		RunCUDAFullSPH();
	void		RunCPUPCISPH();
	void		RunCUDAIndexPCISPH();
	void		RunCUDAFullPCISPH();

	void		computeGasConstAndTimeStep(float densityVariation);
	void		ComputeOtherForceCpu();
	void		PredictionCorrectionStepCpu();
	void		predictPositionAndVelocity(uint index);
	void		computePredictedDensityAndPressure(uint index);
	void		computeCorrectivePressureForce(uint index);
	void        ComputeDensityErrorFactor(uint num_particles);
	void		CreatePreParticlesISPH(uint num_particles);
	void		ComputeGradWValues(uint num_particles);
	void		ComputeGradWValuesSimple(uint num_particles, uint& max_num_neighbors, uint& index);
	void		ComputeFactor(uint num_particles);
	void		ComputeFactorSimple(uint num_particles, uint& max_num_neighbors, uint index);
	void		collisionHandlingSimScale(b3Vector3& pos, b3Vector3& vel);

	// 物理模拟
	void		BoundaryCollisions(b3Vector3* ipos, b3Vector3* iveleval, b3Vector3& acceleration);
	void		AdvanceStepCPU (float time_step);
	void		AdvanceStepSimpleCollision (float time_step);
	void		ExitParticleSystem ();
	void		TransferToCUDA ();
	void		TransferFromCUDA ();

	float		time_step();

	// 模型加载
	void		readInFluidParticles(const char* filename, int& num_points, b3Vector3& minVec, b3Vector3& maxVec);
	int			readInFluidParticleNum(const char* filename);

	// 调试
	void		SaveResults ();
	void		DebugPrintMemory ();
	void		Record ( int param, std::string, Timer& start );
	int			SelectParticle ( int x, int y, int wx, int wy, b3Matrix3x3& cam );
	int		    selected ();

	// 加速数据结构---网格
	int		    GetGridCell ( int p, Vec3i& gc );
	int		    GetGridCell ( b3Vector3& p, Vec3i& gc );
	int		    grid_total ();	
	int			grid_adj_cnt ();		
	Vec3i   GetCell ( int gc );
    Vec3i   grid_res ();
	b3Vector3   grid_min ();		
	b3Vector3   grid_max ();		
	b3Vector3   grid_delta ();

	// 加速数据结构---邻居表
	void		ClearNeighborTable ();
	void		ResetNeighbors ();
	int			neighbor_num ();
	void		ClearNeighbors ( int i );
	int			AddNeighbor();
	int			AddNeighbor( int i, int j, float d );

	// SPH函数	
	void		ComputePressureGrid ();			
	void		ComputeForceGrid ();			
	void		ComputeForceGridNC ();					

	// GPU实用函数
	void		AllocatePackBuf ();

	// 获取模拟方法
	std::string GetModeStr ();

	// 参数设置			
	void		SetParam (int p, float v );
	void		SetParam (int p, int v );
	float		GetParam ( int p );
	float		GetDT ();
	float		SetParam ( int p, float v, float mn, float mx );
	float		IncParam ( int p, float v, float mn, float mx );
	b3Vector3   GetVec ( int p );			
	void		SetVec ( int p, b3Vector3 v );	
	void		Toggle ( int p );				
	bool		GetToggle ( int p );		
	std::string	GetSceneName ();

	// 光滑核函数
	float kernelM4(float dist, float sr);
	float kernelM4Lut(float dist, float sr);
	float kernelPressureGrad(float dist, float sr);	
	float kernelPressureGradLut(float dist, float sr);	

	// 边界碰撞处理
	b3Vector3 boxBoundaryForce(const uint i);

	std::vector<FluidParticle> particles;

private:

	std::string	scene_name_;

	int			frame_;		
	float		time_step_;	
	float		time_step_sph_;
	float		time_step_wcsph_;
	float		time_step_pcisph_;
	float		time_;	

	// 模拟参数
	float		param_  [ MAX_PARAM ];							
	b3Vector3	vec_	[ MAX_PARAM ];
	bool		toggle_ [ MAX_PARAM ];

	// SPH光滑核函数系数
	float		poly6_kern_;
	float		lap_kern_;
	float		spiky_kern_;		

	int			num_points_;
	int			max_points_;
	int			good_points_;

	b3Vector3*	pos_;											
	b3Vector3*	predictedPosition_;								
	b3Vector3*	vel_;
	b3Vector3*	vel_eval_;
	b3Vector3*	correction_pressure_force_;
	b3Vector3*	force_;
	b3Vector3*  sumGradW_;
	float*		sumGradWDot_;
	float*		pressure_;
	float*		correction_pressure_;
	float*		density_;	
	float*		predicted_density_;
	float*      densityError_;
	float*		max_predicted_density_array_;
	uint*		particle_grid_cell_index_;
	uint*		next_particle_index_in_the_same_cell_;	
	uint*		index_;		
	unsigned*		clr_;

	uint*		cluster_cell_;
	ushort*		age_;
	uint*		neighbor_index_;
	uint*		neighbor_particle_numbers_;

	// 加速数据结构---网格相关变量
	uint*		grid_head_cell_particle_index_array_;			
	uint*		grid_particles_number_;
	int			grid_total_;									
	Vec3i	grid_res_;										
	b3Vector3	grid_min_;										
	b3Vector3	grid_max_;		
	b3Vector3	grid_size_;										
	b3Vector3	grid_delta_;									
	int			grid_search_;
	int			grid_adj_cnt_;
	int			grid_neighbor_cell_index_offset_[max_num_adj_grid_cells_cpu];

	// 加速数据结构---邻居表相关变量
	int			neighbor_particles_num_;
	int			neighbor_particles_max_num_;
	int*		neighbor_table_;
	float*		neighbor_dist_;									

	char*		pack_fluid_particle_buf_;						
	int*		pack_grid_buf_;									

	int			vbo_[3];					

	int			sphere_points_;
	int			texture_[1];

	int			selected_;

	uint		image_;

	float maxPredictedDensity;

	uint*		save_neighbor_index_;
	uint*		save_neighbor_cnt_;
	uint*		save_neighbors_;

	// XML 设置文件
	//XmlSettings	xml;

	static int const lutSize = 100000;
	float lutKernelM4[lutSize];
	float lutKernelPressureGrad[lutSize];

	// 边界碰撞处理相关变量
	bool addBoundaryForce;
	float maxBoundaryForce;
	float boundaryForceFactor;
	float forceDistance;

    std::ofstream outfileParticles;
    std::ifstream infileParticles;

};	

inline void		
	ParticleSystem::DrawParticleInfo ()		
{ 
	DrawParticleInfo ( selected_ ); 
}

inline int		
	ParticleSystem::num_points ()
{
	return num_points_; 
}

inline float		
	ParticleSystem::time_step ()
{
	return time_step_; 
}

inline int		
	ParticleSystem::selected ()
{
	return selected_;
}

inline int		
	ParticleSystem::grid_total ()
{ 
	return grid_total_; 
}

inline int		
	ParticleSystem::grid_adj_cnt ()
{ 
	return grid_adj_cnt_; 
}

inline Vec3i
	ParticleSystem::grid_res ()
{ 
	return grid_res_;
}

inline b3Vector3		
	ParticleSystem::grid_min ()
{ 
	return grid_min_;
}

inline b3Vector3		
	ParticleSystem::grid_max ()
{ 
	return grid_max_;
}

inline b3Vector3		
	ParticleSystem::grid_delta ()
{ 
	return grid_delta_;
}


inline int		
	ParticleSystem::neighbor_num ()
{ 
	return neighbor_particles_num_;
}

inline void		
	ParticleSystem::SetParam (int p, float v )
{ 
	param_[p] = v;
}

inline void		
	ParticleSystem::SetParam (int p, int v )
{ 
	param_[p] = (float) v;
}

inline float		
	ParticleSystem::GetParam ( int p )
{ 
	return (float) param_[p];
}

inline float
	ParticleSystem::GetDT()
{
	return time_step_;
}

inline float		
	ParticleSystem::SetParam ( int p, float v, float mn, float mx )
{ 
	param_[p] = v ; 
	if ( param_[p] > mx ) 
		param_[p] = mn; 

	return param_[p];
}

inline float		
	ParticleSystem::IncParam ( int p, float v, float mn, float mx )
{ 
	param_[p] += v; 
	if ( param_[p] < mn ) param_[p] = mn; 
	if ( param_[p] > mx ) param_[p] = mn; 
	return param_[p];
}

inline b3Vector3		
	ParticleSystem::GetVec ( int p )	
{ 
	return vec_[p];
}

inline void		
	ParticleSystem::SetVec ( int p, b3Vector3 v )	
{ 
	vec_[p] = v;
}

inline void		
	ParticleSystem::Toggle ( int p )
{ 
	toggle_[p] = !toggle_[p];
}

inline bool		
	ParticleSystem::GetToggle ( int p )	
{ 
	return toggle_[p];
}

inline std::string		
	ParticleSystem::GetSceneName ()
{ 
	return scene_name_;
}

#endif
