#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(ui->btnModify, &QPushButton::clicked, this, &MainWindow::onModifyClicked);
    connect(ui->btnSearchDigit, &QPushButton::clicked, this, &MainWindow::onSearchByDigitClicked);
    connect(ui->btnSearchPrefix, &QPushButton::clicked, this, &MainWindow::onSearchPrefixClicked);
    connect(ui->btnRegex, &QPushButton::clicked, this, &MainWindow::onRegexSearchClicked);
    connect(ui->btnSearchAll, &QPushButton::clicked, this, &MainWindow::onSearchAllClicked);
    connect(ui->btnImport, &QPushButton::clicked, this, &MainWindow::onImportClicked);
    connect(ui->btnExport, &QPushButton::clicked, this, &MainWindow::onExportClicked);
    connect(ui->btnFuzzy, &QPushButton::clicked, this, &MainWindow::onFuzzyClicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

// 添加
void MainWindow::onAddClicked() {
    QString word = ui->lineWord->text().trimmed();
    int freq = ui->lineFreq->text().toInt();
    if (word.isEmpty() || freq <= 0) {
        ui->textOutput->setText("请输入合法的单词和次数。");
        return;
    }

    int oldFreq = trie.getFreq(word);
    if (oldFreq > 0) {
        ui->textOutput->setText(QString("单词“%1”已存在，输入次数为：%2").arg(word).arg(oldFreq));
        return;
    }

    trie.insert(word, freq);
    ui->textOutput->setText(QString("添加成功：%1 次数为 %2").arg(word).arg(freq));
}

// 删除
void MainWindow::onDeleteClicked() {
    QString word = ui->lineWord->text().trimmed();
    if (trie.remove(word)) {
        ui->textOutput->setText("删除成功：" + word);
    } else {
        ui->textOutput->setText("删除失败，单词不存在：" + word);
    }
}

// 修改
void MainWindow::onModifyClicked() {
    QString word = ui->lineWord->text().trimmed();
    int freq = ui->lineFreq->text().toInt();
    if(freq<=0){
        ui->textOutput->setText("修改失败，请输入合法字符串对应次数");
    }
    else if (trie.modify(word, freq)) {
        ui->textOutput->setText(QString("修改成功：%1 次数为 %2").arg(word).arg(freq));
    } else {
        ui->textOutput->setText("修改失败,"+word+ "不存在");
    }
}

// 九键查询
void MainWindow::onSearchByDigitClicked() {
    QString digits = ui->lineDigit->text().trimmed();
    if (digits.isEmpty()) {
        ui->textOutput->setText("请输入数字");
        return;
    }

    QStringList res = trie.searchByDigitsDetailed(digits);
    if (res.isEmpty()) {
        ui->textOutput->setText("no word");
    } else {
        ui->textOutput->setText("九键路径逐层匹配：\n" + res.join("\n"));
    }
}



// 前缀查询
void MainWindow::onSearchPrefixClicked() {
    QString prefix = ui->linePrefix->text().trimmed();
    QStringList result = trie.searchByPrefix(prefix);
    if (result.isEmpty()) {
        ui->textOutput->setText("无前缀匹配结果");
    } else {
        ui->textOutput->setText("前缀匹配结果：\n" + result.join("\n"));
    }
}

// 正则匹配
void MainWindow::onRegexSearchClicked() {
    QString pattern = ui->lineRegex->text().trimmed();
    QStringList result = trie.regexSearch(pattern);
    if (result.isEmpty()) {
        ui->textOutput->setText("无匹配结果");
    } else {
        ui->textOutput->setText("正则匹配结果：\n" + result.join("\n"));
    }
}

// 显示全部
void MainWindow::onSearchAllClicked() {
    QStringList result = trie.searchAll();
    ui->textOutput->setText("全部单词：\n" + result.join("\n"));
}

// 导入词库
void MainWindow::onImportClicked() {
    QString path = QFileDialog::getOpenFileName(this, "导入词库", "", "JSON Files (*.json)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        QMessageBox::warning(this, "错误", "格式错误");
        return;
    }

    trie.clear();                // 先清空旧词库
    trie.fromJson(doc.object());

    ui->textOutput->setText("词库导入成功");
}


// 导出词库
void MainWindow::onExportClicked() {
    QString path = QFileDialog::getSaveFileName(this, "导出词库", "", "JSON Files (*.json)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "错误", "无法写入文件");
        return;
    }

    QJsonDocument doc(trie.toJson());
    file.write(doc.toJson());
    ui->textOutput->setText("词库导出成功");
}

// 模糊匹配
void MainWindow::onFuzzyClicked() {
    QString digits = ui->lineDigit->text().trimmed();
    QStringList result = trie.fuzzySearch(digits);
    if (result.isEmpty()) {
        ui->textOutput->setText("无匹配单词");
    } else {
        ui->textOutput->setText("模糊匹配候选词：\n" + result.join("\n"));
    }
}
