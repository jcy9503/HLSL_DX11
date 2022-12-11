#pragma once

class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetRotation() const;

	void Render();
	void GetViewMatrix(XMMATRIX&) const;

	void RenderReflection(float);
	XMMATRIX GetReflectionViewMatrix() const;

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_reflectionViewMatrix;
};