#ifndef RAYTRACING_PIPELINE_H
#define RAYTRACING_PIPELINE_H

#include "ShaderInputContainer.h"
#include "ResourceManager/Shader.h"
#include "ResourceManager/Buffers/Buffer.h"


namespace PointCloudViewer
{
	struct RaytracingPipelineArgs
	{
		const char* shaderPath;
	};

	// TODO this ShaderTable implementation only supports single shader record
	class ShaderTable
	{
	public:
		ShaderTable() = delete;
		explicit ShaderTable(uint64_t size, const void* shaderIdentifier);
		void SetRootParam(uint32_t paramIndex, D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle) const;

		[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const { return m_dataBuffer->GetBufferResource()->GetGPUVirtualAddress(); }
		[[nodiscard]] uint64_t GetSize() const { return m_dataBuffer->GetSizeInBytes(); }
	private:
		std::unique_ptr<Buffer> m_dataBuffer;
		uint64_t m_size;
	};

	class RaytracingPipeline
	{
	public:
		RaytracingPipeline() = delete;
		explicit RaytracingPipeline(const RaytracingPipelineArgs&);
		[[nodiscard]] const ShaderInputContainer* GetGlobalInputContainer() const { return &m_globalInputContainer; }
		[[nodiscard]] const ShaderInputContainer* GetLocalInputContainer(const ShaderTableType type) const
		{
			return &m_localInputContainers.at(type);
		}
		[[nodiscard]] const std::map<uint32_t, EngineBindingType>& GetEngineBindings() const
		{
			return m_globalInputContainer.GetEngineBindings();
		}

		[[nodiscard]] ShaderTable* GetShaderTableByType(ShaderTableType type) const
		{
			switch (type)
			{
			case ShaderTableRaygen:
				return m_raygenShaderTable.get();
			case ShaderTableMiss:
				return m_missShaderTable.get();
			case ShaderTableHitGroup:
				return m_hitGroupShaderTable.get();
			case ShaderTableCallable:
			default:
				throw;
			}
		}

		[[nodiscard]] ID3D12StateObject* GetPipelineState() const { return m_stateObject.Get(); }
	private:
		std::unique_ptr<Shader> m_raytracingShader;
		ComPtr<ID3D12StateObject> m_stateObject;

		ShaderInputContainer m_globalInputContainer;
		std::map<ShaderTableType, ShaderInputContainer> m_localInputContainers;

		std::unique_ptr<ShaderTable> m_raygenShaderTable;
		std::unique_ptr<ShaderTable> m_missShaderTable;
		std::unique_ptr<ShaderTable> m_hitGroupShaderTable;
	};
}
#endif // RAYTRACING_PIPELINE_H
