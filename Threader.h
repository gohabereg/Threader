#pragma once

#include <queue>
#include <thread>
#include <fstream>
#include <exception>

class IOException : std::exception {

public:
    IOException() = default;

    explicit IOException(const std::string &file) {
        this->filename = file;
    }

    const char* what() const throw() override {
        std::string message = "Can't open file " + this->filename;
        return message.c_str();
    }

private:
    std::string filename;

};

template<class T>
class Threader {

public:
    Threader(const std::string &inputFile, const std::string &outputFile, std::string (*execFunction)(T),
             int threadsCount = 1) throw(IOException);
    ~Threader() = default;

private:
    std::ifstream ifs;
    std::ofstream ofs;

    std::queue<T> queue;
    std::mutex read_lock;
    std::mutex write_lock;
    std::mutex exec_lock;

    std::string (*function)(T);

    std::vector<std::thread> threads;

    bool inputEmpty = false;

    void write(const std::string &s);

    void read();

    void exec();

};



template<class T>
Threader<T>::Threader(const std::string &inputFile, const std::string &outputFile, std::string (*execFunction)(T),
                   int threadsCount) throw(IOException) {

    this->ifs.open(inputFile, std::fstream::in);

    if (!this->ifs.good()) {
        throw IOException(inputFile);
    }

    this->ofs.open(outputFile, std::fstream::out);

    this->function = execFunction;

    std::thread readThread([this] { this->read(); });
    for (int i = 0; i < threadsCount; i++) {
        this->threads.emplace_back([this] { this->exec(); });
    }

    readThread.join();
    for (auto &i: this->threads) {
        i.join();
    }

    this->ofs.close();

}

template<class T>
void Threader<T>::write(const std::string &s) {

    this->write_lock.lock();
    this->ofs << s << std::endl;
    this->write_lock.unlock();

}

template<class T>
void Threader<T>::read() {

    while(ifs.good()) {

        T number;

        this->read_lock.lock();
        this->ifs >> number;
        this->queue.push(number);
        this->read_lock.unlock();

    }

    this->inputEmpty = true;
    this->ifs.close();

}

template<class T>
void Threader<T>::exec() {

    while (true) {

        this->exec_lock.lock();

        if (this->queue.empty()) {

            this->exec_lock.unlock();
            if (this->inputEmpty) {
                break;
            } else {
                continue;
            }

        }

        T number = this->queue.front();
        this->queue.pop();
        this->exec_lock.unlock();

        std::string result = this->function(number);

        write(result);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}