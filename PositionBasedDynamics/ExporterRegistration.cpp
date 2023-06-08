#include "SimulatorBase.h"
//#include "ExporterBase.h"
//#include "ParticleExporter_Partio.h"
//#include "ParticleExporter_VTK.h"
//#include "RigidBodyExporter_BIN.h"
//#include "RigidBodyExporter_OBJ.h"
//#include "RigidBodyExporter_VTK.h"

using namespace SPH;

void SimulatorBase::createExporters()
{
	//addParticleExporter("enablePartioExport", "Partio Exporter", "Enable/disable partio export.", new ParticleExporter_Partio(this));
	//addParticleExporter("enableVTKExport", "VTK Exporter", "Enable/disable VTK export.", new ParticleExporter_VTK(this));

	//addRigidBodyExporter("enableRigidBodyExport", "Rigid Body Exporter", "Enable/disable rigid body BIN export.", new RigidBodyExporter_BIN(this));
	//addRigidBodyExporter("enableRigidBodyOBJExport", "Rigid Body OBJ Exporter", "Enable/disable rigid body OBJ export.", new RigidBodyExporter_OBJ(this));
	//addRigidBodyExporter("enableRigidBodyVTKExport", "Rigid Body VTK Exporter", "Enable/disable rigid body VTK export.", new RigidBodyExporter_VTK(this));
}