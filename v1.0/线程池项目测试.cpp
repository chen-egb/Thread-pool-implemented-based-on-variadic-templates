#include "threadpool.h"
#include <iostream>
#include <chrono>
#include <thread>

/*
有些场景，是希望能够获取线程执行任务的返回值的
举例：
1 + 。。。 + 30000的和
thread1 1 + ... + 10000
thread2 10001 + ... + 20000
...

main thread:给每一个线程分配计算的区间，并等待他们算完返回结果，合并最终的结果即可
*/
using uLong = unsigned long long;

class MyTask : public Task{
public:
    MyTask(int begin, int end)
        : begin_(begin)
        , end_(end)
    {}
    Any run(){  //run方法最终就在线程池分配的线程中去做执行
        std::cout << "tid:" << std::this_thread::get_id() << "begin!" << std::endl;
        uLong sum = 0;
        for(uLong i = begin_;i <= end_;i++){
            sum += i;
        }
        // std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << "tid:" << std::this_thread::get_id() << "end!" << std::endl;
        
        return sum;
    }
private:
    int begin_;
    int end_;
};

int main(){
    {
        ThreadPool pool;
        //用户自己设置线程池的工作模式
        pool.setMode(PoolMode::MODE_CACHED);
        //开始启动线程池
        pool.start(4);

        Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));
        Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        Result res3 = pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
        pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));

        pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
        pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));

        uLong sum1 = res1.get().cast_<uLong>(); //get返回了一个Any类型
        uLong sum2 = res2.get().cast_<uLong>();
        uLong sum3 = res3.get().cast_<uLong>();

        //Master - Slave线程模型
        //Master线程用来分解任务，然后给各个Salve线程分配任务
        //等待各个Slave线程执行完任务，返回结果
        //Master线程合并各个任务结果，输出
        std::cout << (sum1 + sum2 + sum3) << std::endl;
    }

    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());

    //
    getchar();
}