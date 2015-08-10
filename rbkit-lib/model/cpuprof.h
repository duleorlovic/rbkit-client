#ifndef CPUPROF_H
#define CPUPROF_H
#include <QObject>
#include <QtTest/QtTest>
#include <QMap>
#include "cpustorage.h"

namespace RBKit
{
    class CpuProf : public QObject
    {
        Q_OBJECT
    public:
        void decodeMap(QList<QMap<int, QVariant>> data);
        void parseFrames(QMap<int, QVariant> frames);

    private:
        inline CpuStoragePtr store() {
            return CpuStorage::getStorage();
        }
    };
}
#endif // CPUPROF_H