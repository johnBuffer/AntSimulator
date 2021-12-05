#pragma once
#include <vector>


namespace civ
{

using ID = uint64_t;

template<typename T>
struct Ref;

template<typename T>
struct PRef;

struct Slot
{
    ID id;
    ID data_id;
};


template<typename T>
struct ObjectSlot
{
    ObjectSlot(ID id_, T* object_)
        : id(id_)
        , object(object_)
    {}

    ID id;
    T* object;
};


struct GenericProvider
{
    virtual void* get(civ::ID id) = 0;
    virtual bool  isValid(civ::ID, uint64_t validity_id) const = 0;
};


template<typename T>
struct ObjectSlotConst
{
    ObjectSlotConst(ID id_, const T* object_)
        : id(id_)
        , object(object_)
    {}

    ID    id;
    const T* object;
};


struct SlotMetadata
{
    ID rid;
    ID op_id;
};


template<typename T>
struct Vector : public GenericProvider
{
    Vector()
        : data_size(0)
        , op_count(0)
    {}
    // Data ADD / REMOVE
    template<typename... Args>
    ID                 emplace_back(Args&&... args);
    ID                 push_back(const T& obj);
    void               erase(ID id);
    template<typename TPredicate>
    void               remove_if(TPredicate&& f);
    // Data access by ID
    T&                 operator[](ID id);
    const T&           operator[](ID id) const;
    // Returns a standalone object allowing access to the underlying data
    Ref<T>             getRef(ID id);
    template<typename U>
    PRef<U>            getPRef(ID id);
    // Returns the data at a specific place in the data vector (not an ID)
    T&                 getDataAt(uint64_t i);
    // Check if the data behind the pointer is the same
    bool               isValid(ID id, ID validity) const override;
    uint64_t           getOperationID(ID id) const;
    // Returns the ith object and global_id
    ObjectSlot<T>      getSlotAt(uint64_t i);
    ObjectSlotConst<T> getSlotAt(uint64_t i) const;
    // Iterators
    typename std::vector<T>::iterator       begin();
    typename std::vector<T>::iterator       end();
    typename std::vector<T>::const_iterator begin() const;
    typename std::vector<T>::const_iterator end() const;
    // Number of objects in the provider
    [[nodiscard]]
    uint64_t size() const;

public:
    std::vector<T>            data;
    std::vector<uint64_t>     ids;
    std::vector<SlotMetadata> metadata;
    uint64_t                  data_size;
    uint64_t                  op_count;

    [[nodiscard]]
    bool          isFull() const;
    // Returns the ID of the ith element of the data provider
    [[nodiscard]]
    ID            getID(uint64_t i) const;
    // Returns the data emplacement of an ID
    [[nodiscard]]
    uint64_t      getDataID(ID id) const;
    Slot          createNewSlot();
    Slot          getFreeSlot();
    Slot          getSlot();
    SlotMetadata& getMetadataAt(ID id);
    const T&      getAt(ID id) const;
    [[nodiscard]]
    void*         get(civ::ID id) override;

    template<class U> friend struct PRef;
};

template<typename T>
template<typename ...Args>
inline uint64_t Vector<T>::emplace_back(Args&& ...args)
{
    const Slot slot = getSlot();
    new(&data[slot.data_id]) T(std::forward<Args>(args)...);
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
inline void Vector<T>::erase(ID id)
{
    // Retrieve the object position in data
    const uint64_t data_index = ids[id];
    // Check if the object has been already erased
    if (data_index >= data_size) { return; }
    // Swap the object at the end
    --data_size;
    const uint64_t last_id = metadata[data_size].rid;
    std::swap(data[data_size], data[data_index]);
    std::swap(metadata[data_size], metadata[data_index]);
    std::swap(ids[last_id], ids[id]);
    // Invalidate the operation ID
    metadata[data_size].op_id = ++op_count;
}

template<typename T>
inline T& Vector<T>::operator[](ID id)
{
    return const_cast<T&>(getAt(id));
}

template<typename T>
inline const T& Vector<T>::operator[](ID id) const
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
inline Ref<T> Vector<T>::getRef(ID id)
{
    return Ref<T>(id, this, metadata[ids[id]].op_id);
}

template<typename T>
template<typename U>
PRef<U> Vector<T>::getPRef(ID id) {
    return PRef<U>{id, *this, metadata[ids[id]].op_id};
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
inline SlotMetadata& Vector<T>::getMetadataAt(ID id)
{
    return metadata[getDataID(id)];
}

template<typename T>
inline uint64_t Vector<T>::getDataID(ID id) const
{
    return ids[id];
}

template<typename T>
inline const T& Vector<T>::getAt(ID id) const
{
    return data[getDataID(id)];
}

template<typename T>
inline bool Vector<T>::isValid(ID id, ID validity) const
{
    return validity == metadata[getDataID(id)].op_id;
}

template<typename T>
inline uint64_t Vector<T>::getOperationID(ID id) const
{
    return metadata[getDataID(id)].op_id;
}

template<typename T>
template<typename TPredicate>
void Vector<T>::remove_if(TPredicate&& f)
{
    for (uint64_t data_index{ 0 }; data_index < data_size;) {
        if (f(data[data_index])) {
            erase(metadata[data_index].rid);
        }
        else {
            ++data_index;
        }
    }
}

template<typename T>
void *Vector<T>::get(civ::ID id)
{
    return static_cast<void*>(&data[ids[id]]);
}


template<typename T>
struct Ref
{
    Ref()
        : id(0)
        , array(nullptr)
        , validity_id(0)
    {}

    Ref(ID id_, Vector<T>* a, ID vid)
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

    const T& operator*() const
    {
        return (*array)[id];
    }

    civ::ID getID() const
    {
        return id;
    }

    explicit
    operator bool() const
    {
        return array && array->isValid(id, validity_id);
    }

private:
    ID         id;
    Vector<T>* array;
    ID         validity_id;
};


template<typename T>
struct PRef
{
    using ProviderCallback = T*(*)(ID, GenericProvider*);

    PRef()
        : id(0)
        , provider_callback(nullptr)
        , provider(nullptr)
        , validity_id(0)
    {}

    template<typename U>
    PRef(ID index, Vector<U>& a, ID vid)
        : id(index)
        , provider_callback{PRef<T>::get<U>}
        , provider(&a)
        , validity_id(vid)
    {}

    template<typename U>
    PRef(const PRef<U>& other)
        : id(other.index)
        , provider_callback{PRef<T>::get<U>}
        , provider(other.provider)
        , validity_id(other.validity_id)
    {}

    template<typename U>
    static T* get(ID index, GenericProvider* provider)
    {
        return static_cast<T*>(static_cast<U*>(provider->get(index)));
    }

    T* operator->()
    {
        return provider_callback(id, provider);
    }

    T& operator*()
    {
        return *provider_callback(id, provider);
    }

    const T& operator*() const
    {
        return *provider_callback(id, provider);
    }

    civ::ID getID() const
    {
        return id;
    }

    explicit
    operator bool() const
    {
        return provider && provider->isValid(id, validity_id);
    }

private:
    ID                  id;
    ProviderCallback    provider_callback;
    GenericProvider*    provider;
    uint64_t            validity_id;

    template<class U> friend struct PRef;
    template<class U> friend struct Vector;
};

}
