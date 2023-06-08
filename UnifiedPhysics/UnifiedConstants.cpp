#include <iostream>
#include <cstdlib>
#include <time.h>

#include "UnifiedConstants.h"
#include "global.h"

UnifiedConstants::UnifiedConstants()
{
	srand(time(NULL));

	animating = true;			
	one_animation_step = false;

	sizeFactor = 1.0; 

	// bounding box
	boxLength = 2.0f * sizeFactor;
	boxHeight = 1.5f * sizeFactor;
	boxWidth  = 0.5f * sizeFactor;

	// elastic sheet 
	sheetLength = 0.6f * sizeFactor;
	sheetWidth  = 0.4f * sizeFactor;

	displayEnabled = true;

#ifdef SPH_DEMO_SCENE_2

	// initialized fluid
	fluidLength = 0.22 * sizeFactor; 
	fluidWidth = 0.5 * sizeFactor; 
	fluidHeight = 0.19 * sizeFactor;

	// domain size
	boxLength = 0.5 * sizeFactor; 
	boxWidth = 0.5 * sizeFactor; 
	boxHeight = 0.5 * sizeFactor;

#endif

	readParticles = false; 
	saveParticles = false;

	// environmental constants
	gravityConst = 9.81f;
	fluidRestDensity = 1000;
	relativeFluidJitter = 0.0;
	minSurfaceGrad = 10.0;
	gamma = 7.0;

	// spacing between the particles in the initialized block, set in such a way that pressure forces are 0 at the beginning
	float spacing = sizeFactor / 40.0;
	setParticleSpacing(spacing);

	fluidRestVol = initialMass / fluidRestDensity;

	k_r = 1.0f; // try kr=[1000-10000] 
	k_d = 0.1f;  // try kd=[100-500]
	k_t = 100.0f;
	mu = 0.4f; // (0.3, 0.4)

	diffusionConcConst = 0.0;
	//diffusionTempConst = 0.005;
	diffusionTempConst = 0.0;

	drawTemperature = false;
	drawRigidParticle = true;
	drawSoftParticle = true;
	drawLiquidParticle = true;
	drawGranularParticle = true;
	drawClothParticle = true;
	drawSmokeParticle = true;
	drawFrozenParticle = true;
	drawVelocity = false;
	drawPressureForce = false;

	coefficient_restitution = 0.0f;
	coefficient_slip = 1.0f;

	useVersatilCoupling = true;

	heatingGround = false;
	automaticCooling = 0.3f;

	useSurfaceTension = false;
	surface_tension_gamma = 0.3f;
	surface_adhesion_beta = 0.0f;

	// defines the maximally allowed density error of PCISPH, typically 1%
	maxDensityErrorAllowed = 1.0;
	computeGasConstAndTimeStep(maxDensityErrorAllowed);

	// select from the following simulation method
	// 0: SPH
	// 1: WCSPH
	// 2: PCISPH
	// 3: Particle-based Rigid Body Simulation
	simulationMethod = 2;
	setSimulationMethod(simulationMethod);

	// IMPORTANT: We need to manually tune this parameter to best suit our case which use different time step WHY? Where is the bug? I can't figure it out (2014-03-03)
	const float factor = deltaT_pcisph / 0.0002f;
	densityErrorFactorParameter = 0.05 * factor * factor; // 0.47  deltaT_pcisph = 0.0002f;  ->  0.05   deltaT_pcisph = 0.0008f;  -> 0.8

	// Versatile coupling
	alpha = 0.01f;	// viscosity constant in "Versatile Rigid-Fluid Coupling for Incompressible SPH" [0.08, 0.5] in WCSPH
	rho = 5.0f;
	epsilon = 0.01f;
	epsilon_h_square = epsilon * deltaT * deltaT;
	nu_liquid = alpha * deltaT * speedOfSound * 0.5f * (1.0f / fluidRestDensity);
	nu_rigid_liquid = rho * deltaT * 0.5f * (1.0f / fluidRestDensity);

	springCoefficient = 300.0f;
	springCoefficientBoundary = 300.0f;
	dampingCoefficient = 0.5f;
	shearCoefficient = 0.5f;

#ifdef SPH_DEMO_SCENE_2
	//fluidViscConst = 2.5f;
	//fluidViscConst_tube = 50.0f;
	fluidViscConst = 4.0 * sizeFactor;
	fluidViscConst_tube = 4.0 * sizeFactor;
	//viscFactor = 300.0f; // 2.5f;
	//viscFactor_tube = 6000.0f;

	boundaryForceFact = 64;

#else
	//viscFactor = 300.0f;
	boundaryForceFactor = 64; // 256

	numLiquidParticles = 0;
	numRigidParticles  = 0;

#endif

	wallX = 0.7;

	// set timestep based on courrant condition...
	// "maxParticleSpeed" should be an estimate on how fast particles can get (in m/s)

	addBoundaryForce = false;
	maxBoundaryForce = sizeFactor * boundaryForceFactor * particleSpacing * particleSpacing; 
	addWallWeightFunction = true;

	useIhmsen2010Method = false;

	setBoundingBoxValues();

	drawParticles = 1; // turn off opengl drawing

	// PCISPH params
	maxLoops = 50; // 50 
	minLoops = 3; // 3 iterations seem to be sufficient to propagate pressure information (see paper)
	printDebuggingInfo = 0; // print density infos error etc. (needs more computation time)

	// rigid body
	terminalSpeed = 20.0f;
}

//--------------------------------------------------------------------
UnifiedConstants::~UnifiedConstants()
	//--------------------------------------------------------------------
{

}

void UnifiedConstants::setParticleSpacing(float spacing)
{
	particleSpacing = spacing;
	particleRadius = spacing * 0.5f;
	collisionDistThreshold = 2.0f * particleRadius;
	distance_frozen_to_boundary = particleRadius;

	//particleRenderingSize = 0.5f * particleSpacing - 0.00001f;
	particleRenderingSize = 6.0f * particleRadius - 0.00001f; // still need to be determined for best visual results

	// is set so that avg number of neighbors is approx 30-40
	globalSupportRadius = particleSpacing * 2.0 * 1.00001; 

	forceDistance = 0.25 * globalSupportRadius;
	initialMass = particleSpacing*particleSpacing*particleSpacing * fluidRestDensity;

	//fluidViscConst = viscFactor * particleSpacing;
	//fluidViscConst_tube = viscFactor_tube * particleSpacing;
	fluidViscConst = 1.0 * sizeFactor;	// 4.0 * sizeFactor
	fluidViscConst_tube = 4.0 * sizeFactor; 

	std::cout << "Particle spacing set to " << spacing << std::endl;
	std::cout << "Global support radius to " << globalSupportRadius << std::endl;
}

void UnifiedConstants::setSimulationMethod(int method)
{
	if(method == 0)
	{
		speedOfSound = sqrt(fluidGasConst);
		std::cout << "simulation method set to SPH" << std::endl;
		std::cout << "gas constant for standard SPH: " << fluidGasConst << std::endl;
		std::cout << "speed of sound for SPH " << speedOfSound << std::endl;
		deltaT = deltaT_sph;
		physicsType = 'o';
	}
	else if (method == 1)
	{
		speedOfSound = sqrt(fluidGasConstantWCSPH);
		std::cout << "simulation method set to WCSPH" << std::endl;
		std::cout << "gas constant for WCSPH: " << fluidGasConstantWCSPH << std::endl;
		std::cout << "speed of sound for WCSPH " << speedOfSound << std::endl;
		deltaT = deltaT_wcsph;
		physicsType = 'w';
	}
	else if (method == 2)
	{
		speedOfSound = sqrt(fluidGasConstantWCSPH);
		std::cout << "simulation method set to PCISPH" << std::endl;
		std::cout << "speed of sound for PCISPH " << speedOfSound << std::endl;	// Is it right?
		deltaT = deltaT_pcisph;
		physicsType = 'i';
	}
	else if (method == 3)
	{
		std::cout << "simulation method set to Rigid Body Simulation" << std::endl;
		deltaT_RigidBody = 0.003f;	// 0.003f
		deltaT = deltaT_RigidBody;
		physicsType = 'o';
	}


#ifdef DIRECT_FORCING

	float deltaT_DirectForcing = 0.00001f;
	deltaT = deltaT_DirectForcing;

#endif

	std::cout << "new time step: " << deltaT << std::endl;

}

void UnifiedConstants::computeGasConstAndTimeStep(float densityVariation)
{
	double maxParticleSpeed = 4.0; // max Speed in particular scene
	courantFactor = 0.4;

	if(densityVariation >= 1.0)
	{
		// set PCISPH params
		deltaT_pcisph = 0.0012f; // tuned value, should be modified for specific demo  
		deltaT = deltaT_pcisph;

		// set WCSPH params
		fluidGasConstantWCSPH = 2000; //  set gasConst according to density error (manually tuned for particular Siggraph scene) square(V_f)/square(C_s) = 1%  vf = sqrt(2*g*Height) refer to "Weakly compressible SPH for free surface flows"
		speedOfSound = sqrt(fluidGasConstantWCSPH);
		relevantSpeed = MAX(speedOfSound, maxParticleSpeed);	
		deltaT_wcsph = courantFactor * globalSupportRadius / relevantSpeed;
		deltaT = deltaT_wcsph;

		// set SPH params
		fluidGasConst = 1000;
		speedOfSound = sqrt(fluidGasConst);
		relevantSpeed = MAX(speedOfSound, maxParticleSpeed);	
		deltaT_sph = courantFactor * globalSupportRadius / relevantSpeed;
		deltaT = deltaT_sph;
	}
	else
	{
		// set PCISPH params
		deltaT_pcisph = 0.0006; // manually tuned value for Siggraph demo  
		deltaT = deltaT_pcisph;

		// set WCSPH params
		fluidGasConstantWCSPH = 6000000; // set gasConst according to density error (manually tuned for particular Siggraph scene)
		speedOfSound = sqrt(fluidGasConstantWCSPH);
		relevantSpeed = MAX(speedOfSound, maxParticleSpeed);	
		deltaT_wcsph = courantFactor * globalSupportRadius / relevantSpeed;
		deltaT = deltaT_wcsph;	

		// set SPH params
		fluidGasConst = 1000;
		speedOfSound = sqrt(fluidGasConst);
		relevantSpeed = MAX(speedOfSound, maxParticleSpeed);	
		deltaT_sph = courantFactor * globalSupportRadius / relevantSpeed;
		deltaT = deltaT_sph;
	}	

#ifdef DIRECT_FORCING

	float deltaT_DirectForcing = 0.00001f;
	deltaT = deltaT_DirectForcing;

#endif

	std::cout << "------Delta_T--------" << std::endl;
	std::cout << "deltaT_pcisph: \t"<< deltaT_pcisph << std::endl;
	std::cout << "deltaT_sph: \t" << deltaT_sph << std::endl;
	std::cout << "deltaT_wcsph: \t" << deltaT_wcsph << std::endl;

#ifdef DIRECT_FORCING

	std::cout << "deltaT_DirectForcing: \t" << deltaT_DirectForcing << std::endl;

#endif

	std::cout << "--------------" << std::endl;
}

void UnifiedConstants::setBoundingBoxValues()
{
	// set virtual bounding box
	const b3Vector3 minBBoxVec(0.0, 0.0, 0.0);	 // Set as so to make sure we contain all the static frozen particles in the virtualBoundingBox
	const b3Vector3 maxBBoxVec(2.0, 2.0, 1.5);  
	setBBox(minBBoxVec, maxBBoxVec, virtualBoundingBox);
	// get virtual bounding box range and grid scale along axis for the simulation domain
	b3Vector3 delta = virtualBoundingBox.getMax()-virtualBoundingBox.getMin();
	scales = b3Vector3(GRID_RESOLUTION, GRID_RESOLUTION, GRID_RESOLUTION) / delta;
	// virtual bounding box
	virtualIndexingBoxLength = delta.x;
	virtualIndexingBoxHeight = delta.y;
	virtualIndexingBoxWidth  = delta.z;

	// set real box container (red wire frame box)
	const b3Vector3 minRealBoxContainer = minBBoxVec + b3Vector3(globalSupportRadius, globalSupportRadius, globalSupportRadius);
	const b3Vector3 maxRealBoxContainer = maxBBoxVec - b3Vector3(globalSupportRadius, globalSupportRadius, globalSupportRadius);
	setBBox(minRealBoxContainer, maxRealBoxContainer, realBoxContainer);
	// get initial real container range
	b3Vector3 realBoxDelta = realBoxContainer.getMax() - realBoxContainer.getMin();
	boxLength = realBoxDelta.x;
	boxHeight = realBoxDelta.y;
	boxWidth  = realBoxDelta.z;

	// set fluid box
	b3Vector3 minFluidBoxVec = b3Vector3(distance_frozen_to_boundary, distance_frozen_to_boundary, distance_frozen_to_boundary);
	//Vector3f maxFluidBoxVec = b3Vector3(0.7, 1.2, boxWidth) + minFluidBoxVec - b3Vector3(distance_frozen_to_boundary) * 2;
	b3Vector3 maxFluidBoxVec = b3Vector3(0.6, 0.8, boxWidth) + minFluidBoxVec - b3Vector3(distance_frozen_to_boundary, distance_frozen_to_boundary, distance_frozen_to_boundary) * 2;
	setBBox(minFluidBoxVec, maxFluidBoxVec, fluidBox);
	// get initial fluid box range
	b3Vector3 fluidDelta = fluidBox.getMax()-fluidBox.getMin();
	fluidLength = fluidDelta.x;
	fluidHeight = fluidDelta.y;
	fluidWidth = fluidDelta.z;

	useOldRigidBodyMethod	 = false;
	isTwoWayCoupling		 = false;

	// set rigid box
	b3Vector3 minRigidBoxVec(distance_frozen_to_boundary, distance_frozen_to_boundary, distance_frozen_to_boundary);
	b3Vector3 maxRigidBoxVec = minRigidBoxVec + b3Vector3(0.15, 0.15, 0.15);
	setBBox(minRigidBoxVec, maxRigidBoxVec, rigidBox);
	// get initial rigid box range
	b3Vector3 rigidDelta = rigidBox.getMax()-rigidBox.getMin();
	rigidLength = rigidDelta.x;
	rigidHeight = rigidDelta.y;
	rigidWidth = rigidDelta.z;

	// set rigid cube
	b3Vector3 minRigidCubeVec(distance_frozen_to_boundary, distance_frozen_to_boundary, distance_frozen_to_boundary);
	b3Vector3 maxRigidCubeVec = minRigidCubeVec + b3Vector3(3.0 * particleSpacing, 3.0 * particleSpacing, 3.0 * particleSpacing);
	setBBox(minRigidCubeVec, maxRigidCubeVec, rigidCube);
	cube_side_length = ( rigidCube.getMax()-rigidCube.getMin() ).x;


#ifdef SPH_DEMO_SCENE_2
	//collisionBox.setMin(Vector3f(0.0, 0.0, 0.0));
	//collisionBox.setMax(Vector3f(0.9) * delta);

	// setup collision box inside simulation domain bounding box
	collisionBox.setMin(b3Vector3(0.01 * delta.x, 0.01 * delta.y, 0.01 * delta.z) );
	collisionBox.setMax(b3Vector3(0.99 * delta.x, 0.99 * delta.y, 0.70) );
#else

	// setup collision box INSIDE simulation domain bounding box
	// a little trick to make it stable instead of using collisionBox = bbox;
	collisionBox.setMin(minRealBoxContainer + b3Vector3(particleRadius, particleRadius, particleRadius) );	// we assume the box container has a thickness = particleRadius
	collisionBox.setMax(maxRealBoxContainer - b3Vector3(particleRadius, particleRadius, particleRadius) );
	//collisionBox.setMin(minRealBoxContainer + 0.0001f);
	//collisionBox.setMax(maxRealBoxContainer - 0.0001f);

	zindexStartingVec = virtualBoundingBox.getMin();

#endif
}