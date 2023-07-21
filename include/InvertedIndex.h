#pragma once

#include <vector>
#include <map>
#include <string>
#include <thread>
#include <future>

using namespace std;

struct Entry {
    size_t docId, count;
    // Данный оператор необходим для проведения тестовых сценариев
    bool operator ==(const Entry& other) const {
        return (docId == other.docId &&
                count == other.count);
    }
};

class InvertedIndex {
private:
    vector<string> docs; // список содержимого документов
    map<string, vector<Entry>> freqDictionary; // частотный словарь

public:
    InvertedIndex() = default;

    /**
     * Обновить или заполнить базу документов, по которой будем совершать поиск
     * @param texts_input - содержимое документов
     */
    void UpdateDocumentBase(const vector<string>& inputContent);

    /**
     * Метод определяет количество вхождений слова word в загруженной базе документов
     * @param word - слово, частоту вхождений которого необходимо определить
     * @return возвращает подготовленный список с частотой слов
     */
    vector<Entry> GetWordCount(const string& word) const;
};
