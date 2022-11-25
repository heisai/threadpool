#include <iostream>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <functional>
#include <thread>
#include <vector>
#include<future>
using namespace std;






//自定义枷锁队列。业务中不用关心枷锁，解锁。
//职责分化： 业务就负责逻辑，不用过多操心枷锁 解锁操作。
//         枷锁 解锁操作封装进 队列中。可复用。
template <typename T> class Queue {
public:
  void push(const T &item)
  {
    {
      std::unique_lock<std::mutex> lock(mtx_);
      queue_.push(item);
    }
    cond_.notify_one();
  }

  void push(T &&item) {
    {
      std::unique_lock<std::mutex> lock(mtx_);
      queue_.push(std::move(item));
    }
    cond_.notify_one();
  }

  bool pop(T &item)
  {
    std::unique_lock<std::mutex> lock(mtx_);
    cond_.wait(lock, [&]() { return !queue_.empty() || stop_; });
    if (queue_.empty())
    {
         return false;
    }
    item = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  std::size_t size() const
  {
    std::unique_lock<std::mutex> lock(mtx_);
    return queue_.size();
  }

  bool empty() const
  {
    std::unique_lock<std::mutex> lock(mtx_);
    return queue_.empty();
  }

  void stop()
  {
    {
      std::unique_lock<std::mutex> lock(mtx_);
      stop_ = true;
    }
    cond_.notify_all();
  }

private:
  std::condition_variable cond_;
  mutable std::mutex mtx_;
  std::queue<T> queue_;
  bool stop_ = false;
};


class ThreadPool {
public:
  explicit ThreadPool(size_t thread_num = std::thread::hardware_concurrency())
      : queues(thread_num), thread_num(thread_num)
    {
        //创建线程
        for(int i=0;i<thread_num;i++)
        {
            std::thread  th(&ThreadPool::Run,this,i);
            workers.emplace_back(std::move(th));
        }


   }
    // 消费者
    void Run(int id)
    {
        for(;;)
        {
            std::function<void(void)>task;
            if (!queues[id].pop(task))
            {
                break;
            }
            task();
        }
    }
    // 生产者  c++17
#if 1
   template<class F, class... Args>
    auto AddTask(F&& f, Args&&... args)
    {


        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared< std::packaged_task<return_type()> >(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                    );
        std::future<return_type> res = task->get_future();
        //产生随机数，把task 分配给对应的线程队列中
        id = rand() % thread_num;
        queues[id].push([task = std::move(task)] { (*task)(); });

        return res;
    }
#endif
    //c++11 简单实现
    // 将入参简化去掉了,将参数控制权交给 func, 参数有多少个取决于bind 绑定函数的函数 由调用者决定并处理
    // 例如： std::bind(task1,"3333333333333"，"222222","......")
    // 缺点：无返回值
    template<class Func>
    void  AddTaskSimple(Func fun)
        {
            auto task =  std::make_shared<std::packaged_task<void(void)>>(fun);
            id = rand() % thread_num;
            queues[id].push([task1 = std::move(task)] { (*task1)(); });
        }


    ~ThreadPool()
    {
        for (auto &queue : queues)
        {
            queue.stop();
        }

        for (auto &worker : workers)
        {
            worker.join();
        }
    }

private:
  std::vector<Queue<std::function<void()>>> queues;  //队列
  size_t thread_num;                                 //线程数
  std::vector<std::thread> workers;                  //线程队列
  int id;                                            //线程ID== workers索引
};




