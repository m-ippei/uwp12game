#include "pch.h"
#include "App.h"

#include <ppltasks.h>

using namespace uwp12game;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

using Microsoft::WRL::ComPtr;

// DirectX 12 �A�v���P�[�V���� �e���v���[�g�ɂ��ẮAhttps://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x411 ���Q�Ƃ��Ă�������

// main �֐��́AIFrameworkView �N���X������������ꍇ�ɂ̂ݎg�p���܂��B
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new App();
}

App::App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

// IFrameworkView �̍쐬���ɍŏ��̃��\�b�h���Ăяo����܂��B
void App::Initialize(CoreApplicationView^ applicationView)
{
	// �A�v�� ���C�t�T�C�N���̃C�x���g �n���h���[��o�^���܂��B���̗�ɂ̓A�N�e�B�u�����܂܂�Ă��邽�߁A
	// CoreWindow ���A�N�e�B�u�ɂ��A�E�B���h�E�ŕ`����J�n�ł��܂��B
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &App::OnResuming);
}

//CoreWindow �I�u�W�F�N�g���쐬 (�܂��͍č쐬) �����Ƃ��ɌĂяo����܂��B
void App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);
}

// �V�[���̃��\�[�X�����������邩�A�ȑO�ɕۑ������A�v����Ԃ�ǂݍ��݂܂��B
void App::Load(Platform::String^ entryPoint)
{
	if (m_main == nullptr)
	{
		m_main = std::unique_ptr<uwp12gameMain>(new uwp12gameMain());
	}
}

// ���̃��\�b�h�́A�E�B���h�E���A�N�e�B�u�ɂȂ�ƁA�Ăяo����܂��B
void App::Run()
{
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			auto commandQueue = GetDeviceResources()->GetCommandQueue();
			PIXBeginEvent(commandQueue, 0, L"Update");
			{
				m_main->Update();
			}
			PIXEndEvent(commandQueue);

			PIXBeginEvent(commandQueue, 0, L"Render");
			{
				if (m_main->Render())
				{
					GetDeviceResources()->Present();
				}
			}
			PIXEndEvent(commandQueue);
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

// IFrameworkView �ŕK�v�ł��B
// �I���C�x���g�ł͏����������͌Ăяo����܂���B�A�v���P�[�V�������O�i�ɕ\������Ă���Ԃ�
//IFrameworkView �N���X����̂����ƌĂяo����܂��B
void App::Uninitialize()
{
}

// �A�v���P�[�V���� ���C�t�T�C�N�� �C�x���g �n���h���[�B

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	// Run() �� CoreWindow ���A�N�e�B�u�������܂ŋN������܂���B
	CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// �x����v��������ɃA�v���P�[�V�����̏�Ԃ�ۑ����܂��B�x����Ԃ�ێ����邱�Ƃ́A
	//���f����̎��s�ŃA�v���P�[�V�������r�W�[��Ԃł��邱�Ƃ������܂��B
	//�x���͐����Ȃ��ێ������킯�ł͂Ȃ����Ƃɒ��ӂ��Ă��������B�� 5 �b��ɁA
	// �A�v���P�[�V�����͋����I������܂��B
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
		m_main->OnSuspending();
		deferral->Complete();
	});
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// ���f���ɃA�����[�h���ꂽ�f�[�^�܂��͏�Ԃ𕜌����܂��B����ł́A�f�[�^�Ə�Ԃ�
	// ���f����ĊJ����Ƃ��ɕێ�����܂��B���̃C�x���g�́A�A�v�������ɏI������Ă���ꍇ��
	//�������܂���B

	m_main->OnResuming();
}

// �E�B���h�E �C�x���g �n���h���[�B

void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	GetDeviceResources()->SetLogicalSize(Size(sender->Bounds.Width, sender->Bounds.Height));
	m_main->OnWindowSizeChanged();
}

void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

// DisplayInformation �C�x���g �n���h���[�B

void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	// ����: ���𑜓x�̃f�o�C�X�p�ɃX�P�[�����O����Ă���ꍇ�́A�擾���� LogicalDpi �̒l���A�v���̗L���� DPI �ƈ�v���Ȃ��ꍇ������܂��B
	// DPI �� DeviceResources ��ɐݒ肳�ꂽ�ꍇ�A
	// ��� GetDpi ���\�b�h���g�p���Ă�����擾����K�v������܂��B
	// �ڍׂɂ��ẮADeviceResources.cpp ���Q�Ƃ��Ă��������B
	GetDeviceResources()->SetDpi(sender->LogicalDpi);
	m_main->OnWindowSizeChanged();
}

void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
	GetDeviceResources()->SetCurrentOrientation(sender->CurrentOrientation);
	m_main->OnWindowSizeChanged();
}

void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
	GetDeviceResources()->ValidateDevice();
}

std::shared_ptr<DX::DeviceResources> App::GetDeviceResources()
{
	if (m_deviceResources != nullptr && m_deviceResources->IsDeviceRemoved())
	{
		//������ D3D �f�o�C�X�ւ̂��ׂĂ̎Q�Ƃ��܂��������K�v������A���̌�ŐV�����f�o�C�X��
		//�쐬�ł��܂��B

		m_deviceResources = nullptr;
		m_main->OnDeviceRemoved();

#if defined(_DEBUG)
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
#endif
	}

	if (m_deviceResources == nullptr)
	{
		m_deviceResources = std::make_shared<DX::DeviceResources>();
		m_deviceResources->SetWindow(CoreWindow::GetForCurrentThread());
		m_main->CreateRenderers(m_deviceResources);
	}
	return m_deviceResources;
}
