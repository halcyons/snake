#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <vector>
#include "DirectXCollision.h"
#include "Common\LoadCMOModel.h"
namespace Snake
{
	const float sizeNODE_SIZE = 1.0f;
	enum Direction
	{
		up = 1,
		right = 2,
		in = 3,
		down = -1,
		left = -2,
		out = -3
	};

	enum SnakePlane
	{
		Front,
		Back,
		Top,
		Bottom,
		Left,
		Right
	};

	struct Node
	{
		int x = 0;
		int y = 0;
		int z = 0;
		DirectX::BoundingBox boundingBox;
		Direction direction = Direction::up;
		Node* next = nullptr;
		Node(DirectX::BoundingBox boundingBox)
		{
			this->boundingBox = boundingBox;
		}
		void SetCoordinate(int x, int y, int z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->boundingBox.Center = DirectX::XMFLOAT3((float)x, (float)y, (float)z);
		}
	};

	struct List
	{
		Node* listHead;
		Node* listEnd;
		int count = 1;
		SnakePlane snakePlane = SnakePlane::Front;
		List(int length, DirectX::BoundingBox boundingBox)
		{			
			listHead = new Node(boundingBox);
			listEnd = listHead;
			
			// Add (length - 1) other nodes.
			for (int i = 0; i < length - 1; ++i)
			{
				AddHeader();
			}
		}
		// Add node to the header.
		void AddHeader()
		{
			Node* node = new Node(listHead->boundingBox);
			
			node->direction = listHead->direction;
			switch (listHead->direction)
			{
			case Direction::up:
				switch (snakePlane)
				{
				case Snake::Front:
					node->SetCoordinate(listHead->x, listHead->y + 1, listHead->z);
					break;
				case Snake::Back:
					node->SetCoordinate(listHead->x, listHead->y - 1, listHead->z);
					break;
				case Snake::Top:
					node->SetCoordinate(listHead->x, listHead->y, listHead->z + 1);
					break;
				case Snake::Bottom:
					node->SetCoordinate(listHead->x, listHead->y, listHead->z - 1);
					break;
				case Snake::Left:
					node->SetCoordinate(listHead->x, listHead->y + 1, listHead->z);
					break;
				case Snake::Right:
					node->SetCoordinate(listHead->x, listHead->y + 1, listHead->z);
					break;				
				}								
				break;
			case Direction::down:
				node->SetCoordinate(listHead->x, listHead->y - 1, listHead->z);
				break;
			case Direction::left:
				node->SetCoordinate(listHead->x + 1, listHead->y, listHead->z);
				break;
			case Direction::right:
				node->SetCoordinate(listHead->x - 1, listHead->y, listHead->z);
				break;
			case Direction::in:
				node->SetCoordinate(listHead->x, listHead->y, listHead->z + 1);
				break;
			case Direction::out:
				node->SetCoordinate(listHead->x, listHead->y, listHead->z - 1);
				break;
			}
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
		bool IsCollideWithBody()
		{
			bool is = false;
			if (count > 3)
			{		
				Node* start = listHead->next->next->next;

				is = IsCollideWithBB(listHead->boundingBox, start, listEnd);
			}
			return is;
			
		}		

		bool IsCollideWithBB(DirectX::BoundingBox bb, Node* start, Node* end)
		{			
			Node* tempNode = start;
			DirectX::BoundingBox tempBB(bb.Center, DirectX::XMFLOAT3(bb.Extents.x / 2, bb.Extents.y/ 2, bb.Extents.z / 2));
			while (tempNode != end->next)
			{
				if (tempNode->boundingBox.Contains(tempBB) == DirectX::ContainmentType::CONTAINS)
					return true;
				tempNode = tempNode->next;
			}			
			return false;
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

		void GameInitialize(int snakeLength);

		void ScrollViewMatrix(DirectX::XMFLOAT3 axis, bool isClockwise);


		bool m_isGameOver;
	
	private:
		std::unique_ptr<CMOModel>	m_snakeModel;
		std::unique_ptr<CMOModel>	m_foodModel;
		std::unique_ptr<CMOModel>	m_wallModel;
		Windows::Foundation::Point RandomPosition(int min, int max);
		bool RenderFood(ID3D11DeviceContext* context);
		void RenderBackground();
		void Rotate(float radians);
		std::unique_ptr<List> m_snake;
		bool m_isNeedChangePos;

		DirectX::BoundingBox m_foodBB;

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
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_modelInputLayout;
		// Food resources.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_foodInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_foodVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_foodIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_textureVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_texturePixelShader;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_foodVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_foodPixelShader;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bodySRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_foodSRV;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>		m_samplerState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_dsState;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;

		DirectX::XMFLOAT4X4 m_model;
		uint32	m_indexCount;
		uint32  m_foodIndexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
		bool	m_isNeedTurnIn;
		bool	m_isNeedTurnDown;
		bool	m_isNeedTurnOut;
		bool	m_isNeedTurnUp;
		float	m_angle;
		bool	m_isClockwise;
		
	};
}

