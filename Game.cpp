//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

// �l�p�`�̒��_�f�[�^
VertexPositionNormal g_vertexes[4 * 4] =
{
    // �O��
    { SimpleMath::Vector3(-1.0f,  1.0f,  1.0f), SimpleMath::Vector3( 0.0f, 0.0f, 1.0f) },    // 0
    { SimpleMath::Vector3( 1.0f,  1.0f,  1.0f), SimpleMath::Vector3( 0.0f, 0.0f, 1.0f) },    // 1
    { SimpleMath::Vector3( 1.0f, -1.0f,  1.0f), SimpleMath::Vector3( 0.0f, 0.0f, 1.0f) },    // 2
    { SimpleMath::Vector3(-1.0f, -1.0f,  1.0f), SimpleMath::Vector3( 0.0f, 0.0f, 1.0f) },    // 3
    // ���
    { SimpleMath::Vector3( 1.0f,  1.0f, -1.0f), SimpleMath::Vector3( 0.0f, 0.0f, -1.0f) },   // 4
    { SimpleMath::Vector3(-1.0f,  1.0f, -1.0f), SimpleMath::Vector3( 0.0f, 0.0f, -1.0f) },   // 5
    { SimpleMath::Vector3(-1.0f, -1.0f, -1.0f), SimpleMath::Vector3( 0.0f, 0.0f, -1.0f) },   // 6
    { SimpleMath::Vector3( 1.0f, -1.0f, -1.0f), SimpleMath::Vector3( 0.0f, 0.0f, -1.0f) },   // 7
    // ����
    { SimpleMath::Vector3( 1.0f,  1.0f,  1.0f), SimpleMath::Vector3( 1.0f, 0.0f,  0.0f) },   // 8
    { SimpleMath::Vector3( 1.0f,  1.0f, -1.0f), SimpleMath::Vector3( 1.0f, 0.0f,  0.0f) },   // 9
    { SimpleMath::Vector3( 1.0f, -1.0f, -1.0f), SimpleMath::Vector3( 1.0f, 0.0f,  0.0f) },   // 10
    { SimpleMath::Vector3( 1.0f, -1.0f,  1.0f), SimpleMath::Vector3( 1.0f, 0.0f,  0.0f) },   // 11
    // �E��
    { SimpleMath::Vector3(-1.0f,  1.0f, -1.0f), SimpleMath::Vector3(-1.0f, 0.0f,  0.0f) },   // 12
    { SimpleMath::Vector3(-1.0f,  1.0f,  1.0f), SimpleMath::Vector3(-1.0f, 0.0f,  0.0f) },   // 13
    { SimpleMath::Vector3(-1.0f, -1.0f,  1.0f), SimpleMath::Vector3(-1.0f, 0.0f,  0.0f) },   // 14
    { SimpleMath::Vector3(-1.0f, -1.0f, -1.0f), SimpleMath::Vector3(-1.0f, 0.0f,  0.0f) },   // 15
};

// �l�p�`�̃C���f�b�N�X�f�[�^
uint16_t g_indexes[3 * 2 * 4] = 
{
    0, 1, 2,
    2, 3, 0,

    4, 5, 6,
    6, 7, 4,

    8, 9, 10,
    10, 11, 8,

    12, 13, 14,
    14, 15, 12,
};

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    // �f�o�b�O�J�����̍쐬
    m_debugCamera = std::make_unique<Imase::DebugCamera>(width, height);

    // ���C�g�̕����x�N�g���̏�����
    m_lightDir = SimpleMath::Vector3(1.0f, 0.0f, 0.0f);

}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
        {
            Update(m_timer);
        });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

    // �f�o�b�O�J�����̍X�V
    m_debugCamera->Update();

    // ���C�g��Y����]����
    SimpleMath::Matrix rotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(45.0f) * elapsedTime);
    m_lightDir = SimpleMath::Vector3::Transform(m_lightDir, rotY);

}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;

    // �f�o�b�O�J��������r���[�s����擾����
    SimpleMath::Matrix view = m_debugCamera->GetCameraMatrix();

    // �O���b�h�̏��v�̕`��
    m_gridFloor->Render(context, view, m_proj);

    ///////////////////////////////////////////////////////////

    // �[�x�X�e���V���o�b�t�@�̐ݒ�
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    // �u�����h�X�e�[�g�̐ݒ�
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xffffffff);

    // �J�����O�̐ݒ�
    //context->RSSetState(m_states->CullCounterClockwise());  // ���_�̏��Ԃ��t���v�����J�����O����
    context->RSSetState(m_states->CullNone());              // �J�����O���Ȃ�
    //context->RSSetState(m_states->CullClockwise());         // ���_�̏��Ԃ����v�����J�����O����

    // �����̐ݒ�
    m_basicEffect->SetAmbientLightColor(SimpleMath::Color(0.3f, 0.3f, 0.3f));

    // �}�e���A���̐ݒ�
    m_basicEffect->SetDiffuseColor(Colors::White);
    m_basicEffect->SetSpecularColor(Colors::Black);
    m_basicEffect->SetEmissiveColor(Colors::Black);

    // ���C�g��ON/OFF
    m_basicEffect->SetLightEnabled(0, true);
    m_basicEffect->SetLightEnabled(1, false);
    m_basicEffect->SetLightEnabled(2, false);

    // ���C�g�̐F�̐ݒ�
    m_basicEffect->SetLightDiffuseColor(0, Colors::White);
    m_basicEffect->SetLightSpecularColor(0, Colors::Black);

    // ���C�g�̕�����ݒ肷��
    m_basicEffect->SetLightDirection(0, m_lightDir);

    // �e�N�X�`���T���v���[�̐ݒ�
    //ID3D11SamplerState* samplers[] = { m_states->PointClamp() };
    //context->PSSetSamplers(0, 1, samplers);

    // ���[���h�s��
    SimpleMath::Matrix world;
    m_basicEffect->SetWorld(world);
    // �r���[�s��
    m_basicEffect->SetView(view);
    // �ˉe�s��
    m_basicEffect->SetProjection(m_proj);

    // �e�N�X�`��
    //m_basicEffect->SetTexture(m_texture.Get());

    // �G�t�F�N�g��K������
    m_basicEffect->Apply(context);

    // ���̓��C�A�E�g
    context->IASetInputLayout(m_inputLayout.Get());

    // �v���~�e�B�u�o�b�`�̕`��
    m_primitiveBatch->Begin();

    // �l�p�`�̕`��
    m_primitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_indexes, 6 * 4, g_vertexes, 4 * 4);

    m_primitiveBatch->End();

    ///////////////////////////////////////////////////////////

    // FPS���擾����
    uint32_t fps = m_timer.GetFramesPerSecond();

    // FPS�̕\��
    m_debugFont->AddString(0, 0, Colors::White, L"FPS=%d", fps);

    // �f�o�b�O�t�H���g�̕`��
    m_debugFont->Render(m_states.get());

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;

    // ���ʃX�e�[�g�̍쐬
    m_states = std::make_unique<CommonStates>(device);

    // �f�o�b�O�t�H���g�̍쐬
    m_debugFont = std::make_unique<Imase::DebugFont>(device
        , context, L"Resources\\Font\\SegoeUI_18.spritefont");

    // �O���b�h���̍쐬
    m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, m_states.get());

    // �x�[�V�b�N�G�t�F�N�g�̍쐬
    m_basicEffect = std::make_unique<BasicEffect>(device);
    // ���C�g(ON)
    m_basicEffect->SetLightingEnabled(true);
    // ���_�J���[(OFF)
    m_basicEffect->SetVertexColorEnabled(false);
    // �e�N�X�`��(OFF)
    m_basicEffect->SetTextureEnabled(false);

    // ���̓��C�A�E�g�̍쐬
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionNormal>(
            device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf())
    );

    // �v���~�e�B�u�o�b�`�̍쐬
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionNormal>>(context);

    // DDS�e�N�X�`���̓ǂݍ���
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(
            device, L"Resources\\Models\\image3.dds", nullptr, m_texture.ReleaseAndGetAddressOf())
    );

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

    // ��ʃT�C�Y�̎擾
    RECT rect = m_deviceResources->GetOutputSize();

    // �ˉe�s��̍쐬
    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        XMConvertToRadians(45.0f)
        , static_cast<float>(rect.right) / static_cast<float>(rect.bottom)
        , 0.1f, 100.0f);

}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
