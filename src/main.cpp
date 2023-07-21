#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

#include <iostream>

#define LAST_VER 1

using namespace std;

int main() {
    ConverterJson converterJson;
    cout << "|=======Start=======|" << endl;
    cout << "Name: " << converterJson.GetName() << endl;
    if (converterJson.GetVersion() != LAST_VER) {
        cerr << "Error: Please change the app version to the latest one" << endl;
        exit(1);
    }
    cout << "Version: last" << endl;
    cout << "|======Search=======|" << endl;
    InvertedIndex index;
    index.UpdateDocumentBase(converterJson.GetTextDocuments());
    SearchServer searchServer(index);

    vector<vector<RelativeIndex>> output = searchServer.search(converterJson.GetRequests());
    vector<vector<pair<int, float>>> rev(output.size());

    for (int i = 0; i < output.size(); i++) {
        int size_rev = (output[i].size() > converterJson.GetResponsesLimit() ?
                converterJson.GetResponsesLimit() : (int)output[i].size());
        rev[i].resize(size_rev);
        for (int j = 0; j < size_rev; j++) {
            rev[i][j] = pair<int, float>(output[i][j].docId, output[i][j].rank);
        }
    }

    ConverterJson::putAnswers(rev);
    cout << "|========End========|" << endl;
}