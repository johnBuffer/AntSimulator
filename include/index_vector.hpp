#pragma once
#include <vector>


namespace civ
{

template<typename T>
struct Ref;


struct Slot
{
    uint64_t id;
    uint64_t data_id;
};


template<typename T>
struct ObjectSlot
{
    ObjectSlot(uint64_t id_, T* object_)
        : id(id_)
        , object(object_)
    {}

    uint64_t id;
    T* object;
};


template<typename T>
struct ObjectSlotConst
{
    ObjectSlotConst(uint64_t id_, const T* object_)
        : id(id_)
        , object(object_)
    {}

    uint64_t id;
    const T* object;
};


struct SlotMetadata
{
    uint64_t rid;
    uint64_t op_id;
};


template<typename T>
struct Vector
{
    Vector()
        : data_size(0)
        , op_count(0)
    {}
    // Data ADD / REMOVE
    template<typename... Args>
    uint64_t emplace_back(Args&&... args);
    uint64_t push_back(const T& obj);
    void erase(uint64_t id);
    // Data access by ID
    T& operator[](uint64_t id);
    const T& operator[](uint64_t id) const;
    // Returns a standalone object allowing access to the underlying data
    Ref<T> getRef(uint64_t id);
    // Returns the data at a specific place in the data vector (not an ID)
    T& getDataAt(uint64_t i);
    // Check if the data behind the pointer is the same
    bool isValid(uint64_t id, uint64_t validity) const;
    // Returns the ith object and id
    ObjectSlot<T> getSlotAt(uint64_t i);
    ObjectSlotConst<T> getSlotAt(uint64_t i) const;
    // Iterators
    typename std::vector<T>::iterator begin();
    typename std::vector<T>::iterator end();
    typename std::vector<T>::const_iterator begin() const;
    typename std::vector<T>::const_iterator end() const;
    // Number of objects in the array
    uint64_t size() const;

public:
    std::vector<T> data;
    std::vector<uint64_t> ids;
    std::vector<SlotMetadata> metadata;
    uint64_t data_size;
    uint64_t op_count;

    bool isFull() const;
    // Returns the ID of the ith element of the data array
    uint64_t getID(uint64_t i) const;
    // Returns the data emplacement of an ID
    uint64_t getDataID(uint64_t id) const;
    Slot createNewSlot();
    Slot getFreeSlot();
    Slot getSlot();
    SlotMetadata& getMetadataAt(uint64_t id);
    const T& getAt(uint64_t id) const;
};

template<typename T>
template<typename ...Args>
inline uint64_t Vector<T>::emplace_back(Args&& ...args)
{
    const Slot slot = getSlot();
    new(&data[slot.data_id]) T(args...);
    return slot.id;
}

template<typename T>
inline uint64_t Vector<T>::push_back(const T& obj)
{
    const Slot slot = getSlot();
    data[slot.data_id] = obj;
    return slot.id;
}

template<typename T>
inline void Vector<T>::erase(uint64_t id)
{
    --data_size;
    const uint64_t current_data_id = ids[id];
    const uint64_t last_obj_id = metadata[data_size].rid;
    std::swap(data[data_size], data[current_data_id]);
    std::swap(metadata[data_size], metadata[current_data_id]);
    std::swap(ids[last_obj_id], ids[id]);
    // Invalidate the operation ID
    metadata[data_size].op_id = ++op_count;
}

template<typename T>
inline T& Vector<T>::operator[](uint64_t id)
{
    return const_cast<T&>(getAt(id));
}

template<typename T>
inline const T& Vector<T>::operator[](uint64_t id) const
{
    return getAt(id);
}

template<typename T>
inline ObjectSlot<T> Vector<T>::getSlotAt(uint64_t i)
{
    return ObjectSlot<T>(metadata[i].rid, &data[i]);
}

template<typename T>
inline ObjectSlotConst<T> Vector<T>::getSlotAt(uint64_t i) const
{
    return ObjectSlotConst<T>(metadata[i].rid, &data[i]);
}

template<typename T>
inline Ref<T> Vector<T>::getRef(uint64_t id)
{
    return Ref<T>(id, this, metadata[ids[id]].op_id);
}

template<typename T>
inline T& Vector<T>::getDataAt(uint64_t i)
{
    return data[i];
}

template<typename T>
inline uint64_t Vector<T>::getID(uint64_t i) const
{
    return metadata[i].rid;
}

template<typename T>
inline uint64_t Vector<T>::size() const
{
    return data_size;
}

template<typename T>
inline typename std::vector<T>::iterator Vector<T>::begin()
{
    return data.begin();
}

template<typename T>
inline typename std::vector<T>::iterator Vector<T>::end()
{
    return data.begin() + data_size;
}

template<typename T>
inline typename std::vector<T>::const_iterator Vector<T>::begin() const
{
    return data.begin();
}

template<typename T>
inline typename std::vector<T>::const_iterator Vector<T>::end() const
{
    return data.begin() + data_size;
}

template<typename T>
inline bool Vector<T>::isFull() const
{
    return data_size == data.size();
}

template<typename T>
inline Slot Vector<T>::createNewSlot()
{
    data.emplace_back();
    ids.push_back(data_size);
    metadata.push_back({data_size, op_count++});
    return { data_size, data_size };
}

template<typename T>
inline Slot Vector<T>::getFreeSlot()
{
    const uint64_t reuse_id = metadata[data_size].rid;
    metadata[data_size].op_id = op_count++;
    return { reuse_id, data_size };
}

template<typename T>
inline Slot Vector<T>::getSlot()
{
    const Slot slot = isFull() ? createNewSlot() : getFreeSlot();
    ++data_size;
    return slot;
}

template<typename T>
inline SlotMetadata& Vector<T>::getMetadataAt(uint64_t id)
{
    return metadata[getDataID(id)];
}

template<typename T>
inline uint64_t Vector<T>::getDataID(uint64_t id) const
{
    return ids[id];
}

template<typename T>
inline const T& Vector<T>::getAt(uint64_t id) const
{
    return data[getDataID(id)];
}

template<typename T>
inline bool Vector<T>::isValid(uint64_t id, uint64_t validity) const
{
    return validity == metadata[getDataID(id)].op_id;
}


template<typename T>
struct Ref
{
    Ref()
        : id(0)
        , array(nullptr)
        , validity_id(0)
    {}

    Ref(uint64_t id_, Vector<T>* a, uint64_t vid)
        : id(id_)
        , array(a)
        , validity_id(vid)
    {}

    T* operator->()
    {
        return &(*array)[id];
    }

    T& operator*()
    {
        return (*array)[id];
    }

    operator bool() const
    {
        return array->isValid(id, validity_id) && array;
    }

private:
    uint64_t id;
    Vector<T>* array;
    uint64_t validity_id;
};

}
