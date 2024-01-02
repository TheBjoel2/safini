#ifndef ANY_TYPE_STORAGE
#define ANY_TYPE_STORAGE

#include <vector>
#include <functional>
#include <cstdint>

class AnyTypeStorage
{
public:
    template<typename T, typename... Args>
    AnyTypeStorage(const std::in_place_type_t<T>, Args&&... args)
    {
        storageDestroyFunc = [](void* toDestroy)
        {
            //destructors can also throw sometimes...
            try
            {
                std::destroy_at(std::launder(reinterpret_cast<T*>(toDestroy)));
            }
            catch(...){}
        };

        storage.resize(sizeof(T)+alignof(T)-1);

        auto typeBeginInt = reinterpret_cast<std::uintptr_t>(storage.data())+alignof(T)-1;
        typeBeginInt/=alignof(T);
        typeBeginInt*=alignof(T);

        std::construct_at(reinterpret_cast<T*>(typeBeginInt), std::forward<Args>(args)...);

        typeBegin = reinterpret_cast<void*>(typeBeginInt);
    }

    AnyTypeStorage(const AnyTypeStorage&) = delete;
    void operator=(const AnyTypeStorage&) = delete;

    AnyTypeStorage(AnyTypeStorage&& anyTypeStorage):
        storage(std::move(anyTypeStorage.storage)),
        typeBegin(anyTypeStorage.typeBegin),
        storageDestroyFunc(std::move(anyTypeStorage.storageDestroyFunc))
    {
        anyTypeStorage.typeBegin = nullptr;
    }

    void operator=(AnyTypeStorage&&) = delete;

    template<typename T>
    T& get() noexcept
    {
        return *std::launder(reinterpret_cast<T*>(typeBegin));
    }

    template<typename T>
    const T& get() const noexcept
    {
        return *std::launder(reinterpret_cast<T*>(typeBegin));
    }

    ~AnyTypeStorage()
    {
        if(typeBegin)
            storageDestroyFunc(typeBegin);
    }

private:
    std::vector<uint8_t> storage{};
    void* typeBegin = nullptr;
    std::function<void(void*)> storageDestroyFunc{};
};

#endif // ANY_TYPE_STORAGE
