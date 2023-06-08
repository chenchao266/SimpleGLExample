/*
* This source file is part of the osgOcean library
* 
* Copyright (C) 2009 Kim Bale
* Copyright (C) 2009 The University of Hull, UK
* 
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.

* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
* http://www.gnu.org/copyleft/lesser.txt.
*/

#pragma once

 
#include <Component/GeometryShape.h>
#include <Bullet3Common/BoundingBox.h>
 


#include <string>
#include "WaterTrochoids.h"
#include <Component/MultiGeometryShape.h>

struct GodRayProgram;
struct GodRayGlareProgram;
	/** 
	* Creates and animates God Ray geometry.
	*/
	class  GodRays : public MultiGeometryShape
	{
	private:
		bool           _isDirty;         /**< Rebuild flag. */
		bool           _isStateDirty;    /**< Rebuild stateset flag. */
		unsigned int   _numOfRays;       /**< Number of ray parallepipeds (_numOfRays*_numOfRays). */
		WaterTrochoids _trochoids;       /**< Generates and packs the trochoid variables required in the vertex shader. */
		Vec3f     _sunDirection;    /**< Direction of the sun. */
		Vec3f     _extinction;      /**< Extinction coeffecient (RGB) Controls the dispersion of light along the the length of the God Ray */
		float          _baseWaterHeight; /**< Height of the ocean surface */

		GLuint   _stateSet;
		std::shared_ptr<FloatArray> _constants; /**< Stores the trochoid variables. */

	public:
		GodRays(void);
		GodRays(unsigned int numOfRays, const Vec3f& sunDir, float baseWaterHeight);
		GodRays(const GodRays& copy);
         
	protected:
		~GodRays(void){};

	public:
		/** 
		* Create and add a grid of vertices for vertex shader displacement, and add the glare quad geometry. 
		*/
		void build(void);

		/** 
		* Creates God Rays stateset.
		* Loads shaders and set uniforms.
		* Sets up trochoid generation.
		*/
		void buildStateSet(void);
		
		/**
		* Sets the number rays along one axis of the grid.
		* Dirties the geometry.
		*/
		inline void setNumOfRays( unsigned int num ){
			_numOfRays = num;
			_isDirty = true;
		}

		inline void setBaseWaterLevel(float height){
			_baseWaterHeight = height;
		}

		/** 
		* Sets the sun direction.
		* If stateset is valid it will also update the uniform.
		*/
		void setSunDirection(const Vec3f& dir);

		/** 
		* Set the extinction coefficient (RGB).
		* If stateset is valid it will also update the uniform.
		*/
		void setExtinctionCoeff( const Vec3f& coeff );

	private:
		/** 
		* Create the geometry and textures for the glare quad and attaches shaders.
		*/
        GeometryShape* createGlareQuad(void);

		/** 
		* Create the parallepiped geometry for the rays and attaches shaders.
		*/
        GeometryShape* createRayShafts(void);

		/** 
		* Updates shader uniforms and updates trochoids.
		* Builds geometry/stateset if dirty flags are set.
		*/
		void update(float time, const Vec3f& eye, const double& fov );

		inline int idx( int c, int r, int rowLen )
		{
			return c+r*rowLen;
		}

		/** 
		* Loads and returns god ray shaders.
		*/
        void createGodRayProgram(void);

		/** 
		* Loads and returns god ray glare shaders.
		*/
        void createGodRayGlareProgram(void);

		/** 
		* Computes the refracted ray.
		* @param I Incident ray.
		* @param N Surface normal.
		* &param ration Refractive index ratio (1/1.333 for water).
		*/
		Vec3f refract( const float ratio, const Vec3f& I, const Vec3f& N );

	// ---------------------------------------------
	//            Callback declarations
	// ---------------------------------------------

	protected:
		/** 
		* Datatype for storing values from the cull visitor.
		*/
		class GodRayDataType//: public Referenced
		{
		private:
			GodRays& _godRays;
			Vec3f _eye;
			double _fov;

		public:
			GodRayDataType( GodRays& godRays );
			
			GodRayDataType( const GodRayDataType& copy  );

			inline void setEye( const Vec3f& eye ){
				_eye = eye;
			}

			inline void setFOV( const double& fov ){ 
				_fov = fov;
			}

			inline const Vec3f& getEye( void ) const{
				return _eye;
			}

			void update( float time );
		};

		/** 
		* Update/Cull animation callback.
		* Cull callback stores the eye position and the field of view.
		* Update callback calls GodRays::update().
		*/
        GodRayProgram* m_prog;     
        GodRayGlareProgram* m_progGlare;
 
        virtual void cull(CullVisitor* cv);
        virtual void update(unsigned frameNum, double currentTime);
	 

		/** 
		* Custom compute bound callback.
		* Needed as translations are done within the vertex shader.
		* Moves the bounding box along with the eye's (x,y) position.
		*/
 
        virtual BoundingBox computeBoundingBox( ) const;
  
	};

