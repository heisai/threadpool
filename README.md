## ThreadPool  c++17
- 采用多线程多对列,每个线程控制一个队列，替代老的多个线程公用一个队列。
- 将任务拆分多个下发给每个线程，每个线程掌管 M(tasks) / N(threads)个任务
-  M(tasks) / N(threads)个任务 公用一个队列。减少竞争。
  

## 使用方法：

 - 初始化线程池
  
        ThreadPool pool(4); //4个thread
        std::vector<std::future<int>>vec;
 -  添加任务

        

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
        // 案例4:（静态函数）

        for(int i =0;i<20;i++)
        {
            std::function<int(int,string)> func = bind(&TestObj::taskstatic,placeholders::_1,placeholders::_2);
            auto taskfun = pool.AddTask(func,123+i,"1234567");
            vec.emplace_back(std::move(taskfun));
        }

- 获取返回值 //没有返回值 可忽略

        for(auto &&result:vec)
        {
            std::cout<<"get_data:" << result.get()<<std::endl;
        }