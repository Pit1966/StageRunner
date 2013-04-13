#ifndef YADIDEVICEMANAGER_H
#define YADIDEVICEMANAGER_H

#include <QList>

class YadiDevice;

class YadiDeviceManager
{
private:
	static QList<YadiDevice*>globYadiDeviceList;

public:
	YadiDeviceManager();

	static YadiDevice * deviceAt(int idx);
	inline static int devices() {return globYadiDeviceList.size();}
	static QList<YadiDevice *> & enumerateYadiDevices();
	static void clearYadiDevices();
	static YadiDevice * getDevice(const QString &dev_node, int cap);
	static void updateYadiDevicesStatus();
	static QList<YadiDevice*> & globalDeviceList() {return globYadiDeviceList;}
	static bool yadiDeviceLessThan(const YadiDevice * s1, const YadiDevice * s2);
};


#endif // YADIDEVICEMANAGER_H
