#ifndef TRIE_H
#define TRIE_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QJsonObject>
#include <QJsonDocument>

struct TrieNode {
    QMap<QChar, TrieNode*> children;
    int freq = 0;
    int prefixFreq = 0;
    bool isEnd = false;
};

class Trie {
public:
    Trie();
    ~Trie();
    void insert(const QString &word, int freq);
    bool remove(const QString &word);
    bool modify(const QString &word, int newFreq);
    int getFreq(const QString &word);
    int getPrefixFreq(const QString &prefix);
    QStringList searchByDigitsDetailed(const QString &digits);
    QStringList searchByPrefix(const QString &prefix);
    QStringList searchAll();
    QStringList regexSearch(const QString &pattern);
    QStringList fuzzySearch(const QString &digits);
    QJsonObject toJson();
    void fromJson(const QJsonObject &obj);
    void clear();

private:
    TrieNode *root;
    void dfs(TrieNode *node, const QString &path, QStringList &res);
    void regexDfs(TrieNode *node, const QString &pattern, int index, QString path, QStringList &res);
    void deleteTrie(TrieNode *node);
    QString wordToDigits(const QString &word);
    QMap<QChar, QString> digitMap;
    void initDigitMap();
};

#endif // TRIE_H
