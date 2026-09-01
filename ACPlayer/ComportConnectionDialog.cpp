#include "ComportConnectionDialog.h"
#include "ui_ComportConnectionDialog.h"

ComportConnectionDialog::ComportConnectionDialog(QSerialPort* portConnection, QWidget* parent)
{
	ui.setupUi(this); 
	port = portConnection;

	QPushButton* openButton = ui.buttonBox->button(QDialogButtonBox::Open);
	QPushButton* cancelButton = ui.buttonBox->button(QDialogButtonBox::Cancel);

	connect(openButton, &QPushButton::clicked, this, &ComportConnectionDialog::OpenComport);
	connect(cancelButton, &QPushButton::clicked, this, &ComportConnectionDialog::close);
}

ComportConnectionDialog::~ComportConnectionDialog()
{
	comportList.clear();
}

bool ComportConnectionDialog::OpenComport()
{
	port->setPort(comportList[ui.comport_CB->currentIndex()]);
	port->setBaudRate(QSerialPort::Baud115200);
	port->setFlowControl(QSerialPort::NoFlowControl);
	if (port->open(QIODevice::OpenModeFlag::WriteOnly)) {
		close();
		std::string message = (port->portName().toStdString() + " Connection Opened");
		QMessageBox::warning(this, "Success", QString::fromStdString(message), QMessageBox::StandardButton::Close);
		onConnectionSuccess.Invoke(0);
		return true;
	}
	else
	{
		std::string message = ("Could not open connection to " + port->portName().toStdString());
		QMessageBox::warning(this, "Error", QString::fromStdString(message), QMessageBox::StandardButton::Close);
		onConnectionFailed.Invoke(1);
		return false;
	}
}

void ComportConnectionDialog::Initialize()
{
	LoadComportList();
	FillupDropDown();
}

void ComportConnectionDialog::LoadComportList()
{
	comportList.clear();
	comportList = QSerialPortInfo::availablePorts();
}

void ComportConnectionDialog::FillupDropDown()
{
	QStringList list;
	for(int i = 0; i < comportList.size(); i++)
	{
		list.push_back(comportList[i].portName());
	}
	ui.comport_CB->addItems(list);
}

void ComportConnectionDialog::CloseDialog()
{
	comportList.clear();
}