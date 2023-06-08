//
//  Scenes.h
//  MultiTracker
//
//  Created by Fang Da on 15/1/26.
//
//

#ifndef __MultiTracker__Scenes__
#define __MultiTracker__Scenes__

#include <iostream>
#include "BPS3D.h"

class Scenes
{
public:
    // scene-specific initialization
    static BPS3D * scene(Sim * sim, const std::string & scenename, std::vector<Vec3d> & vs, 
        std::vector<Vec3st> & fs, std::vector<Vec2i> & ls, std::vector<EigenVec3d> & vels, std::vector<char> & solids);
    
    // scene-specific time evolution
    static void pre_step(Sim * sim, const std::string & scenename, double dt, BPS3D * bps);
    static void post_step(Sim * sim, const std::string & scenename, double dt, BPS3D * bps);

};

#endif /* defined(__MultiTracker__Scenes__) */
