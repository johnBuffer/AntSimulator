#include <condition_variable>
#include <atomic>
#include <mutex>
#include <thread>
#include "world.hpp"
#include "distance_field_builder.hpp"


struct AsyncDistanceFieldBuilder
{
    WorldGrid&              map;
    std::atomic<bool>       update_requested;
    bool                    running;
    std::mutex              mutex;
    std::condition_variable condition;
    std::thread             worker;

    explicit
    AsyncDistanceFieldBuilder(WorldGrid& map_)
        : map(map_)
        , update_requested(false)
        , running(true)
    {
        worker = std::thread([this]{update();});
    }

    ~AsyncDistanceFieldBuilder()
    {
        update_requested = true;
        running          = false;
        condition.notify_all();
        worker.join();
    }

    void update()
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (running) {
            condition.wait(lock, [this](){return update_requested.load();});
            update_requested = false;
            DistanceFieldBuilder::computeDistance(map);
        }
    }

    void requestUpdate()
    {
        update_requested = true;
        condition.notify_all();
    }
};
