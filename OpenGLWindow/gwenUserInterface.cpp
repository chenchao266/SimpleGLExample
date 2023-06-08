
#include "gwenUserInterface.h"
#include "gwenInternalData.h"
#include "Gwork/Controls/ImagePanel.h"
#include "Gwork/Controls/ColorPicker.h"
//#include "Gwork/Controls/HSVColorPicker.h"
#include "CommonInterfaces/CommonCallbacks.h"

//class MyGraphWindow* graphWindow = 0;
static GwenUserInterface* gUi = 0;
GwenUserInterface* GwenUserInterface::instance()
{
    return gUi;
}

GwenUserInterface::GwenUserInterface()
{
    gUi = this;
	m_data = new GwenInternalData();
	m_data->m_toggleButtonCallback = 0;
	m_data->m_comboBoxCallback = 0;
}

class MyMenuItems : public Gwk::Controls::Base
{
public:
	b3FileOpenCallback m_fileOpenCallback;
	b3QuitCallback m_quitCallback;

	MyMenuItems() : Gwk::Controls::Base(0), m_fileOpenCallback(0)
	{
	}
	void myQuitApp(Gwk::Controls::Base* pControl)
	{
		if (m_quitCallback)
		{
			(*m_quitCallback)();
		}
	}
	void fileOpen(Gwk::Controls::Base* pControl)
	{
		if (m_fileOpenCallback)
		{
			(*m_fileOpenCallback)();
		}
	}
};

struct MyTestMenuBar : public Gwk::Controls::MenuStrip
{
	Gwk::Controls::MenuItem* m_fileMenu;
	Gwk::Controls::MenuItem* m_viewMenu;
	MyMenuItems* m_menuItems;

	MyTestMenuBar(Gwk::Controls::Base* pParent)
		: Gwk::Controls::MenuStrip(pParent)
	{
		//		Gwk::Controls::MenuStrip* menu = new Gwk::Controls::MenuStrip( pParent );
		{
			m_menuItems = new MyMenuItems();
			m_menuItems->m_fileOpenCallback = 0;
			m_menuItems->m_quitCallback = 0;

			m_fileMenu = AddItem("File");

            Gwk::Controls::MenuItem* miOpen = m_fileMenu->GetMenu()->AddItem("Open"); miOpen->SetAction(m_menuItems, (Gwk::Event::Handler::Function)&MyMenuItems::fileOpen);
            Gwk::Controls::MenuItem* miQuit = m_fileMenu->GetMenu()->AddItem("Quit"); miQuit->SetAction(m_menuItems, (Gwk::Event::Handler::Function)&MyMenuItems::myQuitApp);
			m_viewMenu = AddItem("View");
		}
	}
	virtual ~MyTestMenuBar()
	{
		delete m_menuItems;
	}
};

void GwenUserInterface::exit()
{
	//m_data->m_menubar->RemoveAllChildren();
	delete m_data->m_tab;
	delete m_data->m_windowRight;
	delete m_data->m_leftStatusBar;
	delete m_data->m_TextOutput;
	delete m_data->m_rightStatusBar;
	delete m_data->m_bar;
	delete m_data->m_menubar;

	m_data->m_menubar = 0;
	delete m_data->pCanvas;
	m_data->pCanvas = 0;
}

GwenUserInterface::~GwenUserInterface()
{
	for (int i = 0; i < m_data->m_handlers.size(); i++)
	{
		delete m_data->m_handlers[i];
	}

	m_data->m_handlers.clear();

	delete m_data;
}

void GwenUserInterface::resize(int width, int height)
{
	m_data->pCanvas->SetSize(width, height);
}

struct MyComboBoxHander : public Gwk::Event::Handler
{
	GwenInternalData* m_data;
	int m_buttonId;

	MyComboBoxHander(GwenInternalData* data, int buttonId)
		: m_data(data),
		  m_buttonId(buttonId)
	{
	}

	void onSelect(Gwk::Controls::Base* pControl)
	{
		Gwk::Controls::ComboBox* but = (Gwk::Controls::ComboBox*)pControl;

		Gwk::String str = (but->GetSelectedItem()->GetText());

		if (m_data->m_comboBoxCallback)
			(*m_data->m_comboBoxCallback)(m_buttonId, str.c_str());
	}
};

struct MyButtonHander : public Gwk::Event::Handler
{
	GwenInternalData* m_data;
	int m_buttonId;

	MyButtonHander(GwenInternalData* data, int buttonId)
		: m_data(data),
		  m_buttonId(buttonId)
	{
	}

	void onButtonA(Gwk::Controls::Base* pControl)
	{
		Gwk::Controls::Button* but = (Gwk::Controls::Button*)pControl;
		//		int dep = but->IsDepressed();
		int tog = but->GetToggleState();
		if (m_data->m_toggleButtonCallback)
			(*m_data->m_toggleButtonCallback)(m_buttonId, tog);
	}
};

void GwenUserInterface::textOutput(const char* message)
{
	Gwk::String msg = (message);
	m_data->m_TextOutput->AddItem(msg);
	m_data->m_TextOutput->ScrollToBottom();
}

void GwenUserInterface::setExampleDescription(const char* message)
{
	//Gwen apparently doesn't have text/word wrap, so do rudimentary brute-force implementation here.

	std::string wrapmessage = message;
	int startPos = 0;

	std::string lastFit = "";
	bool hasSpace = false;
	std::string lastFitSpace = "";
	int spacePos = 0;

	m_data->m_exampleInfoTextOutput->Clear();
	int fixedWidth = m_data->m_exampleInfoTextOutput->GetBounds().w - 25;
	int wrapLen = int(wrapmessage.length());
	for (int endPos = 0; endPos <= wrapLen; endPos++)
	{
		std::string sub = wrapmessage.substr(startPos, (endPos - startPos));
		Gwk::Point pt = m_data->pRenderer->MeasureText(m_data->pCanvas->GetSkin()->GetDefaultFont(), sub);

		if (pt.x <= fixedWidth)
		{
			lastFit = sub;

			if (message[endPos] == ' ' || message[endPos] == '.' || message[endPos] == ',')
			{
				hasSpace = true;
				lastFitSpace = sub;
				spacePos = endPos;
			}
		}
		else
		{
			//submit and
			if (hasSpace)
			{
				endPos = spacePos + 1;
				hasSpace = false;
				lastFit = lastFitSpace;
				startPos = endPos;
			}
			else
			{
				startPos = endPos - 1;
			}
			Gwk::String msg = (lastFit);

			m_data->m_exampleInfoTextOutput->AddItem(msg);
			m_data->m_exampleInfoTextOutput->ScrollToBottom();
		}
	}

	if (lastFit.length())
	{
		Gwk::String msg = (lastFit);
		m_data->m_exampleInfoTextOutput->AddItem(msg);
		m_data->m_exampleInfoTextOutput->ScrollToBottom();
	}
}

void GwenUserInterface::setStatusBarMessage(const char* message, bool isLeft)
{
	Gwk::String msg = (message);
	if (isLeft)
	{
		m_data->m_leftStatusBar->SetText(msg);
	}
	else
	{
		m_data->m_rightStatusBar->SetText(msg);
	}
}

void GwenUserInterface::registerFileOpenCallback(b3FileOpenCallback callback)
{
	m_data->m_menuItems->m_fileOpenCallback = callback;
}

void GwenUserInterface::registerQuitCallback(b3QuitCallback callback)
{
	m_data->m_menuItems->m_quitCallback = callback;
}

void GwenUserInterface::init(int width, int height, Gwk::Renderer::Base* renderer, float retinaScale)
{
	m_data->m_curYposition = 20;
	//m_data->m_primRenderer = new GLPrimitiveRenderer(width,height);
	m_data->pRenderer = renderer;  //new GwenOpenGL3CoreRenderer(m_data->m_primRenderer,stash,width,height,retinaScale);

	m_data->skin.SetRender(m_data->pRenderer);

	m_data->pCanvas = new Gwk::Controls::Canvas(&m_data->skin);
	m_data->pCanvas->SetSize(width, height);
	m_data->pCanvas->SetDrawBackground(false);
	m_data->pCanvas->SetBackgroundColor(Gwk::Color(150, 170, 170, 255));

	MyTestMenuBar* menubar = new MyTestMenuBar(m_data->pCanvas);
	m_data->m_viewMenu = menubar->m_viewMenu;
	m_data->m_menuItems = menubar->m_menuItems;
	m_data->m_menubar = menubar;

	Gwk::Controls::StatusBar* bar = new Gwk::Controls::StatusBar(m_data->pCanvas);
	m_data->m_bar = bar;

	m_data->m_rightStatusBar = new Gwk::Controls::Label(bar);

	m_data->m_rightStatusBar->SetWidth(width / 2);
	//m_data->m_rightStatusBar->SetText( "Label Added to Right" );
	bar->AddControl(m_data->m_rightStatusBar, true);

	m_data->m_TextOutput = new Gwk::Controls::ListBox(m_data->pCanvas);

	m_data->m_TextOutput->Dock(Gwk::Position::Bottom);
	m_data->m_TextOutput->SetHeight(100);

	m_data->m_leftStatusBar = new Gwk::Controls::Label(bar);

	//m_data->m_leftStatusBar->SetText( "Label Added to Left" );
	m_data->m_leftStatusBar->SetWidth(width / 2);
	bar->AddControl(m_data->m_leftStatusBar, false);

	//Gwk::KeyboardFocus
	/*Gwk::Controls::GroupBox* box = new Gwk::Controls::GroupBox(m_data->pCanvas);
	box->SetText("text");
	box->SetName("name");
	box->SetHeight(500);
	*/
	Gwk::Controls::ScrollControl* windowRight = new Gwk::Controls::ScrollControl(m_data->pCanvas);
	windowRight->Dock(Gwk::Position::Right);
	windowRight->SetWidth(250);
	windowRight->SetHeight(250);
	windowRight->SetScroll(false, true);
	m_data->m_windowRight = windowRight;

	//windowLeft->SetSkin(
	Gwk::Controls::TabControl* tab = new Gwk::Controls::TabControl(windowRight);
	m_data->m_tab = tab;

	//tab->SetHeight(300);
	tab->SetWidth(240);
	tab->SetHeight(13250);
	//tab->Dock(Gwk::Pos::Left);
	tab->Dock(Gwk::Position::Fill);
	//tab->SetMargin( Gwk::Margin( 2, 2, 2, 2 ) );

	Gwk::String str1("Params");
	m_data->m_demoPage = tab->AddPage(str1);

	//	Gwk::String str2("OpenCL");
	//	tab->AddPage(str2);
	//Gwk::String str3("page3");
	//	tab->AddPage(str3);

	//but->onPress.Add(handler, &MyHander::onButtonA);

	//box->Dock(Gwk::Pos::Left);

	/*Gwk::Controls::WindowControl* windowBottom = new Gwk::Controls::WindowControl(m_data->pCanvas);
	windowBottom->SetHeight(100);
	windowBottom->Dock(Gwk::Position::Bottom);
	windowBottom->SetTitle("bottom");
	*/
	//	Gwk::Controls::Property::Text* prop = new Gwk::Controls::Property::Text(m_data->pCanvas);
	//prop->Dock(Gwk::Position::Bottom);
	/*Gwk::Controls::SplitterBar* split = new Gwk::Controls::SplitterBar(m_data->pCanvas);
	split->Dock(Gwk::Pos::Center);
	split->SetHeight(300);
	split->SetWidth(300);
	*/
	/*


	*/

	Gwk::Controls::ScrollControl* windowLeft = new Gwk::Controls::ScrollControl(m_data->pCanvas);
	windowLeft->Dock(Gwk::Position::Left);
	//	windowLeft->SetTitle("title");
	windowLeft->SetScroll(false, false);
	windowLeft->SetWidth(250);
	windowLeft->SetPos(50, 50);
	windowLeft->SetHeight(500);
	//windowLeft->SetClosable(false);
	//	windowLeft->SetShouldDrawBackground(true);
	windowLeft->SetTabable(true);

	Gwk::Controls::TabControl* explorerTab = new Gwk::Controls::TabControl(windowLeft);

	//tab->SetHeight(300);
	//	explorerTab->SetWidth(230);
	explorerTab->SetHeight(250);
	//tab->Dock(Gwk::Pos::Left);
	explorerTab->Dock(Gwk::Position::Fill);

	//m_data->m_exampleInfoTextOutput->SetBounds(2, 10, 236, 400);

	//windowRight

	Gwk::String explorerStr1("Explorer");
	m_data->m_explorerPage = explorerTab->AddPage(explorerStr1);
	Gwk::String shapesStr1("Test");
	Gwk::Controls::TabButton* shapes = explorerTab->AddPage(shapesStr1);

	///todo(erwincoumans) figure out why the HSV color picker is extremely slow
	//Gwk::Controls::HSVColorPicker* color = new Gwk::Controls::HSVColorPicker(shapes->GetPage());
	Gwk::Controls::ColorPicker* color = new Gwk::Controls::ColorPicker(shapes->GetPage());
	color->SetKeyboardInputEnabled(true);

	Gwk::Controls::TreeControl* ctrl = new Gwk::Controls::TreeControl(m_data->m_explorerPage->GetPage());
	m_data->m_explorerTreeCtrl = ctrl;
	ctrl->SetKeyboardInputEnabled(true);
	ctrl->Focus();
	ctrl->SetBounds(2, 10, 236, 300);

	m_data->m_exampleInfoGroupBox = new Gwk::Controls::Label(m_data->m_explorerPage->GetPage());
	m_data->m_exampleInfoGroupBox->SetPos(2, 314);
	m_data->m_exampleInfoGroupBox->SetHeight(15);
	m_data->m_exampleInfoGroupBox->SetWidth(234);
	m_data->m_exampleInfoGroupBox->SetText("Example Description");

	m_data->m_exampleInfoTextOutput = new Gwk::Controls::ListBox(m_data->m_explorerPage->GetPage());

	//m_data->m_exampleInfoTextOutput->Dock( Gwk::Position::Bottom );
	m_data->m_exampleInfoTextOutput->SetPos(2, 332);
	m_data->m_exampleInfoTextOutput->SetHeight(150);
	m_data->m_exampleInfoTextOutput->SetWidth(233);
}

void GwenUserInterface::forceUpdateScrollBars()
{
	b3Assert(m_data);
	b3Assert(m_data->m_explorerTreeCtrl);
	if (m_data && m_data->m_explorerTreeCtrl)
	{
		m_data->m_explorerTreeCtrl->Scroller()->UpdateScrollBars();
	}
}

void GwenUserInterface::setFocus()
{
	b3Assert(m_data);
	b3Assert(m_data->m_explorerTreeCtrl);
	if (m_data && m_data->m_explorerTreeCtrl)
	{
		m_data->m_explorerTreeCtrl->Focus();
	}
}

b3ToggleButtonCallback GwenUserInterface::getToggleButtonCallback()
{
	return m_data->m_toggleButtonCallback;
}

void GwenUserInterface::setToggleButtonCallback(b3ToggleButtonCallback callback)
{
	m_data->m_toggleButtonCallback = callback;
}
void GwenUserInterface::registerToggleButton2(int buttonId, const char* name)
{
	assert(m_data);
	assert(m_data->m_demoPage);

	Gwk::Controls::Button* but = new Gwk::Controls::Button(m_data->m_demoPage->GetPage());

	///some heuristic to find the button location
	int ypos = m_data->m_curYposition;
	but->SetPos(10, ypos);
	but->SetWidth(200);
	//but->SetBounds( 200, 30, 300, 200 );

	MyButtonHander* handler = new MyButtonHander(m_data, buttonId);
	m_data->m_handlers.push_back(handler);
	m_data->m_curYposition += 22;
	but->onToggle.Add(handler, &MyButtonHander::onButtonA);
	but->SetIsToggle(true);
	but->SetToggleState(false);
	but->SetText(name);
}

void GwenUserInterface::setComboBoxCallback(b3ComboBoxCallback callback)
{
	m_data->m_comboBoxCallback = callback;
}

b3ComboBoxCallback GwenUserInterface::getComboBoxCallback()
{
	return m_data->m_comboBoxCallback;
}
void GwenUserInterface::registerComboBox2(int comboboxId, int numItems, const char** items, int startItem)
{
	Gwk::Controls::ComboBox* combobox = new Gwk::Controls::ComboBox(m_data->m_demoPage->GetPage());
	MyComboBoxHander* handler = new MyComboBoxHander(m_data, comboboxId);
	m_data->m_handlers.push_back(handler);

	combobox->onSelection.Add(handler, &MyComboBoxHander::onSelect);
	int ypos = m_data->m_curYposition;
	combobox->SetPos(10, ypos);
	combobox->SetWidth(100);
	//box->SetPos(120,130);
	for (int i = 0; i < numItems; i++)
	{
		Gwk::Controls::MenuItem* item = combobox->AddItem((items[i]));
		if (i == startItem)
			combobox->OnItemSelected(item);
	}

	m_data->m_curYposition += 22;
}

void GwenUserInterface::draw(int width, int height)
{
	//	printf("width = %d, height=%d\n", width,height);
	if (m_data->pCanvas)
	{
		m_data->pCanvas->SetSize(width, height);
		//m_data->m_primRenderer->setScreenSize(width,height);
		m_data->pRenderer->Resize(width, height);
		m_data->pCanvas->RenderCanvas();
		//restoreOpenGLState();
	}
}

bool GwenUserInterface::mouseMoveCallback(float x, float y)
{
	bool handled = false;

	static int m_lastmousepos[2] = {0, 0};
	static bool isInitialized = false;
	if (m_data->pCanvas)
	{
		if (!isInitialized)
		{
			isInitialized = true;
			m_lastmousepos[0] = x + 1;
			m_lastmousepos[1] = y + 1;
		}
		handled = m_data->pCanvas->InputMouseMoved(x, y, m_lastmousepos[0], m_lastmousepos[1]);
	}
	return handled;
}

bool GwenUserInterface::keyboardCallback(int bulletKey, int state)
{
	int gwenKey = -1;
	if (m_data->pCanvas)
	{
		//convert 'Bullet' keys into 'Gwen' keys
		switch (bulletKey)
		{
			case B3G_RETURN:
			{
				gwenKey = Gwk::Key::Return;
				break;
			}
			case B3G_LEFT_ARROW:
			{
				gwenKey = Gwk::Key::Left;
				break;
			}
			case B3G_RIGHT_ARROW:
			{
				gwenKey = Gwk::Key::Right;
				break;
			}
			case B3G_UP_ARROW:
			{
				gwenKey = Gwk::Key::Up;
				break;
			}
			case B3G_DOWN_ARROW:
			{
				gwenKey = Gwk::Key::Down;
				break;
			}
			case B3G_BACKSPACE:
			{
				gwenKey = Gwk::Key::Backspace;
				break;
			}
			case B3G_DELETE:
			{
				gwenKey = Gwk::Key::Delete;
				break;
			}
			case B3G_HOME:
			{
				gwenKey = Gwk::Key::Home;
				break;
			}
			case B3G_END:
			{
				gwenKey = Gwk::Key::End;
				break;
			}
			case B3G_SHIFT:
			{
				gwenKey = Gwk::Key::Shift;
				break;
			}
			case B3G_CONTROL:
			{
				gwenKey = Gwk::Key::Control;
				break;
			}

			default:
			{
			}
		};

		if (gwenKey >= 0)
		{
			return m_data->pCanvas->InputModifierKey(gwenKey, state == 1);
		}
		else
		{
			if (bulletKey < 256 && state)
			{
				Gwk::UnicodeChar c = (Gwk::UnicodeChar)bulletKey;
				return m_data->pCanvas->InputCharacter(c);
			}
		}
	}
	return false;
}

bool GwenUserInterface::mouseButtonCallback(int button, int state, float x, float y)
{
	bool handled = false;
	if (m_data->pCanvas)
	{
		handled = m_data->pCanvas->InputMouseMoved(x, y, x, y);

		if (button >= 0)
		{
			handled = m_data->pCanvas->InputMouseButton(button, (bool)state);
			if (handled)
			{
				//if (!state)
				//	return false;
			}
		}
	}
	return handled;
}
