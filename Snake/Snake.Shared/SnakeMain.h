#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"
#include "Content\SampleFpsTextRenderer.h"

// Renders Direct2D and 3D content on the screen.
namespace Snake
{
	class SnakeMain : public DX::IDeviceNotify
	{
	public:
		SnakeMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~SnakeMain();
		void CreateWindowSizeDependentResources();
		void StartTracking() { m_sceneRenderer->StartTracking(); }
		void TrackingUpdate(float positionX) { m_pointerLocationX = positionX; }
		void StopTracking() { m_sceneRenderer->StopTracking(); }
		bool IsTracking() { return m_sceneRenderer->IsTracking(); }
		void GameInitialize(int length) { m_sceneRenderer->GameInitialize(length); }

		void Update();
		bool Render();

		
		void Move(int step, Direction direction) { m_sceneRenderer->Move(step, direction); }

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();
		bool m_isGameOver;
		SnakePlane m_snakePlane;
	private:
		void ProcessInput();
		

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// TODO: Replace with your own content renderers.
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
		std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;

		// Track current input pointer position.
		float m_pointerLocationX;

		
	};
}