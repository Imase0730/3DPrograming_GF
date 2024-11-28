//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include "ImaseLib/DebugFont.h"
#include "ImaseLib/GridFloor.h"
#include "ImaseLib/DebugCamera.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

private:

    // 射影行列
    DirectX::SimpleMath::Matrix m_proj;

    // 共通ステート
    std::unique_ptr<DirectX::CommonStates> m_states;

    // デバッグフォント
    std::unique_ptr<Imase::DebugFont> m_debugFont;

    // グリッドの床
    std::unique_ptr<Imase::GridFloor> m_gridFloor;

    // デバッグカメラ
    std::unique_ptr<Imase::DebugCamera> m_debugCamera;

    // モデル
    std::unique_ptr<DirectX::Model> m_model;

    // 軸のモデル
    std::unique_ptr<DirectX::Model> m_modelX;
    std::unique_ptr<DirectX::Model> m_modelY;
    std::unique_ptr<DirectX::Model> m_modelZ;

    // 飛行機の移動の速さ（１秒間に移動する速さ）
    static const float SPEED;

    // 飛行機の回転の速さ（１秒間に回転する速さ）
    static const float ROTATIONAL_SPEED;

    // プレイヤーの位置
    DirectX::SimpleMath::Vector3 m_position;

    // プレイヤーの各軸に対する回転角（ラジアン）
    float m_rotateX;
    float m_rotateY;
    float m_rotateZ;

 };
