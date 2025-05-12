#ifndef __CTRL_MANAGER_H__
#define __CTRL_MANAGER_H__


namespace System 
{

class Manager
{
public:
										Manager() {};
	virtual								~Manager() {};
	
	virtual	void						Init(void) = 0;
	virtual	void						Service(void) = 0;

	static	Manager*					GetInstance(void);
private:
	static	Manager*					_inst;	
};

class IrrigationManager : public Manager
{
public:
										IrrigationManager() {};
										~IrrigationManager() {};
	
	void								Init(void) override;
	void								Service(void) override;
private:
	bool								_init;
};

} //namespace System

#endif //__CTRL_MANAGER_H__
