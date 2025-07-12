#include "trie.h"
#include <QQueue>
#include <QDebug>

Trie::Trie() {
    root = new TrieNode();
    initDigitMap();
}

Trie::~Trie() {
    deleteTrie(root);
}

void Trie::initDigitMap() {
    digitMap['2'] = "abc";
    digitMap['3'] = "def";
    digitMap['4'] = "ghi";
    digitMap['5'] = "jkl";
    digitMap['6'] = "mno";
    digitMap['7'] = "pqrs";
    digitMap['8'] = "tuv";
    digitMap['9'] = "wxyz";
}

void Trie::deleteTrie(TrieNode *node) {
    for (auto child : node->children) deleteTrie(child);
    delete node;
}

void Trie::insert(const QString &word, int freq) {
    TrieNode *node = root;
    for (QChar ch : word) {
        if (!node->children.contains(ch))
            node->children[ch] = new TrieNode();
        node = node->children[ch];
        node->prefixFreq += freq;
    }
    if (node->isEnd) node->prefixFreq += (freq - node->freq); // already existed
    node->isEnd = true;
    node->freq = freq;
}

bool Trie::remove(const QString &word) {
    TrieNode *node = root;
    QVector<QPair<TrieNode*, QChar>> path;

    for (QChar ch : word) {
        if (!node->children.contains(ch)) return false;
        path.append({node, ch});
        node = node->children[ch];
    }

    if (!node->isEnd) return false;

    int oldFreq = node->freq;
    node->isEnd = false;
    node->freq = 0;

    // 更新前缀频率
    for (auto &[parent, ch] : path)
        parent->children[ch]->prefixFreq -= oldFreq;

    // 删除无用节点
    for (int i = path.size() - 1; i >= 0; --i) {
        TrieNode *parent = path[i].first;
        QChar ch = path[i].second;
        TrieNode *child = parent->children[ch];
        if (child->isEnd || !child->children.isEmpty()) break;
        delete child;
        parent->children.remove(ch);
    }

    return true;
}

bool Trie::modify(const QString &word, int newFreq) {
    if (!remove(word)||newFreq<=0) return false;
    insert(word, newFreq);
    return true;
}

int Trie::getFreq(const QString &word) {
    TrieNode *node = root;
    for (QChar ch : word) {
        if (!node->children.contains(ch)) return 0;
        node = node->children[ch];
    }
    return node->isEnd ? node->freq : 0;
}

int Trie::getPrefixFreq(const QString &prefix) {
    TrieNode *node = root;
    for (QChar ch : prefix) {
        if (!node->children.contains(ch)) return 0;
        node = node->children[ch];
    }
    return node->prefixFreq;
}

void Trie::dfs(TrieNode *node, const QString &path, QStringList &res) {
    if (node->isEnd) res.append(path + " " + QString::number(node->freq));
    for (auto ch : node->children.keys())
        dfs(node->children[ch], path + ch, res);
}

QStringList Trie::searchAll() {
    QStringList res;
    dfs(root, "", res);
    return res;
}

QStringList Trie::searchByPrefix(const QString &prefix) {
    TrieNode *node = root;
    for (QChar ch : prefix) {
        if (!node->children.contains(ch)) return {};
        node = node->children[ch];
    }
    QStringList res;
    dfs(node, prefix, res);
    return res;
}

void Trie::regexDfs(TrieNode *node, const QString &pattern, int index, QString path, QStringList &res) {
    if (!node) return;
    if (index == pattern.size()) {
        if (node->isEnd) res.append(path);
        return;
    }
    QChar ch = pattern[index];
    if (ch == '?') {
        for (auto c : node->children.keys())
            regexDfs(node->children[c], pattern, index + 1, path + c, res);
    } else if (ch == '*') {
        regexDfs(node, pattern, index + 1, path, res);
        for (auto c : node->children.keys())
            regexDfs(node->children[c], pattern, index, path + c, res);
    } else {
        if (node->children.contains(ch))
            regexDfs(node->children[ch], pattern, index + 1, path + ch, res);
    }
}

QStringList Trie::regexSearch(const QString &pattern) {
    QStringList res;
    regexDfs(root, pattern, 0, "", res);
    return res;
}

QString Trie::wordToDigits(const QString &word) {
    QString result;
    for (QChar ch : word) {
        for (auto key : digitMap.keys()) {
            if (digitMap[key].contains(ch)) {
                result += key;
                break;
            }
        }
    }
    return result;
}

QStringList Trie::searchByDigitsDetailed(const QString &digits) {
    QStringList result;
    if (digits.isEmpty()) return result;

    QQueue<QPair<TrieNode*, QString>> q;
    q.enqueue({root, ""});

    for (int i = 0; i < digits.size(); ++i) {
        QChar digit = digits[i];
        if (!digitMap.contains(digit)) {
            result.append("非法数字：" + QString(digit)+"\n请重新输入合法字符串或数字序列");
            return result;
        }

        int levelSize = q.size();
        QSet<QString> currentLevel;

        for (int j = 0; j < levelSize; ++j) {
            auto [node, path] = q.dequeue();
            for (QChar c : digitMap[digit]) {
                if (node->children.contains(c)) {
                    QString newPath = path + c;
                    q.enqueue({node->children[c], newPath});
                    currentLevel.insert(newPath);
                }
            }
        }

        for (const QString &prefix : currentLevel) {
            result.append(digits.left(i + 1) + " " + prefix);
        }
    }

    return result;
}


void Trie::clear() {
    deleteTrie(root);
    root = new TrieNode();
}
QStringList Trie::fuzzySearch(const QString &digits) {
    QMap<QString, QStringList> numMap;
    QStringList all = searchAll();
    for (const QString &entry : all) {
        QString word = entry.split(" ")[0];
        QString d = wordToDigits(word);
        numMap[d].append(word);
    }
    QStringList candidates;
    for (auto it = numMap.begin(); it != numMap.end(); ++it) {
        const QString &key = it.key();
        if (abs(key.length() - digits.length()) > 1) continue;
        int dist = 0;
        for (int i = 0; i < std::min(key.length(), digits.length()); ++i)
            if (key[i] != digits[i]) ++dist;
        dist += abs(key.length() - digits.length());
        if (dist <= 1) candidates.append(it.value());
    }
    return candidates;
}

QJsonObject Trie::toJson() {
    QJsonObject obj;
    QStringList all = searchAll();
    for (const QString &entry : all) {
        QStringList parts = entry.split(" ");
        if (parts.size() == 2)
            obj[parts[0]] = parts[1].toInt();
    }
    return obj;
}

void Trie::fromJson(const QJsonObject &obj) {
    for (auto key : obj.keys()) {
        insert(key, obj[key].toInt());
    }
}
