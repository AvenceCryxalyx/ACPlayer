#pragma once
#include "ui_ComportConnectionDialog.h"
#include "UARTLibrary.h"
#include <QtWidgets>
#include <QtCore>
#include <QtGui>
#include <QtSerialPort>
#include <QIODevice>
#include <QSerialPortInfo>
#include <QThread>
#include "Events.h"
#include "RealTimeSerialWorker.h"

using namespace std;
class ComportConnectionDialog : public QDialog
{
	Q_OBJECT
public:
	ComportConnectionDialog(QSerialPort* portConnection, QWidget* parent);
	ComportConnectionDialog(RealTimeSerialWorker* portConnection, QThread* thread, QWidget* parent);
	~ComportConnectionDialog();
	bool OpenComport();
	void Initialize();
	Events* DoConnection;
private:
	QObject* m_parent;
	QThread* m_thread;
	QSerialPort* m_port;
	RealTimeSerialWorker* worker;
	Ui_ComportConnectionDialog ui;
	void LoadComportList();
	void FillupDropDown();
	void CloseDialog();
	QList<QSerialPortInfo> comportList;
};

