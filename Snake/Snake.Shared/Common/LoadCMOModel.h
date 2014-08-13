

#pragma once
#include "DirectXCollision.h"
#include "Common\DirectXHelper.h"

using namespace DX;
using namespace DirectX;

const uint32_t MAX_TEXTURE = 8;
#pragma pack(push,1)

struct SubMesh
{
	UINT MaterialIndex;
	UINT IndexBufferIndex;
	UINT VertexBufferIndex;
	UINT StartIndex;
	UINT PrimCount;
};

const uint32_t NUM_BONE_INFLUENCES = 4;

struct SkinningVertex
{
	UINT boneIndex[NUM_BONE_INFLUENCES];
	float boneWeight[NUM_BONE_INFLUENCES];
};

struct MeshExtents
{
	float CenterX, CenterY, CenterZ;
	float Radius;

	float MinX, MinY, MinZ;
	float MaxX, MaxY, MaxZ;
};

struct Bone
{
	INT ParentIndex;
	DirectX::XMFLOAT4X4 InvBindPos;
	DirectX::XMFLOAT4X4 BindPos;
	DirectX::XMFLOAT4X4 LocalTransform;
};

struct Clip
{
	float StartTime;
	float EndTime;
	UINT  keys;
};

struct Keyframe
{
	UINT BoneIndex;
	float Time;
	DirectX::XMFLOAT4X4 Transform;
};

struct MaterialSetting
{
	DirectX::XMFLOAT4   Ambient;
	DirectX::XMFLOAT4   Diffuse;
	DirectX::XMFLOAT4   Specular;
	float               SpecularPower;
	DirectX::XMFLOAT4   Emissive;
	DirectX::XMFLOAT4X4 UVTransform;
	
};

struct Material
{
	std::wstring			Name;
	const MaterialSetting*	pMatSetting;
	std::wstring			PSName;
	std::wstring            Texture[MAX_TEXTURE];
};

struct IBData
{
	size_t          nIndices;
	const USHORT*   ptr;
};

struct VertexPositionNormalTangentColorTexture
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 tangent;
	uint32_t color;
	DirectX::XMFLOAT2 textureCoordinate;
	VertexPositionNormalTangentColorTexture(XMFLOAT3 const& position, XMFLOAT3 const& normal, XMFLOAT4 const& tangent, uint32_t rgba, XMFLOAT2 const& textureCoordinate)
		: position(position),
		normal(normal),
		tangent(tangent),
		color(rgba),
		textureCoordinate(textureCoordinate)
	{
	}

	VertexPositionNormalTangentColorTexture(FXMVECTOR position, FXMVECTOR normal, FXMVECTOR tangent, uint32_t rgba, CXMVECTOR textureCoordinate)
		: color(rgba)
	{
		XMStoreFloat3(&this->position, position);
		XMStoreFloat3(&this->normal, normal);
		XMStoreFloat4(&this->tangent, tangent);
		XMStoreFloat2(&this->textureCoordinate, textureCoordinate);
	}

	VertexPositionNormalTangentColorTexture(XMFLOAT3 const& position, XMFLOAT3 const& normal, XMFLOAT4 const& tangent, XMFLOAT4 const& color, XMFLOAT2 const& textureCoordinate)
		: position(position),
		normal(normal),
		tangent(tangent),
		textureCoordinate(textureCoordinate)
	{
		SetColor(color);
	}

	VertexPositionNormalTangentColorTexture(FXMVECTOR position, FXMVECTOR normal, FXMVECTOR tangent, CXMVECTOR color, CXMVECTOR textureCoordinate)
	{
		XMStoreFloat3(&this->position, position);
		XMStoreFloat3(&this->normal, normal);
		XMStoreFloat4(&this->tangent, tangent);
		XMStoreFloat2(&this->textureCoordinate, textureCoordinate);

		SetColor(color);
	}

	void SetColor(XMFLOAT4 const& color) { SetColor(XMLoadFloat4(&color)); }
	void XM_CALLCONV SetColor(FXMVECTOR color);

	static const int InputElementCount = 5;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VBData
{
	size_t                                          nVerts;
	const VertexPositionNormalTangentColorTexture*  ptr;
};

struct ModelViewProConstData
{
	DirectX::XMMATRIX model;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};
class Mesh
{
public:
	std::wstring													Name;
	std::vector<Material>											Materials;
	std::vector<IBData>												IndexDatas;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>				IndexBuffers;
	std::vector<VBData>												VertexDatas;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>				VertexBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>				MatConstBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	ShaderResourceView;
	DirectX::BoundingSphere											BoundingSphere;
	DirectX::BoundingBox											BoundingBox;
	void XM_CALLCONV Draw(_In_ ID3D11DeviceContext* deviceContext, _In_ ID3D11Device* device, 
		FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection, std::function<void()> customSettings ) const;
};

#pragma pack(pop)


static HRESULT ReadEntireFile(_In_z_ wchar_t const* fileName, _Inout_ std::unique_ptr<uint8_t[]>& data, _Out_ size_t* dataSize);

class CMOModel
{
public:
	std::vector<std::shared_ptr<Mesh>>	meshes;
	std::wstring						name;
	static std::unique_ptr<CMOModel> CreateFromCMO(_In_ ID3D11Device* d3dDevice, _In_reads_bytes_(dataSize) const uint8_t* meshData, size_t dataSize);
	static std::unique_ptr<CMOModel> CreateFromCMO(_In_ ID3D11Device* d3dDevice, const wchar_t* szFileName);
	void XM_CALLCONV Draw(_In_ ID3D11DeviceContext* deviceContext, _In_ ID3D11Device* device,
		FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection, std::function<void()> customSettings) const;
private:
	
};