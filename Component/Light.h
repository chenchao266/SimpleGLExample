/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#ifndef OSG_LIGHT
#define OSG_LIGHT 1

 
#include <Bullet3Common/geometry.h>
 
class State;
/** Light state class which encapsulates OpenGL glLight() functionality. */
class   Light //: public StateAttribute
{
    public :

        Light();

        Light(unsigned int lightnum);

        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        Light(const Light& light):
            //StateAttribute(light),
            _lightnum(light._lightnum),
            _ambient(light._ambient),
            _diffuse(light._diffuse),
            _specular(light._specular),
            _position(light._position),
            _direction(light._direction),
            _constant_attenuation(light._constant_attenuation),
            _linear_attenuation(light._linear_attenuation),
            _quadratic_attenuation(light._quadratic_attenuation),
            _spot_exponent(light._spot_exponent),
            _spot_cutoff(light._spot_cutoff) {}
          
        virtual unsigned int getMember() const { return _lightnum; }
          
        /** Set which OpenGL light to operate on. */
        void setLightNum(int num);

        /** Get which OpenGL light this Light operates on. */
        int getLightNum() const { return _lightnum; }

        /** Set the ambient component of the light. */
        inline void setAmbient( const Vec4f& ambient ) { _ambient = ambient; }

        /** Get the ambient component of the light. */
        inline const Vec4f& getAmbient() const { return _ambient; }

        /** Set the diffuse component of the light. */
        inline void setDiffuse( const Vec4f& diffuse ) { _diffuse = diffuse; }

        /** Get the diffuse component of the light. */
        inline const Vec4f& getDiffuse() const { return _diffuse; }

        /** Set the specular component of the light. */
        inline void setSpecular( const Vec4f& specular ) { _specular = specular; }

        /** Get the specular component of the light. */
        inline const Vec4f& getSpecular() const { return _specular; }

        /** Set the position of the light. */
        inline void setPosition( const Vec4f& position ) { _position = position; }

        /** Get the position of the light. */
        inline const Vec4f& getPosition() const { return _position; }

        /** Set the direction of the light. */
        inline void setDirection( const Vec3f& direction ) { _direction = direction; }

        /** Get the direction of the light. */
        inline const Vec3f& getDirection() const { return _direction; }

        /** Set the constant attenuation of the light. */
        inline void setConstantAttenuation( float constant_attenuation )     { _constant_attenuation = constant_attenuation; }

        /** Get the constant attenuation of the light. */
        inline float getConstantAttenuation() const { return _constant_attenuation; }

        /** Set the linear attenuation of the light. */
        inline void setLinearAttenuation ( float linear_attenuation )        { _linear_attenuation = linear_attenuation; }

        /** Get the linear attenuation of the light. */
        inline float getLinearAttenuation () const { return _linear_attenuation; }

        /** Set the quadratic attenuation of the light. */
        inline void setQuadraticAttenuation ( float quadratic_attenuation )  { _quadratic_attenuation = quadratic_attenuation; }

        /** Get the quadratic attenuation of the light. */
        inline float getQuadraticAttenuation()  const { return _quadratic_attenuation; }

        /** Set the spot exponent of the light. */
        inline void setSpotExponent( float spot_exponent )                   { _spot_exponent = spot_exponent; }

        /** Get the spot exponent of the light. */
        inline float getSpotExponent() const { return _spot_exponent; }

        /** Set the spot cutoff of the light. */
        inline void setSpotCutoff( float spot_cutoff )                       { _spot_cutoff = spot_cutoff; }

        /** Get the spot cutoff of the light. */
        inline float getSpotCutoff() const                                   { return _spot_cutoff; }

        /** Capture the lighting settings of the current OpenGL state
          * and store them in this object.
        */
        void captureLightState();

        /** Apply the light's state to the OpenGL state machine. */
        virtual void apply(State& state) const;
        virtual ~Light();

    protected :

   
        /** Initialize the light's settings with some decent defaults. */
        void init();

        int _lightnum;                           // OpenGL light number

        Vec4f _ambient;                           // r, g, b, w
        Vec4f _diffuse;                           // r, g, b, w
        Vec4f _specular;                          // r, g, b, w
        Vec4f _position;                          // x, y, z, w
        Vec3f _direction;                         // x, y, z
        float _constant_attenuation;             // constant
        float _linear_attenuation;               // linear
        float _quadratic_attenuation;            // quadratic
        float _spot_exponent;                    // exponent
        float _spot_cutoff;                      // spread
};
 

#endif
