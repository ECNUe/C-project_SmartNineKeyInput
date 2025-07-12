#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "trie.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddClicked();
    void onDeleteClicked();
    void onModifyClicked();
    void onSearchByDigitClicked();
    void onSearchPrefixClicked();
    void onRegexSearchClicked();
    void onSearchAllClicked();
    void onImportClicked();
    void onExportClicked();
    void onFuzzyClicked();

private:
    Ui::MainWindow *ui;
    Trie trie;
};

#endif // MAINWINDOW_H
