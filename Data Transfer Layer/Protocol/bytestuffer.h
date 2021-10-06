#include <vector>
using namespace std;


class bytestuffer{
public:
    bytestuffer(vector<bool>,vector<bool>);
    ~bytestuffer();
    vector<bool> stuff(vector<bool>);
    vector<bool> unstuff(vector<bool>);



private:
    vector<bool> flag;
    vector<bool> stuff;
};