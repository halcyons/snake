

#include "pch.h"
#include "LoadCMOModel.h"
#include "Common\DirectXHelper.h"
#include "DirectXPackedVector.h"
#include "DDSTextureLoader.h"
// Reads from the filesystem into memory.
static HRESULT ReadEntireFile(_In_z_ wchar_t const* fileName, _Inout_ std::unique_ptr<uint8_t[]>& data, _Out_ size_t* dataSize)
{
	// Open the file.
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
	ScopedHandle hFile(safe_handle(CreateFile2(fileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
	ScopedHandle hFile(safe_handle(CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif

	if (!hFile)
		return HRESULT_FROM_WIN32(GetLastError());

	// Get the file size.
	LARGE_INTEGER fileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	FILE_STANDARD_INFO fileInfo;

	if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	fileSize = fileInfo.EndOfFile;
#else
	GetFileSizeEx(hFile.get(), &fileSize);
#endif

	// File is too big for 32-bit allocation, so reject read.
	if (fileSize.HighPart > 0)
		return E_FAIL;

	// Create enough space for the file data.
	data.reset(new uint8_t[fileSize.LowPart]);

	if (!data)
		return E_OUTOFMEMORY;

	// Read the data in.
	DWORD bytesRead = 0;

	if (!ReadFile(hFile.get(), data.get(), fileSize.LowPart, &bytesRead, nullptr))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (bytesRead < fileSize.LowPart)
		return E_FAIL;

	*dataSize = bytesRead;

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Vertex struct for Visual Studio Shader Designer (DGSL) holding position, normal,
// tangent, color (RGBA), and texture mapping information
const D3D11_INPUT_ELEMENT_DESC VertexPositionNormalTangentColorTexture::InputElements[] =
{
	{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

void XM_CALLCONV VertexPositionNormalTangentColorTexture::SetColor(FXMVECTOR color)
{
	DirectX::PackedVector::XMUBYTEN4 rgba;
	XMStoreUByteN4(&rgba, color);
	this->color = rgba.v;
}


_Use_decl_annotations_
std::unique_ptr<CMOModel> CMOModel::CreateFromCMO(_In_ ID3D11Device* d3dDevice, _In_reads_bytes_(dataSize) const uint8_t* meshData, size_t dataSize)
{

	if (!d3dDevice || !meshData)
		throw std::exception("Device and meshData cannot be null");

	std::unique_ptr<CMOModel> model(new CMOModel());

	// Meshes
	auto pMeshCount = reinterpret_cast<const UINT*>(meshData);

	model->meshes.reserve(*pMeshCount);

	size_t usedSize = sizeof(UINT);
	if (dataSize < usedSize)
		throw std::exception("End of file");

	if (!*pMeshCount)
		throw std::exception("No meshes found");

	//std::shared_ptr<Mesh> mesh(new Mesh());

	for (UINT meshIndex = 0; meshIndex < *pMeshCount; ++meshIndex)
	{
		// Mesh name
		auto pNameLength = reinterpret_cast<const UINT*>(meshData + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		auto meshName = reinterpret_cast<const wchar_t*>(meshData + usedSize);

		usedSize += sizeof(wchar_t)*(*pNameLength);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		auto mesh = std::make_shared<Mesh>();
		mesh->Name.assign(meshName, *pNameLength);
		//mesh->ccw = ccw;
		//mesh->pmalpha = pmalpha;

		// Materials
		auto matCount = reinterpret_cast<const UINT*>(meshData + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		mesh->Materials.reserve(*matCount);
		mesh->ShaderResourceView.resize(*matCount);
		for (UINT j = 0; j < *matCount; ++j)
		{
			Material m;

			// Material name
			pNameLength = reinterpret_cast<const UINT*>(meshData + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			auto matName = reinterpret_cast<const wchar_t*>(meshData + usedSize);

			usedSize += sizeof(wchar_t)*(*pNameLength);
			if (dataSize < usedSize)
				throw std::exception("End of file");
			
			m.Name.assign(matName, *pNameLength);


			// Material settings
			auto matSetting = reinterpret_cast<const MaterialSetting*>(meshData + usedSize);
			usedSize += sizeof(MaterialSetting);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			m.pMatSetting = matSetting;

			// Pixel shader name
			pNameLength = reinterpret_cast<const UINT*>(meshData + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			auto psName = reinterpret_cast<const wchar_t*>(meshData + usedSize);

			usedSize += sizeof(wchar_t)*(*pNameLength);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			m.PSName.assign(psName, *pNameLength);

			for (UINT t = 0; t < MAX_TEXTURE; ++t)
			{
				pNameLength = reinterpret_cast<const UINT*>(meshData + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				auto txtName = reinterpret_cast<const wchar_t*>(meshData + usedSize);

				usedSize += sizeof(wchar_t)*(*pNameLength);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				m.Texture[t].assign(txtName, *pNameLength);
			}
			mesh->Materials.emplace_back(m);
			Microsoft::WRL::ComPtr<ID3D11Resource> texture;
			if (m.Texture[0][0] == '\0')
			{
				mesh->ShaderResourceView[j] = nullptr;
			}
			else
			{
				DX::ThrowIfFailed(
					CreateDDSTextureFromFile(d3dDevice, m.Texture[0].c_str(), &texture, &mesh->ShaderResourceView[j])
					);
			}
			
		}

		assert(mesh->Materials.size() == *matCount);

		// Skeletal data?
		auto bSkeleton = reinterpret_cast<const BYTE*>(meshData + usedSize);
		usedSize += sizeof(BYTE);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		// Submeshes
		auto nSubmesh = reinterpret_cast<const UINT*>(meshData + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		if (!*nSubmesh)
			throw std::exception("No submeshes found\n");

		auto subMesh = reinterpret_cast<const SubMesh*>(meshData + usedSize);
		usedSize += sizeof(SubMesh) * (*nSubmesh);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		// Index buffers
		auto IBCount = reinterpret_cast<const UINT*>(meshData + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		if (!*IBCount)
			throw std::exception("No index buffers found\n");

		mesh->IndexDatas.reserve(*IBCount); 
		mesh->IndexBuffers.resize(*IBCount);
		for (UINT j = 0; j < *IBCount; ++j)
		{
			auto nIndexes = reinterpret_cast<const UINT*>(meshData + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (!*nIndexes)
				throw std::exception("Empty index buffer found\n");

			size_t ibBytes = sizeof(USHORT)* (*(nIndexes));

			auto indexes = reinterpret_cast<const USHORT*>(meshData + usedSize);
			usedSize += ibBytes;
			if (dataSize < usedSize)
				throw std::exception("End of file");

			IBData ib;
			ib.nIndices = *nIndexes;
			ib.ptr = indexes;
			
			mesh->IndexDatas.emplace_back(ib);

			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = static_cast<UINT>(ibBytes);
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA initData = { 0 };
			initData.pSysMem = indexes;

			DX::ThrowIfFailed(
				d3dDevice->CreateBuffer(&desc, &initData, &mesh->IndexBuffers[j])
				);

		}
		
		assert(mesh->IndexDatas.size() == *IBCount);

		// Vertex buffers 
		auto pVBCount = reinterpret_cast<const UINT*>(meshData + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		if (!*pVBCount)
			throw std::exception("No vertex buffers found\n");

		
		mesh->VertexDatas.reserve(*pVBCount);
		mesh->VertexBuffers.resize(*pVBCount);
		mesh->MatConstBuffer.resize(*pVBCount);
		//mesh->Materials.resize(*pVBCount);
		struct MaterialConst
		{
			DirectX::XMFLOAT4   Ambient;
			DirectX::XMFLOAT4   Diffuse;
			DirectX::XMFLOAT4   Specular;
			DirectX::XMFLOAT4   Emissive;
			float               SpecularPower;
			float				pad1;
			float				pad2;
			float				pad3;
			DirectX::XMFLOAT4X4 UVTransform;
		};
		MaterialConst materialData;
		for (UINT j = 0; j < *pVBCount; ++j)
		{
			auto nVerts = reinterpret_cast<const UINT*>(meshData + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (!*nVerts)
				throw std::exception("Empty vertex buffer found\n");

			size_t vbBytes = sizeof(VertexPositionNormalTangentColorTexture)* (*(nVerts));

			auto verts = reinterpret_cast<const VertexPositionNormalTangentColorTexture*>(meshData + usedSize);
			usedSize += vbBytes;
			if (dataSize < usedSize)
				throw std::exception("End of file");

			VBData vb;
			vb.nVerts = *nVerts;
			vb.ptr = verts;
			
			mesh->VertexDatas.emplace_back(vb);

			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = static_cast<UINT>(vbBytes);
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA initData = { 0 };
			initData.pSysMem = verts;

			DX::ThrowIfFailed(
				d3dDevice->CreateBuffer(&desc, &initData, &mesh->VertexBuffers[j])
				);
			// Create const buffer
			materialData.Ambient = mesh->Materials[j].pMatSetting->Ambient;
			materialData.Diffuse = mesh->Materials[j].pMatSetting->Diffuse;
			materialData.Specular = mesh->Materials[j].pMatSetting->Specular;
			materialData.Emissive = mesh->Materials[j].pMatSetting->Emissive;
			materialData.SpecularPower = mesh->Materials[j].pMatSetting->SpecularPower;
			materialData.UVTransform = mesh->Materials[j].pMatSetting->UVTransform;
			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(MaterialConst), D3D11_BIND_CONSTANT_BUFFER);
			
			initData.pSysMem = &materialData;
			
			DX::ThrowIfFailed(
				d3dDevice->CreateBuffer(
				&constantBufferDesc,
				&initData,
				&mesh->MatConstBuffer[j]
				)
				);
		}

		assert(mesh->VertexDatas.size() == *pVBCount);

		// Skinning vertex buffers
		auto pSkinVBCount = reinterpret_cast<const UINT*>(meshData + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");
		if (*pSkinVBCount)
		{
			if (*pSkinVBCount != *pVBCount)
				throw std::exception("Number of VBs not equal to number of skin VBs");

			for (UINT j = 0; j < *pSkinVBCount; ++j)
			{
				auto nVerts = reinterpret_cast<const UINT*>(meshData + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				if (!*nVerts)
					throw std::exception("Empty skinning vertex buffer found\n");

				if (mesh->VertexDatas[j].nVerts != *nVerts)
					throw std::exception("Mismatched number of verts for skin VBs");

				size_t vbBytes = sizeof(SkinningVertex) * (*(nVerts));

				auto verts = reinterpret_cast<const SkinningVertex*>(meshData + usedSize);
				usedSize += vbBytes;
				if (dataSize < usedSize)
					throw std::exception("End of file");
				
			}
		}

		// Extents
		auto extents = reinterpret_cast<const MeshExtents*>(meshData + usedSize);
		usedSize += sizeof(MeshExtents);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		mesh->BoundingSphere.Center.x = extents->CenterX;
		mesh->BoundingSphere.Center.y = extents->CenterY;
		mesh->BoundingSphere.Center.z = extents->CenterZ;
		mesh->BoundingSphere.Radius = extents->Radius;

		DirectX::XMVECTOR min = DirectX::XMVectorSet(extents->MinX, extents->MinY, extents->MinZ, 0.f);
		DirectX::XMVECTOR max = DirectX::XMVectorSet(extents->MaxX, extents->MaxY, extents->MaxZ, 0.f);
		DirectX::BoundingBox::CreateFromPoints(mesh->BoundingBox, min, max);

		// Animation data
		if (*bSkeleton)
		{
			// Bones
			auto pBonesCount = reinterpret_cast<const UINT*>(meshData + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (!*pBonesCount)
				throw std::exception("Animation bone data is missing\n");

			for (UINT j = 0; j < *pBonesCount; ++j)
			{
				// Bone name
				pNameLength = reinterpret_cast<const UINT*>(meshData + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				auto boneName = reinterpret_cast<const wchar_t*>(meshData + usedSize);

				usedSize += sizeof(wchar_t)*(*pNameLength);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				// TODO - What to do with bone name?
				boneName;

				// Bone settings
				auto bones = reinterpret_cast<const Bone*>(meshData + usedSize);
				usedSize += sizeof(Bone);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				// TODO - What to do with bone data?
				bones;
			}

			// Animation Clips
			auto pClipsCount = reinterpret_cast<const UINT*>(meshData + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			for (UINT j = 0; j < *pClipsCount; ++j)
			{
				// Clip name
				pNameLength = reinterpret_cast<const UINT*>(meshData + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				auto clipName = reinterpret_cast<const wchar_t*>(meshData + usedSize);

				usedSize += sizeof(wchar_t)*(*pNameLength);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				// TODO - What to do with clip name?
				clipName;

				auto clip = reinterpret_cast<const Clip*>(meshData + usedSize);
				usedSize += sizeof(Clip);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				if (!clip->keys)
					throw std::exception("Keyframes missing in clip");

				auto keys = reinterpret_cast<const Keyframe*>(meshData + usedSize);
				usedSize += sizeof(Keyframe) * clip->keys;
				if (dataSize < usedSize)
					throw std::exception("End of file");

				// TODO - What to do with keys and clip->StartTime, clip->EndTime?
				keys;
			}
		}

		model->meshes.emplace_back(mesh);
	}

	return model;
}

_Use_decl_annotations_
std::unique_ptr<CMOModel> CMOModel::CreateFromCMO(_In_ ID3D11Device* d3dDevice, const wchar_t* szFileName)
{
	size_t dataSize = 0;
	std::unique_ptr<uint8_t[]> data;
	HRESULT hr = ReadEntireFile(szFileName, data, &dataSize);
	if (FAILED(hr))
	{
		DebugTrace("CreateFromCMO failed (%08X) loading '%S'\n", hr, szFileName);
		throw std::exception("CreateFromCMO");
	}

	auto model = CreateFromCMO(d3dDevice, data.get(), dataSize);

	model->name = szFileName;

	return model;
}

void XM_CALLCONV Mesh::Draw(_In_ ID3D11DeviceContext* deviceContext, _In_ ID3D11Device* device, 
	FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection, std::function<void()> customSettings) const
{
	
	// Set const buffer.
	ModelViewProConstData constBufferData = { world, view, projection };
	Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer;
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(constBufferData), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		device->CreateBuffer(
		&constantBufferDesc,
		nullptr,
		&constBuffer
		)
		);
	deviceContext->UpdateSubresource(
		constBuffer.Get(),
		0,
		nullptr,
		&constBufferData,
		0,
		0
		);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(VertexPositionNormalTangentColorTexture);
	UINT offset = 0;		
	
	for (size_t j = 0; j < Materials.size(); ++j)
	{
		if (customSettings)
		{
			customSettings();
		}
		deviceContext->VSSetConstantBuffers(
			1,
			1,
			constBuffer.GetAddressOf()
			);
		//deviceContext->PSSetShaderResources(0, 1, ShaderResourceView[j].GetAddressOf());
		deviceContext->VSSetConstantBuffers(
			0,
			1,
			MatConstBuffer[j].GetAddressOf()
			);
		deviceContext->PSSetConstantBuffers(
			0,
			1,
			MatConstBuffer[j].GetAddressOf()
			);
		deviceContext->IASetVertexBuffers(
			0,
			1,
			VertexBuffers[j].GetAddressOf(),
			&stride,
			&offset
			);
		deviceContext->IASetIndexBuffer(
			IndexBuffers[j].Get(),
			DXGI_FORMAT_R16_UINT,
			0
			);
		deviceContext->DrawIndexed(IndexDatas[j].nIndices, 0, 0);
	}
	
}

void XM_CALLCONV CMOModel::Draw(_In_ ID3D11DeviceContext* deviceContext, _In_ ID3D11Device* device,
	FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection, std::function<void()> customSettings) const
{
	for (auto it = meshes.begin(); it != meshes.end(); ++it)
	{
		auto mesh = it->get();
		assert(mesh != 0);
		deviceContext->PSSetShaderResources(0, 1, mesh->ShaderResourceView[0].GetAddressOf());
		mesh->Draw(deviceContext, device, world, view, projection, customSettings);
	}
}