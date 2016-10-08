#ifndef DATAASSISTANT_H
#define DATAASSISTANT_H

#include <QWidget>
#include <QDate>

namespace Ui {
class DataAssistant;
}

enum IOType : quint8 {
    In = 0,
    Out,
    Pay
};

///< \note 记录一事件（出库、入库、结算）
class IOEvent
{
public:
    quint8 type;
    QDate date;
    QString name;
    quint32 count;
    float value;
    QString factory;

};

class QTreeWidgetItem;
class DataAssistant : public QWidget
{
    Q_OBJECT

public:
    explicit DataAssistant(QWidget *parent = 0);
    ~DataAssistant();

private slots:
    ///< \note 登陆按钮，初始化不同界面.
    void on_pLoginBtn_clicked();

    void on_pMAddBtn_clicked();
    void on_pMType_activated(int index);
    void on_pMNameCalc_activated(int index);
    void on_pMFactoryCalc_activated(int index);

    void on_pFAddBtn_clicked();
    void on_pFNameCalc_activated(int index);
    void on_pFSearchBtn_clicked();

    void on_pPAddBtn_clicked();
    void on_pPNameDocCalc_activated(int index);
    void on_pPDateCalc_dateChanged();

    void on_pProfitAddBtn_clicked();
    void on_pProfitTree_itemChanged(QTreeWidgetItem *item);

    void on_pSAddBtn_clicked();
    void on_pSNameCalc_activated(int index);
    void on_pSDateCalc_dateChanged();

protected:
    void closeEvent(QCloseEvent *event);

private:
    ///< \note 插入一事件至列表
    void insertEvent(const IOEvent &event);

    ///< \note 加载目前材质统计表信息.
    void loadMList();

    ///< \note 将目前材质统计表信息写入文件保存.
    void writeMList();

    ///< \note 更新计算当前种类库存显示.
    void calcMLeftCount();

    ///< \note 更新计算当前供货商待结算金额.
    void calcMLeftValue();

    ///< \note 加载目前器械统计表信息.
    void loadFList();

    ///< \note 将目前器械统计表信息写入文件保存.
    void writeFList();

    ///< \note 更新计算当前种类库存显示.
    void calcFLeftCount();

    ///< \note 加载目前加工费统计表信息.
    void loadPList();

    ///< \note 将目前加工费统计表信息写入文件保存.
    void writePList();

    ///< \note 更新计算当前金额显示.
    void calcPLeftValue();

    ///< \note 加载目前利润统计表信息.
    void loadProfitList();

    ///< \note 将目前利润统计表信息写入文件保存.
    void writeProfitList();

    ///< \note 加载目前销售报表信息.
    void loadSList();

    ///< \note 将目前销售报表信息写入文件保存.
    void writeSList();

    ///< \note 更新计算当前金额显示.
    void calcSLeftValue();

private:
    Ui::DataAssistant *ui;

    QList<IOEvent> _MList;
};

#endif // DATAASSISTANT_H
