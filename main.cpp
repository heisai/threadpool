#include"threadpool.h"

int main()
{
    cout << "Hello World!" << endl;
    std::vector<std::future<int>>vec;
    ThreadPool pool(4);
    for(int i =0;i<20;i++)
    {

        vec.emplace_back(
                    pool.AddTask([i]{
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    return i*i;
        }));
    }
    for(auto &&result:vec)
    {
        std::cout<<"get_data:" << result.get()<<std::endl;
    }

    return 0;
}
