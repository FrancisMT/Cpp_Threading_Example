#include<iostream>
#include<thread>
#include<vector>
#include<algorithm>
#include<cassert> 

void thread_function() {
    std::cout << "thread function" << std::endl;
}

void thread_function_with_param(std::string &s) {
    std::cout << "thread function" << std::endl;
    std::cout << "message is = " << s << std::endl;

    s += " message";
}

class functor {

public:
    void operator()(){
        std::cout << "functor" << std::endl;
    }
};

template<class Container, class Function>
Function for_each_range(Container &cont, Function func) {
    return std::for_each(std::begin(cont), std::end(cont), func);
}

int main() {

    std::cout << "main thread" << std::endl;

    // Create a thread
    {
        auto thread = std::thread(&thread_function);
        thread.join();
    }

    // Detach a thread: create a Daemon process
    {
        auto daemon = std::thread(&thread_function);
        daemon.detach(); // Once detached, the thread should live that way forever.
    }

    // Thread creation with functor
    {
        auto thread = std::thread(functor());
        thread.join();
    }

    // Thread creation with lambda
    {
        // Check sugested number of thread available
        const auto MAXTHREADS = std::thread::hardware_concurrency();

        std::vector<std::thread> worker_threads;

        for(unsigned int thread_idx = 0; thread_idx < MAXTHREADS; ++thread_idx){
            worker_threads.emplace_back([thread_idx](){

                //To have a deterministic access, the code below is using mutex: lock
                std::cout << "In thread = " << thread_idx << std::endl;
            });

            /* Alternative
            
                auto thread = std::thread([thread_idx](){
                    std::cout << "In thread = " << thread_idx << std::endl;
                });
                worker_threads.push_back(std::move(thread));            
            */
        }

        // The lambda function takes its argument as a reference to a thread.
        for_each_range(worker_threads, [](std::thread & w_thread){
            assert(w_thread.joinable());
            w_thread.join();
        });
    }

    // Creat a thread with one param
    {
        std::string message_for_thread = "this is a thread";

        // In order to pass a variable by reference to a thread, use a reference wrapper. Otherwise it will be passed by value.
        auto thread = std::thread(&thread_function_with_param, std::ref(message_for_thread));
        thread.join();
        std::cout << message_for_thread << std::endl;
    }

    // Thread transfer of ownership
    {
        auto threadA = std::thread(&thread_function);
        auto threadB = std::move(threadA);

        threadB.join();
    }

    // Get Thread ID
    {
        auto thread = std::thread(&thread_function);
        std::cout << "main thread id = " << std::this_thread::get_id() << std::endl;
        std::cout << "child thread id = " << thread.get_id() << std::endl;
        thread.join();
    }

    return 0;
}