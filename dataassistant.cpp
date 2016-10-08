#include "dataassistant.h"
#include "ui_dataassistant.h"
#include "customtoolbutton.h"
#include "godefaultbutton.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>
#include <QMessageBox>
#include <QCompleter>
#include <QDir>
#include <QCloseEvent>

const char *separator = "#";
const char *password = "529529";

DataAssistant::DataAssistant(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataAssistant)
{
    ui->setupUi(this);

    this->setWindowTitle(QString("DataAssistant"));

    ui->pMAddBtn->initialize(":/images/add", ":/images/add_press", ":/images/add_hover");
    ui->pFAddBtn->initialize(":/images/add", ":/images/add_press", ":/images/add_hover");
    ui->pFSearchBtn->initialize(":/images/go", ":/images/go_press", ":/images/go_hover");
    ui->pPAddBtn->initialize(":/images/add", ":/images/add_press", ":/images/add_hover");
    ui->pProfitAddBtn->initialize(":/images/add", ":/images/add_press", ":/images/add_hover");
    ui->pSAddBtn->initialize(":/images/add", ":/images/add_press", ":/images/add_hover");
    ui->pLoginBtn->initialize(":/images/login", ":/images/login_press", ":/images/login_hover");

    new GoDefaultButton(ui->pPassword, QString());
    ui->pPassword->setEchoMode(QLineEdit::Password);
    connect(ui->pPassword, &QLineEdit::editingFinished, this, &DataAssistant::on_pLoginBtn_clicked);

    setWindowIcon(QIcon(":/images/bird.ico"));

    QDir dir;
    if(!dir.exists(".//data")) {
        dir.mkdir(".//data");
    }

    ui->pMDate->setDate(QDate::currentDate());
    ui->pFDate->setDate(QDate::currentDate());
    ui->pPDate->setDate(QDate::currentDate());
    ui->pPDateCalc->setDate(QDate::currentDate());
    ui->pSDate->setDate(QDate::currentDate());
    ui->pSDateCalc->setDate(QDate::currentDate());

    loadMList();
    loadFList();
    loadPList();
    loadProfitList();
    loadSList();
}

DataAssistant::~DataAssistant()
{
    writeMList();
    writeFList();
    writePList();
    writeProfitList();
    writeSList();

    delete ui;
}

void DataAssistant::on_pLoginBtn_clicked()
{
    if(ui->pPassword->text() != password) {
        return;
    }

    if(ui->pAccount->currentIndex() == 0) {
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void DataAssistant::closeEvent(QCloseEvent *event)
{
    if (ui->stackedWidget->currentIndex() != 0) {
        ui->stackedWidget->setCurrentIndex(0);
        ui->pPassword->clear();
        // 有bug，导致还是hover时状态，临时这么解决下.
        ui->pLoginBtn->initialize(":/images/login", ":/images/login_press", ":/images/login_hover");
        event->ignore();
    } else {
        event->accept();
    }
}

void DataAssistant::on_pMAddBtn_clicked()
{
    if((ui->pMName->isVisible() && ui->pMName->currentText().isEmpty()) || (ui->pMFactory->isVisible() && ui->pMFactory->currentText().isEmpty())) {
        QMessageBox::warning(this, QString("提示"), QString("录入信息不完整"));
        return;
    }

    IOEvent event;
    event.type = ui->pMType->currentIndex();
    event.date = ui->pMDate->date();
    event.name = ui->pMName->currentText();
    event.count = ui->pMCount->value();
    event.value = ui->pMValue->value();
    event.factory = ui->pMFactory->currentText();

    insertEvent(event);

    //更新当前统计数据.
    calcMLeftCount();
    calcMLeftValue();
}

void DataAssistant::insertEvent(const IOEvent &event)
{
    _MList << event;

    QStringList list;
    if(event.type == In)
    {
        list << QString("入库");
        list << event.date.toString("yyyy-MM-dd");
        list << event.name;
        list << QString::number(event.count);
        list << QString::number(event.value);
        list << event.factory;

        // 更新品名和供应商列表.
        if(ui->pMName->findText(event.name) < 0) {
            ui->pMName->insertItem(0, event.name);
            ui->pMNameCalc->insertItem(0, event.name);
        }
        if(ui->pMFactory->findText(event.factory) < 0) {
            ui->pMFactory->insertItem(0, event.factory);
            ui->pMFactoryCalc->insertItem(0, event.factory);
        }
    }
    else if(event.type == Out)
    {
        list << QString("出库");
        list << event.date.toString("yyyy-MM-dd");
        list << event.name;
        list << QString::number(event.count);
        list << QString("--");
        list << QString("--");
    }
    else if(event.type == Pay)
    {
        list << QString("结算");
        list << event.date.toString("yyyy-MM-dd");
        list << QString("--");
        list << QString("--");
        list << QString::number(event.value);
        list << event.factory;
    }

    // 新建item并设置图标.
    QStringList imageList;
    imageList << ":/images/in" << ":/images/out" << ":/images/pay";
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->pMTree, list);
    QPixmap pix(imageList[event.type]);
    item->setData(0, Qt::DecorationRole, QVariant(pix));
}

void DataAssistant::loadMList()
{
    QFile file("./data/material.data");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        QString str = in.readLine();
        QStringList list = str.split(separator);
        if(list.count() != 6) {
            // 为了提示用户数据文件被人为改写破坏.
            QMessageBox::warning(this, QString("提示"), QString("数据有误，可能造成数据缺失，请修正后再行打开"));
            file.copy("./data/material" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm") + ".backup");
            return;
        }
        IOEvent event;
        event.type = list[0].toUInt();
        event.date = QDate::fromString(list[1], "yyyy-MM-dd");
        event.name = list[2];
        event.count = list[3].toUInt();
        event.value = list[4].toFloat();
        event.factory = list[5];

        insertEvent(event);
    }

    file.close();

    //更新当前统计数据.
    calcMLeftCount();
    calcMLeftValue();
}

void DataAssistant::writeMList()
{
    QFile file("./data/material.data");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    for(auto i = _MList.begin(); i != _MList.end(); ++i)
    {
        IOEvent e = *i;
        out << e.type  << separator << e.date.toString("yyyy-MM-dd") << separator
            << e.name << separator << e.count << separator << e.value << separator << e.factory << endl;
    }

    file.close();
}

void DataAssistant::calcMLeftCount()
{
    QString name = ui->pMNameCalc->currentText();
    if(name.isEmpty()) {
        return;
    }

    int sum = 0;
    for(auto i = _MList.begin(); i != _MList.end(); ++i)
    {
        IOEvent e = *i;
        if(e.type == In && name == e.name) {
            sum += e.count;
        } else if(e.type == Out && name == e.name) {
            sum -= e.count;
        }
    }

    ui->pMLeftCount->display(sum);
}

void DataAssistant::calcMLeftValue()
{
    QString name = ui->pMFactoryCalc->currentText();
    if(name.isEmpty()) {
        return;
    }

    float sum = 0;
    for(auto i = _MList.begin(); i != _MList.end(); ++i)
    {
        IOEvent e = *i;
        if(e.type == In && name == e.factory) {
            sum += e.value*e.count;
        } else if(e.type == Pay && name == e.factory) {
            sum -= e.value;
        }
    }

    ui->pMLeftValue->display(sum);
}

void DataAssistant::on_pMType_activated(int index)
{
    if(index == In)
    {
        ui->pMName->show();
        ui->pMNameLabel->show();
        ui->pMCount->show();
        ui->pMCountLabel->show();
        ui->pMFactory->show();
        ui->pMFactoryLabel->show();
        ui->pMValue->show();
        ui->pMValueLabel->show();

        // 只有出货可以自定义新的类型和厂商.
        ui->pMName->setEditable(true);
        ui->pMFactory->setEditable(true);
    }
    else if(index == Out)
    {
        ui->pMName->show();
        ui->pMNameLabel->show();
        ui->pMCount->show();
        ui->pMCountLabel->show();
        ui->pMFactory->hide();
        ui->pMFactoryLabel->hide();
        ui->pMValue->hide();
        ui->pMValueLabel->hide();

        ui->pMName->setEditable(false);
    }
    else if(index == Pay)
    {
        ui->pMName->hide();
        ui->pMNameLabel->hide();
        ui->pMCount->hide();
        ui->pMCountLabel->hide();
        ui->pMFactory->show();
        ui->pMFactoryLabel->show();
        ui->pMValue->show();
        ui->pMValueLabel->show();

        ui->pMFactory->setEditable(false);
    }
}

void DataAssistant::on_pMNameCalc_activated(int index)
{
    calcMLeftCount();
}

void DataAssistant::on_pMFactoryCalc_activated(int index)
{
    calcMLeftValue();
}

void DataAssistant::on_pFAddBtn_clicked()
{
    QString name = ui->pFName->currentText();
    QString factory = ui->pFFactory->currentText();

    if(name.isEmpty() || factory.isEmpty()) {
        QMessageBox::warning(this, QString("提示"), QString("录入信息不完整"));
        return;
    }

    QStringList list;

    list << ui->pFDate->date().toString("yyyy-MM-dd");
    list << name;
    list << QString::number(ui->pFValue->value());
    list << factory;
    list << ui->pFNum->text();

    // 新建item并设置图标.
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->pFTree, list);
    QPixmap pix(":/images/series");
    item->setData(4, Qt::DecorationRole, QVariant(pix));

    // 更新品名和供应商列表.
    if(ui->pFName->findText(name) < 0) {
        ui->pFName->insertItem(0, name);
        ui->pFNameCalc->insertItem(0, name);
    }
    if(ui->pFFactory->findText(factory) < 0) {
        ui->pFFactory->insertItem(0, factory);
    }

    //更新当前统计数据.
    calcFLeftCount();
}

void DataAssistant::loadFList()
{
    QFile file("./data/fixAsset.data");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        QString str = in.readLine();
        QStringList list = str.split(separator);
        if(list.count() != 5) {
            // 为了提示用户数据文件被人为改写破坏.
            QMessageBox::warning(this, QString("提示"), QString("数据有误，可能造成数据缺失，请修正后再行打开"));
            file.copy("./data/fixAsset" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm") + ".backup");
            return;
        }

        // 新建item并设置图标.
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->pFTree, list);
        QPixmap pix(":/images/series");
        item->setData(4, Qt::DecorationRole, QVariant(pix));

        QString name = list[1];
        QString factory = list[3];

        // 更新品名和供应商列表.
        if(ui->pFName->findText(name) < 0) {
            ui->pFName->insertItem(0, name);
            ui->pFNameCalc->insertItem(0, name);
        }
        if(ui->pFFactory->findText(factory) < 0) {
            ui->pFFactory->insertItem(0, factory);
        }
    }

    file.close();

    //更新当前统计数据.
    calcFLeftCount();
}

void DataAssistant::writeFList()
{
    QFile file("./data/fixAsset.data");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    int num = ui->pFTree->topLevelItemCount();

    for(int i = 0; i < num; ++i)
    {
        QTreeWidgetItem *item = ui->pFTree->topLevelItem(i);
        out << item->text(0)  << separator << item->text(1)  << separator << item->text(2)
            << separator << item->text(3)  << separator << item->text(4) << endl;
    }

    file.close();
}

void DataAssistant::calcFLeftCount()
{
    QString name = ui->pFNameCalc->currentText();
    if(name.isEmpty()) {
        return;
    }

    int sum = 0;
    int num = ui->pFTree->topLevelItemCount();

    for(int i = 0; i < num; ++i)
    {
        QTreeWidgetItem *item = ui->pFTree->topLevelItem(i);
        if(item->text(1) == name) {
            sum++;
        }
    }

    ui->pFLeftCount->display(sum);
}

void DataAssistant::on_pFNameCalc_activated(int index)
{
    calcFLeftCount();
}

void DataAssistant::on_pFSearchBtn_clicked()
{
    QString str = ui->pFSearchEdit->text();
    int num = ui->pFTree->topLevelItemCount();

    for(int i = 0; i < num; ++i) {
        QTreeWidgetItem *item = ui->pFTree->topLevelItem(i);
        for(int j = 0; j < item->columnCount(); ++j) {
            if(item->text(j) == str) {
                ui->pFTree->scrollToItem(item);
                item->setSelected(true);
            }
        }
    }
}

void DataAssistant::on_pPAddBtn_clicked()
{
    QString docName = ui->pPNameDoc->currentText();
    QString patName = ui->pPNamePat->currentText();

    if(docName.isEmpty() || patName.isEmpty()) {
        QMessageBox::warning(this, QString("提示"), QString("录入信息不完整"));
        return;
    }

    QStringList list;

    list << ui->pPDate->date().toString("yyyy-MM-dd");
    list << docName << patName;
    list << QString::number(ui->pPValue->value());

    // 新建item并设置图标.
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->pPTree, list);
    QPixmap pix;
    pix.load(":/images/doctor");
    item->setData(1, Qt::DecorationRole, QVariant(pix));
    pix.load(":/images/patient");
    item->setData(2, Qt::DecorationRole, QVariant(pix));

    // 更新品名和供应商列表.
    if(ui->pPNameDoc->findText(docName) < 0) {
        ui->pPNameDoc->insertItem(0, docName);
        ui->pPNameDocCalc->insertItem(0, docName);
    }
    if(ui->pPNamePat->findText(patName) < 0) {
        ui->pPNamePat->insertItem(0, patName);
    }

    //更新当前统计数据.
    calcPLeftValue();
}

void DataAssistant::loadPList()
{
    QFile file("./data/process.data");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        QString str = in.readLine();
        QStringList list = str.split(separator);
        if(list.count() != 4) {
            // 为了提示用户数据文件被人为改写破坏.
            QMessageBox::warning(this, QString("提示"), QString("数据有误，可能造成数据缺失，请修正后再行打开"));
            file.copy("./data/process" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm") + ".backup");
            return;
        }

        // 新建item并设置图标.
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->pPTree, list);
        QPixmap pix;
        pix.load(":/images/doctor");
        item->setData(1, Qt::DecorationRole, QVariant(pix));
        pix.load(":/images/patient");
        item->setData(2, Qt::DecorationRole, QVariant(pix));

        QString docName = list[1];
        QString patName = list[2];

        // 更新品名和供应商列表.
        if(ui->pPNameDoc->findText(docName) < 0) {
            ui->pPNameDoc->insertItem(0, docName);
            ui->pPNameDocCalc->insertItem(0, docName);
        }
        if(ui->pPNamePat->findText(patName) < 0) {
            ui->pPNamePat->insertItem(0, patName);
        }
    }

    file.close();

    //更新当前统计数据.
    calcPLeftValue();
}

void DataAssistant::writePList()
{
    QFile file("./data/process.data");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    int num = ui->pPTree->topLevelItemCount();

    for(int i = 0; i < num; ++i)
    {
        QTreeWidgetItem *item = ui->pPTree->topLevelItem(i);
        out << item->text(0)  << separator << item->text(1)  << separator << item->text(2)
            << separator << item->text(3) << endl;
    }

    file.close();
}

void DataAssistant::calcPLeftValue()
{
    QString name = ui->pPNameDocCalc->currentText();
    if(name.isEmpty()) {
        return;
    }

    float sum = 0;
    int num = ui->pPTree->topLevelItemCount();
    QDate date = ui->pPDateCalc->date();

    for(int i = 0; i < num; ++i)
    {
        QTreeWidgetItem *item = ui->pPTree->topLevelItem(i);
        QDate date2 = QDate::fromString(item->text(0), "yyyy-MM-dd");
        if(name == item->text(1) && date.year() == date2.year() && date.month() == date2.month()) {
            sum += item->text(3).toFloat();
        }
    }

    ui->pPLeftValue->display(sum);
}


void DataAssistant::on_pPNameDocCalc_activated(int index)
{
    calcPLeftValue();
}

void DataAssistant::on_pPDateCalc_dateChanged()
{
    calcPLeftValue();
}

void DataAssistant::on_pProfitAddBtn_clicked()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->pProfitTree);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setText(0, QDate::currentDate().toString("yyyy-MM"));
    item->setData(8, Qt::ForegroundRole, QBrush(QColor(Qt::red)));
    QPixmap pix(":/images/profit");
    item->setData(8, Qt::DecorationRole, QVariant(pix));
}

void DataAssistant::loadProfitList()
{
    QFile file("./data/profit.data");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        QString str = in.readLine();
        QStringList list = str.split(separator);
        if(list.count() != 9) {
            // 为了提示用户数据文件被人为改写破坏.
            QMessageBox::warning(this, QString("提示"), QString("数据有误，可能造成数据缺失，请修正后再行打开"));
            file.copy("./data/profit" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm") + ".backup");
            return;
        }

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->pProfitTree, list);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(8, Qt::ForegroundRole, QBrush(QColor(Qt::red)));

        QPixmap pix(":/images/profit");
        item->setData(8, Qt::DecorationRole, QVariant(pix));
    }

    file.close();
}

void DataAssistant::writeProfitList()
{
    QFile file("./data/profit.data");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    int num = ui->pProfitTree->topLevelItemCount();

    for(int i = 0; i < num; ++i)
    {
        QTreeWidgetItem *item = ui->pProfitTree->topLevelItem(i);
        for(int j = 0; j < item->columnCount() - 1; ++j) {
            out << item->text(j)  << separator;
        }
        out << item->text(8) << endl;
    }

    file.close();
}

void DataAssistant::on_pProfitTree_itemChanged(QTreeWidgetItem * item)
{
    float profit = item->text(2).toFloat();
    for(int i = 3; i < 8; ++i) {
        profit -= item->text(i).toFloat();
    }

    //  防止死循环.
    if(item->text(8).toFloat() != profit) {
        item->setText(8, QString::number(profit));
    }
}

void DataAssistant::on_pSAddBtn_clicked()
{
    QString docName = ui->pSName->currentText();

    if(docName.isEmpty()) {
        QMessageBox::warning(this, QString("提示"), QString("录入信息不完整"));
        return;
    }

    QStringList list;

    list << ui->pSDate->date().toString("yyyy-MM-dd");
    list << docName;
    list << QString::number(ui->pSValue1->value());
    list << QString::number(ui->pSValue2->value());
    list << QString::number(ui->pSValue3->value());
    list << QString::number(ui->pSValue4->value());
    list << QString::number(ui->pSValue1->value() + ui->pSValue2->value() + ui->pSValue3->value() + ui->pSValue4->value());

    // 新建item并设置图标.
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->pSTree, list);
    QPixmap pix;
    pix.load(":/images/doctor");
    item->setData(1, Qt::DecorationRole, QVariant(pix));
    pix.load(":/images/profit");
    item->setData(6, Qt::DecorationRole, QVariant(pix));

    // 更新品名和供应商列表.
    if(ui->pSName->findText(docName) < 0) {
        ui->pSName->insertItem(0, docName);
        ui->pSNameCalc->insertItem(0, docName);
    }

    //更新当前统计数据.
    calcSLeftValue();
}

void DataAssistant::loadSList()
{
    QFile file("./data/sale.data");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        QString str = in.readLine();
        QStringList list = str.split(separator);
        if(list.count() != 7) {
            // 为了提示用户数据文件被人为改写破坏.
            QMessageBox::warning(this, QString("提示"), QString("数据有误，可能造成数据缺失，请修正后再行打开"));
            file.copy("./data/sale" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm") + ".backup");
            return;
        }

        // 新建item并设置图标.
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->pSTree, list);
        QPixmap pix;
        pix.load(":/images/doctor");
        item->setData(1, Qt::DecorationRole, QVariant(pix));
        pix.load(":/images/profit");
        item->setData(6, Qt::DecorationRole, QVariant(pix));

        QString docName = list[1];

        // 更新品名和供应商列表.
        if(ui->pSName->findText(docName) < 0) {
            ui->pSName->insertItem(0, docName);
            ui->pSNameCalc->insertItem(0, docName);
        }
    }

    file.close();

    //更新当前统计数据.
    calcSLeftValue();
}

void DataAssistant::writeSList()
{
    QFile file("./data/sale.data");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    int num = ui->pSTree->topLevelItemCount();

    for(int i = 0; i < num; ++i)
    {
        QTreeWidgetItem *item = ui->pSTree->topLevelItem(i);
        for(int j = 0; j < item->columnCount() - 1; ++j) {
            out << item->text(j)  << separator;
        }
        out << item->text(6) << endl;
    }

    file.close();
}

void DataAssistant::calcSLeftValue()
{
    QString name = ui->pSNameCalc->currentText();
    if(name.isEmpty()) {
        return;
    }

    float sum = 0;
    int num = ui->pSTree->topLevelItemCount();
    QDate date = ui->pSDateCalc->date();

    for(int i = 0; i < num; ++i)
    {
        QTreeWidgetItem *item = ui->pSTree->topLevelItem(i);
        QDate date2 = QDate::fromString(item->text(0), "yyyy-MM-dd");
        if(name == item->text(1) && date.year() == date2.year() && date.month() == date2.month()) {
            sum += item->text(6).toFloat();
        }
    }

    ui->pSLeftValue->display(sum);
}


void DataAssistant::on_pSNameCalc_activated(int index)
{
    calcSLeftValue();
}

void DataAssistant::on_pSDateCalc_dateChanged()
{
    calcSLeftValue();
}
