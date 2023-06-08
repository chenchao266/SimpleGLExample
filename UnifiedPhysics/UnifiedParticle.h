#ifndef GPU_UNIFIED_PARTICLE_H_
#define GPU_UNIFIED_PARTICLE_H_

#include "UnifiedConstants.h"
#include "Bullet3Common/b3Vector3.h"

#include "Bullet3Common/geometry.h"
#include "zIndex.h"

//typedef vmml::Vector3<unsigned short int> Vector3ui;

extern ZIndex zIndex;

enum ParticleObjectType
{
	RIGID_PARTICLE		= 0,		// rigid body
	SOFT_PARTICLE		= 1,		// soft/deformable body
	LIQUID_PARTICLE		= 2,		// liquids
	GRANULAR_PARTICLE	= 3,		// granular materials
	CLOTH_PARTICLE		= 4,		// cloth, shell, thin object, etc...
	SMOKE_PARTICLE		= 5,		// smoke
	FROZEN_PARTICLE		= 6			// frozen boundaries (static or kinematic boundaries)
};

enum DisplayType {
	SHOW_SPHERE,
	SHOW_POINT
};

class RigidBody;

struct UnifiedParticle 
{
	UnifiedParticle();
	UnifiedParticle(float x, float y, float z, double rest_density, double particle_mass, int type);
	UnifiedParticle(const b3Vector3& pos, const int type, const b3Vector3& vel, const float corr_pressure, const float predicted_density, const float rest_density, double particle_mass);
	~UnifiedParticle();

	float GetPositionX();
	float GetPositionY();
	float GetPositionZ();

	void CalculateTempColor();

	// --- elastic/rigid body properties ---
	/**
	* Calculates hookeMatrix from young_modulus and poissonRatio assuming an
	* isotropic material
	*/
	void CalculateHookeMatrix();

	void SetPosition(float new_x, float new_y, float new_z)
	{
		position_.setValue(new_x, new_y, new_z);
	}

	void SetPosition(b3Vector3 &new_pos)
	{
		position_ = new_pos;
	}

	inline bool 
		operator<(const UnifiedParticle& p) const
	{
		return index_ < p.index_;
	}

	inline bool 
		operator>(const UnifiedParticle& p) const
	{
		return index_ > p.index_;
	}

	inline bool 
		operator<=(const UnifiedParticle& p) const
	{
		return index_ <= p.index_;
	}

	inline bool 
		operator>=(const UnifiedParticle& p) const
	{
		return index_ >= p.index_;
	}

	inline bool 
		operator<(const unsigned int i) const
	{
		return index_ < i;
	}

	void SetParticleParameters();

	// physics quantities
	b3Vector3 position_;
	b3Vector3 velocity_;	
	b3Vector3 velocity_leapfrog_;	
	b3Vector3 force_;
	b3Vector3 pressure_force_;
	b3Vector3 init_relative_pos_;	// initial relative position with respect to center of mass 

	b3Vector3 color_;

	float density_;
	float pressure_;	
	float particle_mass_;	
	float rest_density_;
	float visc_const_;
	float gas_const_;	

	// V_bi : for boundary particles, it's the weighted volume of boundary particle according to E.q(4) from paper "Versatile Rigid-Fluid Coupling for Incompressible SPH"
	float weighted_volume_; 
	float shepard_filter_den_kernel_factor_;		// Shepard filter corrected density kernel factor 1.0f / Sum_Wc(x) according to E.q(2) from paper "Direct forcing for Lagrangian rigid-fluid coupling"

	int type_;	// particle type -> see ParticleObjectType

	// PCISPH params
	float correction_pressure_;
	float previous_correction_pressure_;
	b3Vector3 correction_pressure_force_;
	b3Vector3 predicted_position_;
	b3Vector3 predicted_velocity_;
	float predicted_density_;			// Note: here we also use predictedDensity to function as previousPredictedDensity does in CUDA version.
	float density_error_;
	b3Vector3 sum_grad_w_;
	float sum_grad_w_dot_;	

	b3Vector3 normalized_normal_;	// keep track of boundary particle normal information (it should be normalized)

	// temperature diffusion
	float temperature_;

	// --- elastic/rigid body properties ---

	float solid_volume_;			// The volume of the particle in body space
	float hooke_matrix_[6][6];	// Stiffness matrix for a hookean material (stress = hookeMatrix x strain)
	float young_modulus_;
	float poisson_ratio_;
	float plastic_strain_[6];		// Stored as {strainXX,strainYY,strainZZ,strainXY,strainYZ,strainZX}
	float elastic_limit_;			// Limit to strain before plastic deformation occurs (-> von Mise)
	float plastic_limit_;			// Limit to the plastic deformation that can occur
	float volume_conserving_coeff_;// Coefficient for the volume conserving force defined in "point based animation of elastic, plastic and melting objects"
	/**
	* Temporary variable holding grad u (of some solidNeigh) with respect 
	* to displacement (of this)
	* d_ij
	* @see UnifiedPhysics::calculateForces()
	*/
	b3Vector3 strain_factor_;
	/**
	* This struct contains all particle properties that change with 
	* temperature. If new properties are to be added, the operators must
	* be adapted as well.
	*/
	struct Properties
	{
		float temperature;
		float young_modulus;
		float visc_const;										// Viscosity parameter
		Properties operator+(const Properties& summand);		// Adds Properties by adding their variables.
		Properties operator-(const Properties& subtrahend);		// Subtracts Properties by subtracting their variables.
		Properties operator*(float factor);						// Multiplies Properties by multiplying their variables with factor.
	};

	Properties solid_properties_;							// Properties when in solid state
	Properties fluid_properties_;							// Properties when in fluid state
	float old_temperature_;										// Temperature before the latest update to the state of matter
	RigidBody* parent_rigidbody_;									// RigidBody 'this' is part of (NULL for particles that are not part of a rigid body)
	bool rigidbody_;												// When cold enough, will this particle become part of a rigid body?
	bool has_been_parsed_;											// used as a marker when parsing over the particles of a rigid body
	bool has_made_solid_;											// used as a marker when make a particle solid
	float inverted_moment_matrix_[3][3];							// used for MLS: A^-1
	unsigned int index_;											// spatial indexing information

	int	parent_rigid_body_index_;								// For the sake of GPU version
	int order_in_child_particles_array_;							// For the sake of GPU version
};

inline bool 
	operator<(const unsigned int i, const UnifiedParticle &a)
{
	return a.index_ < i;
}


class Particle {
    friend std::ostream &operator<<(std::ostream &output, const Particle&);
    friend std::istream &operator>>(std::istream &input, Particle& p);
protected:
    b3Vector3 location;
    float   radius;

public:
    Particle() { radius = 0; }
    Particle(const b3Vector3& loc) { location = loc; }
    Particle(const b3Vector3& loc, const float rad) { location = loc; radius = rad; }

    const b3Vector3&  Location() const { return location; }
    void  SetLocation(const b3Vector3& new_location) { location = new_location; }
 
    const float&  Radius() const { return radius; }
    void  SetRadius(const float& new_radius) { radius = new_radius; }

    virtual void Render();
};

#endif	// GPU_UNIFIED_PARTICLE_H_
