#pragma once

class TextureArrayClass;

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, const char*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*) const;

	int GetIndexCount() const;
	ID3D11ShaderResourceView** GetTextureArray() const;

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*) const;

	bool LoadTextures(ID3D11Device*, WCHAR*);
	void ReleaseTextures();

	bool LoadModel(const char*);
	void ReleaseModel();

	void CalculateModelVectors() const;
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&) const;
	void CalculateNormal(VectorType, VectorType, VectorType&) const;

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	ModelType* m_model = nullptr;
	TextureArrayClass* m_TextureArray = nullptr;
};