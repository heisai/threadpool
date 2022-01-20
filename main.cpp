#include"threadpool.h"



int task(int num,const std::string &data)
{
    return num + data.size();
}

class TestObj
{
public:
    int task(int num,const std::string &data)
    {
        return num + data.size();
    }
    static int taskstatic(int num,const std::string &data)
    {
        return num + data.size();
    }
};
int main()
{
    cout << "Hello World!" << endl;
    std::vector<std::future<int>>vec;
    ThreadPool pool(4);
    // 案例1 lambda表达式：
    for(int i =0;i<20;i++)
    {

        vec.emplace_back(
                    pool.AddTask([i]{
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    return i*i;
        }));
    }
    // 案例2:（普通函数，）
    for(int i =0;i<20;i++)
    {
        auto taskfun = pool.AddTask(task,123 + i,"data");
        vec.emplace_back(std::move(taskfun));
    }

    // 案例3:（类成员函数）
    TestObj obj;
    for(int i =0;i<20;i++)
    {
        std::function<int(int,string)> func = bind(&TestObj::task,&obj,placeholders::_1,placeholders::_2);
        auto taskfun = pool.AddTask(func,123+i,"1234567");
        vec.emplace_back(std::move(taskfun));
    }
     // 案例3:（静态函数）
    for(int i =0;i<20;i++)
    {
        std::function<int(int,string)> func = bind(&TestObj::taskstatic,placeholders::_1,placeholders::_2);
        auto taskfun = pool.AddTask(func,123+i,"1234567");
        vec.emplace_back(std::move(taskfun));
    }

    for(auto &&result:vec)
    {
        std::cout<<"get_data:" << result.get()<<std::endl;
    }

    return 0;
}
