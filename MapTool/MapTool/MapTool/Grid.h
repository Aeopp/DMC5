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
	UINT	m_nCX;	//정점 간 X 간격
	UINT	m_nCZ;	//정점 간 Y 간격
	UINT	m_nX;	//행 정점 개수
	UINT	m_nZ;	//열 정점 개수
	UINT	m_nFace;//면 개수.
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
