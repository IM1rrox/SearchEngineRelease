#include "ConverterJSON.h"

class ItemsNotFound : public exception {
    const char* what() const noexcept override {
        return "Error: some items were not found in json files";
    }
};

class JsonFilesNotFound : public exception {
    const char* what() const noexcept override {
        return "Error: some important json file(s) was(were) not found";
    }
};

class UnableToCreateFile : public exception {
    const char* what() const noexcept override {
        return "Error: unable to create file";
    }
};

class DocumentsNotFound : public exception {
    const char* what() const noexcept override {
        return "Error: some document(s) was(were) not found";
    }
};

ConverterJson::ConverterJson() {
    try {
        ifstream configFile("config.json");
        ifstream requestsFile("requests.json");
        {
            bool exit = false;
            if (!configFile.is_open()) {
                exit = true;
                ofstream configCreateFile("config.json");
                configJson["config"]["name"] = "";
                configJson["config"]["version"] = 0;
                configJson["config"]["max_responses"] = 5;
                configJson["files"] = {"your file001", "your file002"};
                configCreateFile << setw(2) << configJson;
                configCreateFile.close();
            }
            if (!requestsFile.is_open()) {
                exit = true;
                ofstream requestsCreateFile("requests.json");
                requestsJson["requests"] = {"your request001", "your request002"};
                requestsCreateFile << setw(2) << requestsJson;
                requestsCreateFile.close();
            }
            if (exit) {
                configFile.close();
                requestsFile.close();
                throw JsonFilesNotFound();
            }
        }

        {
            stringstream ss1, ss2;
            ss1 << configFile.rdbuf();
            configJson = json::parse(ss1.str());
            ss2 << requestsFile.rdbuf();
            requestsJson = json::parse(ss2.str());
            configFile.close();
            requestsFile.close();
        }

        if (!configJson["config"].count("name") || !configJson["config"].count("version")
                || !configJson["config"].count("max_responses") || !configJson.count("files")
                || !requestsJson.count("requests")) {
            throw ItemsNotFound();
        }
    }
    catch (json::parse_error& e) {
        cerr << "Error: The syntax of json documents is corrupted!" << endl;
        exit(1);
    }
    catch (exception& e) {
        cerr << e.what() << endl;
        exit(1);
    }
}

string ConverterJson::GetName() const {
    return configJson["config"]["name"];
}

int ConverterJson::GetVersion() const {
    return configJson["config"]["version"];
}

/**
 * вспомогательня функция для получения содержимого документов
 * @param fileName - название файла для чтения
 * @return содержимое файла
 */
string readFile(const string& fileName) {
    try {
        ifstream file(fileName);
        if (!file.is_open()) {
            file.close();
            throw DocumentsNotFound();
        }
        stringstream ss;
        ss << file.rdbuf();
        string s = ss.str();
        transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) {
            return tolower(ch);
        });
        return s;
    }
    catch (exception& e) {
        cerr << e.what() << endl;
        exit(1);
    }
}

vector<string> ConverterJson::GetTextDocuments() const {
    vector<string> result(configJson["files"].size());
    for (int i = 0; i < result.size(); i++) {
        result[i] = readFile(configJson["files"][i]);
    }
    return result;
}

int ConverterJson::GetResponsesLimit() const {
    return configJson["config"]["max_responses"];
}

vector<string> ConverterJson::GetRequests() const {
    vector<string> result(requestsJson["requests"].size());
    for (int i = 0; i < result.size(); i++) {
        result[i] = requestsJson["requests"][i];
        transform(result[i].begin(), result[i].end(), result[i].begin(), [](unsigned char ch) {
            return tolower(ch);
        });
    }
    return result;
}

/**
 * вспомогательная функция получения чисел (001, 002, 003...)
 * @param num - число в стандартном виде (без нулей в начале)
 * @return возвращает число с нулями в начале
 */
string getStrNum(int num) {
    string strNum = to_string(num);
    string retNum;
    for (int i = 0; i < 3-strNum.length(); i++) {
        retNum += "0";
    }
    return retNum + strNum;
}

void ConverterJson::putAnswers(const vector<vector<pair<int, float>>>& answers) {
    json answersJson;
    for (int i = 0; i < answers.size(); i++) {
        if (answers[i].empty()) {
            answersJson["answers"]["request" + getStrNum(i+1)]["result"] = false;
        }
        else {
            vector<json> relevance;
            answersJson["answers"]["request" + getStrNum(i+1)]["result"] = true;
            for (auto &j : answers[i]) relevance.push_back({{"docid", j.first}, {"rank", j.second}});
            answersJson["answers"]["request" + getStrNum(i+1)]["relevance"] = relevance;
        }
    }
    try {
        ofstream answersFile("answers.json");
        if (!answersFile.is_open()) {
            answersFile.close();
            throw UnableToCreateFile();
        }
        answersFile << setw(2) << answersJson;
        answersFile.close();
    }
    catch (exception& e) {
        cerr << e.what() << endl;
        exit(1);
    }
}