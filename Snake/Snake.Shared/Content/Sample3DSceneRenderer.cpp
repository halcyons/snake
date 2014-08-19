#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include "math.h"
#include "Common\WICTextureLoader.h"
#include <random>
#include "DirectXPackedVector.h"

using namespace Snake;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_foodIndexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources),
	m_isNeedChangePos(true),
	m_isGameOver(false)
{
	XMStoreFloat4x4(&m_model, XMMatrixIdentity());
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void Sample3DSceneRenderer::Move(int step)
{
	for (int i = 0; i < step; ++i)
	{
		m_snake->DelTail();
		m_snake->AddHeader();
	}
}

// Move default model matrix
void Sample3DSceneRenderer::Move(int step, Direction direction)
{
	// Change direction of header	
	if (direction != (Direction)(-(int)m_snake->listHead->direction))
	{
		m_snake->listHead->direction = direction;

		Move(step);
	}
	
}


void Sample3DSceneRenderer::GameInitialize(int snakeLength)
{
	if (m_snake != nullptr)
	{
		m_snake.release();
	}
	m_snake = std::make_unique<List>(snakeLength, m_snakeModel->meshes[0]->BoundingBox);
}

void Sample3DSceneRenderer::ScrollViewMatrix()
{

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
	
	XMMATRIX perspectiveMatrix = XMMatrixOrthographicLH(outputSize.Width / 30.f, outputSize.Height / 30.f, 0.01f, 1000.0f);
	/*XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);*/

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);


	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		perspectiveMatrix * orientationMatrix
		);

	static const XMVECTORF32 eye = { 5.0f, 3.0f, -15.0f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixLookAtLH(eye, at, up));

	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixIdentity());
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	double seconds = (timer.GetTotalSeconds());
	
	static int i = 0;
	if ((int)seconds == i)
	{
		Move(1, m_snake->listHead->direction);
		++i;
	}
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
			m_foodBB = m_foodModel->meshes[0]->BoundingBox;
			m_foodBB.Center = XMFLOAT3(pt.X, pt.Y, 0.0f);
		} while ((m_snake->IsCollideWithBB(m_foodBB, m_snake->listHead, m_snake->listEnd)));
		m_isNeedChangePos = false;
	}
	
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranslation(pt.X, pt.Y, 0.0f));

	m_foodModel->Draw(context, m_deviceResources->GetD3DDevice(), XMLoadFloat4x4(&m_constantBufferData.model),
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

	//// Each vertex is one instance of the FoodVertex struct.
	//UINT stride = sizeof(FoodVertex);
	//UINT offset = 0;
	//context->IASetVertexBuffers(
	//	0,
	//	1,
	//	m_foodVertexBuffer.GetAddressOf(),
	//	&stride,
	//	&offset
	//	);

	//context->IASetIndexBuffer(
	//	m_foodIndexBuffer.Get(),
	//	DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
	//	0
	//	);

	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//context->IASetInputLayout(m_foodInputLayout.Get());

	//// Attach our vertex shader.
	//context->VSSetShader(
	//	m_foodVertexShader.Get(),
	//	nullptr,
	//	0
	//	);

	//context->RSSetState(m_rsState.Get());

	//// Attach our pixel shader.
	//context->PSSetShader(
	//	m_foodPixelShader.Get(),
	//	nullptr,
	//	0
	//	);

	//context->PSSetShaderResources(0, 1, m_foodSRV.GetAddressOf());

	//context->PSSetSamplers(
	//	0,                          // starting at the first sampler slot 
	//	1,                          // set one sampler binding 
	//	m_samplerState.GetAddressOf()
	//	);

	//
	//// Prepare the constant buffer to send it to the graphics device.
	//context->UpdateSubresource(
	//	m_constantBuffer.Get(),
	//	0,
	//	NULL,
	//	&m_constantBufferData,
	//	0,
	//	0
	//	);


	//// Send the constant buffer to the graphics device.
	//context->VSSetConstantBuffers(
	//	0,
	//	1,
	//	m_constantBuffer.GetAddressOf()
	//	);

	//// Draw the objects.
	//context->DrawIndexed(
	//	m_foodIndexCount,
	//	0,
	//	0
	//	);

	return true;
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

	Node* snakeNode = m_snake->listHead;
	
	for (int i = 0; i < m_snake->count; ++i)
	{
		if (snakeNode == nullptr)
		{
			continue;
		}

		XMMATRIX translation = XMMatrixTranslation((float)(snakeNode->x * snakeNode->boundingBox.Extents.x * 2), 
			(float)snakeNode->y * snakeNode->boundingBox.Extents.y * 2, 0.0f);
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixMultiply(XMLoadFloat4x4(&m_model), translation));
		
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

		
		//// Each vertex is one instance of the VertexPositionColor struct.
		//UINT stride = sizeof(VertexPositionColor);
		//UINT offset = 0;
		//context->IASetVertexBuffers(
		//	0,
		//	1,
		//	m_vertexBuffer.GetAddressOf(),
		//	&stride,
		//	&offset
		//	);

		//context->IASetIndexBuffer(
		//	m_indexBuffer.Get(),
		//	DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		//	0
		//	);

		//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//context->IASetInputLayout(m_inputLayout.Get());

		//// Attach our vertex shader.
		//context->VSSetShader(
		//	m_vertexShader.Get(),
		//	nullptr,
		//	0
		//	);

		//// Attach our pixel shader.
		//context->PSSetShader(
		//	m_pixelShader.Get(),
		//	nullptr,
		//	0
		//	);

		//XMMATRIX translation = XMMatrixTranslation((float)snakeNode->x, (float)snakeNode->y, 0.0f);
		//XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixMultiply(XMLoadFloat4x4(&m_model), translation));

		//// Prepare the constant buffer to send it to the graphics device.
		//context->UpdateSubresource(
		//	m_constantBuffer.Get(),
		//	0,
		//	NULL,
		//	&m_constantBufferData,
		//	0,
		//	0
		//	);
		//
		//
		//// Send the constant buffer to the graphics device.
		//context->VSSetConstantBuffers(
		//	0,
		//	1,
		//	m_constantBuffer.GetAddressOf()
		//	);

		//context->RSSetState(m_rsState.Get());

		////context->OMSetDepthStencilState(m_dsState.Get(), 0);

		//// Draw the objects.
		//context->DrawIndexed(
		//	m_indexCount,
		//	0,
		//	0
		//	);
		
		snakeNode = snakeNode->next;		
	}
	snakeNode = nullptr;
	if (m_snake->IsCollideWithBB(m_foodBB, m_snake->listHead, m_snake->listHead))
	{
		m_snake->AddHeader();
		m_isNeedChangePos = true;
	}
	if (m_snake->IsCollideWithBody())
	{
		m_isGameOver = true;
	}
	else
	{
		m_isGameOver = false;
	}
	return true;
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	auto device = m_deviceResources->GetD3DDevice();
	m_snakeModel = CMOModel::CreateFromCMO(device, L"CrateModel.cmo");
	m_foodModel = CMOModel::CreateFromCMO(device, L"CrateModel.cmo");
	GameInitialize(3);


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
	auto LoadFoodVS = DX::ReadDataAsync(L"FoodVertexShader.cso");
	auto LoadFoodPS = DX::ReadDataAsync(L"FoodPixelShader.cso");

	auto createFoodVS = LoadFoodVS.then([this](const std::vector<byte>& fileData){
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_foodVertexShader
			)
			);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
			vertexDesc,
			ARRAYSIZE(vertexDesc),
			&fileData[0],
			fileData.size(),
			&m_foodInputLayout
			)
			);
	});
	
	auto createFoodPS = LoadFoodPS.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_foodPixelShader
			)
			);		
	});

	auto createFoodCube = (createFoodVS && createFoodPS).then([this](){
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