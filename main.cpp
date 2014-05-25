
#include "version.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include "ranknode.h"
#include "RankManager.h"
using namespace  std;

void test_node() 
{
    typedef std::vector<RankNode>  NodeList;

    NodeList    node_list;
    for (int i = 0; i < 10; i++)
    {
        NodeData  node_data;
        node_data.id = rand() % 100000;
        node_data.value = rand() % 10000;
        node_data.buf_data_len = rand() % (KEY_BUFF_MAX-1);

        int j = 0;
        for (; j < node_data.buf_data_len; j++)
        {
            node_data.buf[j] = rand() % 26 + 'a';
        }
        node_data.buf[j] = '\0';
        
        RankNode  node;
        node.set_data(node_data);
        
        node_list.push_back(node);
    }

    int rank = 1;
    for (NodeList::iterator itor = node_list.begin(); itor != node_list.end(); ++itor)
    {
        cout << "Rank: " << rank++ << " value: " << (*itor).data().value << " id:" << (*itor).data().id << endl;
    }

    sort(node_list.begin(), node_list.end(), RankNodeComparator());

    rank = 1;
    for (NodeList::iterator itor = node_list.begin(); itor != node_list.end(); ++itor)
    {
        cout << "Rank: " << rank++ << " value: " << (*itor).data().value << " id:" << (*itor).data().id 
             << " data len:" << (*itor).data().buf_data_len  << endl;
    }

}

void test_rank()
{
    RankManager  rank_mgr;

    size_t  node_count = 100000;

    typedef std::vector<KeyType>  KeysContainer;
    KeysContainer   all_keys;

    int rank = -1;
    double start, end, cost;
    start = clock();
    for (int i = 0; i < node_count; i++)
    {
        //cout << endl;
        //cout << " ========================= " << endl;

        NodeData  node_data;
        node_data.id = rand() % 5000000;
        all_keys.push_back(node_data.id);
        node_data.value = rand() % 1000000;
        node_data.buf_data_len = rand() % (KEY_BUFF_MAX - 1);

        int j = 0;
        for (; j < node_data.buf_data_len; j++)
        {
            node_data.buf[j] = rand() % 26 + 'a';
        }
        node_data.buf[j] = '\0';

        RankNode  node;
        node.set_data(node_data);

        rank_mgr.upsert_node(node);
        //rank = rank_mgr.get_rank_2(node.data().id);
        
        //cout << "Node value: " << node.data().value << " rank:" << rank << endl;

        /*rank = 1;
        NodeList  node_list;
        rank_mgr.get_top_rank(node_list, 10);
        for (NodeList::iterator itor = node_list.begin(); itor != node_list.end(); ++itor)
        {
        cout << "Rank: " << rank++ << " value: " << (*itor).data().value << " id:" << (*itor).data().id << endl;
        }*/
    }
    end = clock();
    cost = end - start;
    
    cout << endl << "================================" << endl;
    cout << "insert " << node_count << " node, cost " << cost << " clock." << cost / CLOCKS_PER_SEC << " sec." << endl;
    cout << "block count:" << rank_mgr.rank_container().size() << endl;
    cout << "rank count:" << rank_mgr.rank_count() << endl;
    int block_index = 0;
    rank = 1;
    //for (auto& block : rank_mgr.rank_container())
    //{
    //    cout << "Block: " << block_index++ << " count:" << block.rank_count() << endl;
    //    cout << "Range: [" << block.get_value_range().first << ", " << block.get_value_range().second << "]" << endl;
    //    /*for (auto& node : block.node_list())
    //    {
    //    cout << "Rank: " << rank++ << " value: " << node.data().value << " id:" << node.data().id << endl;
    //    }*/
    //}

    // search time
    double total_time = 0, max_search = 0, min_search = INT_MAX;
    for (size_t i = 0; i < node_count/10; i++)
    {
        KeyType key = all_keys[rand()%node_count];
        start = clock();
        rank = rank_mgr.get_rank_2(key);
        if (rank < 1)
        {
            cout << "get rank bug!!!" << endl;
        }
        end = clock();
        cost = end - start;
        if (cost > max_search) max_search = cost;
        if (cost < min_search) min_search = cost;
        total_time += cost;
    }
    
    cout << "Find " << node_count / 10 << " key, cost " << total_time << " min:" << min_search << " max:" << max_search << endl;
    cout << "Cache hit:" << rank_mgr.cache_hit << " cache miss:" << rank_mgr.cache_miss << endl;
}




int main()
{
    srand((unsigned int)time(NULL));
    cout << "Version:" << VERSION << endl;

    //test_node();
    test_rank();
    getchar();

    return 0;
}



