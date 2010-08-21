/**************************************************************************
 *  This file is part of QuteScoop.
 *  Copyright (C) 2007-2008 Martin Domig <martin@domig.net>
 *
 *  QuteScoop is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  QuteScoop is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with QuteScoop.  If not, see <http://www.gnu.org/licenses/>
 **************************************************************************/

#ifndef AIRPORTDETAILSATCMODEL_H_
#define AIRPORTDETAILSATCMODEL_H_

#include <QAbstractTableModel>
#include <QList>
#include "Controller.h"

class AirportDetailsAtcModel : public QAbstractTableModel {
	Q_OBJECT

public:
	AirportDetailsAtcModel(QObject *parent = 0) : QAbstractTableModel(parent) {}

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const { return controllers.count(); }
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const { return 5; } 
	
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation,
	                         int role = Qt::DisplayRole) const;
	
public slots:
	void setClients(const QList<Controller*>& controllers);
	void modelSelected(const QModelIndex& index);

private:
	QList<Controller*> controllers;
};

#endif /*CLIENTLISTMODEL_H_*/