#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <set>
#include <tuple>

using namespace std;

struct Rule {
    Rule() = default;
    Rule(char _left, const string& _right): left(_left), right(_right) {
        if (right == "_") {
            right = string();
        }
    }
    bool operator==(const Rule& another) const {
        return left == another.left && right == another.right;
    }
    bool operator<(const Rule& another) const {
        return std::tie(left, right) < std::tie(another.left, another.right);
    }
    char left;
    string right;
};

struct Configuration {
    Configuration() = default;
    Configuration(const Rule& _rule, size_t _pos, size_t _readOnStart, size_t _readNow): rule(_rule), pos(_pos),
                    readOnStart(_readOnStart), readNow(_readNow) {}
    bool operator<(const Configuration& another) const {
        return std::tie(rule, pos, readOnStart, readNow) < std::tie(another.rule, another.pos, another.readOnStart, another.readNow);
    }
    Rule rule;
    size_t pos;
    size_t readOnStart;
    size_t readNow;
};

class Algo {
public:
    Algo(const vector<Rule>& _rules): rules(_rules) {}
    bool predict(const string& word) const;
private:
    set<Configuration> Scan(const set<Configuration>& confs, char letter) const;
    void Predict(vector<set<Configuration>>& stages, size_t step) const;
    void Complete(vector<set<Configuration>>& stages, size_t step) const;
    bool isUpperCase(char letter) const {
        return ('A' <= letter) && (letter <= 'Z');
    }
    vector<Rule> rules;
};

void Algo::Predict(vector<set<Configuration>>& stages, size_t step) const {
    set<Configuration> newConfs;
    for (const auto& conf : stages[step]) {
        if (conf.pos != conf.rule.right.size() && isUpperCase(conf.rule.right[conf.pos])) {
            for (const auto& rule: rules) {
                if (rule.left == conf.rule.right[conf.pos]) {
                    newConfs.emplace(rule, 0, conf.readNow, conf.readNow);
                }
            }
        }
    }
    for (const auto& newConf: newConfs) {
        stages[step].insert(newConf);
    }
}

void Algo::Complete(vector<set<Configuration>>& stages, size_t step) const {
    set<Configuration> newConfs;
    for (const auto& conf : stages[step]) {
        if (conf.pos == conf.rule.right.size()) {
            for (const auto& possibleParent: stages[conf.readOnStart]) {
                if (possibleParent.pos == possibleParent.rule.right.size()) {
                    continue;
                }
                if (possibleParent.rule.right[possibleParent.pos] != conf.rule.left) {
                    continue;
                }
                newConfs.emplace(possibleParent.rule, possibleParent.pos + 1, possibleParent.readOnStart, step);
            }
        }
    }
    for (const auto& newConf: newConfs) {
        stages[step].insert(newConf);
    }
}

set<Configuration> Algo::Scan(const set<Configuration>& confs, char letter) const {
    set<Configuration> newConfs;
    for (const auto& conf: confs) {
        if (conf.pos == conf.rule.right.size()) {
            continue;
        }
        if (conf.rule.right[conf.pos] == letter) {
            newConfs.emplace(conf.rule, conf.pos + 1, conf.readOnStart, conf.readNow + 1);
        }
    }
    return newConfs;
}

bool Algo::predict(const string& word) const {
    vector<set<Configuration>> stages(word.size() + 1);
    size_t oldSize = stages[0].size();
    stages[0].emplace(rules[0], 0, 0, 0);
    while (oldSize != stages[0].size()) {
        oldSize = stages[0].size();
        Predict(stages, 0);
        Complete(stages, 0);
    }
    for (size_t i = 1; i <= word.size(); ++i) {
        size_t oldSize = 0;
        stages[i] = Scan(stages[i - 1], word[i - 1]);
        while (oldSize != stages[i].size()) {
            oldSize = stages[i].size();
            Predict(stages, i);
            Complete(stages, i);
        }
    }
    bool isFound = false;
    for (const auto& conf : stages.back()) {
        if (conf.rule == rules[0] && conf.pos == 1 && conf.readOnStart == 0 && conf.readNow == word.size()) {
            isFound = true;
            break;
        }
    }
    return isFound;
}

void Test() {
    for (size_t i = 1; i <= 11; ++i) {
        std::string p1 = "input";
        std::string p2 = ".txt";
        std::string p3 = "output";
        std::ifstream f(p1 + to_string(i) + p2);

        size_t n;
        f >> n;
        vector<Rule> rules;
        for (size_t i = 0; i < n; ++i) {
            char left;
            string right;
            f >> left >> right;
            rules.emplace_back(left, right);
        }
        Algo earley(rules);
        string word;
        f >> word;
        f.close();

        std::ofstream tmp("tmp.txt");
        tmp << earley.predict(word);
        tmp.close();
        std::string res1, res2;
        std::ifstream tmpIn("tmp.txt");
        std::ifstream right(p3 + to_string(i) + p2);
        tmpIn >> res1;
        right >> res2;
        tmpIn.close();
        right.close();

        if (res1 == res2) {
            std::cout << "Test " << i << " is OK." << '\n';
        }
        else {
            std::cout << "Test " << i << " is failed." << '\n';
        }
    }
}

int main(int argc, char** argv) {
    if (argc > 1) {
        Test();
        return 0;
    }
    size_t n;
    cin >> n;
    vector<Rule> rules;
    for (size_t i = 0; i < n; ++i) {
        char left;
        string right;
        cin >> left >> right;
        rules.emplace_back(left, right);
    }
    Algo algorythm(rules);
    string word;
    cin >> word;
    std::cout << algorythm.predict(word);
    return 0;
}
