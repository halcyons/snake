//
// DirectXPage.xaml.cpp
// Implementation of the DirectXPage class.
//

#include "pch.h"
#include "DirectXPage.xaml.h"

using namespace Snake;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

DirectXPage^ DirectXPage::current = nullptr;

DirectXPage::DirectXPage():
	m_windowVisible(true),
	m_coreInput(nullptr),
	m_isNeedRender(true)
{
	InitializeComponent();

	// This is a static public property that allows downstream pages to get a handle to the MainPage instance 
	// in order to call methods that are in this class. 
	DirectXPage::current = this;

	// Register event handlers for page lifecycle.
	CoreWindow^ window = Window::Current->CoreWindow;

	//this->AddHandler(UIElement::PointerWheelChangedEvent, ref new PointerEventHandler(this, &DirectXPage::OnWheelChanged), true);

	window->PointerWheelChanged += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &DirectXPage::OnWheelChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &DirectXPage::OnVisibilityChanged);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDisplayContentsInvalidated);

	swapChainPanel->CompositionScaleChanged += 
		ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &DirectXPage::OnCompositionScaleChanged);

	swapChainPanel->SizeChanged +=
		ref new SizeChangedEventHandler(this, &DirectXPage::OnSwapChainPanelSizeChanged);

	// Disable all pointer visual feedback for better performance when touching.
	auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
	pointerVisualizationSettings->IsContactFeedbackEnabled = false; 
	pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;

	// At this point we have access to the device. 
	// We can create the device-dependent resources.
	m_deviceResources = std::make_shared<DX::DeviceResources>();
	m_deviceResources->SetSwapChainPanel(swapChainPanel);

	// Register our SwapChainPanel to get independent input pointer events
	auto workItemHandler = ref new WorkItemHandler([this] (IAsyncAction ^)
	{
		// The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
		m_coreInput = swapChainPanel->CreateCoreIndependentInputSource(
			Windows::UI::Core::CoreInputDeviceTypes::Mouse |
			Windows::UI::Core::CoreInputDeviceTypes::Touch |
			Windows::UI::Core::CoreInputDeviceTypes::Pen
			);

		// Register for pointer events, which will be raised on the background thread.
		m_coreInput->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &DirectXPage::OnPointerPressed);
		m_coreInput->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &DirectXPage::OnPointerMoved);
		m_coreInput->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &DirectXPage::OnPointerReleased);

		// Begin processing input messages as they're delivered.
		m_coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
	});

	// Run task on a dedicated high priority background thread.
	m_inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);

	m_main = std::unique_ptr<SnakeMain>(new SnakeMain(m_deviceResources));
	//m_main->StartRenderLoop();

	CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &DirectXPage::OnRendering));
}

DirectXPage::~DirectXPage()
{
	// Stop rendering and processing events on destruction.
	m_isNeedRender = false;
	m_coreInput->Dispatcher->StopProcessEvents();
}


void DirectXPage::OnRendering(Object^ sender, Object^ args)
{
	if (m_isNeedRender)
	{
		m_main->Update();
		if (m_main->Render())
		{
			m_deviceResources->Present();
			if (m_main->m_isGameOver)
			{
				SetGameOver();
			}
		}
	}	
}


void DirectXPage::OnWheelChanged(CoreWindow^ sender, PointerEventArgs^ e)
{
	Windows::Foundation::Point pt = e->CurrentPoint->Position;
}

void DirectXPage::SetGameOver()
{

	// This function may be called from a different thread. 
	// All XAML updates need to occur on the UI thread so dispatch to ensure this is true. 
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		overlayGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
		m_isNeedRender = false;		
		
	})
		);
	
}

// Saves the current state of the app for suspend and terminate events.
void DirectXPage::SaveInternalState(IPropertySet^ state)
{
	m_deviceResources->Trim();

	// Stop rendering when the app is suspended.
	m_isNeedRender = false;

	// Put code to save app state here.
}

// Loads the current state of the app for resume events.
void DirectXPage::LoadInternalState(IPropertySet^ state)
{
	// Put code to load app state here.

	// Start rendering when the app is resumed.
	m_isNeedRender = true;
}

// Window event handlers.

void DirectXPage::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
	if (m_windowVisible)
	{
		m_isNeedRender = true;
	}
	else
	{
		m_isNeedRender = false;
	}
}

// DisplayInformation event handlers.

void DirectXPage::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	m_deviceResources->SetDpi(sender->LogicalDpi);
	m_main->CreateWindowSizeDependentResources();
}

void DirectXPage::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
	m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
	m_main->CreateWindowSizeDependentResources();
}


void DirectXPage::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
	m_deviceResources->ValidateDevice();
}

// Called when the app bar button is clicked.
void DirectXPage::AppBarButton_Click(Object^ sender, RoutedEventArgs^ e)
{
	// Use the app bar if it is appropriate for your app. Design the app bar, 
	// then fill in event handlers (like this one).
}


void DirectXPage::OnPointerPressed(Object^ sender, PointerEventArgs^ e)
{
	// When the pointer is pressed begin tracking the pointer movement.
	//m_main->StartTracking();
}

void DirectXPage::OnPointerMoved(Object^ sender, PointerEventArgs^ e)
{
	//// Update the pointer tracking code.
	//if (m_main->IsTracking())
	//{
	//	m_main->TrackingUpdate(e->CurrentPoint->Position.X);
	//}
}

void DirectXPage::OnPointerReleased(Object^ sender, PointerEventArgs^ e)
{
	// Stop tracking pointer movement when the pointer is released.
	//m_main->StopTracking();
}

void DirectXPage::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args)
{
	m_deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
	m_main->CreateWindowSizeDependentResources();
}

void DirectXPage::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
	m_deviceResources->SetLogicalSize(e->NewSize);
	m_main->CreateWindowSizeDependentResources();
}


void DirectXPage::Page_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	auto key = e->Key;
	// figure out the command from the keyboard
	if (key == Windows::System::VirtualKey::W || key == Windows::System::VirtualKey::Up)		// forward
	{
		switch (m_main->m_snakePlane)
		{
		case SnakePlane::Front:
			m_main->Move(1, Direction::up);
			break;
		case SnakePlane::Top:
			m_main->Move(1, Direction::in);
			break;
		case SnakePlane::Back:
			m_main->Move(1, Direction::down);
			break;
		case SnakePlane::Bottom:
			m_main->Move(1, Direction::out);
			break;
		}		
	}
		
	if (key == Windows::System::VirtualKey::S || key == Windows::System::VirtualKey::Down)		// back
	{
		switch (m_main->m_snakePlane)
		{
		case SnakePlane::Front:
			m_main->Move(1, Direction::down);
			break;
		case SnakePlane::Top:
			m_main->Move(1, Direction::out);
			break;
		case SnakePlane::Back:
			m_main->Move(1, Direction::up);
			break;
		case SnakePlane::Bottom:
			m_main->Move(1, Direction::in);
			break;
		}
	}		
	if (key == Windows::System::VirtualKey::A || key == Windows::System::VirtualKey::Left)		// left
		m_main->Move(1, Direction::left);
	if (key == Windows::System::VirtualKey::D || key == Windows::System::VirtualKey::Right)		// right
		m_main->Move(1, Direction::right);
}


void DirectXPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	overlayGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	m_main->GameInitialize(3);
	m_isNeedRender = true;
}

