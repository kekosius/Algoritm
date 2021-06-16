#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;
const int INF = 1000000000; //определяем "бесконечность"


int main()
{
    cout << "Enter the starting point\n";
    int st; //вводим точку А
    cin >> st;

    cout << "Enter the end point\n";
    int end; //вводим точку В
    cin >> end;

    ifstream fin; //открываем поток чтения из файла
    ofstream fout; //открываем поток записи в файл

    char filename[100];
    cout << "Enter file name to open\n";
    cin >> filename; //вводим имя файла для открытия
    fin.open(filename);

    while (!fin.is_open()){ //если файл открыть не удалось, то вводим имя файла заново
        cout << "File Name" << filename << " doesn't exist.\nMake sure that you have specified the full path to the file\n"; // сообщить об этом
        cin >> filename;
        fin.open(filename);
    }

    int count = 0; //счётчик чисел
    int temp; //временная переменная

    while (!fin.eof())
    {
        fin >> temp; //впустую считываем числа
        count++; //считаем количество чисел
    }

    int n = (int) sqrt(count/2); //зная, что матрица квадратная, а на каждую дорогу приходится два значения (S и P), считаем размер матрицы

    int GR[n][n]; //массив, хранящий вес каждого ребра
    int S[n][n]; //массив, отдельно хранящий значения S для каждого ребра
    int P[n][n]; //массив, отдельно хранящий значения P для каждого ребра

    fin.seekg(0, ios::beg); //переходим в начало файла
    fin.clear();

    vector <vector<pair<int, int>>> list; //создаём безразмерный массив для отображения графа в виде списка

    //заполняем структуры представления графа
    for (int i = 0; i < n; ++i) {
        list.emplace_back(vector<pair<int, int>>());
        for (int j = 0; j < n; ++j) {
            fin >> S[i][j]; //считываем значение S
            fin >> P[i][j]; //считываем значение P
            GR[i][j] = S[i][j] + P[i][j]; //суммируем эти значения, получаем матрицу весов
            if (GR[i][j] != 0) list[list.size() - 1].emplace_back(make_pair(j, GR[i][j])); //параллельно заполняем список связанности
        }
    }

    fin.close(); //закрываем файл

    cout << "Enter the name of the file to record the results in\n";
    cin >> filename; //считываем название файла для записи представлений графа

    fout.open(filename, std::ofstream::out | std::ofstream::trunc); //открываем файл, полностью очищая его

    //если файл не удалось открыть или создать, вводим название ещё раз
    while (!fout.is_open()){
        cout << "Unable to create file" << filename << "\nTry again\n";
        cin >> filename;
        fout.open(filename, std::ofstream::out | std::ofstream::trunc);
    }

    fout<<"Matrix representation of a graph: \n";

    //выводим в файл матрицу весов
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j<n; j++){
            fout << setw(5) << GR[i][j];
        }
        fout << "\n";
    }

    fout << "\n";
    fout << "Graph representation as a list:\n";

    //выводим в файл список связанности
    for (int i = 0; i < list.size(); ++i) {
        fout << i+1 << ": ";
        for (auto & j : list[i]) {

            fout << "(" <<j.first+1 << "," << j.second << ") ";

        }
        fout << "\n";
    }

    fout.close(); //закрываем файл для записи

    //реализация алгоритма Дейкстры
    //вектор d[i] - массив расстояний
    //вектор p[i] - массив предков
    //логический массив used[i] - массив меток
    //заполняем массив расстояний бесконечностями, а массив предков особым невозможным значением
    vector <int> d(n+int(n/2),INF),p(n+int(n/2),-1);

    vector <bool> used(n+int(n/2));

    //приводим введённые точки А и В, учитывая то, что массив начинается с нуля
    int finish = end-1,mn,u;
    int start = st -1;

    //обозначаем расстояние до стартовой точки за ноль
    d[start] = 0;
    for (int i=0;i<n;++i){
        mn = INF, u = -1;

        //находим непосещённую вершину u, расстояние которой не равно бесконечности
        for (int j=0;j<n;++j)
            if (!used[j] && d[j] < mn)
                mn = d[j], u = j;

        //если такой вершины нет, то алгоритм останавливается
        if (u == -1)    break;

        //помечаем найденную вершину как посещённую
        used[u] = true;

        //просматриваются все рёбра, исходящие из данной вершины
        //если где-то существует путь, длиннее найденного, то заменяем его, а также сохраняем предка
        for (int j=0;j<n;++j)
            if (d[j] > d[u] + GR[u][j] && GR[u][j] > 0)
                d[j] = d[u] + GR[u][j], p[j] = u;
    }

    vector <int> v; //создаём массив пути

    int s = 0;
    int t = 0;

    //проверка того, что путь возможен
    if (p[finish] == -1)
        cout<<"Route unavailable\n";
    else{

        //выводим "стоимость" самого "дешёвого" пути
        cout << "The cost of the path from the " << st <<" to the " << end << " \t\n" << d[finish]<< " \n";

        //заполняем путь из массива предков
        for (int k = finish ; k != -1; k = p[k])
            v.push_back(k);

        //переворачиваем путь, так как он начинается с конца
        reverse(v.begin(),v.end());
        for (int i = 1; i < v.size(); ++i) {
            //зная полный путь, считаем отдельно значения S и P
            s+=S[v[i-1]][v[i]];
            t+=P[v[i-1]][v[i]];
        }

        cout << "Full path to the end: \n";

        //выводим полный путь
        for (int i=0;i<v.size();++i){
            if (i > 0)    cout<<"->";
            cout<<v[i]+1;
        }
    }

    //выводим отдельно полученные значения S и P
    cout << "\n";
    cout << "The sum of the road lengths of the path: S = "<< s <<"\n";
    cout << "The sum of taxes of the roads passed: P = " << t <<"\n";

    return 0;

}
