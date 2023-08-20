#ifndef __CTRL_MANAGER_H__
#define __CTRL_MANAGER_H__


namespace System 
{

class Manager
{
public:
										Manager() {};
	virtual								~Manager() {};
	
	virtual	void						Init(void);
	virtual	void						Service(void);

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
	static void							run_flow_meter_task(void *);
	static void							run_mqtt_task(void *);
	static void							run_sntp_task(void *);
	static TaskHandle_t					_flow_meter_task_handle;
	static TaskHandle_t					_mqtt_task_handle;
	static TaskHandle_t					_sntp_task_handle;	
};

} //namespace System

#endif //__CTRL_MANAGER_H__
