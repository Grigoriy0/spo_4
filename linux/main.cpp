#include <csignal>
#include <cstring>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdio>
#include <list>
#include <pthread.h>
#include <curses.h>
#include <fcntl.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int startThread(pthread_t &thread, pthread_mutex_t &mutex);

void *threadRoutine(void *arg);

int initMutex(pthread_mutex_t &mutex);

int main(int argc, char **argv) {
    fcntl(0, F_SETFL, O_NONBLOCK);

    std::list<pthread_t> threadIds;

    pthread_mutex_t mutex;
    initMutex(mutex);

    std::cout << "If you want to create new, please press '+'" << std::endl;
    std::cout << "If you want to delete last, please press '-'" << std::endl;
    std::cout << "If you want to quit, please press 'q'" << std::endl;

    while (true) {
        switch (getchar()) {
            case '+':
                pthread_t thread;
                if (startThread(thread, mutex)) {
                    std::cout << "Error while creation thread" << std::endl;
                    exit(EXIT_FAILURE);
                }
                threadIds.push_back(thread);
                break;
            case '-':
                if (threadIds.empty()) {
                    std::cout << "There are nothing to delete!" << std::endl;
                } else {
                    pthread_cancel(threadIds.back());
                    pthread_join(threadIds.back(), nullptr);
                    threadIds.pop_back();
                }
                break;
            case 'q':
                if (!threadIds.empty()) {
                    for (auto &id : threadIds) {
                        pthread_cancel(id);
                        pthread_join(id, nullptr);
                    }
                    threadIds.clear();
                }
                pthread_mutex_destroy(&mutex);
                exit(EXIT_SUCCESS);
        }
    }
}

int startThread(pthread_t &thread, pthread_mutex_t &mutex) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    return pthread_create(&thread, &attr, threadRoutine, &mutex);
}

void *threadRoutine(void *arg) {
    auto mutex = static_cast<pthread_mutex_t *> (arg);

    while (true) {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
        pthread_mutex_lock(mutex);

        std::string str = "Thread " + std::to_string(pthread_self());
        for (auto &c:str) {
            std::cout << c;
        }
        std::cout << std::endl;

        pthread_mutex_unlock(mutex);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);

        sleep(2);
    }
}

int initMutex(pthread_mutex_t &mutex) {
    return pthread_mutex_init(&mutex, nullptr);
}