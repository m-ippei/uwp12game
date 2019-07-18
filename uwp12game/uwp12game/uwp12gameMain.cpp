#include "pch.h"
#include "uwp12gameMain.h"
#include "Common\DirectXHelper.h"

using namespace uwp12game;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// DirectX 12 �A�v���P�[�V���� �e���v���[�g�ɂ��ẮAhttps://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x411 ���Q�Ƃ��Ă�������

// �A�v���P�[�V�����̓ǂݍ��ݎ��ɃA�v���P�[�V�������Y��ǂݍ���ŏ��������܂��B
uwp12gameMain::uwp12gameMain()
{
	// TODO: ����̉σ^�C���X�e�b�v ���[�h�ȊO�̃��[�h���K�v�ȏꍇ�́A�^�C�}�[�ݒ��ύX���Ă��������B
	// ��: 60 FPS �Œ�^�C���X�e�b�v�X�V���W�b�N�ł́A�����Ăяo���܂�:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

//�����_���[���쐬���āA���������܂��B
void uwp12gameMain::CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	// TODO: ������A�v���̃R���e���c�̏������Œu�������܂��B
	m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(deviceResources));

	OnWindowSizeChanged();
}

// �A�v���P�[�V������Ԃ��t���[�����Ƃ� 1 ��X�V���܂��B
void uwp12gameMain::Update()
{
	// �V�[�� �I�u�W�F�N�g���X�V���܂��B
	m_timer.Tick([&]()
	{
		// TODO: ������A�v���̃R���e���c�̍X�V�֐��Œu�������܂��B
		m_sceneRenderer->Update(m_timer);
	});
}

// ���݂̃A�v���P�[�V������Ԃɉ����Č��݂̃t���[���������_�����O���܂��B
// �t���[���������_�����O����A�\����������������ƁAtrue ��Ԃ��܂��B
bool uwp12gameMain::Render()
{
	// ����X�V�O�Ƀ����_�����O�͍s��Ȃ��悤�ɂ��Ă��������B
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	// �V�[�� �I�u�W�F�N�g�������_�����O���܂��B
	// TODO: ������A�v���̃R���e���c�̃����_�����O�֐��Œu�������܂��B
	return m_sceneRenderer->Render();
}

//�A�v���P�[�V�����̏�Ԃ̍X�V���A�E�B���h�E�̃T�C�Y���ύX����� (�f�o�C�X�̕����̕ύX�Ȃ�)�A
void uwp12gameMain::OnWindowSizeChanged()
{
	// TODO: ������A�v���̃R���e���c�̃T�C�Y�Ɉˑ����鏉�����Œu�������܂��B
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

//�ꎞ��~���Ă��邱�Ƃ��A�v���ɒʒm���܂��B
void uwp12gameMain::OnSuspending()
{
	//TODO: ������A�v���̈ꎞ��~���W�b�N�Œu�������܂��B

	//�ꎞ��~�����A�v���́A�v���Z�X�������ԊǗ��ɂ���Ă��ł����f�����\��������܂��B���̂��߁A
	//�A�v�������f������Ԃ���Ďn���ł���悤�ɁA�K�v�ȏ�Ԃ�ۑ����Ă������Ƃ������߂��܂��B

	m_sceneRenderer->SaveState();

	//�A�v���P�[�V�����ŁA�č쐬�̗e�Ղȃr�f�I ����������U�肪�g�p����Ă���ꍇ�A
	//���̃�������������āA���̃A�v���P�[�V�����Ŏg�p�ł���悤�ɂ��邱�Ƃ��������Ă��������B
}

//�ꎞ��~���������ꂽ���Ƃ��A�v���ɒʒm���܂��B
void uwp12gameMain::OnResuming()
{
	//TODO: ������A�v���̍ċN�����W�b�N�Œu�������܂��B
}

// �f�o�C�X ���\�[�X���������K�v�����������Ƃ������_���[�ɒʒm���܂��B
void uwp12gameMain::OnDeviceRemoved()
{
	//TODO: �K�v�ȃA�v���P�[�V�����܂��̓����_���[�̏�Ԃ�ύX���A�����_���[
	//�ƗL���ł͂Ȃ��Ȃ������̃��\�[�X��������܂��B
	m_sceneRenderer->SaveState();
	m_sceneRenderer = nullptr;
}
