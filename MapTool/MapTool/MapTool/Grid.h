#ifndef __GRID_H__
#define __GRID_H__

class Grid
{
public:
	static Grid Instance;
private:
	typedef struct _Vertex
	{
		D3DXVECTOR3 vPosition;
		static DWORD dwFVF;
	}VERTEX, * LPVERTEX;
	typedef struct _Index16
	{
		WORD _0, _1, _2;
	}INDEX16, * LPINDEX16;
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	UINT	m_nCX;	//���� �� X ����
	UINT	m_nCZ;	//���� �� Y ����
	UINT	m_nX;	//�� ���� ����
	UINT	m_nZ;	//�� ���� ����
	UINT	m_nFace;//�� ����.
public:
	Grid();
	~Grid();
public:
	static HRESULT Ready();
public:
	static HRESULT CreateBuffer();
	static void Draw();
};

#endif // !__GRID_H__
