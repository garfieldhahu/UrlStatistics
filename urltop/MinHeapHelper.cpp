#include <cstdio>
#include "MinHeapHelper.h"


vector<pair<unsigned int, unsigned int>> MinHeapHelper::GetResult(const unsigned int input[], const int size)
{
    for(unsigned int i = 0; i < size; ++i)
    {
        if(i < capicity_)
        {
            vec_.push_back(std::make_pair(i, input[i]));
            if(i == capicity_ - 1)
                MakeHeap();
        }
        else 
        {
            if(input[i] > vec_[0].second)
            {
                //if(vec_[0].first == 235322738)
                //{
                //    printf("current i:%u, input[i]:%u, vec[0].first:%u, second:%u\n", i, input[i], vec_[0].first, vec_[0].second);
                //}
                vec_[0].first = i;
                vec_[0].second = input[i];
                SiftDown(0, capicity_);
            }
        }
    }
    return vec_;
}

vector<pair<string, unsigned int>> MinHeapHelper::GetResult2(unordered_map<string, unsigned int>& url_map)
{
    int num = 0;
    while(num != 100)
    {
        auto max = std::make_pair<string, unsigned int>("fake", 0);
        for(auto it = url_map.begin(); it != url_map.end(); ++it)
        {
            if(it->second > max.second)
            {
                max.first = it->first;
                max.second = it->second;
            }
        }
        vec2_.push_back(max);
        url_map.erase(max.first);
        ++num;
    }
    return vec2_;
}

void MinHeapHelper::MakeHeap()
{
    int cur = (capicity_ - 2) / 2;
    while(cur >= 0)
    {
        SiftDown(cur, capicity_);
        --cur;
    }
    return;
}


void MinHeapHelper::SiftDown(int start, int m)
{
    int i = start, j = 2 * i + 1;
    auto tmp = vec_[i];
    while(j < m)
    {
        if(j + 1 < m && vec_[j + 1].second < vec_[j].second)
            ++j;
        if(tmp.second <= vec_[j].second)
            break;
        else
        {
            vec_[i].swap(vec_[j]);
            i = j;
            j = 2 * i + 1;
        }
    }
    vec_[i].swap(tmp);
}

