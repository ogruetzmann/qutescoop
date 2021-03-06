/**************************************************************************
 *  This file is part of QuteScoop. See README for license
 **************************************************************************/

#include "MetarModel.h"

#include "Settings.h"
#include "Whazzup.h"
#include "Window.h"

#define MAX_METARS 60

MetarModel::MetarModel(QObject *parent):
        QAbstractListModel(parent) {
}

int MetarModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    if(_airportList.size() > MAX_METARS)
        return 1;

    return _metarList.size();
}

int MetarModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 1;
}

QVariant MetarModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole) {
        if(_airportList.size() > MAX_METARS)
            return QString("Too many airports match your search (%1)").arg(_airportList.size());

        Airport* a = _metarList[index.row()];
        switch(index.column()) {
        case 0: return a->metar.encoded; break;
        }
    }

    return QVariant();
}

QVariant MetarModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical)
        return QVariant();

    if (section != 0)
        return QVariant();

    if (_metarList.isEmpty())
        return QString("No Metars");

    if (_metarList.size() == 1)
        return QString("1 Metar");

    return QString("%1 Metars").arg(_metarList.size());
}

void MetarModel::setData(const QList<Airport*>& airports)  {
    _airportList = airports;
    _metarList = airports;

    // remove all entries from metarList with invalid or dead METARs
    for(int i = _metarList.size() - 1; i >= 0; i--) {
        if(_metarList[i]->metar.needsRefresh() || _metarList[i]->metar.doesNotExist())
            _metarList.removeAt(i);
    }
    refresh();
}

void MetarModel::refresh() {
    abortAll();

    if(_airportList.size() > MAX_METARS) {
        // avoid hammering the server with gazillions of metar requests
        return;
    }

    for(int i = 0; i < _airportList.size(); i++) {
        if(_airportList[i] == 0) continue;

        if(!_airportList[i]->metar.doesNotExist() && _airportList[i]->metar.needsRefresh()) {
            downloadMetarFor(_airportList[i]);
        }
    }
}

void MetarModel::downloadMetarFor(Airport* airport) {
    QString location = Whazzup::instance()->atisLink(airport->label);
    if(location.isEmpty())
        return;

    QUrl url(location);

    DownloadQueue dq;
    dq.airport = airport;
    dq.buffer = new QBuffer;
    dq.buffer->open(QBuffer::ReadWrite);
    qDebug() << "MetarModel::downloadMetarFor()" << url;

    _metarReply = Net::g(url);
    connect(_metarReply, SIGNAL(finished()), this, SLOT(downloaded()));
}

void MetarModel::modelClicked(const QModelIndex& index) {
    Airport *a = _metarList[index.row()];
    if(a != 0)
        if (Window::instance(false) != 0)
            Window::instance()->updateMetarDecoder(a->label,
                                                          a->metar.encoded + "<hr>" + a->metar.decodedHtml());
}

void MetarModel::abortAll() {
    _metarReply->abort();
}

void MetarModel::downloaded() {
    qDebug() << "MetarModel::downloaded()";
    disconnect(_metarReply, SIGNAL(finished()), this, SLOT(downloaded()));
    _metarReply->deleteLater();

    if(_metarReply->error() != QNetworkReply::NoError) {
        return;
    }

    QString line = _metarReply->readAll().trimmed();

    // TODO
//    if(!line.isEmpty())
//        dq.airport->metar = Metar(line);
//    if(!dq.airport->metar.isNull() && dq.airport->metar.isValid())
//        gotMetarFor(dq.airport);
}

void MetarModel::gotMetarFor(Airport* airport) {
    if(_airportList.contains(airport)) {
        beginResetModel();
        if(!_metarList.contains(airport))
            _metarList.append(airport);
        qDebug() << "MetarModel::gotMetarFor()" << airport->label << ":" << airport->metar.encoded;
        emit gotMetar(airport->label);
        endResetModel();
    }
}
