#pragma once

#include <vector>
#include <map>
#include <string>
#include <cstring>

#include "InvertedIndex.h"

using namespace std;

struct RelativeIndex{
    size_t docId;
    float rank;
    bool operator ==(const RelativeIndex& other) const {
        return (docId == other.docId && rank == other.rank);
    }
    bool operator >(const RelativeIndex& other) const {
        return (rank > other.rank);
    }
};

class SearchServer {
private:
    InvertedIndex _index;

public:
    /**
     * @param idx - в конструктор класса передаётся ссылка на класс InvertedIndex,
     * чтобы SearchServer мог узнать частоту слов встречаемых в запросе
     */
    explicit SearchServer(InvertedIndex& idx) : _index(idx){ };

    /**
     * Метод обработки поисковых запросов
     * @param queries_input - поисковые запросы взятые из файла requests.json
     * @return возвращает отсортированный список релевантных ответов для заданных запросов
     */
    vector<vector<RelativeIndex>> search(const vector<string>& queriesInput);
};
