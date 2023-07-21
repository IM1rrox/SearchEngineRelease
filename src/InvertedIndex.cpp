#include "InvertedIndex.h"

/**
 * вспомогательная функция для нахождения встречаемости слов
 * @param text - текст для поиска слов
 * @param doc_id - номер документа для вывода
 * @return частотный словарь встераемости слов в каждом документе
 */
map<string, map<size_t, size_t>> findFreq(const string& text, const size_t& docId) {
    map<string, map<size_t, size_t>> result;
    string word;
    for (auto &letter: text) {
        if (letter != ' ' && letter != '.' &&
            letter != ',' && letter != '!') {
            word += letter;
        }
        else {
            auto iterator = result.find(word);
            if (iterator == result.end()) {
                result.insert(pair<string, map<size_t, size_t>>(word, {{docId, 1}}));
            }
            else if (iterator->second.find((size_t)docId) == iterator->second.end()) {
                iterator->second.insert(pair<size_t, size_t>((size_t)docId, 1));
            }
            else {
                iterator->second.find(docId)->second += 1;
            }
            word.clear();
        }
    }
    auto iterator = result.find(word);
    if (iterator == result.end()) {
        result.insert(pair<string, map<size_t, size_t>>(word, {{docId, 1}}));
    }
    else if (iterator->second.find((size_t)docId) == iterator->second.end()) {
        iterator->second.insert(pair<size_t, size_t>((size_t)docId, 1));
    }
    else {
        iterator->second.find(docId)->second += 1;
    }

    return result;
}

void InvertedIndex::UpdateDocumentBase(const vector<string>& inputContent) {
    docs = inputContent;
    vector<future<map<string, map<size_t, size_t>>>> threads;
    for (int i = 0; i < docs.size(); i++) {
        threads.emplace_back(async(launch::async, findFreq, docs[i], i));
    }
    for (auto& thread : threads) {
        auto s = thread.get();
        for (auto& [word, value] : s) {
            vector<Entry> result;
            for (auto& entry : value) {
                result.push_back({entry.first, entry.second});
            }
            auto iterator = freqDictionary.find(word);
            if (iterator == freqDictionary.end()){
                freqDictionary.insert(pair<string, vector<Entry>>(word, result));
            }
            else {
                for (auto& i : result) {
                    iterator->second.push_back(i);
                }
            }
        }
    }
}

vector<Entry> InvertedIndex::GetWordCount(const string &word) const{
    auto iterator = freqDictionary.find(word);
    if (iterator == freqDictionary.end()) {
        return {};
    }
    return iterator->second;
}