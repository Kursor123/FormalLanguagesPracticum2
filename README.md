# FormalLanguagesPracticum2
В данном репозитории приведена реализация алгоритма Эрли для определения принадлежности слова контекстно-свободной грамматике.
Сначала на вход подаётся количество правил в грамматике, затем на каждой новой строке по одному правилу.
Правила подаются в виде двух слов через пробел: первое слово есть левая часть правила, второе слово - правое.
Подразумевается, что среди символов алфавита и нетерминалов грамматики не встречается символов ```$``` и ```_```.
Подразумевается, что первое правило это всегда ```$ -> S```.
Подразумевается, что правила вида ```A -> epsilon``` подаются как ```A -> _```.
Дальше на вход подаётся слово, выводимость которого требуется проверить в ввёденной грамматике.
Алгоритм выводит 1, если слово принимается, иначе 0.
Чтобы запустить тестирование программы, необходимо передать в аргументы командной строки хоть что-нибудь.

В программе определены следующие классы:
1. ```
struct Rule {
    char left;
    string right;
};
```
  Эта структура представляет собой правило грамматика в виде его левой и правой части.
2. ```
struct Configuration {
    Rule rule;
    size_t pos;
    size_t readOnStart;
    size_t readNow;
};
```
Эта структура представялет собой конфигурацию алгоритма Эрли: ```rule``` - правило, ```pos``` - позиция прочерка в нём (прочерк стоит перед символом с индексом ```pos```), ```readOnStart``` - сколько букв было прочитано при входе в поддерево, ```readNow``` - сколько букв прочитано сейчас.
3. ```
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
```

Этот класс представляет собой реализацию алгоритма Эрли. Класс конструируется из массива правил, представляющих собой грамматику, в которой нужно будет проверять выводимость слова. Публичная функция ```predict``` принимает на вход слово и возвращает ```bool```, отвечающий, выводится слово или нет. В теле функции ```predict``` используется массив множеств конфигураций, которые возникают на соответствующей стадии алгоритма. Для реализации использованы три основные функции алгоритма Эрли: ```Predict``` принимает массив множеств конфигураций и индекс множества конфигураций, для каждой конфигурации которого нужно сделать ```Predict```. ```Complete``` работает аналогично. Как ```Predict```, так и ```Complete``` принимают первый аргумент по неконстантной ссылке и изменяют его, добавляя туда новые правила. ```Scan``` принимает на вход множество конфигураций и символ, а возвращает множество конфигураций, полученных из исходного множества чтением символа в соответствующем правиле каждой конфигурации, если это возможно, при этом ```Scan``` получает аргумент по константной ссылке. Заметим, что ни одна из функций не работает как замыкание - все они делают действия лишь на основе только полученных конфигураций, а затем сливают результат с исходным множеством. Корректность алгоритма достигается за счет внешнего цикла в функции ```predict```.
