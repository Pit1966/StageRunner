#include "yadidevicemanager.h"
#include "yadidevice.h"
#include "serialwrapper.h"

#ifdef __unix__
#include <libudev.h>
#include <QProcess>
#endif

#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QTime>


QList<YadiDevice*>YadiDeviceManager::globYadiDeviceList;

YadiDeviceManager::YadiDeviceManager()
{
}

YadiDevice *YadiDeviceManager::deviceAt(int idx)
{
	if (idx < 0 || idx >= globYadiDeviceList.size()) {
		return nullptr;
	} else {
		return globYadiDeviceList.at(idx);
	}
}

int YadiDeviceManager::enumerateYadiDevices(bool update)
{

	for (int t=0; t<globYadiDeviceList.size(); t++) {
		globYadiDeviceList.at(t)->deviceNodePresent = false;
	}
#if defined(QTSERIAL)
	QList<QSerialPortInfo>yadidevs = SerialWrapper::discoverQtSerialPorts("YADI DMX");

	for (const QSerialPortInfo &port : yadidevs) {
		// Check if new or already in known list
		YadiDevice *yadi = nullptr;
		bool found = false;
		for (YadiDevice *t : globYadiDeviceList) {
			if (port.portName() == t->devNodePath) {
				yadi = t;
				found = true;
				break;
			}
		}
		// new or not?
		if (!found) {
			yadi = new YadiDevice(port.portName());
			globYadiDeviceList.append(yadi);
		}

		QString product = port.description();

		yadi->devNodePath = port.portName();
		yadi->deviceProductName = product;
		yadi->deviceManufacturer = port.manufacturer();
		yadi->deviceSerial = port.serialNumber();
		yadi->idVendor = port.vendorIdentifier();
		yadi->idProduct = port.productIdentifier();
		yadi->deviceNodePresent = true;
		if (product.contains("Transceiver")) {
			yadi->capabilities = YadiDevice::FL_INPUT_UNIVERSE | YadiDevice::FL_OUTPUT_UNIVERSE;
			yadi->maxDeviceDmxInChannels = 512;
			yadi->maxDeviceDmxOutChannels = 512;
			if (!found || !update) {
				yadi->usedDmxOutChannels = 512;
				yadi->usedDmxInChannels = 512;
			}
		}
		else if (product.contains("Receiver")) {
			yadi->capabilities = YadiDevice::FL_INPUT_UNIVERSE;
			yadi->usedDmxOutChannels = 0;
			yadi->maxDeviceDmxOutChannels = 0;
			if (!found || !update) {
				yadi->usedDmxInChannels = 128;
				yadi->maxDeviceDmxInChannels = 128;
			}
		}
		else if (product.contains("Sender")) {
			yadi->capabilities = YadiDevice::FL_OUTPUT_UNIVERSE;
		}
		qDebug("YadiDeviceManager: new device at: %s",yadi->devNodePath.toLocal8Bit().data());


	}

#elif defined(WIN32)
	for (int com=3; com<10; com++) {
		SerialWrapper ser(QString("com%1").arg(com));
		if (ser.openSerial()) {
			bool found_input_device = false;
			bool found_output_device = false;
			QString product;
			qDebug() << "Connection to" << ser.deviceNode() << " established";
			char send[] = "v";
			int ret = ser.writeSerial(send,1);
			QTime wait;
			wait.restart();
			while (wait.elapsed() < 200) ;;
			int wait_loop = 0;
			while ( wait_loop < 5 ) {
				QByteArray rec = ser.readSerial(40);
				if (rec.size() < 1) break;

				if (rec.contains("Stonechip")) {
					if (rec.contains("Transceiver")) {
						product = "Stonechip DMX Transceiver";
						found_output_device = true;
						found_input_device = true;
					}
					else if (rec.contains("Sender")) {
						product = "Stonechip DMX Sender";
						found_output_device = true;
					}
					else if (rec.contains("Receiver")) {
						product = "Stonechip DMX Receiver";
						found_input_device = true;
					}
					break;
				}
				wait.restart();
				while (wait.elapsed() < 30) ;;
				wait_loop++;
			}
			qDebug() << "Found Input:" << found_input_device << "Output:" << found_output_device;
			if (found_input_device || found_output_device) {
				YadiDevice *yadi = new YadiDevice(QString("com%1").arg(com));
				globYadiDeviceList.append(yadi);

				yadi->devNodePath = QString("com%1").arg(com);
				yadi->deviceProductName = product;
				yadi->deviceManufacturer = "Stonechip Entertainment";
				yadi->deviceSerial = "";
				yadi->idVendor = "";
				yadi->idProduct = "";
				yadi->deviceNodePresent = true;
				if (product.contains("Transceiver")) {
					yadi->capabilities = YadiDevice::FL_INPUT_UNIVERSE | YadiDevice::FL_OUTPUT_UNIVERSE;
					yadi->maxDeviceDmxInChannels = 512;
					yadi->maxDeviceDmxOutChannels = 512;
					yadi->usedDmxOutChannels = 512;
					yadi->usedDmxInChannels = 512;
				}
				else if (product.contains("Receiver")) {
					yadi->capabilities = YadiDevice::FL_INPUT_UNIVERSE;
					yadi->usedDmxOutChannels = 0;
					yadi->maxDeviceDmxOutChannels = 0;
					yadi->usedDmxInChannels = 128;
					yadi->maxDeviceDmxInChannels = 128;
				}
				else if (product.contains("Sender")) {
					yadi->capabilities = YadiDevice::FL_OUTPUT_UNIVERSE;
				}

			}
		}

	}


#elif defined(__unix__)
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device *dev;

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		qDebug("Can't create udev\n");
		return globYadiDeviceList.size();
	}
	qDebug("YadiDeviceManager: devices:");

	/* Create a list of the devices in the 'tty' subsystem. */
	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "tty");
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);
	/* For each item enumerated, print out its information.
		   udev_list_entry_foreach is a macro which expands to
		   a loop. The loop will be executed for each member in
		   devices, setting dev_list_entry to a list entry
		   which contains the device's path in /sys. */
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path;

		/* Get the filename of the /sys entry for the device
			   and create a udev_device object (dev) representing it */
		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);

		/* usb_device_get_devnode() returns the path to the device node
			   itself in /dev. */

		QString devnode = udev_device_get_devnode(dev);

		/* The device pointed to by dev contains information about
			   the device. In order to get information about the
			   USB device, get the parent device with the
			   subsystem/devtype pair of "usb"/"usb_device". This will
			   be several levels up the tree, but the function will find
			   it.*/
		dev = udev_device_get_parent_with_subsystem_devtype(
					dev,
					"usb",
					"usb_device");
		if (dev) {
			/* From here, we can call get_sysattr_value() for each file
			   in the device's /sys entry. The strings passed into these
			   functions (idProduct, idVendor, serial, etc.) correspond
			   directly to the files in the directory which represents
			   the USB device. Note that USB strings are Unicode, UCS2
			   encoded, but the strings returned from
			   udev_device_get_sysattr_value() are UTF-8 encoded. */
			QString idvendor = udev_device_get_sysattr_value(dev,"idVendor");
			QString idproduct = udev_device_get_sysattr_value(dev, "idProduct");
			QString manufacturer = udev_device_get_sysattr_value(dev,"manufacturer");
			QString product = udev_device_get_sysattr_value(dev,"product");
			QString serial = udev_device_get_sysattr_value(dev, "serial");

			udev_device_unref(dev);

			if (product.startsWith("YADI DMX")) {
				// Before we append the device to the global list, we check if it is not
				// already in
				bool found = false;
				int idx = 0;
				while (idx < globYadiDeviceList.size() && !found) {
					if (globYadiDeviceList.at(idx)->devNodePath == devnode) {
						// Ok, is in list
						found = true;
					} else {
						idx++;
					}
				}
				// So have we found?
				YadiDevice *yadi;
				if (!found) {
					yadi = new YadiDevice(devnode);
					globYadiDeviceList.append(yadi);
				} else {
					yadi = globYadiDeviceList.at(idx);
				}

				yadi->devNodePath = devnode;
				yadi->deviceProductName = product;
				yadi->deviceManufacturer = manufacturer;
				yadi->deviceSerial = serial;
				yadi->idVendor = idvendor;
				yadi->idProduct = idproduct;
				yadi->deviceNodePresent = true;
				if (product.contains("Transceiver")) {
					yadi->capabilities = YadiDevice::FL_INPUT_UNIVERSE | YadiDevice::FL_OUTPUT_UNIVERSE;
					yadi->maxDeviceDmxInChannels = 512;
					yadi->maxDeviceDmxOutChannels = 512;
					if (!found || !update) {
						yadi->usedDmxOutChannels = 512;
						yadi->usedDmxInChannels = 512;
					}
				}
				else if (product.contains("Receiver")) {
					yadi->capabilities = YadiDevice::FL_INPUT_UNIVERSE;
					yadi->usedDmxOutChannels = 0;
					yadi->maxDeviceDmxOutChannels = 0;
					if (!found || !update) {
						yadi->usedDmxInChannels = 128;
						yadi->maxDeviceDmxInChannels = 128;
					}
				}
				else if (product.contains("Sender")) {
					yadi->capabilities = YadiDevice::FL_OUTPUT_UNIVERSE;
				}
				qDebug("YadiDeviceManager: new device at: %s",yadi->devNodePath.toLocal8Bit().data());

			}
		}
	}
	/* Free the enumerator object */
	udev_enumerate_unref(enumerate);

	udev_unref(udev);

	// Sort List by Serialnumber
	qSort(globYadiDeviceList.begin(),globYadiDeviceList.end(),yadiDeviceLessThan);

	// Now set correct parameters for tty communication
	for (int t=0; t<globYadiDeviceList.size(); t++) {
		if (globYadiDeviceList.at(t)->deviceNodePresent) {
			QProcess stty;
			QString cmd = QString("stty -F %1 115200 raw -echo -echoe").arg(globYadiDeviceList.at(t)->devNodePath);
			stty.start(cmd);
			stty.waitForFinished(2000);
			// qDebug() <<  "QProcess" << cmd;
			QString err =  QString::fromUtf8(stty.readAllStandardError().data());
			if (err.size()) {
				QMessageBox::warning(nullptr,QObject::tr("System error"),QObject::tr("An error occured while setting the device parameter"
															 "\n\n%1").arg(err));
			}
		}
	}

#endif

	return globYadiDeviceList.size();
}

void YadiDeviceManager::clearYadiDevices()
{
	while (!globYadiDeviceList.isEmpty()) {
		YadiDevice *yadi = globalDeviceList().takeFirst();
		yadi->deActivateDevice();
		delete yadi;
	}
}

void YadiDeviceManager::initYadiDevices()
{
	for (int t=0; t<globYadiDeviceList.size(); t++) {
		YadiDevice *yadi = globYadiDeviceList.at(t);
		yadi->activateDevice();
	}
}

YadiDevice *YadiDeviceManager::getDevice(const QString & dev_node, int cap)
{
	for (int t=0; t<globYadiDeviceList.size(); t++) {
		if (globYadiDeviceList.at(t)->devNodePath == dev_node && (globYadiDeviceList.at(t)->capabilities & cap))
			return globYadiDeviceList.at(t);
	}
	return 0;
}

void YadiDeviceManager::updateYadiDevicesStatus()
{
	bool rescan = false;
	enumerateYadiDevices(true);
	for (int t=0; t<globYadiDeviceList.size(); t++) {
		YadiDevice *yadi = globYadiDeviceList.at(t);
		if (!yadi->deviceNodePresent && QFile::exists(yadi->devNodePath)) {
			yadi->closeInOut();
			rescan = true;
		}
	}
	if (rescan) enumerateYadiDevices(true);
}

bool YadiDeviceManager::yadiDeviceLessThan(const YadiDevice *s1, const YadiDevice *s2)
{
	if (s1->deviceSerial < s2->deviceSerial) {
		return true;
	} else {
		return false;
	}
}

bool YadiDeviceManager::deviceNodeExists(const QString &devNode)
{
	bool exists = false;
	for (int t=0; t<globYadiDeviceList.size(); t++) {
		if (globYadiDeviceList.at(t)->devNodePath == devNode && globYadiDeviceList.at(t)->deviceNodePresent)
			exists = true;
	}
	return exists;
}

bool YadiDeviceManager::removeDevice(const QString &devNode)
{
	bool removed = false;
	for (int t=globYadiDeviceList.size()-1; t>=0; t--) {
		if (globYadiDeviceList.at(t)->devNodePath == devNode) {
			delete globYadiDeviceList.takeAt(t);
			removed = true;
		}
	}
	return removed;
}
