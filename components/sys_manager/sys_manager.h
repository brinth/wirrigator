#ifndef __CTRL_MANAGER_H__
#define __CTRL_MANAGER_H__


namespace System 
{

class Manager
{
public:
	Manager() = delete;
	~Manager();
	
	void				Init(void);
	void				Service(void);

	Manager*			GetInstance(void) { return _inst; };
private:
	Manager*	_inst;	
};

} //namespace System
  //
#endif //__CTRL_MANAGER_H__
