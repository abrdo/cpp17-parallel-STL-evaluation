#ifndef SORTING
#define SORTING

#ifndef GPU
// for CPU:
#include <pstl/algorithm>
#include <pstl/numeric>
#include <pstl/execution>
#else
// for GPU:
#include <algorithm>
#include <numeric>
#include <execution>
#endif

#include <vector>
#include <chrono>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/tuple/tuple.hpp>

#include "pairedvectoriterator.h" // implemented by me and Kompi
#include "tupleit.hh"  // a boost::tuple iterator, implemented by Anthony Williams  - https://pastebin.com/LFkTHdQk  

namespace sort{

/////////// v1: sort to vector<vector>   (keyPtrs not needed)  --> to Thesis: advantege, disadvantage... /////////////////////////////
     // -- push back nem szálbiztos !! - ezért nem párhuzamosítható.
    float sort_intoVectorVector(const std::vector<int> &data, const std::vector<int> &keys, std::vector<std::vector<int>>& dataAtKeys){ // output.size() = keyN
        //--- Init ---//
        std::vector<std::pair<int,int>> data_keys(data.size());
        
        //--- Operations - time measuring starts ---//
        auto t_begin = std::chrono::high_resolution_clock::now();
        
        //  TRANSFORM the 2 vector to one --> data_keys
        std::transform(std::execution::par, keys.begin(), keys.end(), data.begin(), data_keys.begin(), [](int key, int data){ std::pair<int,int> tmp = std::make_pair(key, data); return tmp; });
        
        // PUT each data to the right key-s vector -- push back nem szálbiztos !!
        std::for_each(data_keys.begin(), data_keys.end(), [&dataAtKeys](std::pair<int,int> p){ dataAtKeys[p.second].push_back(p.first); });


        auto t_end = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_begin).count();
        return time;
    }



////////////////////// v2: sort to 2 plain vector + generateKeyPtrs vector //////////////////////////////////////////////////

    float generateKeyPtrs(const std::vector<int>& sortedKeys, std::vector<int> keyPtrs){ // keyPtrs.size() = keyN
        // custom find_first_occurance of key (or that what is grater than it. (<=)) algorithm wit binary search (we utilise that the input is sorted)    
        int keyN = keyPtrs.size();
        auto t_begin = std::chrono::high_resolution_clock::now();
        #pragma omp parallel for
        for(int key = 0; key < keyN; key++){
            auto it_lower = std::lower_bound(sortedKeys.begin(), sortedKeys.end(), key);
            keyPtrs[key] = std::distance(sortedKeys.begin(), it_lower);
        }
        auto t_end = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_begin).count();
        return time;
    }   




    //---------------- sorting algorithms with different DATASTRUCTURES ----------------------------
    //      std::pair seems is the fastest so far

    float sort_STD_PAIR(std::vector<int> &data, std::vector<int> &keys){
        //--- Init ---//
        int N = keys.size();
        std::vector<std::pair<int,int>> data_keys(N);
        
        //--- Operations - time measuring starts ---//
        auto t_begin = std::chrono::high_resolution_clock::now();
        
        //  TRANSFORM the 2 vector to one std::pair vector -- data_keys
        std::transform(std::execution::par, keys.begin(), keys.end(), data.begin(), data_keys.begin(), [](int key, int data){ std::pair<int,int> tmp = std::make_pair(key, data); return tmp; });
        
        // SORT
        std::sort(std::execution::par, data_keys.begin(), data_keys.end(), [](std::pair<int,int> a, std::pair<int,int> b){ return a.second < b.second;});
        
        // transform back
        std::transform(std::execution::par, data_keys.begin(), data_keys.end(), data.begin(), [](std::pair<int, int> d_k){ return d_k.first; });
        std::transform(std::execution::par, data_keys.begin(), data_keys.end(), keys.begin(), [](std::pair<int, int> d_k){ return d_k.second; });
        
        auto t_end = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_begin).count();
        return time;
    }
        

    float sort_HELPER_INDICES_VECTOR(std::vector<int> &data, std::vector<int> &keys){
        //--- Init ---//
        long int N = keys.size();
        std::vector<int> indices(N);
        std::iota(indices.begin(), indices.end(), 0);
        
        int* key_ptr = &keys[0];   
        
        std::vector<int> sorted_keys(N);
        std::vector<int> sorted_data(N);
        
        //--- Operations - time measuring starts ---//
        auto t_begin = std::chrono::high_resolution_clock::now();
        
        std::sort(std::execution::par, indices.begin(), indices.end(),
            [=](const int& a, const int& b){
                return key_ptr[a] < key_ptr[b];
            }
        );
        
        // might slow...
        for(long int i = 0; i<N; i++){
            sorted_keys[i] = keys[indices[i]];
            sorted_data[i] = data[indices[i]];
        }
        
        // copy
        std::copy(std::execution::par, sorted_keys.begin(), sorted_keys.end(), keys.begin());
        std::copy(std::execution::par, sorted_data.begin(), sorted_data.end(), data.begin());
        
        auto t_end = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_begin).count();
        return time;
    }


    float sort_PAIRED_VECTOR_ITERATOR(std::vector<int> &data, std::vector<int> &keys){
        // todo with my paired vector iterator
        return 0;
    }



    inline float sort_BOOSTTUPLEIT(std::vector<int> &data, std::vector<int> &keys){

        // icpc - on CPU - one warning :
        /*include/tupleit.hh(281): warning #1478: class "std::auto_ptr<boost::tuples::cons<int, boost::tuples::cons<int, boost::tuples::null_type>>>" (declared at line 87 of "/usr/include/c++/4.8.5/backward/auto_ptr.h") was declared deprecated
                std::auto_ptr<OwnedType> tupleBuf;
                                        ^
        */
        // on GPU - error
        // g++ - error in the tupleit.hh
        typedef boost::tuple<int&,int&> tup_t;
        
        auto t_begin = std::chrono::high_resolution_clock::now();
        /*
        std::sort(
            std::execution::par,
            iterators::makeTupleIterator(data.begin(), keys.begin()),
            iterators::makeTupleIterator(date.end(), keys.end()),
            [](tup_t i, tup_t j){
                return i.get<1>() < j.get<1>();
            }
        );
        */
        auto t_end = std::chrono::high_resolution_clock::now();
        
        float time = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_begin).count();
        return time;

    }




    /*   boost ziphez:
    template <typename... T>
    auto zip(T&... containers)
        -> boost::iterator_range<decltype(iterators::makeTupleIterator(std::begin(containers)...))> {
    return boost::make_iterator_range(iterators::makeTupleIterator(std::begin(containers)...),
                                        iterators::makeTupleIterator(std::end(containers)...));
    }
    */

} // namespace sort

#endif //SORTING