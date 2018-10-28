#include <string>
#include <utility>
#include <unordered_map>
#include <vector>

using std::pair;
using std::vector;
using std::unordered_map;
using std::string;


class MinHeapHelper
{
public:
    MinHeapHelper(int capicity):capicity_(capicity){}
    vector<pair<unsigned int, unsigned int>> GetResult(const unsigned int input[], const int size);
    vector<pair<string, unsigned int>> GetResult2(unordered_map<string, unsigned int>& url_map);

    private:
        void MakeHeap();
        void SiftDown(int start, int m); 
        const int capicity_;
        vector<pair<unsigned int, unsigned int>> vec_; //first:hash_value, second:count
        vector<pair<string, unsigned int>> vec2_; //first:string, second:count
};

