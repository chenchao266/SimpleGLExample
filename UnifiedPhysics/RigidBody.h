#ifndef GPU_UNIFIED_RIGID_BODY_H_
#define GPU_UNIFIED_RIGID_BODY_H_ 

#include "UnifiedParticle.h"
#include "Bullet3Common/b3Matrix3x3.h"
#include "global.h"

#include "Bullet3Common/geometry.h"

// Represents a rigid body consisting of particles
class RigidBody
{
public:

	RigidBody();

	/**
	* Adds force to the sum of forces that will influence linear velocity 
	* (Force, not force density)
	*/
	inline void ApplyForce(const b3Vector3& force);

	/**
	* Increases net torque that will cause angular momentum by applying 
	* force at the position of bodyPart (Force, not force density)
	*/
	void ApplyTorque(const b3Vector3& force, const UnifiedParticle& body_part);

	/**
	* Inverts inertia if it's possible. Otherwise, invertedInertia is set 
	* to 3/trace(inertia) * Identity Matrix
	*/
	void InvertInertia();

	/*
	* Temperately add new particle index into the rigid_particle_buffers_
	*/
	inline void AddToParentRigidBody(const int i);

	/*
	* Clear rigid_particle_buffers_ 
	*/
	inline void ClearBuffers();

	/*
	* Update rigid_particle_indices_ 
	*/
	void UpdateRigidBodyParticleIndices();

	// TODO: The CPU version is not fully correct, should be fixed if the original one doesn't behave well in some cases
	void PerformLinearStep(const float delta);
	void PerformAngularStep(const float delta);
	void UpdateRotationMatrix();
	void UpdateInverseInertiaTensor();
	void PerformStep(const float delta);
	void UpdateMomenta(const float deltaT, const float terminalMomentum);
	//---------------------------------------------------------------------------------------------------

	std::vector<int>&			rigid_particle_indices()		{ return rigid_particle_indices_; }
	const std::vector<int>&		rigid_particle_indices() const  { return rigid_particle_indices_; }

	float mass() const { return mass_; }	
	void  set_mass (const float mass) { mass_ = mass; }

	b3Vector3 rigidbody_pos() const { return rigidbody_pos_; }
	void set_rigidbody_pos(const b3Vector3& pos) { rigidbody_pos_ = pos; }

	b3Vector3 old_center_of_mass() const { return old_center_of_mass_; }
	void set_center_of_mass(const b3Vector3& com) { old_center_of_mass_ = com; }

	b3Vector3 velocity() const { return velocity_; }
	void set_velocity(const b3Vector3& vel) { velocity_ = vel; }

	b3Vector3 linear_momentum() const { return linear_momentum_; }
	void set_linear_momentum(const b3Vector3& linear_momentum) { linear_momentum_ = linear_momentum; }

	b3Vector3 angular_velocity() const { return angular_velocity_; }
	void set_angular_velocity(const b3Vector3& angular_velocity) { angular_velocity_ = angular_velocity; }

	b3Vector3 angular_momentum() const { return angular_momentum_; }
	void set_angular_momentum(const b3Vector3& angular_momentum) { angular_momentum_ = angular_momentum; }

	b3Matrix3x3 inertia() const { return inertia_; }
	void set_inertia(const b3Matrix3x3& mat) { inertia_ = mat; }

	b3Matrix3x3 inverted_inertia() const { return inverted_inertia_; }
	void set_inverted_inertia(const b3Matrix3x3& mat) { inverted_inertia_ = mat; }

	b3Matrix3x3 inverted_inertia_local() const { return inverted_inertia_local_; }

	b3Vector4 quaternion() const { return quaternion_; }
	void set_quaternion(const b3Vector4& vec) { quaternion_ = vec; }

	b3Vector3 torque() const { return torque_; }
	void set_torque(const b3Vector3& vec) { torque_ = vec; }

	b3Vector3 force() const { return force_; }
	void set_force(const b3Vector3& vec) { force_ = vec; }

	b3Matrix3x3 rotation_matrix() const { return rotation_matrix_; }
	void set_rotation_matrix(const b3Matrix3x3& mat) { rotation_matrix_ = mat; }

private:

	float mass_;									// sum of mass of all particles
	b3Vector3 rigidbody_pos_;	
	b3Vector3 old_center_of_mass_;				// center of mass before the last integration step (for use in UnifiedPhysics::timeIntegration())
	b3Vector3 velocity_;						// linear velocity = velocity at rigidbody_pos_
	b3Vector3 linear_momentum_;				// linear_momentum_ = mass * linear velocity

	/**
	* angular velocity "omega", where omega/|omega| is the axis of rotation
	* and |omega| is the amount of rotation
	*/
	b3Vector3 angular_velocity_;

	/**
	* angular_momentum_ = inertia * angular velocity
	* The angular momentum vector l is always measured for a particular frame of reference
	* here angular momentum about the center of mass of the body is used.
	*/
	b3Vector3 angular_momentum_;

	//------------------------ new methods : "Real-Time Rigid Body Interactions" ------------------------
	// TODO: it's not correct yet, should be fixed if the original one doesn't behave well in some cases
	/**
	* quaternion = [s, V] represents a rotation of s radians about an axis defined by the vector V
	* it's more accurate than rotationMatrix in physics simulation 
	*/
	b3Vector4 quaternion_;

	//---------------------------------------------------------------------------------------------------
	b3Vector3 force_;
	b3Vector3 torque_;	

	/**
	* The rotation matrix holds the rotation for one step.
	* defining rotationMatrix to only constitute the last step of rotation renders storing the original i ˆr for every
	particle unnecessary as well.
	*/
	b3Matrix3x3 rotation_matrix_;

	/**
	* r_i = particles[i]->position - body->rigidbody_pos_
	*
	* 							|	r_i[1]^2+r_i[2]^2	-r_i[0]*r_i[1]		-r_i[0]*r_i[2]		|
	* inertia=	sum_{i}	m_i * 	|	-r_i[0]*r_i[1]		r_i[0]^2+r_i[2]^2	-r_i[1]*r_i[2]		|
	*							|	-r_i[0]*r_i[2]		-r_i[1]*r_i[2]		r_i[0]^2+r_i[1]^2	|
	* (Eq. 5.9) from paper "Simulation of Fluid-Solid Interaction"
	*/
	b3Matrix3x3 inertia_;
	b3Matrix3x3 inverted_inertia_;
	b3Matrix3x3 inverted_inertia_world_;
	b3Matrix3x3 inverted_inertia_local_;

	std::vector<int> rigid_particle_indices_;		// rigid body particle indices array
	std::vector<int> rigid_particle_buffers_;		// Used to temperately store new particle indices after sorting

};

//--------------------------------------------------------------------
inline void RigidBody::ApplyForce(const b3Vector3& force)
{
	this->force_+=force;
}

//--------------------------------------------------------------------
inline void RigidBody::AddToParentRigidBody(const int i)
{
	rigid_particle_buffers_.push_back(i);
}

//--------------------------------------------------------------------
inline void RigidBody::ClearBuffers()
{
	rigid_particle_buffers_.clear();
}

#endif	// GPU_UNIFIED_RIGID_BODY_H_
