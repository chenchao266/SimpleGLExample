#include "GwenTextureWindow.h"
#include "gwenUserInterface.h"
#include "gwenInternalData.h"
#include "Gwork/Controls/ImagePanel.h"

class MyGraphWindow : public Gwk::Controls::WindowControl
{
	Gwk::Controls::ImagePanel* m_imgPanel;

public:
	class MyMenuItems2* m_menuItems;

	MyGraphWindow(const MyGraphInput& input)
		: Gwk::Controls::WindowControl(input.m_data->pCanvas),
		  m_menuItems(0)
	{
		Gwk::String str = (input.m_name);
		SetTitle(str);

		SetPos(input.m_xPos, input.m_yPos);
		SetSize(12 + input.m_width + 2 * input.m_borderWidth, 30 + input.m_height + 2 * input.m_borderWidth);

		m_imgPanel = new Gwk::Controls::ImagePanel(this);
		if (input.m_texName)
		{
			Gwk::String texName = (input.m_texName);
			m_imgPanel->SetImage(texName);
		}
		m_imgPanel->SetBounds(input.m_borderWidth, input.m_borderWidth,
							  input.m_width,
							  input.m_height);
		//		this->Dock( Gwk::Pos::Bottom);
	}
	virtual ~MyGraphWindow()
	{
		delete m_imgPanel;
	}
};

class MyMenuItems2 : public Gwk::Controls::Base
{
	MyGraphWindow* m_graphWindow;

public:
	Gwk::Controls::MenuItem* m_item;

	MyMenuItems2(MyGraphWindow* graphWindow)
		: Gwk::Controls::Base(0),
		  m_graphWindow(graphWindow),
		  m_item(0)
	{
	}

	void MenuItemSelect(Gwk::Controls::Base* pControl)
	{
		if (m_graphWindow->Hidden())
		{
			m_graphWindow->SetHidden(false);
			//@TODO(erwincoumans) setCheck/SetCheckable drawing is broken, need to see what's wrong
			//			if (m_item)
			//				m_item->SetCheck(false);
		}
		else
		{
			m_graphWindow->SetHidden(true);
			//			if (m_item)
			//				m_item->SetCheck(true);
		}
	}
};

MyGraphWindow* setupTextureWindow(const MyGraphInput& input)
{
	MyGraphWindow* graphWindow = new MyGraphWindow(input);
	MyMenuItems2* menuItems = new MyMenuItems2(graphWindow);
	graphWindow->m_menuItems = menuItems;

	Gwk::String str = (input.m_name);
    menuItems->m_item = input.m_data->m_viewMenu->GetMenu()->AddItem(str); 
    menuItems->m_item->SetAction(menuItems, (Gwk::Event::Handler::Function)&MyMenuItems2::MenuItemSelect);
	//	menuItems->m_item->SetCheckable(true);

	return graphWindow;
}

void destroyTextureWindow(MyGraphWindow* window)
{
	delete window->m_menuItems->m_item;
	delete window->m_menuItems;
	delete window;
}
