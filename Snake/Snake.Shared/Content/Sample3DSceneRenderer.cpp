#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include "math.h"
#include "Common\WICTextureLoader.h"
#include <random>
#include "DirectXPackedVector.h"
#include "PathFinding.h"
using namespace Snake;

using namespace DirectX;
using namespace Windows::Foundation;

const int TopBoundary = 10;
const int BottomBoundary = -10;
const int FrontBoundary = 0;
const int BackBoundary = 10;
const int LeftBoundary = 30;
const int RightBoundary = -30;


// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_foodIndexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources),
	m_isNeedChangePos(true),
	m_isGameOver(false),
	m_isNeedScroll(false),
	m_angle(0.0f),
	m_snakePlane(SnakePlane::Front),
	m_isAutoPlay(false),
	m_nextDir(Direction::up)
{
	XMStoreFloat4x4(&m_model, XMMatrixIdentity());
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

bool Sample3DSceneRenderer::IsAvailable(int x, int y)
{
	BaseNode node = BaseNode(float3((float)x, (float)y, 0.0f));
	if (m_snake->IsCollideWithSnake(node, 0, m_snake->GetSize()))
	{
		return false;
	}
	else
	{
		return true;
	}
		
}

Direction Sample3DSceneRenderer::MoveTo(const BaseNode& node)
{
	float3 pos = m_snake->GetSnakeList().front().position;
	if (pos.x > node.position.x)
	{
		return Direction::left;
	}
	else if (pos.x < node.position.x)
	{
		return Direction::right;
	}
	if (pos.y > node.position.y)
	{
		return Direction::down;
	}
	else if (pos.y < node.position.y)
	{
		return Direction::up;
	}
}

void Sample3DSceneRenderer::Move(Direction dir)
{
	m_snake->Move(dir);
}

//void Sample3DSceneRenderer::Move(int step)
//{
//	for (int i = 0; i < step; ++i)
//	{
//		m_snake->DelTail();
//		m_snake->AddHeader();
//	}
//}
//
//// Move default model matrix
//void Sample3DSceneRenderer::Move(int step, Direction direction)
//{
//	if (m_isNeedScroll)
//	{
//		return;
//	}
//	float angle = 0.0f; // The angle need to rotate per frame.
//	XMVECTOR axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // Rotate around this axis.
//
//
//	// When the snake just reachs the right limit.
//	if (m_snake->listHead->x == RightBoundary && m_snake->listHead->direction == Direction::right)
//	{
//		m_isGameOver = true;
//	}
//	// When the snake just reachs the left limit.
//	else if (m_snake->listHead->x == LeftBoundary && m_snake->listHead->direction == Direction::left)
//	{
//		m_isGameOver = true;
//	}
//	else
//	{
//		// When the snake is at the front plane.
//		if (m_snake->listHead->z == FrontBoundary)
//		{
//			// When the snake just reachs the top limit. 
//			if (m_snake->listHead->y == TopBoundary && m_snake->listHead->direction == Direction::up)
//			{
//				m_snakePlane = SnakePlane::Top;
//				direction = Direction::in;
//				m_isNeedScroll = true;
//				angle = 0.1f;
//				axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//			}
//			// When the snake just reachs the bottom limit.
//			else if (m_snake->listHead->y == BottomBoundary && m_snake->listHead->direction == Direction::down)
//			{
//				m_snakePlane = SnakePlane::Bottom;
//				direction = Direction::in;
//				m_isNeedScroll = true;
//				angle = -0.1f;
//				axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//			}
//		}
//
//		// When the snake is at the top plane.
//		if (m_snake->listHead->y == TopBoundary)
//		{
//			// When the snake just reachs the back limit.
//			if (m_snake->listHead->z == BackBoundary && m_snake->listHead->direction == Direction::in)
//			{
//				m_snakePlane = SnakePlane::Back;
//				direction = Direction::down;
//				m_isNeedScroll = true;
//				angle = 0.1f;
//				axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//			}
//			// When the snake just reachs the front limit.
//			else if (m_snake->listHead->z == FrontBoundary && m_snake->listHead->direction == Direction::out)
//			{
//				m_snakePlane = SnakePlane::Front;
//				direction = Direction::down;
//				m_isNeedScroll = true;
//				angle = -0.1f;
//				axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//			}
//
//		}
//
//		// When the snake is at the back plane.
//		if (m_snake->listHead->z == BackBoundary)
//		{
//			// When the snake just reachs the bottom limit.
//			if (m_snake->listHead->y == BottomBoundary && m_snake->listHead->direction == Direction::down)
//			{
//				m_snakePlane = SnakePlane::Bottom;
//				direction = Direction::out;
//				m_isNeedScroll = true;
//				angle = 0.1f;
//				axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//			}
//			// When the snake just reachs the top limit.
//			else if (m_snake->listHead->y == TopBoundary && m_snake->listHead->direction == Direction::up)
//			{
//				m_snakePlane = SnakePlane::Top;
//				direction = Direction::out;
//				m_isNeedScroll = true;
//				angle = -0.1f;
//				axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//			}
//
//		}
//
//		// When the snake is at the bottom plane.
//		if (m_snake->listHead->y == BottomBoundary)
//		{
//			// When the snake just reachs the front limit.
//			if (m_snake->listHead->z == FrontBoundary && m_snake->listHead->direction == Direction::out)
//			{
//				m_snakePlane = SnakePlane::Front;
//				direction = Direction::up;
//				m_isNeedScroll = true;
//				angle = 0.1f;
//				axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//			}
//			//  When the snake just reachs the back limit.
//			else if (m_snake->listHead->z == BackBoundary && m_snake->listHead->direction == Direction::in)
//			{
//				m_snakePlane = SnakePlane::Back;
//				direction = Direction::up;
//				m_isNeedScroll = true;
//				angle = -0.1f;
//				axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//			}
//
//		}
//
//	}
//
//	
//
//	XMStoreFloat4x4(&m_viewRotation, XMMatrixRotationAxis(axis, angle));
//
//	// Change direction of header
//	if (direction == (Direction)(-(int)m_snake->listHead->direction))
//	{
//		return;
//	}
//	m_snake->listHead->direction = direction;
//
//	Move(step);
//}
//
//void Sample3DSceneRenderer::Move(int x, int y)
//{
//	m_snake->DelTail();
//	m_snake->AddHeader(x, y);
//}

//void Sample3DSceneRenderer::ChangeDirection(Direction direction)
//{
//	// Change direction of header
//	if (direction == (Direction)(-(int)m_snake->listHead->direction))
//	{
//		return;
//	}
//	m_snake->listHead->direction = direction;
//}

void Sample3DSceneRenderer::GameInitialize()
{
	/*if (m_snake != nullptr)
	{
		m_snake.release();
	}*/
	m_snake = std::make_shared<SnakeBase>();
	m_isGameOver = false;
	ResetViewMatrix();
	//m_snakePlane = SnakePlane::Front;
	m_isNeedChangePos = true;
}

void Sample3DSceneRenderer::ScrollViewMatrix()
{
	if (m_angle >= XM_PIDIV2)
	{		
		m_isNeedScroll = false;
		m_angle = 0.0f;
		return;
	}
	
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixMultiply(XMLoadFloat4x4(&m_constantBufferData.view), XMLoadFloat4x4(&m_viewRotation)));
	m_angle += 0.1f;
}

void Sample3DSceneRenderer::ResetViewMatrix()
{
	const XMVECTORF32 eye = { 0.0f, 0.0f, 35.0f, 0.0f };
	const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}



// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	
	//XMMATRIX perspectiveMatrix = XMMatrixOrthographicLH(outputSize.Width / 30.f, outputSize.Height / 30.f, 0.01f, 1000.0f);
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);


	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	ResetViewMatrix();

	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixIdentity()));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{	
	/*double seconds = (timer.GetTotalSeconds());
	

	static int i = 0;
	
	if ((int)seconds == i)
	{
		if (!m_isNeedScroll)
		{			
			Move(1, m_snake->listHead->direction);
		}
		else
		{
			i += 1;
		}
		i += 1;
	}
	
	if (m_isNeedScroll)
	{
		ScrollViewMatrix();

	}*/
	double seconds = (timer.GetTotalSeconds());


	static int i = 0;
	PathFinding pathFinding;
	std::shared_ptr<PathFindNode> node = std::make_shared<PathFindNode>();
	if (!m_isNeedChangePos)
	{
		std::shared_ptr<PathFindNode> start = std::make_shared<PathFindNode>();
		start->position = XMFLOAT3(m_snake->GetSnakeList().front().position.x, m_snake->GetSnakeList().front().position.y, 0.0f);
		std::shared_ptr<PathFindNode> end = std::make_shared<PathFindNode>();
		end->position = XMFLOAT3(m_foodNode.position.x, m_foodNode.position.y, 0.0f);
		//std::function<bool(int, int)> func = std::bind(&Sample3DSceneRenderer::IsAvailable, this);
		std::function<bool(int, int)> func = [=](int x, int y) {
			BaseNode node = BaseNode(float3((float)x, (float)y, 0.0f));

			if (m_snake->IsCollideWithSnake(node, 0, m_snake->GetSize())
				|| x > 11 || y > 11
				|| x < -11 || y < -11
				)
			{
				return false;
			}
			else
			{
				return true;
			}
		};

		//std::function<bool(int, int)> func1 = [=](int x, int y) {
		//	BaseNode node = BaseNode(float3((float)x, (float)y, 0.0f));

		//	if (m_snake->IsCollideWithSnake(node, 0, m_snake->GetSize() - 1) // ignore the tail.
		//		|| x > 11 || y > 11
		//		|| x < -11 || y < -11
		//		)
		//	{
		//		return false;
		//	}
		//	else
		//	{
		//		return true;
		//	}
		//};

		//std::function<bool(int, int)> canFindTail = [=](int x, int y){
		//	std::shared_ptr<PathFindNode> start = std::make_shared<PathFindNode>();
		//	start->position = XMFLOAT3(x, y, 0.0f);
		//	std::shared_ptr<PathFindNode> end = std::make_shared<PathFindNode>();
		//	end->position = XMFLOAT3(m_snake->GetSnakeList().back().position.x, m_snake->GetSnakeList().back().position.y, 0.0f);
		//	PathFinding pathFind;
		//	if (pathFind.FindPath(start, end, func1) != nullptr)
		//	{
		//		return true;
		//	}
		//	else
		//	{
		//		return false;
		//	}
		//};
		//
		//std::function<bool(int, int)> canAchieve = [=](int x, int y){
		//	if (func(x, y) && canFindTail(x, y))
		//	{
		//		return true;
		//	}
		//	else
		//	{
		//		return false;
		//	}
		//};
		node.swap(pathFinding.FindPath(start, end, func));
		if (node == nullptr)
		{
			
		}
		else
		{
			while (node->parent->parent != nullptr)
			{
				node = node->parent;
			}
		}
		m_nextDir = MoveTo(BaseNode(float3(node->position.x, node->position.y, node->position.z)));
		node = nullptr;
		m_snake->Move(m_nextDir);
	}

	
	//if ((int)seconds == i)
	//{		
	//	PathFinding pathFinding;
	//	std::shared_ptr<PathFindNode> node = std::make_shared<PathFindNode>();
	//	if (!m_isNeedChangePos)
	//	{
	//		std::shared_ptr<PathFindNode> start = std::make_shared<PathFindNode>();
	//		start->position = XMFLOAT3(m_snake->GetSnakeList().front().position.x, m_snake->GetSnakeList().front().position.y, 0.0f);
	//		std::shared_ptr<PathFindNode> end = std::make_shared<PathFindNode>();
	//		end->position = XMFLOAT3(m_foodNode.position.x, m_foodNode.position.y, 0.0f);
	//		//std::function<bool(int, int)> func = std::bind(&Sample3DSceneRenderer::IsAvailable, this);

	//		std::function<bool(int, int)> func = [=](int x, int y) {
	//			BaseNode node = BaseNode(float3((float)x, (float)y, 0.0f));
	//			if (m_snake->IsCollideWithSnake(node))
	//			{
	//				return false;
	//			}
	//			else
	//			{
	//				return true;
	//			}
	//		};
	//		node.swap(pathFinding.FindPath(start, end, func));
	//		if (node == nullptr)
	//		{

	//		}
	//		else
	//		{
	//			while (node->parent->parent != nullptr)
	//			{
	//				node = node->parent;
	//			}
	//		}
	//		m_nextDir = MoveTo(BaseNode(float3(node->position.x, node->position.y, node->position.z)));
	//		node = nullptr;
	//	}		

	//	m_snake->Move(m_nextDir);
	//	i += 1;
	//}

}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		//Move(0.1f, Direction::right, m_constantBufferData.model);

		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		//Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

Point Sample3DSceneRenderer::RandomPosition(int min, int max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);
	return Point((float)dis(gen), (float)dis(gen));
}

bool Sample3DSceneRenderer::RenderFood(ID3D11DeviceContext* context)
{
	static Point pt(0.0f, 0.0f);
	
	if (m_isNeedChangePos)
	{
		do
		{
			pt = RandomPosition(-10, 10);
			m_foodNode = BaseNode(float3(pt.X, pt.Y, 0.0f));
			//m_foodBB = m_foodModel->meshes[0]->BoundingBox;
			//m_foodBB.Center = XMFLOAT3(pt.X, pt.Y, 0.0f);
		} while (m_snake->IsCollideWithSnake(m_foodNode, 0, m_snake->GetSize()));

		m_isNeedChangePos = false;
	}

	

	m_foodModel->Draw(context, m_deviceResources->GetD3DDevice(), XMMatrixTranspose(XMMatrixTranslation(pt.X, pt.Y, 0.0f)),
		XMLoadFloat4x4(&m_constantBufferData.view),
		XMLoadFloat4x4(&m_constantBufferData.projection),
		[&](){

		context->IASetInputLayout(m_modelInputLayout.Get());
		context->VSSetShader(m_textureVertexShader.Get(), nullptr, 0);
		context->PSSetSamplers(
			0,                          // starting at the first sampler slot 
			1,                          // set one sampler binding 
			m_samplerState.GetAddressOf()
			);
		//context->PSSetShaderResources(0, 1, m_foodSRV.GetAddressOf());
		context->PSSetShader(m_texturePixelShader.Get(), nullptr, 0);

		context->RSSetState(m_rsState.Get());
	});	

	return true;
}

void Sample3DSceneRenderer::RenderBackground()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	m_wallModel->Draw(context, m_deviceResources->GetD3DDevice(), XMMatrixTranspose(XMMatrixIdentity()),
		XMLoadFloat4x4(&m_constantBufferData.view),
		XMLoadFloat4x4(&m_constantBufferData.projection),
		[&](){

		context->IASetInputLayout(m_modelInputLayout.Get());
		context->VSSetShader(m_normalVertexShader.Get(), nullptr, 0);
		//context->PSSetSamplers(
		//	0,                          // starting at the first sampler slot 
		//	1,                          // set one sampler binding 
		//	m_samplerState.GetAddressOf()
		//	);
		//context->PSSetShaderResources(0, 1, m_foodSRV.GetAddressOf());
		context->PSSetShader(m_normalPixelShader.Get(), nullptr, 0);
		
		context->OMSetBlendState(m_blendState.Get(), nullptr, 0xffffffff);

		context->RSSetState(m_rsState.Get());
	});
}

// Renders one frame using the vertex and pixel shaders.
bool Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();
	
	RenderFood(context);
	//RenderBackground();
	//Node* snakeNode = m_snake->listHead;
	
	for (int i = 0; i < m_snake->GetSize(); ++i)
	{
		std::shared_ptr<BaseNode> snakeNode = std::make_shared<BaseNode>(m_snake->GetSnakeList()[i]);
		if (snakeNode == nullptr)
		{
			continue;
		}

		XMMATRIX translation = XMMatrixTranslation(snakeNode->position.x,
			snakeNode->position.y, snakeNode->position.z);
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(translation));
		
		m_snakeModel->Draw(context, device, XMLoadFloat4x4(&m_constantBufferData.model),
			XMLoadFloat4x4(&m_constantBufferData.view),
			XMLoadFloat4x4(&m_constantBufferData.projection),
			[&](){
			
			context->IASetInputLayout(m_modelInputLayout.Get());
			context->VSSetShader(m_textureVertexShader.Get(), nullptr, 0);
			context->PSSetSamplers(
				0,                          // starting at the first sampler slot 
				1,                          // set one sampler binding 
				m_samplerState.GetAddressOf()
				);
			//context->PSSetShaderResources(0, 1, m_bodySRV.GetAddressOf());
			context->PSSetShader(m_texturePixelShader.Get(), nullptr, 0);
			
			context->RSSetState(m_rsState.Get());
		});		
		
		
	}
	
	if (m_snake->IsCollideWithNode(m_foodNode))
	{
		m_snake->Eat();
		m_isNeedChangePos = true;
	}
	if (m_snake->IsCollideWithBody() || m_snake->IsCollideWithWall())
	{
		m_isGameOver = true;
	}
	
	return true;
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	auto device = m_deviceResources->GetD3DDevice();
	m_snakeModel = CMOModel::CreateFromCMO(device, L"CrateModel.cmo");
	m_foodModel = CMOModel::CreateFromCMO(device, L"CrateModel.cmo");
	m_wallModel = CMOModel::CreateFromCMO(device, L"wall.cmo");
	GameInitialize();


	auto loadTextureVSTask = DX::ReadDataAsync(L"TextureVertexShader.cso");
	auto loadTexturePSTask = DX::ReadDataAsync(L"TexturePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createTextureVSTask = loadTextureVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_textureVertexShader
			)
			);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				VertexPositionNormalTangentColorTexture::InputElements,
				VertexPositionNormalTangentColorTexture::InputElementCount,
				&fileData[0],
				fileData.size(),
				&m_modelInputLayout
			)
			);

	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createTexturePSTask = loadTexturePSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_texturePixelShader
			)
			);
	});


	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
			);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
			);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);
	});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this] () {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor cubeVertices[] = 
		{
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},

			{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		};
		
		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
				)
			);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices [] =
		{
			0,2,1, // -x
			1,2,3,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
				)
			);
	});


	// Food vertex initialization.
	auto LoadNormalVS = DX::ReadDataAsync(L"NormalVertexShader.cso");
	auto LoadNormalPS = DX::ReadDataAsync(L"NormalPixelShader.cso");

	auto createNormalVS = LoadNormalVS.then([this](const std::vector<byte>& fileData){
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_normalVertexShader
			)
			);
		
	});
	
	auto createNormalPS = LoadNormalPS.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_normalPixelShader
			)
			);		
	});

	auto createFoodCube = (createNormalVS && createNormalPS).then([this](){
		static const FoodVertex cubeVertices[] =
		{
			// -z
			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f) },

			// +z
			{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3( 0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3( 0.5f,  0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-0.5f,  0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },

			// +y
			{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-0.5f, 0.5f,  0.5f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3( 0.5f, 0.5f,  0.5f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3( 0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f) },

			// -y
			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT2(1.0f, 1.0f) },

			// -x
			{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f) },

			// +x
			{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT2(1.0f, 1.0f) },
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_foodVertexBuffer
			)
			);

		static const unsigned short cubeIndices[] =
		{
			// Front Face
			0, 1, 2,
			0, 2, 3,

			// Back Face
			4, 5, 6,
			4, 6, 7,

			// Top Face
			8, 9, 10,
			8, 10, 11,

			// Bottom Face
			12, 13, 14,
			12, 14, 15,

			// Left Face
			16, 17, 18,
			16, 18, 19,

			// Right Face
			20, 21, 22,
			20, 22, 23
		};

		m_foodIndexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_foodIndexBuffer
			)
			);
	});

	// Once the cube is loaded, the object is ready to be rendered.
	(createFoodCube && createCubeTask && createTextureVSTask && createTexturePSTask).then([this]() {
		m_loadingComplete = true;
	});

	

	// Create the sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;

	// Allow use of all mip levels 
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	DX::ThrowIfFailed(
		device->CreateSamplerState(
		&samplerDesc,
		&m_samplerState)
		);

	Microsoft::WRL::ComPtr<ID3D11Resource> texture;
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(device, m_deviceResources->GetD3DDeviceContext(), L"head.jpg", &texture, &m_foodSRV)
		);
	Microsoft::WRL::ComPtr<ID3D11Resource> texture1;
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(device, m_deviceResources->GetD3DDeviceContext(), L"texture.jpg", &texture1, &m_bodySRV)
		);
	// RS state.
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	DX::ThrowIfFailed(
		device->CreateRasterizerState(&rsDesc, &m_rsState)
		);

	// Depth stencil state.
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	dsDesc.BackFace = dsDesc.FrontFace;
	
	DX::ThrowIfFailed(
		device->CreateDepthStencilState(&dsDesc, &m_dsState)
		);

	// Blend state.
	D3D11_BLEND_DESC blendDesc = {};
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DX::ThrowIfFailed(
		device->CreateBlendState(&blendDesc, &m_blendState)
		);
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_snake.reset();
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}