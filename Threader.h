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

    std::string outputFile;

    std::queue<T> queue;
    std::mutex read_lock;
    std::mutex write_lock;
    std::mutex pause_lock;

    std::condition_variable invoke;

    std::string (*function)(T);

    std::vector<std::thread> threads;

    bool inputEmpty = false;
    bool stopped = false;
    bool paused = false;

    void write(const std::string &s);
    void read();
    void exec();
    void cliInput();

};



template<class T>
Threader<T>::Threader(const std::string &inputFile, const std::string &outputFile, std::string (*execFunction)(T),
                   int threadsCount) throw(IOException) {

    this->ifs.open(inputFile, std::fstream::in);

    if (!this->ifs.good()) {
        throw IOException(inputFile);
    }

    this->outputFile = outputFile;
    this->ofs.open(this->outputFile, std::fstream::out);

    this->function = execFunction;

    std::thread readThread([this] { this->read(); });
    std::thread cliThread([this] { this->cliInput(); });

    for (int i = 0; i < threadsCount; i++) {
        this->threads.emplace_back([this] { this->exec(); });
    }

    readThread.join();
    cliThread.join();

    for (auto &i: this->threads) {
        i.join();
    }

    this->ofs.close();

}

template<class T>
void Threader<T>::write(const std::string &s) {

    this->write_lock.lock();

    if (!ofs.is_open()) {
        ofs.open(this->outputFile, std::fstream::app);
    }

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

        this->read_lock.lock();

        if (this->queue.empty()) {

            this->read_lock.unlock();
            if (this->inputEmpty) {
                break;
            } else {
                continue;
            }

        }

        T number = this->queue.front();
        this->queue.pop();

        this->read_lock.unlock();

        std::string result = this->function(number);

        write(result);

        if (this->stopped) {
            break;
        }

        if (this->paused) {
            std::unique_lock<std::mutex> locker(this->pause_lock);
            this->invoke.wait(locker);
            this->ofs.close();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

template<class T>
void Threader<T>::cliInput() {

    std::string in;

    while (!this->stopped && !(this->queue.empty() && this->inputEmpty)) {

        std::cin >> in;

        if (in == "exit") {
            this->stopped = true;
        }

        if (in == "pause") {
            this->paused = true;
        }

        if (in == "resume") {
            this->paused = false;
            invoke.notify_all();
        }

    }
}
