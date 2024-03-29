#include "Mesh.h"

#include "DataManager/DataManager.h"
#include "EngineDataProvider/EngineDataProvider.h"
#include "MemoryManager/MemoryManager.h"

namespace PointCloudViewer
{
	Mesh::Mesh(uint32_t vertexDataSize,
	           uint32_t indexDataSize,
	           std::ifstream& modelStream,
	           uint32_t vertexDataStreamOffset,
	           uint32_t indexDataStreamOffset)
	{
		InitMesh(vertexDataSize,
		         indexDataSize,
		         modelStream,
		         vertexDataStreamOffset,
		         indexDataStreamOffset);
	}

	void Mesh::InitMesh(
		uint32_t vertexDataSize,
		uint32_t indexDataSize,
		std::ifstream& modelStream,
		uint32_t vertexDataStreamOffset,
		uint32_t indexDataStreamOffset)
	{
		m_verticesData = static_cast<Vertex*>(malloc(vertexDataSize));
		m_indicesData = static_cast<Index*>(malloc(indexDataSize));

		//modelStream.clear();
		//modelStream.seekg(vertexDataStreamOffset);
		modelStream.read(reinterpret_cast<char*>(m_verticesData), vertexDataSize);

		//modelStream.clear();
		//modelStream.seekg(indexDataStreamOffset);
		modelStream.read(reinterpret_cast<char*>(m_indicesData), indexDataSize);

		m_vertexCount = vertexDataSize / sizeof(Vertex);
		m_indexCount = indexDataSize / sizeof(Index);

		//MeshContainer* mc = EngineDataProvider::Get()->GetMeshContainer();
/*
		mc->CreateMeshView(vertexDataSize, indexDataSize, m_meshView);

		MemoryManager::Get()->LoadDataToBuffer(m_verticesData, vertexDataSize, mc->GetVertexBuffer(), m_meshView.vertexBufferOffset);
		MemoryManager::Get()->LoadDataToBuffer(m_indicesData, indexDataSize, mc->GetIndexBuffer(), m_meshView.indexBufferOffset);
	*/}

	Mesh::~Mesh()
	{
		free(m_verticesData);
		free(m_indicesData);
	}
}
