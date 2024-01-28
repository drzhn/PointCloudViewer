#include "AbstractPipelineObject.h"

namespace PointCloudViewer
{
	AbstractPipelineObject::AbstractPipelineObject(
		const char* shaderPath,
		ShaderTypeFlags shaderTypes,
		D3D12_ROOT_SIGNATURE_FLAGS flags)
	{
		m_shader = std::make_unique<Shader>(shaderPath, shaderTypes);
		m_inputContainer.InitContainer(m_shader->GetInputMap(), flags);
	}

	ShaderInput const* AbstractPipelineObject::GetShaderInputByName(const std::string& name) const
	{
		if (m_shader->GetInputMap().contains(name))
		{
			return &m_shader->GetInputMap().find(name)->second;
		}
		return nullptr;
	}
}
