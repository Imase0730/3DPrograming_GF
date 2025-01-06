//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

// 四角形の頂点データ
VertexPositionTexture g_vertexes[4] =
{
    { SimpleMath::Vector3(-1.0f,  1.0f, 0.0f), SimpleMath::Vector2(0.0f, 0.0f) },    // 0
    { SimpleMath::Vector3( 1.0f,  1.0f, 0.0f), SimpleMath::Vector2(1.0f, 0.0f) },    // 1
    { SimpleMath::Vector3( 1.0f, -1.0f, 0.0f), SimpleMath::Vector2(1.0f, 1.0f) },    // 2
    { SimpleMath::Vector3(-1.0f, -1.0f, 0.0f), SimpleMath::Vector2(0.0f, 1.0f) }     // 3
};

// 四角形のインデックスデータ
uint16_t g_indexes[3 * 2] = 
{
    0, 1, 2,
    2, 3, 0
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

    ///////////////////////////////////////////////////////////

    // 深度ステンシルバッファの設定
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);

    // ブレンドステートの設定
    context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xffffffff);

    // カリングの設定
    context->RSSetState(m_states->CullCounterClockwise());  // 頂点の順番が逆時計回りをカリングする
    //context->RSSetState(m_states->CullNone());              // カリングしない
    //context->RSSetState(m_states->CullClockwise());         // 頂点の順番が時計回りをカリングする

    // テクスチャサンプラーの設定
    ID3D11SamplerState* samplers[] = { m_states->PointClamp() };
    context->PSSetSamplers(0, 1, samplers);

    // ワールド行列
    SimpleMath::Matrix world;
    m_basicEffect->SetWorld(world);
    // ビュー行列
    m_basicEffect->SetView(view);
    // 射影行列
    m_basicEffect->SetProjection(m_proj);

    // テクスチャ
    m_basicEffect->SetTexture(m_texture.Get());

    // エフェクトを適応する
    m_basicEffect->Apply(context);

    // 入力レイアウト
    context->IASetInputLayout(m_inputLayout.Get());

    // プリミティブバッチの描画
    m_primitiveBatch->Begin();

    // 四角形の描画
    m_primitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_indexes, 6, g_vertexes, 4);

    m_primitiveBatch->End();

    ///------------------------------------------------------//

    // ワールド行列
    world = SimpleMath::Matrix::CreateTranslation(1.0f, 0.0f, -1.0f);
    m_basicEffect->SetWorld(world);

    // エフェクトを適応する
    m_basicEffect->Apply(context);

    // プリミティブバッチの描画
    m_primitiveBatch->Begin();

    // 四角形の描画
    m_primitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_indexes, 6, g_vertexes, 4);

    m_primitiveBatch->End();

    ///////////////////////////////////////////////////////////

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

    // ベーシックエフェクトの作成
    m_basicEffect = std::make_unique<BasicEffect>(device);
    // ライト(OFF)
    m_basicEffect->SetLightingEnabled(false);
    // 頂点カラー(OFF)
    m_basicEffect->SetVertexColorEnabled(false);
    // テクスチャ(ON)
    m_basicEffect->SetTextureEnabled(true);

    // 入力レイアウトの作成
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionTexture>(
            device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf())
    );

    // プリミティブバッチの作成
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

    // DDSテクスチャの読み込み
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(
            device, L"Resources\\Models\\image3.dds", nullptr, m_texture.ReleaseAndGetAddressOf())
    );

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
