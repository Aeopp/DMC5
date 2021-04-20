#ifndef __INSPECTOR_H__
#define __INSPECTOR_H__
class Inspector
{
public:
	static Inspector Instance;
public:
	void Update();

	void DrawTransform();
};
#endif // !__INSPECTOR_H__