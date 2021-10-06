#include <vector>
using namespace std;


class bytestuffer{
public:
    bytestuffer(vector<bool>,vector<bool>);
    ~bytestuffer();

    vector<int> boolToInt(vector<bool>);
    vector<bool> intToBool(vector<int>);

   // vector<int> stuff(vector<bool>);
    
    //vector<bool> unstuff(vector<int>);
    //vector<int> splitbuffer(vector<int>&); //pass by ref til split af buffer




private:
    vector<bool> _flag;
    vector<bool> _etc;
    int _etcI;
    int _flagI;
};