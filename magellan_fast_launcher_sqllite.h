#ifndef MAGELLAN_FAST_LAUNCHER_SQLLITE_H
#define MAGELLAN_FAST_LAUNCHER_SQLLITE_H


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QList>

#include "Lnk.h"

class LnkDB {
public:
    LnkDB();

    ~LnkDB();

    bool create_db();

    bool insert_lnk(Lnk lnk);

    bool insert_lnk(QList<Lnk> lnk_list);

    bool delete_lnk(QString full_path);

    bool delete_lnk(Lnk lnk);

    bool delete_lnk(QList<Lnk> lnk_list);

    bool delete_lnk(QList<int> id_list);

    bool update_lnk(Lnk lnk);

    bool update_lnk(QList<Lnk> lnk_list);

    bool access(QString full_path);

    QList<Lnk> select_lnk();

    QList<Lnk> select_lnk(Lnk lnk, bool order);

    QList<Lnk> select_lnk(QString lnk_name, bool order);

    QList<Lnk> select_lnk(QList<Lnk> lnk_list);

    int getTotalCount();

private:
    QSqlDatabase db;
};


#endif // MAGELLAN_FAST_LAUNCHER_SQLLITE_H


