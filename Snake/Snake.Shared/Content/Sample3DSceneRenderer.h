#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <vector>

#include "Common\LoadCMOModel.h"
#include "SnakeClass.h"
namespace Snake
{
	enum SnakePlane
	{
		Front,
		Back,
		Top,
		Bottom,
		Left,
		Right
	};

	/*struct Node
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
	};*/

	//struct List
	//{
	//	Node* listHead;
	//	Node* listEnd;
	//	int count = 1;
	//	List(int length, DirectX::BoundingBox boundingBox)
	//	{			
	//		listHead = new Node(boundingBox);
	//		listEnd = listHead;
	//		
	//		// Add (length - 1) other nodes.
	//		for (int i = 0; i < length - 1; ++i)
	//		{
	//			AddHeader();
	//		}
	//	}
	//	// Add node to the header.
	//	void AddHeader()
	//	{
	//		Node* node = new Node(listHead->boundingBox);
	//		
	//		node->direction = listHead->direction;
	//		switch (listHead->direction)
	//		{
	//		case Direction::up:
	//			node->SetCoordinate(listHead->x, listHead->y + 1, listHead->z);
	//			break;
	//		case Direction::down:
	//			node->SetCoordinate(listHead->x, listHead->y - 1, listHead->z);
	//			break;
	//		case Direction::left:
	//			node->SetCoordinate(listHead->x + 1, listHead->y, listHead->z);
	//			break;
	//		case Direction::right:
	//			node->SetCoordinate(listHead->x - 1, listHead->y, listHead->z);
	//			break;
	//		case Direction::in:
	//			node->SetCoordinate(listHead->x, listHead->y, listHead->z + 1);
	//			break;
	//		case Direction::out:
	//			node->SetCoordinate(listHead->x, listHead->y, listHead->z - 1);
	//			break;
	//		}
	//		node->next = listHead;
	//		listHead = node;
	//		count++;
	//	}
	//	// Add header to a specify point. Used for AI.
	//	void AddHeader(int x, int y)
	//	{
	//		Node* node = new Node(listHead->boundingBox);
	//		
	//		node->SetCoordinate(x, y, listHead->z);

	//		node->next = listHead;
	//		listHead = node;
	//		count++;
	//	}

	//	// Delete the tail.
	//	void DelTail()
	//	{
	//		Node* node = listHead;
	//		while (node->next != listEnd)
	//		{
	//			node = node->next;
	//		}
	//		delete node->next;
	//		node->next = nullptr;
	//		

	//		listEnd = node;
	//		node = nullptr;

	//		count--;
	//	}

	//	// 
	//	bool IsCollideWithBody()
	//	{
	//		bool is = false;
	//		if (count > 3)
	//		{		
	//			Node* start = listHead->next->next->next;

	//			is = IsCollideWithBB(listHead->boundingBox, start, listEnd);
	//		}
	//		return is;
	//		
	//	}		

	//	bool IsCollideWithBB(DirectX::BoundingBox bb, Node* start, Node* end)
	//	{			
	//		Node* tempNode = start;
	//		DirectX::BoundingBox tempBB(bb.Center, DirectX::XMFLOAT3(bb.Extents.x / 2, bb.Extents.y/ 2, bb.Extents.z / 2));
	//		while (tempNode != end->next)
	//		{
	//			if (tempNode->boundingBox.Contains(tempBB) == DirectX::ContainmentType::CONTAINS)
	//				return true;
	//			tempNode = tempNode->next;
	//		}			
	//		return false;
	//	}

	//	bool Contains(XMFLOAT3 pt)
	//	{
	//		Node* tempNode = listHead;
	//		for (int i = 0; i < count; ++i)
	//		{				
	//			if (tempNode->x == pt.x && tempNode->y == pt.y && tempNode->z == pt.z)
	//			{
	//				return true;
	//			}
	//			tempNode = tempNode->next;
	//		}
	//		return false;
	//	}
	//};

	

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

		void Move(Direction dir);
		Direction MoveTo(const DXSnakeNode& node);

		void ChangeDirection(Direction direction);

		void GameInitialize();

		void ScrollViewMatrix();


		bool m_isGameOver;
		SnakePlane m_snakePlane;
	private:
		std::unique_ptr<CMOModel>	m_snakeModel;
		std::unique_ptr<CMOModel>	m_foodModel;
		std::unique_ptr<CMOModel>	m_wallModel;
		Windows::Foundation::Point RandomPosition(int min, int max);
		bool RenderFood(ID3D11DeviceContext* context);
		void RenderBackground();
		void Rotate(float radians);
		void ResetViewMatrix();
		bool IsAvailable(int x, int y);

		std::shared_ptr<SnakeClass> m_snake;
		
		bool m_isNeedChangePos;

		//DirectX::BoundingBox m_foodBB;
		DXSnakeNode	m_foodNode;

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
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_normalVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_normalPixelShader;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bodySRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_foodSRV;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>		m_samplerState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_dsState;
		Microsoft::WRL::ComPtr<ID3D11BlendState>		m_blendState;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;

		DirectX::XMFLOAT4X4 m_model;
		uint32	m_indexCount;
		uint32  m_foodIndexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
		bool	m_isNeedScroll;
		float	m_angle;
		bool	m_isAutoPlay;
		DirectX::XMFLOAT4X4 m_viewRotation;

		Direction m_nextDir;
		
	};
}

