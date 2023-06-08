
#ifndef TINY_VR_GUI_H
#define TINY_VR_GUI_H

#include "Bullet3Common/b3Transform.h"
#include "Bullet3Common/Color.h"
class TinyVRGui
{
	struct TinyVRGuiInternalData* m_data;

public:
	TinyVRGui(struct ComboBoxParams& params, struct CommonRenderInterface* renderer);
	virtual ~TinyVRGui();

	bool init();
	void tick(b3Scalar deltaTime, const b3Transform& guiWorldTransform);

	void clearTextArea();
	void grapicalPrintf(const char* str, int rasterposx, int rasterposy, const Colorb& clr);
};

#endif  //TINY_VR_GUI_H
