#include "magellan_fast_launcher_sqllite.h"

LnkDB::LnkDB() {}
LnkDB::~LnkDB() {
    if (this->db.isOpen()) {
        this->db.close();
    }
}

bool LnkDB::create_db() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("magellan_fast_launcher.db");

    if (!db.open()) {
        qDebug() << "无法打开数据库:" << db.lastError();
        return false;
    }
    qDebug() << "数据库打开成功！";
    QSqlQuery query;
    bool success = query.exec(R"(
        CREATE TABLE IF NOT EXISTS launch (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            base_name TEXT NOT NULL,
            count INTEGER DEFAULT 0,
            status INTEGER DEFAULT 1
        )
    )");
    if (!success) {
        qDebug() << "创建表失败:" << query.lastError();
        return false;
    }
    this->db = db;
    return true;
}

bool LnkDB::insert_lnk(Lnk lnk) {
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO launch (id, name, base_name, count, status) VALUES (:id, :name, :base_name, :count, 1)");
    insertQuery.bindValue(":id", lnk.getId());
    insertQuery.bindValue(":name", lnk.getName());
    insertQuery.bindValue(":base_name", lnk.getBaseName());
    insertQuery.bindValue(":count", lnk.getCount());
    return insertQuery.exec();
}

bool LnkDB::insert_lnk(QList<Lnk> lnk_list) {
    // if (!this->db.transaction()) {
    //     qDebug() << "开启事务失败:" << db.lastError();
    //     return false;
    // }
    // QSqlQuery insertQuery;
    // insertQuery.prepare("INSERT INTO launch (id, name, count) VALUES (?, ?, ?)");
    // bool success = true;
    // for (const Lnk lnk : lnk_list) {
    //     insertQuery.addBindValue(lnk.getId());
    //     insertQuery.addBindValue(lnk.getName());
    //     insertQuery.addBindValue(lnk.getCount());

    //     if (!insertQuery.exec()) {
    //         qDebug() << "插入数据失败:" << insertQuery.lastError();
    //         success = false;
    //         break; // 插入失败，跳出循环
    //     }
    // }
    // if (success) {
    //     if (!db.commit()) {
    //         qDebug() << "提交事务失败:" << db.lastError();
    //         db.rollback(); // 提交失败，尝试回滚
    //     }
    // } else {
    //     if (db.rollback()) {
    //         qDebug() << "事务已回滚。";
    //     } else {
    //         qDebug() << "回滚事务失败:" << db.lastError();
    //     }
    // }
    return false;
}

bool LnkDB::delete_lnk(QString full_path) {
    QSqlQuery deleteQuery;
    deleteQuery.prepare("UPDATE launch SET status = 0 WHERE name = :full_path");
    deleteQuery.bindValue(":full_path", full_path);
    return deleteQuery.exec();
}

bool LnkDB::delete_lnk(Lnk lnk) {
    QSqlQuery deleteQuery;
    deleteQuery.prepare("UPDATE launch SET status = 0 WHERE id = :id");
    deleteQuery.bindValue(":id", lnk.getId());
    return deleteQuery.exec();
}

bool LnkDB::delete_lnk(QList<Lnk> lnk_list) {
    return false;
}

bool LnkDB::delete_lnk(QList<int> id_list) {
    return false;
}

bool LnkDB::update_lnk(Lnk lnk) {
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE launch SET count = :count WHERE id = :id");
    updateQuery.bindValue(":count", lnk.getCount());
    updateQuery.bindValue(":id", lnk.getId());
    return updateQuery.exec();
}

bool LnkDB::update_lnk(QList<Lnk> lnk_list) {
    return false;
}

bool LnkDB::access(QString full_path) {
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE launch SET count = count+1 WHERE name = :full_path");
    updateQuery.bindValue(":full_path", full_path);
    // updateQuery.bindValue(":id", lnk.getId());
    return updateQuery.exec();
}

QList<Lnk> LnkDB::select_lnk() {
    QList<Lnk> res;
    QString sql = "SELECT id, name, base_name, count, status FROM launch WHERE status=1";
    QSqlQuery selectQuery(sql);
    if (!selectQuery.exec()) {
        qDebug() << "Query failed:" << selectQuery.lastError();
    }
    while (selectQuery.next()) {
        int id = selectQuery.value("id").toInt();
        QString name = selectQuery.value("name").toString();
        QString base_name = selectQuery.value("base_name").toString();
        int count = selectQuery.value("count").toInt();
        int status = selectQuery.value("status").toInt();
        Lnk item = Lnk(id, name,base_name, count);
        res.append(item);
        qDebug() << "ID:" << id << "姓名:" << name << "计数:" << count;
    }
    return res;
}

QList<Lnk> LnkDB::select_lnk(Lnk lnk, bool order) {
    QList<Lnk> res;
    QString sql = "SELECT id, name, base_name, count FROM launch WHERE base_name LIKE :base_name AND status = 1";
    if (order) {
        sql += " order by count desc";
    }
    qDebug() << sql;
    QSqlQuery selectQuery;
    if (!selectQuery.prepare(sql)) {  // 准备SQL语句
        qDebug() << "Prepare failed:" << selectQuery.lastError();
        return res;
    }
    selectQuery.bindValue(":base_name", lnk.getBaseName() + "%");
    if (!selectQuery.exec()) {
        qDebug() << "Query failed:" << selectQuery.lastError();
    }
    while (selectQuery.next()) {
        int id = selectQuery.value("id").toInt();
        QString name = selectQuery.value("name").toString();
        QString base_name = selectQuery.value("base_name").toString();
        int count = selectQuery.value("count").toInt();
        Lnk item = Lnk(id, name, base_name, count);
        res.append(item);
        qDebug() << "ID:" << id << "姓名:" << name << "计数:" << count;
    }
    return res;
}

QList<Lnk> LnkDB::select_lnk(QString lnk_name, bool order) {
    QList<Lnk> res;
    QString sql = "SELECT id, name,base_name, count FROM launch WHERE base_name LIKE :base_name AND status = 1";
    if (order) {
        sql += " order by count desc";
    }
    QSqlQuery selectQuery;
    if (!selectQuery.prepare(sql)) {  // 准备SQL语句
        qDebug() << "Prepare failed:" << selectQuery.lastError();
        return res;
    }
    selectQuery.bindValue(":base_name", lnk_name + "%");
    if (!selectQuery.exec()) {
        qDebug() << "Query failed:" << selectQuery.lastError();
    }
    while (selectQuery.next()) {
        int id = selectQuery.value("id").toInt();
        QString name = selectQuery.value("name").toString();
        QString base_name = selectQuery.value("base_name").toString();
        int count = selectQuery.value("count").toInt();
        Lnk item = Lnk(id, name,base_name, count);
        res.append(item);
        qDebug() << "ID:" << id << "姓名:" << name << "计数:" << count;
    }
    return res;
}

QList<Lnk> LnkDB::select_lnk(QList<Lnk> lnk_list) {
    QList<Lnk> res;
    return res;
}

int LnkDB::getTotalCount() {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM launch"); // 直接统计 launch 表的总行数

    if (query.exec()) {
        if (query.next()) {
            // COUNT(*) 的结果在第一列
            return query.value(0).toInt();
        }
    } else {
        qDebug() << "查询失败:" << query.lastError();
    }
    return -1; // 表示出错
}
