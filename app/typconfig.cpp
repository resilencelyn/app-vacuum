/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       型号管理界面
*******************************************************************************/
#include "typconfig.h"

const int maxRow = 20;
const QString textConf = "<p style='font-size:12pt;color:#FFFFFF;' align='left'>%1</p>";

TypConfig::TypConfig(QWidget *parent) : QWidget(parent)
{
    initUI();
}

void TypConfig::initUI()
{
    initLayout();
    initViewBar();
    initItemBar();
    initConfigBar();
    initButtonBar();
}

void TypConfig::initLayout()
{
    blayout = new QGridLayout;
    hlayout = new QVBoxLayout;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(hlayout);
    layout->addLayout(blayout);
    layout->setStretch(0, 5);
    layout->setStretch(1, 4);
    layout->setSpacing(10);

    QVBoxLayout *mlayout = new QVBoxLayout(this);
    QGroupBox *box = new QGroupBox(this);
    box->setLayout(layout);
    mlayout->addWidget(box);
}

void TypConfig::initViewBar()
{
    QStringList headers;
    headers << tr("编号") << tr("型号名称");
    view = new QTableWidget(this);
    view->setRowCount(maxRow);
    view->verticalHeader()->hide();
    view->setColumnCount(headers.size());
    view->setHorizontalHeaderLabels(headers);
    view->horizontalHeader()->setFixedHeight(30);
    view->horizontalHeader()->setDefaultSectionSize(52);
    view->horizontalHeader()->setStretchLastSection(true);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectModel()));
    connect(view, SIGNAL(clicked(QModelIndex)), this, SLOT(clickViewBar()));
    for (int col=0; col < headers.size(); col++) {
        for (int row=0; row < maxRow; row++) {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setTextAlignment(Qt::AlignCenter);
            view->setItem(row, col, item);
        }
    }
    QVBoxLayout *vlay = new QVBoxLayout;
    vlay->addWidget(view);
    hlayout->addLayout(vlay);
    hlayout->setStretch(0, 15);

    QHBoxLayout *blay = new QHBoxLayout;
    vlay->addLayout(blay);
    QPushButton *prev = new QPushButton(this);
    prev->setFixedSize(97, 40);
    prev->setText(tr("上一页"));
    blay->addWidget(prev);
    connect(prev, SIGNAL(clicked(bool)), this, SLOT(clickButtons()));

    page = new QLineEdit(this);
    page->setText("1");
    page->setFixedSize(50, 36);
    page->setAlignment(Qt::AlignCenter);
    blay->addWidget(page);

    QPushButton *next = new QPushButton(this);
    next->setFixedSize(97, 40);
    next->setText(tr("下一页"));
    blay->addWidget(next);
    connect(next, SIGNAL(clicked(bool)), this, SLOT(clickButtons()));

    blay->addStretch();
}

void TypConfig::initItemBar()
{
    QVBoxLayout *layout = new QVBoxLayout;
    blayout->addLayout(layout, 0, 0);

    itemNams << "电阻" << "反嵌" << "绝缘" << "交耐" << "直耐"
             << "匝间" << "电参" << "电感" << "堵转" << "低启"
             << "霍尔" << "负载" << "空载" << "BEMF"  << "缺相";

    QStringList headers;
    headers << tr("选中") << tr("测试项目");

    mView = new BoxQModel(this);
    mView->setColumnCount(headers.size());
    mView->setHorizontalHeaderLabels(headers);

    item = new QTableView(this);
    item->setModel(mView);
    layout->addWidget(item);
    item->verticalHeader()->hide();
    item->horizontalHeader()->setFixedHeight(30);
    item->horizontalHeader()->setDefaultSectionSize(52);
    item->horizontalHeader()->setStretchLastSection(true);
    item->setEditTriggers(QAbstractItemView::NoEditTriggers);
    item->setSelectionBehavior(QAbstractItemView::SelectRows);
    item->setSelectionMode(QAbstractItemView::SingleSelection);
    item->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    QPushButton *btnUpper = new QPushButton(tr("上移"), this);
    layout->addWidget(btnUpper);
    btnUpper->setFixedHeight(44);
    connect(btnUpper, SIGNAL(clicked(bool)), this, SLOT(clickMove()));

    QPushButton *btnLower = new QPushButton(tr("下移"), this);
    layout->addWidget(btnLower);
    btnLower->setFixedHeight(44);
    connect(btnLower, SIGNAL(clicked(bool)), this, SLOT(clickMove()));

    layout->addStretch();
}

void TypConfig::initConfigBar()
{
    QVBoxLayout *layout = new QVBoxLayout;
    blayout->addLayout(layout, 0, 1);

    QDir dir("./types");
    QStringList typeNames =  dir.entryList(QDir::Files);
    for (int i=0; i < typeNames.size(); i++) {
        typeNames[i].remove(".jpg");
    }
    if (typeNames.isEmpty())
        typeNames.append("None");

    layout->addWidget(new QLabel(textConf.arg(tr("电机类型")), this));

    testTypeBox = new QComboBox(this);
    testTypeBox->addItems(typeNames);
    testTypeBox->setMinimumSize(97, 35);
    testTypeBox->setView(new QListView);
    connect(testTypeBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(autoPixmap(QString)));
    layout->addWidget(testTypeBox);

    typePixmap = new QLabel(this);
    autoPixmap(typeNames.at(0));
    layout->addWidget(typePixmap);

    layout->addWidget(new QLabel(textConf.arg(tr("线夹颜色")), this));

    QGridLayout *cLayout = new QGridLayout;
    for (int i=0; i < 8; i++) {
        QPushButton *btn = new QPushButton(QString::number(i+1));
        colors.append(btn);
        cLayout->addWidget(btn, i/2, i%2);
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(selectColor()));
    }
    layout->addLayout(cLayout);

    testAutoBox = new QCheckBox(tr("感应启动"), this);
    testAutoBox->setToolTip(tr("离开光栅感应范围后自动启动测试"));
    testAutoBox->setFixedHeight(44);
    layout->addWidget(testAutoBox);

    testDrivBox = new QCheckBox(tr("外置驱动"), this);
    testDrivBox->setFixedHeight(44);
    layout->addWidget(testDrivBox);
    testDrivBox->setToolTip(tr("切换后需重启设备"));

    testKeepBox = new QCheckBox(tr("不合格中断测试"), this);
    testKeepBox->setFixedHeight(44);
    layout->addWidget(testKeepBox);
    testKeepBox->hide();

    layout->addStretch();
}

void TypConfig::initButtonBar()
{
    QHBoxLayout *blay = new QHBoxLayout;
    blayout->addLayout(blay, 1, 0, 1, 2);

    blay->addWidget(new QLabel(tr("当前型号:"), this));
    type = new QLabel(this);
    type->setFixedSize(342, 40);
    blay->addWidget(type);
    type->setStyleSheet("color:yellow;font:24pt;qproperty-alignment:AlignLeft;");
    blay->addStretch();

    QHBoxLayout *btnLayout = new QHBoxLayout;
    blayout->addLayout(btnLayout, 2, 0, 1, 2);

    QRegExp rx1;
    rx1.setPattern("^[-|0-9|A-Z|a-z|^\s]{1,50}$"); // 限制接受1至50个字符,减号、数字和英文字母
    QValidator *validator_16c = new QRegExpValidator(rx1, this);

    btnLayout->addWidget(new QLabel(tr("选中型号:"), this));
    name = new QLineEdit(this);
    name->setFixedHeight(40);
    btnLayout->addWidget(name);
    name->setValidator(validator_16c);
    connect(name, SIGNAL(textChanged(QString)), this, SLOT(change()));
    QString str;
    str += tr("切换型号:双击型号\n");
    str += tr("添加型号:选中空白型号->输入型号名称->保存\n");
    str += tr("修改型号:选中已有型号->修改型号名称->保存\n");
    str += tr("删除型号:选中已有型号->删除型号名称->保存");
    name->setToolTip(str);
    name->setToolTipDuration(60000);

    btnSave = new QPushButton(this);
    btnSave->setFixedSize(97, 40);
    btnSave->setText(tr("保存"));
    btnLayout->addWidget(btnSave);
    connect(btnSave, SIGNAL(clicked(bool)), this, SLOT(clickSave()));
}

void TypConfig::initSettings()
{
    QSqlQuery query(QSqlDatabase::database("config"));
    query.exec("select name from sqlite_master where type='table' order by name");
    tmpTyp.clear();
    while (query.next()) {
        QString t = query.value(0).toString();
        QString numb = t.mid(1, 4);
        tmpTyp.insert(numb.toInt(), t.mid(6, NAME_SIZE));
    }
    int p = page->text().toInt() - 1;    // 页码
    int s = maxRow*p;       // 起始地址
    for (int i=0; i < maxRow; i++) {
        int t = s + i;
        view->item(i, 0)->setText(QString("%1").arg(t+1, 4, 10, QChar('0')));
        view->item(i, 1)->setText(tmpTyp.value(t+1).toString());
    }

    type->setText(QString("%1").arg(tmpSet.value(DataType).toString()));

    int r = tmpSet.value(4000 + Qt::Key_0).toInt();
    QStringList testItems = tmpSet.value(r + ADDRITEM).toString().split(",");
    testTypeBox->setCurrentText(tmpSet.value(r + ADDRTYPE).toString());
    autoPixmap(tmpSet.value(r + ADDRTYPE).toString());
    QStringList wireColor = tmpSet.value(r + ADDRWIRE).toString().split(",");
    for (int t=0; t < wireColor.size(); t++)
        colors.at(t)->setStyleSheet(QString("background-color:%1").arg(wireColor.at(t)));

    testAutoBox->setChecked((tmpSet.value(r + ADDRAUTO).toInt() == 0) ? false : true);
    testDrivBox->setChecked((tmpSet.value(r + ADDRDRIV).toInt() == 0) ? false : true);

    r = tmpSet.value(1000 + Qt::Key_0).toInt();
    r += 0x10;
    QStringList userItems;
    for (int i=0; i < itemNams.size(); i++) {   // 可用项目
        int stat = tmpSet.value(r + i).toInt();
        if (stat != 0) {
            userItems.append(QString::number(i + 1));
        }
    }
    for (int i=0; i < testItems.size(); i++) {  // 按测试顺序排序
        int t = userItems.indexOf(testItems.at(i));
        if (t >= 0) {
            userItems.removeAt(t);
            userItems.insert(i, testItems.at(i));
        }
    }
    mView->setRowCount(userItems.size());
    for (int i=0; i < userItems.size(); i++) {
        int t = userItems.at(i).toInt();
        mView->setData(mView->index(i, 0), "", Qt::DisplayRole);
        mView->setData(mView->index(i, 1), itemNams.at(t-1), Qt::DisplayRole);
        mView->item(i, 0)->setCheckable(true);
        if (i < testItems.size())
            mView->item(i, 0)->setData(Qt::Checked, Qt::CheckStateRole);
        else
            mView->item(i, 0)->setData(Qt::Unchecked, Qt::CheckStateRole);
    }
    item->setFixedHeight(userItems.size() * 40 + 20);

    int back = tmpSet.value(1000 + Qt::Key_0).toInt();
    int mode = tmpSet.value(back + backMode).toInt();
    testAutoBox->setVisible((mode == 1) ? true : false);
    testDrivBox->setVisible((mode >= 2) ? true : false);
    isRemove = false;
    isInit = true;
}

void TypConfig::saveSettings()
{
    confSettings();
    int r = tmpSet.value(4000 + Qt::Key_0).toInt();
    QStringList testItems;
    for (int i=0; i < mView->rowCount(); i++) {
        int c = mView->index(i, 0).data(Qt::CheckStateRole).toInt();
        if (c != 0) {
            QString name = mView->index(i, 1).data().toString();
            testItems.append(QString::number(itemNams.indexOf(name) + 1));
        }
    }
    tmpMsg.insert(r + ADDRITEM, testItems.join(","));
    tmpMsg.insert(r + ADDRTYPE, testTypeBox->currentText());

    QStringList wireColor;
    for (int i=0; i < colors.size(); i++) {
        QPalette palette = colors.at(i)->palette();
        wireColor.append(palette.color(QPalette::Background).name());
    }
    tmpMsg.insert(r + ADDRWIRE, wireColor.join(","));
    tmpMsg.insert(r + ADDRAUTO, testAutoBox->isChecked() ? 1 : 0);
    tmpMsg.insert(r + ADDRDRIV, testDrivBox->isChecked() ? 1 : 0);

    tmpMsg.insert(Qt::Key_0, Qt::Key_Save);
    tmpMsg.insert(Qt::Key_1, "aip_config");
    emit sendAppMsg(tmpMsg);
    tmpMsg.clear();
}

void TypConfig::confSettings()
{
    QStringList sendItems;
    for (int i=0; i < mView->rowCount(); i++) {
        int c = mView->index(i, 0).data(Qt::CheckStateRole).toInt();
        if (c != 0) {
            QString name = mView->index(i, 1).data().toString();
            sendItems.append(QString::number(itemNams.indexOf(name) + 1));
        }
    }
    tmpMap.insert("test", 0);
    tmpMap.insert("type", testTypeBox->currentText());

    QStringList wireColor;
    for (int i=0; i < colors.size(); i++) {
        QPalette palette = colors.at(i)->palette();
        wireColor.append(palette.color(QPalette::Background).name());
    }
    tmpMap.insert("color", wireColor.join(","));
    tmpMap.insert("autostart", testAutoBox->isChecked() ? 1 : 0);
    tmpMap.insert("driver", testDrivBox->isChecked() ? 1 : 0);

    config.insert("Conf", tmpMap);
    config.insert("enum", Qt::Key_Save);
    emit sendAppMap(config);
    config.clear();
    tmpMap.clear();

    tmpMap.insert("Test_Item", sendItems.join(","));
    config.insert("Sys", tmpMap);
    config.insert("enum", Qt::Key_Save);
    emit sendAppMap(config);
    config.clear();
    tmpMap.clear();
}

void TypConfig::createModel()
{
    int row = view->currentRow();
    QString t_numb = view->item(row, 0)->text();
    QString t_name = name->text();
    if (t_name.isEmpty())
        return;
    if (t_numb.isEmpty()) {
        return;
    }
    for (int i=0; i < tmpTyp.keys().size(); i++) {
        QString name = tmpTyp.value(tmpTyp.keys().at(i)).toString();
        if (t_name == name) {
            QMessageBox::warning(this, tr("警告"), tr("型号已存在"), QMessageBox::Ok);
            return;
        }
    }
    QString sqlName = "config";
    QString c_numb = tr("%1").arg(tmpSet.value(DataFile).toInt(), 4, 10, QChar('0'));
    QString c_name = tmpSet.value(DataType).toString();
    c_name = tr("T%1_%2").arg(c_numb).arg(c_name);
    t_name = tr("T%1_%2").arg(t_numb).arg(t_name);
    QSqlQuery query(QSqlDatabase::database(sqlName));
    QSqlDatabase::database(sqlName).transaction();
    QString cmd = tr("create table if not exists '%1' (").arg(t_name);
    cmd += "uuid integer primary key, parm text)";
    if (!query.exec(cmd)) {
        qWarning() << t_name << query.lastError();
    }
    if (!query.exec(tr("insert into '%1' select * from '%2'").arg(t_name).arg(c_name)))
        qWarning() << "sql error:" << query.lastError() << t_name << c_name;
    QSqlDatabase::database(sqlName).commit();

    initSettings();
}

void TypConfig::selectModel()
{
    int row = view->currentRow();
    if (row < 0)
        return;
    QString t_numb = view->item(row, 0)->text();
    QString t_name = view->item(row, 1)->text();
    if (t_name.isEmpty())
        return;

    tmpMsg.insert(DataFile, t_numb);
    tmpMsg.insert(DataType, t_name);
    tmpMsg.insert(Qt::Key_0, Qt::Key_Save);
    tmpMsg.insert(Qt::Key_1, "aip_reload");
    emit sendAppMsg(tmpMsg);
    tmpMsg.clear();

    initSettings();
}

void TypConfig::removeModel()
{
    int row = view->currentRow();
    if (row < 0)
        return;
    QString t_numb = view->item(row, 0)->text();
    QString c_name = view->item(row, 1)->text();
    QString t_name = name->text();
    if (c_name == type->text()) {
        QMessageBox::warning(this, tr("警告"), tr("不能修改当前型号"), QMessageBox::Ok);
        btnSave->setEnabled(true);
        return;
    }
    c_name = tr("T%1_%2").arg(t_numb).arg(c_name);
    QString sqlName = "config";
    QSqlQuery query(QSqlDatabase::database(sqlName));
    QSqlDatabase::database(sqlName).transaction();
    if (!isRemove) {
        t_name = tr("T%1_%2").arg(t_numb).arg(t_name);
        QString cmd = tr("create table if not exists '%1' (").arg(t_name);
        cmd += "uuid integer primary key, parm text)";
        if (!query.exec(cmd)) {
            qWarning() << t_name << query.lastError();
        }
        if (!query.exec(tr("insert into '%1' select * from '%2'").arg(t_name).arg(c_name)))
            qWarning() << "sql error:" << query.lastError() << t_name << c_name;
    }
    query.exec(tr("drop table '%1'").arg(c_name));
    QSqlDatabase::database(sqlName).commit();

    initSettings();
    btnSave->setEnabled(true);
}

void TypConfig::autoPixmap(QString name)
{
    QString pixmap = QString("./types/%1.jpg").arg(name);
    typePixmap->setPixmap(QPixmap(pixmap));
}

void TypConfig::selectColor()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid())
        btn->setStyleSheet(QString("background-color:%1").arg(color.name()));
}

void TypConfig::clickMove()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn->text() == tr("上移")) {
        int r = item->currentIndex().row();
        if (r <= 0)
            return;
        int statPrev = mView->data(mView->index(r-1, 0), Qt::CheckStateRole).toInt();
        QString namePrev = mView->data(mView->index(r-1, 1), Qt::DisplayRole).toString();
        int statCurr = mView->data(mView->index(r-0, 0), Qt::CheckStateRole).toInt();
        QString nameCurr = mView->data(mView->index(r-0, 1), Qt::DisplayRole).toString();
        mView->setData(mView->index(r-1, 0), statCurr, Qt::CheckStateRole);
        mView->setData(mView->index(r-1, 1), nameCurr, Qt::DisplayRole);
        mView->setData(mView->index(r-0, 0), statPrev, Qt::CheckStateRole);
        mView->setData(mView->index(r-0, 1), namePrev, Qt::DisplayRole);
        item->setCurrentIndex(mView->index(r-1, 1));
    } else {
        int r = item->currentIndex().row();
        if (r >= mView->rowCount() - 1)
            return;
        int statNext = mView->data(mView->index(r+1, 0), Qt::CheckStateRole).toInt();
        QString nameNext = mView->data(mView->index(r+1, 1), Qt::DisplayRole).toString();
        int statCurr = mView->data(mView->index(r+0, 0), Qt::CheckStateRole).toInt();
        QString nameCurr = mView->data(mView->index(r+0, 1), Qt::DisplayRole).toString();
        mView->setData(mView->index(r+1, 0), statCurr, Qt::CheckStateRole);
        mView->setData(mView->index(r+1, 1), nameCurr, Qt::DisplayRole);
        mView->setData(mView->index(r+0, 0), statNext, Qt::CheckStateRole);
        mView->setData(mView->index(r+0, 1), nameNext, Qt::DisplayRole);
        item->setCurrentIndex(mView->index(r+1, 1));
    }
}

void TypConfig::clickSave()
{
    if (isRemove) {
        removeModel();
        isRemove = false;
        return;
    }
    btnSave->setEnabled(false);
    int row = qMax(0, view->currentRow());
    QString n = view->item(row, 1)->text();  // 点中的型号
    QString c = name->text();  // 输出的型号
    if ((!c.isEmpty() && !n.isEmpty() && n == c) || c.isEmpty()) {  // 保存型号
        saveSettings();
    }
    if ((!c.isEmpty() && !n.isEmpty() && n != c)) {  // 修改型号
        removeModel();
    }
    if ((!c.isEmpty() && n.isEmpty())) {  // 添加型号
        createModel();
        selectModel();
    }
}

void TypConfig::clickButtons()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    int p = page->text().toInt();
    if (btn->text() == tr("上一页")) {
        p--;
    }
    if (btn->text() == tr("下一页")) {
        p++;
    }
    p = (p >= 0x01) ? p : 0x01;
    p = (p <= 0xFF) ? p : 0xFF;

    page->setText(QString::number(p));
    initSettings();
}

void TypConfig::clickViewBar()
{
    int row = view->currentRow();
    if (row >= 0)
        name->setText(view->item(row, 1)->text());
}

void TypConfig::change()
{
    int row = view->currentRow();
    if (row >= 0) {
        isRemove = (!view->item(row, 1)->text().isEmpty() && name->text().isEmpty());
    }
}

void TypConfig::recvAppMsg(QTmpMap msg)
{
    switch (msg.value(Qt::Key_0).toInt()) {
    case Qt::Key_Copy:
        tmpSet = msg;
        btnSave->setEnabled(true);
        break;
    case Qt::Key_Down:
        if (msg.value(Qt::Key_1).toString() == this->objectName()) {
            recvShow();
            confSettings();
        }
        break;
    default:
        break;
    }
}

void TypConfig::recvShow()
{
    isInit = false;
    tmpMap.insert("enum", Qt::Key_View);
    tmpMap.insert("text", QString("6004 Conf"));
    emit sendAppMap(tmpMap);
    tmpMap.clear();
    initSettings();
}

void TypConfig::showEvent(QShowEvent *e)
{
    this->setFocus();
    recvShow();
    e->accept();
}
