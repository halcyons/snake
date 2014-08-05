#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <vector>
#include "DirectXCollision.h"

namespace Snake
{
	const float NODE_SIZE = 1.0f;
	enum Direction
	{
		up = 1,
		right = 2,
		down = -1,
		left = -2
	};

	struct Node
	{
		int x = 0;
		int y = 0;
		int size = 1;
		DirectX::BoundingBox boundingBox = DirectX::BoundingBox(
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(NODE_SIZE / 2, NODE_SIZE / 2, NODE_SIZE / 2));
		Direction direction = Direction::up;
		Node* next = nullptr;
	};

	struct List
	{
		Node* listHead = new Node;
		Node* listEnd = listHead;
		int count = 1;
		
		// Add node to the header.
		void AddHeader()
		{
			Node* node = new Node;
			node->direction = listHead->direction;
			switch (listHead->direction)
			{
			case Direction::up:
				node->x = listHead->x;
				node->y = listHead->y + listHead->size;
				break;
			case Direction::down:
				node->x = listHead->x;
				node->y = listHead->y - listHead->size;
				break;
			case Direction::left:
				node->x = listHead->x - listHead->size;
				node->y = listHead->y;
				break;
			case Direction::right:
				node->x = listHead->x + listHead->size;
				node->y = listHead->y;
				break;
			}
			node->size = listHead->size;
			node->next = listHead;
			listHead = node;
			count++;
		}

		// Delete the tail.
		void DelTail()
		{
			Node* node = listHead;
			while (node->next != listEnd)
			{
				node = node->next;
			}
			delete node->next;
			node->next = nullptr;
			

			listEnd = node;
			node = nullptr;

			count--;
		}

		// 
		bool IsIntersectWithBody()
		{
			bool is = false;
			if (listHead->next->next != nullptr)
			{
				DirectX::BoundingBox headBB;
				switch (listHead->direction)
				{
				case Direction::up:
				case Direction::down:
					headBB = DirectX::BoundingBox(listHead->boundingBox.Center,
						DirectX::XMFLOAT3(listHead->boundingBox.Extents.x / 2, listHead->boundingBox.Extents.y, listHead->boundingBox.Extents.z));
					break;
				case Direction::left:
				case Direction::right:
					headBB = DirectX::BoundingBox(listHead->boundingBox.Center,
						DirectX::XMFLOAT3(listHead->boundingBox.Extents.x, listHead->boundingBox.Extents.y / 2, listHead->boundingBox.Extents.z));
					break;
				default:
					break;
				}
				Node* tempNode = listHead->next->next;

				while (tempNode != nullptr)
				{
					DirectX::BoundingBox tempBB = tempNode->boundingBox;
					if (headBB.Contains(tempBB) == DirectX::ContainmentType::INTERSECTS ||
						headBB.Contains(tempBB) == DirectX::ContainmentType::CONTAINS)
					{
						is = true;
						break;
					}
					tempNode = tempNode->next;
				}
				tempNode = nullptr;
			}
			return is;
			
		}
	};
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		bool Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }

		
		void Move(int step);
		void Move(int step, Direction direction);
		void Move(float step, Direction direction, DirectX::XMFLOAT4X4 &matrix);

		void GameInitialize();

	private:
		
		void Rotate(float radians);
		std::unique_ptr<List> m_snake;

	private:

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;

		DirectX::XMFLOAT4X4 m_model;
		uint32	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}

