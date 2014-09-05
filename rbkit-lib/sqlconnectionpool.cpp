#include "sqlconnectionpool.h"

namespace RBKit {

SqlConnectionPool* SqlConnectionPool::singleton = NULL;

SqlConnectionPool::SqlConnectionPool()
    : currentVersion(0)
{
    setupDatabase();
}

SqlConnectionPool *SqlConnectionPool::getInstance() {
    if (!singleton) {
        singleton = new SqlConnectionPool();
        return singleton;
    } else {
        return singleton;
    }
}

void SqlConnectionPool::setupDatabase()
{
    qDebug() << "Creating new shared resource";
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(":memory:");

    if (!database.open()) {
        qDebug() << query.lastError();
        return;
    }
    query = QSqlQuery(database);
    qDebug() << "Setting up database done";
}

void SqlConnectionPool::prepareTables()
{
    QVector<QString> objectCreation;
    objectCreation.append(QString("create table rbkit_objects_%0(id integer primary key"
                                  ", class_name varchar, size integer "
                                  ", reference_count integer, file varchar)").arg(currentVersion));
    objectCreation.append(QString("create table rbkit_object_references_%0(id integer primary key asc"
                                  ", object_id integer"
                                  ", child_id integer)").arg(currentVersion));

    for(auto createStatement : objectCreation) {
        if (!query.exec(createStatement)) {
            qDebug() << "Error creating table " << createStatement;
            qDebug() << query.lastError();
        }
    }
    qDebug() << "Preparing tables done";
}

void SqlConnectionPool::loadSnapshot(ObjectStore *objectStore)
{
    currentVersion += 1;
    prepareTables();

    qDebug() << "Loading db snapshot";
    if (!query.prepare(
                QString("insert into rbkit_objects_%0(id, class_name, size, reference_count, file) values (?, ?, ?, ?, ?)")
                .arg(currentVersion))) {
        qDebug() << query.lastError();
        return;
    }

    objectStore->insertObjectsInDB(query);

    if (!query.prepare(QString("insert into rbkit_object_references_%0(object_id, child_id) values (?, ?)").arg(currentVersion)))
        qDebug() << query.lastError();

    objectStore->insertReferences(query);
}

HeapItem *SqlConnectionPool::rootOfSnapshot(int snapShotVersion)
{
    HeapItem *rootItem = new HeapItem(snapShotVersion);
    QSqlQuery searchQuery(QString("select class_name, count(id) as object_count, "
                          "sum(reference_count) as total_ref_count, sum(size) as total_size from rbkit_objects_%0 group by (class_name)").arg(snapShotVersion));
    while(searchQuery.next()) {
        HeapItem* item = new HeapItem(searchQuery.value(0).toString(), searchQuery.value(1).toInt(),
                                            searchQuery.value(2).toInt(), searchQuery.value(3).toInt(), snapShotVersion);
        rootItem->addChildren(item);
    }
    rootItem->childrenFetched = true;
    rootItem->computePercentage();
    return rootItem;
}

int SqlConnectionPool::getCurrentVersion()
{
   return currentVersion;
}

} // namespace RBKit
