#ifndef LNK_H
#define LNK_H
#include <QCoreApplication>

class Lnk
{
public:
    Lnk();

    Lnk(int id, QString name);

    Lnk(int id, QString name, QString base_name, int count);
    Lnk(int id, QString name, QString base_name, int count, int status);

    ~Lnk();

    int getId() const;

    void setId(int id);

    QString getName() const;

    void setName(QString name);

    QString getBaseName() const;

    void setBaseName(QString base_name);

    int getCount() const;

    void setCount(int count);

    void coundAdd();

    int getStatus() const;

    void setStatus(int status);

private:
    int id;
    QString name;
    QString base_name;
    int count;
    int status;
};

#endif // LNK_H
