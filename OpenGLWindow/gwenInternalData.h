#ifndef GWEN_INTERNAL_DATA_H
#define GWEN_INTERNAL_DATA_H

#include "OpenGLWindow/GwenOpenGL3CoreRenderer.h"
#include "OpenGLWindow/GLPrimitiveRenderer.h"
#include "Gwork/Platform.h"
#include "Gwork/Controls/TreeControl.h"
#include "Gwork/Controls/RadioButtonController.h"
#include "Gwork/Controls/VerticalSlider.h"
#include "Gwork/Controls/HorizontalSlider.h"
#include "Gwork/Controls/GroupBox.h"
#include "Gwork/Controls/CheckBox.h"
#include "Gwork/Controls/StatusBar.h"
#include "Gwork/Controls/Button.h"
#include "Gwork/Controls/ComboBox.h"
#include "Gwork/Controls/MenuStrip.h"
#include "Gwork/Controls/Slider.h"
#include "Gwork/Controls/Property/Text.h"
#include "Gwork/Controls/SplitterBar.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "Gwork/Gwork.h"
#include "Gwork/Align.h"
#include "Gwork/Utility.h"
#include "Gwork/Controls/WindowControl.h"
#include "Gwork/Controls/TabControl.h"
#include "Gwork/Controls/ListBox.h"
#include "Gwork/Skins/Simple.h"
//#include "Gwork/Skins/TexturedBase.h"
#include "gwenUserInterface.h"

struct GwenInternalData
{
	//struct sth_stash;
	//class GwenOpenGL3CoreRenderer*	pRenderer;
	Gwk::Renderer::Base* pRenderer;
	Gwk::Skin::Simple skin;
	Gwk::Controls::Canvas* pCanvas;
	//GLPrimitiveRenderer* m_primRenderer;
	Gwk::Controls::TabButton* m_demoPage;
	Gwk::Controls::TabButton* m_explorerPage;
	Gwk::Controls::TreeControl* m_explorerTreeCtrl;
	Gwk::Controls::MenuItem* m_viewMenu;
	class MyMenuItems* m_menuItems;
	Gwk::Controls::ListBox* m_TextOutput;
	Gwk::Controls::Label* m_exampleInfoGroupBox;
	Gwk::Controls::ListBox* m_exampleInfoTextOutput;
	struct MyTestMenuBar* m_menubar;
	Gwk::Controls::StatusBar* m_bar;
	Gwk::Controls::ScrollControl* m_windowRight;
	Gwk::Controls::TabControl* m_tab;

	int m_curYposition;

	Gwk::Controls::Label* m_rightStatusBar;
	Gwk::Controls::Label* m_leftStatusBar;
	b3AlignedObjectArray<class Gwk::Event::Handler*> m_handlers;
	b3ToggleButtonCallback m_toggleButtonCallback;
	b3ComboBoxCallback m_comboBoxCallback;
};

#endif  //GWEN_INTERNAL_DATA_H
