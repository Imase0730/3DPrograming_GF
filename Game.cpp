//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

// 飛行機の移動の速さ（１秒間に移動する距離）
const float Game::SPEED = 3.0f;

// 飛行機の回転の速さ（１秒間に回転する速さ）
const float Game::ROTATIONAL_SPEED = 90.0f;

Game::Game() noexcept(false)
    : m_rotateX(0.0f), m_rotateY(0.0f), m_rotateZ(0.0f)
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

    // デバッグカメラの作成
    m_debugCamera = std::make_unique<Imase::DebugCamera>(width, height);

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

    // デバッグカメラの更新
    m_debugCamera->Update();

    // キーボードの情報取得する
    auto kb = Keyboard::Get().GetState();

    // Z軸回転
    if (kb.E)
    {
        m_rotateZ += XMConvertToRadians(ROTATIONAL_SPEED) * elapsedTime;
    }
    if (kb.Q)
    {
        m_rotateZ -= XMConvertToRadians(ROTATIONAL_SPEED) * elapsedTime;
    }

    // X軸回転
    if (kb.W)
    {
        m_rotateX += XMConvertToRadians(ROTATIONAL_SPEED) * elapsedTime;
    }
    if (kb.S)
    {
        m_rotateX -= XMConvertToRadians(ROTATIONAL_SPEED) * elapsedTime;
    }

    // Y軸回転
    if (kb.A)
    {
        m_rotateY += XMConvertToRadians(ROTATIONAL_SPEED) * elapsedTime;
    }
    if (kb.D)
    {
        m_rotateY -= XMConvertToRadians(ROTATIONAL_SPEED) * elapsedTime;
    }

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

    // デバッグカメラからビュー行列を取得する
    SimpleMath::Matrix view = m_debugCamera->GetCameraMatrix();

    // グリッドの床」の描画
    m_gridFloor->Render(context, view, m_proj);

    // ワールド行列
    SimpleMath::Matrix world;

    // 初期回転行列を作成する
    SimpleMath::Matrix initRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(270.0f));

    // 平行移動行列を作成する
    //SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_position);

    // 各軸に対する回転行列を作成する
    SimpleMath::Matrix rotX = SimpleMath::Matrix::CreateRotationX(m_rotateX);
    SimpleMath::Matrix rotY = SimpleMath::Matrix::CreateRotationY(m_rotateY);
    SimpleMath::Matrix rotZ = SimpleMath::Matrix::CreateRotationZ(m_rotateZ);

    // ワールド行列を更新する
    world = initRotY * rotZ * rotX * rotY;

     // モデルの描画
    m_model->Draw(context, *m_states.get(), world, view, m_proj);

    // Z軸モデルの描画
    world = rotZ * rotX * rotY;
    m_modelZ->Draw(context, *m_states.get(), world, view, m_proj);

    // X軸モデルの描画
    world = rotX * rotY;
    m_modelX->Draw(context, *m_states.get(), world, view, m_proj);

    // Y軸モデルの描画
    world = rotY;
    m_modelY->Draw(context, *m_states.get(), world, view, m_proj);

    // FPSを取得する
    uint32_t fps = m_timer.GetFramesPerSecond();

    // FPSの表示
    m_debugFont->AddString(0, 0, Colors::White, L"FPS=%d", fps);

    // デバッグフォントの描画
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

    // 共通ステートの作成
    m_states = std::make_unique<CommonStates>(device);

    // デバッグフォントの作成
    m_debugFont = std::make_unique<Imase::DebugFont>(device
        , context, L"Resources\\Font\\SegoeUI_18.spritefont");

    // グリッド床の作成
    m_gridFloor = std::make_unique<Imase::GridFloor>(device, context, m_states.get());

    // モデルの読み込み
    EffectFactory fx(device);
    fx.SetDirectory(L"Resources\\Models");
    m_model = Model::CreateFromSDKMESH(device, L"Resources\\Models\\player.sdkmesh", fx);

    // 軸モデルの読み込み
    m_modelX = Model::CreateFromSDKMESH(device, L"Resources\\Models\\RingX.sdkmesh", fx);
    m_modelY = Model::CreateFromSDKMESH(device, L"Resources\\Models\\RingY.sdkmesh", fx);
    m_modelZ = Model::CreateFromSDKMESH(device, L"Resources\\Models\\RingZ.sdkmesh", fx);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

    // 画面サイズの取得
    RECT rect = m_deviceResources->GetOutputSize();

    // 射影行列の作成
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
