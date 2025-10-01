#include "Lnk.h"

Lnk::Lnk() {}

Lnk::Lnk(int id, QString name) {
    this->id = id;
    this->name = name;
    this->count = 0;
}

Lnk::Lnk(int id, QString name, QString base_name, int count) {
    this->id = id;
    this->name = name;
    this->base_name = base_name;
    this->count = count;
}

Lnk::Lnk(int id, QString name, QString base_name, int count, int status) {
    this->id = id;
    this->name = name;
    this->base_name = base_name;
    this->count = count;
    this->status = status;
}

Lnk::~Lnk() {}


int Lnk::getId() const {
    return this->id;
}

void Lnk::setId(int id) {
    this->id = id;
}

QString Lnk::getName() const {
    return this->name;
}

void Lnk::setName(QString name) {
    this->name = name;
}

QString Lnk::getBaseName() const {
    return this->base_name;
}

void Lnk::setBaseName(QString base_name){
    this->base_name = base_name;
}

int Lnk::getCount() const {
    return this->count;
}

void Lnk::setCount(int count) {
    this->count = count;
}

void Lnk::coundAdd() {
    this->count++;
}

int Lnk::getStatus() const {
    return this->status;
}

void Lnk::setStatus(int status) {
    this->status = status;
}

