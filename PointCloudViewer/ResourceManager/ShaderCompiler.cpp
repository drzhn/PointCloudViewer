#include "ShaderCompiler.h"

#include <codecvt>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <functional>

#include "DataManager/DataManager.h"
#include "Utils/Log.h"

#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                            \
  (uint32_t)(uint8_t)(ch0)        | (uint32_t)(uint8_t)(ch1) << 8  | \
  (uint32_t)(uint8_t)(ch2) << 16  | (uint32_t)(uint8_t)(ch3) << 24   \
  )

namespace PointCloudViewer
{
	ShaderTableType ShaderKindToShaderTableType(D3D12_SHADER_VERSION_TYPE kind)
	{
		ShaderTableType tableType;
		switch (kind)
		{
		case D3D12_SHVER_RAY_GENERATION_SHADER:
			tableType = ShaderTableRaygen;
			break;
		case D3D12_SHVER_INTERSECTION_SHADER:
		case D3D12_SHVER_ANY_HIT_SHADER:
		case D3D12_SHVER_CLOSEST_HIT_SHADER:
			tableType = ShaderTableHitGroup;
			break;
		case D3D12_SHVER_MISS_SHADER:
			tableType = ShaderTableMiss;
			break;
		case D3D12_SHVER_CALLABLE_SHADER:
			tableType = ShaderTableCallable;
			break;
		case D3D12_SHVER_MESH_SHADER:
		case D3D12_SHVER_AMPLIFICATION_SHADER:
		case D3D12_SHVER_RESERVED0:
		case D3D12_SHVER_PIXEL_SHADER:
		case D3D12_SHVER_VERTEX_SHADER:
		case D3D12_SHVER_GEOMETRY_SHADER:
		case D3D12_SHVER_HULL_SHADER:
		case D3D12_SHVER_DOMAIN_SHADER:
		case D3D12_SHVER_COMPUTE_SHADER:
		case D3D12_SHVER_LIBRARY:
		default:
			ASSERT(false);
			throw;
		}

		return tableType;
	}

	ShaderSystemIncludeHandler::ShaderSystemIncludeHandler() :
		m_shadersFolderPath(std::filesystem::absolute(R"(Data/shaders)").generic_string())
	{
		const std::vector<char> commonEngineStructsData = ReadFile(
			std::filesystem::absolute(R"(PointCloudViewer/CommonEngineStructs.h)").generic_string(),
			0);
		ASSERT_SUCC(ShaderCompiler::s_dxcUtils->CreateBlob(
			commonEngineStructsData.data(),
			commonEngineStructsData.size(),
			0,
			&m_commonEngineStructsDataBlob));
	}

	// CommonEngineStructs.h is the header for the shader system and for the engine code.
	// The meaning of this is to use same structs in both places to reduce number of errors
	// This header is in the engine folder and we load it separately and cache.
	// Other shaders use JoyData/shaders as the include folder
	HRESULT ShaderSystemIncludeHandler::LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource)
	{
		if (wcscmp(L"./CommonEngineStructs.h", pFilename) == 0)
		{
			m_commonEngineStructsDataBlob->AddRef();
			*ppIncludeSource = m_commonEngineStructsDataBlob.Get();
		}
		else
		{
			ComPtr<IDxcBlobEncoding> includeBlob;
			const std::vector<char> includeData = ReadFile(
				(std::filesystem::absolute(m_shadersFolderPath) /
					std::filesystem::path(pFilename)).generic_string(),
				0);
			ASSERT_SUCC(ShaderCompiler::s_dxcUtils->CreateBlob(
				includeData.data(),
				includeData.size(),
				0,
				&includeBlob));
			includeBlob->AddRef();
			*ppIncludeSource = includeBlob.Get();
		}
		return S_OK;
	}

	HMODULE dxil_module = nullptr;
	DxcCreateInstanceProc dxil_create_func = nullptr;

	HMODULE dxc_module = nullptr;
	DxcCreateInstanceProc dxc_create_func = nullptr;


	void ShaderCompiler::Compile(
		ShaderType type,
		const std::wstring& shaderName,
		const std::vector<char>& shaderData,
		ID3DBlob** module,
		ShaderInputMap& globalInputMap,
		std::map<ShaderTableType, ShaderInputMap>& localInputMaps,
		std::map<D3D12_SHADER_VERSION_TYPE, std::wstring>& typeFunctionNameMap)
	{
		if (dxil_module == nullptr)
		{
			dxil_module = LoadLibrary(L"ThirdParty/dxc/bin/x64/dxil.dll");
			ASSERT(dxil_module != nullptr);
			dxil_create_func = (DxcCreateInstanceProc)GetProcAddress(dxil_module, "DxcCreateInstance");
			ASSERT(dxil_create_func != nullptr);
		}

		if (dxc_module == nullptr)
		{
			dxc_module = LoadLibrary(L"ThirdParty/dxc/bin/x64/dxcompiler.dll");
			ASSERT(dxc_module != nullptr);
			dxc_create_func = (DxcCreateInstanceProc)GetProcAddress(dxc_module, "DxcCreateInstance");
			ASSERT(dxc_create_func != nullptr);
		}

		if (s_dxcUtils == nullptr)
		{
			(dxc_create_func(CLSID_DxcUtils, IID_PPV_ARGS(&s_dxcUtils)));
		}

		if (s_dxcCompiler == nullptr)
		{
			(dxc_create_func(CLSID_DxcCompiler, IID_PPV_ARGS(&s_dxcCompiler)));
		}

		if (s_dxcReflection == nullptr)
		{
			(dxc_create_func(CLSID_DxcContainerReflection, IID_PPV_ARGS(&s_dxcReflection)));
		}

		if (s_validator == nullptr)
		{
			(dxil_create_func(CLSID_DxcValidator, IID_PPV_ARGS(&s_validator)));
		}


		if (m_includeHandler == nullptr)
		{
			m_includeHandler = std::make_unique<ShaderSystemIncludeHandler>();
		}


		LPCWSTR entryPointL = nullptr;
		LPCWSTR targetL = nullptr;
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;

		switch (type)
		{
		case JoyShaderTypeVertex:
			entryPointL = L"VSMain";
			targetL = L"vs_6_5";
			visibility = D3D12_SHADER_VISIBILITY_VERTEX;
			break;
		case JoyShaderTypeGeometry:
			entryPointL = L"GSMain";
			targetL = L"gs_6_5";
			visibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
			break;
		case JoyShaderTypePixel:
			entryPointL = L"PSMain";
			targetL = L"ps_6_5";
			visibility = D3D12_SHADER_VISIBILITY_PIXEL;
			break;
		case JoyShaderTypeCompute:
			entryPointL = L"CSMain";
			targetL = L"cs_6_5";
			visibility = D3D12_SHADER_VISIBILITY_ALL;
			break;
		case JoyShaderTypeRaytracing:
			entryPointL = L"RTMain";
			targetL = L"lib_6_6";
			visibility = D3D12_SHADER_VISIBILITY_ALL;
			break;
		default:
			ASSERT(false);
		}


		DxcDefine Shader_Macros[] = {{L"SHADER", L"1"}, nullptr, nullptr};
		IDxcIncludeHandler* includeHandler = m_includeHandler.get();

		ComPtr<IDxcBlobEncoding> sourceBlob;
		ComPtr<IDxcOperationResult> dxcOperationResult;

		ASSERT_SUCC(s_dxcUtils->CreateBlobFromPinned(
			shaderData.data(),
			shaderData.size(),
			0,
			&sourceBlob));

#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		LPCWSTR arguments[] = {
			L"/Od", // D3DCOMPILE_SKIP_OPTIMIZATION 
			L"/Zi", // D3DCOMPILE_DEBUG
			L"-Qembed_debug",
		};
		uint32_t argCount = 3;
#else
		LPCWSTR* arguments = nullptr;
		uint32_t argCount = 0;
#endif

		HRESULT res = s_dxcCompiler->Compile(
			sourceBlob.Get(), // pSource
			shaderName.c_str(), // pSourceName
			entryPointL, // pEntryPoint
			targetL, // pTargetProfile
			arguments, argCount, // pArguments, argCount
			Shader_Macros, 1, // pDefines, defineCount
			includeHandler, // pIncludeHandler
			&dxcOperationResult);
		if (SUCCEEDED(res))
		{
			dxcOperationResult->GetStatus(&res);
		}
		if (dxcOperationResult)
		{
			ComPtr<IDxcBlobEncoding> errorsBlob;
			res = dxcOperationResult->GetErrorBuffer(&errorsBlob);
			if (SUCCEEDED(res) && errorsBlob)
			{
				const char* errorMsg = static_cast<const char*>(errorsBlob->GetBufferPointer());
				Logger::Log(errorMsg);
			}
		}
		ASSERT_SUCC(res);

		ASSERT_SUCC(dxcOperationResult->GetResult(reinterpret_cast<IDxcBlob**>(module)));

		s_validator->Validate(reinterpret_cast<IDxcBlob*>(*module), 0, &dxcOperationResult);
		dxcOperationResult->GetStatus(&res);
		if (FAILED(res))
		{
			ComPtr<IDxcBlobEncoding> errorsBlob;
			res = dxcOperationResult->GetErrorBuffer(&errorsBlob);
			if (SUCCEEDED(res) && errorsBlob)
			{
				const char* errorMsg = static_cast<const char*>(errorsBlob->GetBufferPointer());
				Logger::Log(errorMsg);
			}
		}

		ASSERT_SUCC(s_dxcReflection->Load(reinterpret_cast<IDxcBlob*>(*module)));

		uint32_t shaderId;
		ASSERT_SUCC(s_dxcReflection->FindFirstPartKind(DXIL_FOURCC('D', 'X', 'I', 'L'), &shaderId));
		auto processShaderInputBindDesc = [&](ShaderInputMap& inputMap, D3D12_SHADER_INPUT_BIND_DESC& inputBindDesc)
		{
			const std::string name = inputBindDesc.Name;
			if (!inputMap.contains(name))
			{
				inputMap.insert({
					inputBindDesc.Name,
					{
						inputBindDesc.Type,
						inputBindDesc.BindPoint,
						inputBindDesc.BindCount,
						inputBindDesc.Space,
						visibility
					}
				});
			}
			else
			{
				inputMap[name].Visibility = D3D12_SHADER_VISIBILITY_ALL;
			}
		};

		if (type == JoyShaderTypeRaytracing)
		{
			D3D12_LIBRARY_DESC libraryDesc;
			ComPtr<ID3D12LibraryReflection> libraryReflection;

			ASSERT_SUCC(s_dxcReflection->GetPartReflection(shaderId, __uuidof(ID3D12LibraryReflection), (void**)&libraryReflection));
			libraryReflection->GetDesc(&libraryDesc);
			for (uint32_t i = 0; i < libraryDesc.FunctionCount; i++)
			{
				// D3D12_FUNCTION_DESC.Name is mangled https://github.com/microsoft/DirectXShaderCompiler/blob/main/docs/DXIL.rst#identifiers
				// dxc header doesn't support translation this name to unmangled version,
				// but library subobjects require them to be unmangled
				auto GetUnmangledName = [](const std::string& name)
				{
					auto ToWstring = [](const std::string& str)
					{
						return std::wstring(str.begin(), str.end());
					};

					if (!name.starts_with("\x1?"))
					{
						return ToWstring(name);
					}
					const size_t pos = name.find("@@");
					if (pos == name.npos)
					{
						return ToWstring(name);
					}

					return ToWstring(name.substr(2, pos - 2));
				};

				ID3D12FunctionReflection* functionReflection = libraryReflection->GetFunctionByIndex(i);
				D3D12_FUNCTION_DESC functionDesc;
				functionReflection->GetDesc(&functionDesc);

				std::wstring unmangledName = GetUnmangledName(functionDesc.Name);

				// https://github.com/microsoft/DirectXShaderCompiler/blob/bae2325380a69d16ca244dc01dbe284946778b27/include/dxc/DxilContainer/DxilContainer.h#L562
				// https://learn.microsoft.com/en-us/windows/win32/api/d3d12shader/ne-d3d12shader-d3d12_shader_version_type
				auto kind = static_cast<D3D12_SHADER_VERSION_TYPE>(D3D12_SHVER_GET_TYPE(functionDesc.Version));
				typeFunctionNameMap.insert({kind, unmangledName});

				ShaderTableType tableType = ShaderKindToShaderTableType(kind);

				if (!localInputMaps.contains(tableType))
				{
					localInputMaps.insert({tableType, {}});
				}

				for (uint32_t j = 0; j < functionDesc.BoundResources; j++)
				{
					D3D12_SHADER_INPUT_BIND_DESC inputBindDesc;
					functionReflection->GetResourceBindingDesc(j, &inputBindDesc);

					const char* localPrefix = "local_";
					if (strncmp(inputBindDesc.Name, localPrefix, 6) != 0)
					{
						processShaderInputBindDesc(globalInputMap, inputBindDesc);
					}
					else
					{
						processShaderInputBindDesc(localInputMaps.at(tableType), inputBindDesc);
					}
				}
			}
		}
		else
		{
			D3D12_SHADER_DESC shaderDesc;
			ComPtr<ID3D12ShaderReflection> shaderReflection;

			ASSERT_SUCC(s_dxcReflection->GetPartReflection(shaderId, __uuidof(ID3D12ShaderReflection), (void**)&shaderReflection));
			shaderReflection->GetDesc(&shaderDesc);

			for (uint32_t i = 0; i < shaderDesc.BoundResources; i++)
			{
				D3D12_SHADER_INPUT_BIND_DESC inputBindDesc;
				shaderReflection->GetResourceBindingDesc(i, &inputBindDesc);

				processShaderInputBindDesc(globalInputMap, inputBindDesc);
			}
		}
	}
}
