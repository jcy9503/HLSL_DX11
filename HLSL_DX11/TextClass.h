#pragma once

class FontClass;
class FontShaderClass;

class TextClass : public AlignedAllocationPolicy<16>
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX) const;

	bool SetMousePosition(int, int, ID3D11DeviceContext*) const;
	bool KeyInput(ID3D11DeviceContext*, const char) const;

private:
	static bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, const char*, int, int, float, float, float, ID3D11DeviceContext*) const;
	static void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, const SentenceType*, XMMATRIX, XMMATRIX) const;

private:
	FontClass* m_font = nullptr;
	FontShaderClass* m_fontShader = nullptr;
	int m_screenWidth = 0;
	int m_screenHeight = 0;
	XMMATRIX m_baseViewMatrix;
	SentenceType* m_sentence1;
	SentenceType* m_sentence2;
	SentenceType* m_key;
};