
#ifdef _MSC_VER
#pragma once
#endif


#if defined(_XBOX_ONE) && defined(_TITLE) && MONOLITHIC
#include <d3d11_x.h>
#else
#include <d3d11_1.h>
#endif

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

namespace DirectX
{
    HRESULT CreateWICTextureFromMemory( _In_ ID3D11Device* d3dDevice,
                                        _In_opt_ ID3D11DeviceContext* d3dContext,
                                        _In_reads_bytes_(wicDataSize) const uint8_t* wicData,
                                        _In_ size_t wicDataSize,
                                        _Out_opt_ ID3D11Resource** texture,
                                        _Out_opt_ ID3D11ShaderResourceView** textureView,
                                        _In_ size_t maxsize = 0
                                      );

    HRESULT CreateWICTextureFromFile( _In_ ID3D11Device* d3dDevice,
                                      _In_opt_ ID3D11DeviceContext* d3dContext,
                                      _In_z_ const wchar_t* szFileName,
                                      _Out_opt_ ID3D11Resource** texture,
                                      _Out_opt_ ID3D11ShaderResourceView** textureView,
                                      _In_ size_t maxsize = 0
                                    );

    HRESULT CreateWICTextureFromMemoryEx( _In_ ID3D11Device* d3dDevice,
                                          _In_opt_ ID3D11DeviceContext* d3dContext,
                                          _In_reads_bytes_(wicDataSize) const uint8_t* wicData,
                                          _In_ size_t wicDataSize,
                                          _In_ size_t maxsize,
                                          _In_ D3D11_USAGE usage,
                                          _In_ unsigned int bindFlags,
                                          _In_ unsigned int cpuAccessFlags,
                                          _In_ unsigned int miscFlags,
                                          _In_ bool forceSRGB,
                                          _Out_opt_ ID3D11Resource** texture,
                                          _Out_opt_ ID3D11ShaderResourceView** textureView
                                      );

    HRESULT CreateWICTextureFromFileEx( _In_ ID3D11Device* d3dDevice,
                                        _In_opt_ ID3D11DeviceContext* d3dContext,
                                        _In_z_ const wchar_t* szFileName,
                                        _In_ size_t maxsize,
                                        _In_ D3D11_USAGE usage,
                                        _In_ unsigned int bindFlags,
                                        _In_ unsigned int cpuAccessFlags,
                                        _In_ unsigned int miscFlags,
                                        _In_ bool forceSRGB,
                                        _Out_opt_ ID3D11Resource** texture,
                                        _Out_opt_ ID3D11ShaderResourceView** textureView
                                    );
}