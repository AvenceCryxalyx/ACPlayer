#include "ComportConnectionDialog.h"
#include "ui_ComportConnectionDialog.h"

ComportConnectionDialog::ComportConnectionDialog(QSerialPort* portConnection, QWidget* parent)
{
	DoConnection = new Events();
	ui.setupUi(this); 
	m_port = portConnection;
	m_parent = parent;
	QPushButton* openButton = ui.buttonBox->button(QDialogButtonBox::Open);
	QPushButton* cancelButton = ui.buttonBox->button(QDialogButtonBox::Cancel);

	connect(openButton, &QPushButton::clicked, this, &ComportConnectionDialog::OpenComport);
	connect(cancelButton, &QPushButton::clicked, this, &ComportConnectionDialog::close);
}

ComportConnectionDialog::ComportConnectionDialog(RealTimeSerialWorker* port, QThread* thread, QWidget* parent)
{
	DoConnection = new Events();
	ui.setupUi(this);
	worker = port;
	m_thread = thread;
	m_parent = parent;
	QPushButton* openButton = ui.buttonBox->button(QDialogButtonBox::Open);
	QPushButton* cancelButton = ui.buttonBox->button(QDialogButtonBox::Cancel);

	connect(openButton, &QPushButton::clicked, this, &ComportConnectionDialog::OpenComport);
	connect(cancelButton, &QPushButton::clicked, this, &ComportConnectionDialog::close);
}

ComportConnectionDialog::~ComportConnectionDialog()
{
	delete DoConnection;
	comportList.clear();
}

bool ComportConnectionDialog::OpenComport()
{
	DoConnection->Invoke(comportList[ui.comport_CB->currentIndex()].portName().toStdString());
	close();
	return true;
}

void ComportConnectionDialog::Initialize()
{
	LoadComportList();
	FillupDropDown();
}

void ComportConnectionDialog::LoadComportList()
{
	ui.comport_CB->clear();
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