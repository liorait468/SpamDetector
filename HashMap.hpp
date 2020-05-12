// HashMap.hpp

#ifndef CPP_EX3_HASHMAP_HPP
#define CPP_EX3_HASHMAP_HPP

#define DEFAULT_SIZE 0
#define DEFAULT_CAPACITY 16
#define DEFAULT_LOWER_LOAD_FACTOR 0.25
#define DEFAULT_HIGH_LOAD_FACTOR  0.75
#define MIN_CAPACITY_SIZE 1

// -------------------------------------- includes -------------------------------------------------

#include <iostream>
#include <list>
#include <vector>
#include <utility>
#include <cassert>
#include <cmath>
#include <exception>
#include <algorithm>

// ------------------------------------------- function declaration --------------------------------

template <class KeyT, class ValueT>

/**
 * @brief a class that represents a template Hash Map
 * @tparam KeyT - the template parameter that represents the key
 * @tparam ValueT - the template parameter that represents the value
 */
class HashMap
{
    typedef typename std::list<std::pair<KeyT, ValueT>>::iterator it;
    typedef std::pair<KeyT, ValueT> pair;
    typedef std::list<std::pair<KeyT, ValueT>> listPair;
    typedef std::vector<std::pair<KeyT, ValueT>> vector;

private:
    int _size;                                    // saves the current size of the hash map
    int _capacity;                                // saves the capacity of the hash map
    std::list<std::pair<KeyT, ValueT>>* _listArr; // the array of linked lists

    const double _lowerLoadFactor = DEFAULT_LOWER_LOAD_FACTOR;
    const double _highLoadFactor  = DEFAULT_HIGH_LOAD_FACTOR;

//-----------------------------------------private functions----------------------------------------

    // Gets a key and calculates the hash code fot the key
    int _hashCode(const KeyT& key) const;

    // a private function to rehash the hash map
    void _changeSize(int newSize);

    // a private function that checks if the capacity should be decreased
    void _checkIfDecrease();

public:

    /**
     * @brief  a class that represents a const iterator that is used to iterate over the
     * items in the hash map
     */
    class const_iterator
    {
    public:

        typedef const_iterator self_type;
        typedef std::pair<KeyT, ValueT> value_type;
        typedef std::pair<KeyT, ValueT> &reference;
        typedef std::pair<KeyT, ValueT> *pointer;
        typedef std::forward_iterator_tag iterator_category;
        typedef int difference_type;

        /**
         * @brief a constructor for const_iterator class
         * @param thisMap - the HashMap object to iterate on
         * @param iterator - an iterator of the first no empty list in the array of lists
         */
        explicit const_iterator(const HashMap * thisMap, typename listPair::iterator
                                beginIterator, typename listPair::iterator endIterator, int i):
                                _obj(thisMap), _iterator(beginIterator), _endIterator(endIterator),
                                _index(i)
        {
        }

        /**
         * @brief returns the value that the iterator points at
         * @return the current pair that the iterator iterates on
         */
        std::pair<KeyT, ValueT> &operator*()
        {
            return *_iterator;
        }

        /**
         * @brief returns the value of the current pair (const)
         * @return the current pair
         */
        const std::pair<KeyT, ValueT> &operator*() const
        {
            return *_iterator;
        }

        /**
         * @brief returns the object that the iterator iterates on
         * @return the object that the iterator iterates on
         */
        std::pair<KeyT, ValueT> *operator->()
        {
            return &*_iterator;
        }

        /**
         * @brief returns the object that the iterator iterates on (const)
         * @return the object that the iterator iterates on
         */
        const std::pair<KeyT, ValueT> *operator->() const
        {
            return &*_iterator;
        }

        /**
         * @brief moves the iterator to the next object and returns the pointer to the next object
         * @return the pointer to the next object in the hash map
         */
        const_iterator &operator++()
        {
            _iterator++;
            _checkIfEnd();
            return *this;
        }

        /**
         * @brief returns the pointer to the current object and than moves the iterator to the
         *        next object in the hash map
         * @return the next object in the hash map
         */
        const_iterator operator++(int)
        {
            const_iterator temp = *this;
            _iterator++;
            _checkIfEnd();
            return temp;
        }

        /**
         * @brief Checks if two iterators are equal
         * @param other - the other iterator
         * @return true if the iterators are equal, false otherwise
         */
        bool operator==(const const_iterator &other) const
        {
            bool a = other._index    == this->_index;
            bool b = other._iterator == this->_iterator;
            return  a && b;
        }

        /**
         * @brief Checks if two iterators are not equal
         * @param other - the other iterator
         * @return true if the iterators are not equal, false otherwise
         */
        bool operator!=(const const_iterator &other) const
        {
            return !(*this == other);
        }

    private:
        const HashMap* _obj; // a pointer to the object of the HashMap
        typename listPair::iterator _iterator;    // an iterator to iterate the current list
        typename listPair::iterator _endIterator; // an iterator that saves the end of the curr list
        int _index; // the current index in the array of linked lists

        // Checks if we reached the end of the current list or the hash map
        // if we reached the end of the current list - moves to the next list. if we reached the
        // end of the hash map - changes the iterator to be the end iterator
        void _checkIfEnd()
        {
            // Checks if we reached the end of the current list
            if (_iterator == _endIterator)
            {
                // move to the next index in the array
                _index++;

                // while the current list is empty and we have not reached the end of the array
                // searches for the next no-empty list
                while ((_index < _obj->capacity()) && (_obj->_listArr[_index].empty()))
                {
                    _index++;
                }

                // ended the loop
                // Checks if we reached the end of the array
                if (_index >= _obj->capacity())
                {
                    // all fields become an "end" kind of iterator fields
                    _index = _obj->capacity() - 1;
                    _iterator = _obj->_listArr[_index].end();
                    _endIterator = _obj->_listArr[_index].end();
                }
                else
                {
                    // saves the new begin iterator (over the new list)
                    _iterator = _obj->_listArr[_index].begin();
                    // saves the end iterator over the new list
                    _endIterator = _obj->_listArr[_index].end();
                }
            }
        }
    }; // end of class const_iterator

    /**
     * @brief returns iterator to the begin of the hash map
     * @return iterator to the begin of the hash map
     */
    const_iterator begin() const
    {
        // Checks if the hash map is empty
        if (empty())
        {
            return this->end();
        }

        // Search the first no empty list in the array
        for (int i = 0; i < _capacity; i++)
        {
            if (_listArr[i].empty())
            {
                continue;
            }
            else
            {
                return const_iterator(this, _listArr[i].begin(), _listArr[i].end(), i);
            }
        }
        return const_iterator(this, _listArr[0].begin(), _listArr[0].end(), 0);
    }

    /**
     * @brief returns an iterator to the end of the hash map
     * @return an iterator to the end of the hash map
     */
    const_iterator end() const
    {
        int lastIndex = capacity() - 1;
        return const_iterator(this, _listArr[lastIndex].end(), _listArr[lastIndex].end(), lastIndex);
    }

    /**
     * @brief returns iterator to the begin of the hash map
     * @return an iterator to the begin of the hash map
     */
    const_iterator cbegin() const
    {
        //Checks if the hash map is empty
        if (empty())
        {
            return this->end();
        }

        // Search the first no empty list in the array
        for (int i = 0; i < _capacity; i++)
        {
            if (_listArr[i].empty())
            {
                continue;
            }
            else
            {
                return const_iterator(this, _listArr[i].begin(), _listArr[i].end(), i);
            }
        }
        return const_iterator(this, _listArr[0].begin(), _listArr[0].end(), 0);
    }

    /**
     * @brief returns an iterator to the end of the hash map
     * @return an iterator to the end of the hash map
     */
    const_iterator cend() const
    {
        int lastIndex = capacity() - 1;
        return const_iterator(this, _listArr[lastIndex].end(), _listArr[lastIndex].end(), lastIndex);
    }

    /**
     * @brief default constructor, initializes a hash map with default values
     */
    HashMap();

    /**
     * @brief a constructor for hash map, receives a vector of keys and a vector of values and
     *        saves them into the hash map
     * @param keys   - a vector that contains keys
     * @param values - a vector that contains values
     */
    HashMap(const std::vector<KeyT>& keys, const std::vector<ValueT>& values);

    /**
     * @brief copy constructor for hash map
     * @param other - the other map
     */
    HashMap(const HashMap& other);

    /**
     * @brief destructor for hash map
     */
    ~HashMap() noexcept;

    /**
     * @brief inserts a new pair<key, value> into the hash map
     * @param key - the key to insert
     * @param value - the value to insert
     * @return true if the pair was inserted, false otherwise
     */
    bool insert(const KeyT& key, const ValueT& value);

    /**
     * @brief checks if the key exists in the map
     * @param key - the key to check if exist
     * @return true if the key exists, false otherwise
     */
    bool containsKey(const KeyT& key) const;

    /**
     * @brief returns true if the hash map is empty
     * @return true if the hash map is empty, false otherwise
     */
    bool empty() const;

    /**
     * @brief returns the number size
     * @return - the size of the hash map (number of elements in the hash map)
     */
    int size() const;

    /**
     * @brief returns the number capacity
     * @return - the capacity of the hash map
     */
    int capacity() const;

    /**
     * @brief gets a key, checks if the key exists in the map, if yes, returns it's value
     * @param key - the key
     * @return - the value of the key
     */
    ValueT& at(const KeyT& key);

    /**
     * @brief gets a key, checks if the key exists in the map, if yes, returns it's value (const)
     * @param key - the key
     * @return - the value of the key (const)
     */
    const ValueT& at(const KeyT& key) const;

    /**
     * @brief return the load factor
     * @return the load factor
     */
    double getLoadFactor() const;

    /**
     * @brief erases the value in the given key
     * @param key - the key
     * @return - true if the erase worked, false if the erase failed
     */
    bool erase(const KeyT& key);

    /**
     * @brief copies the values of the other hash map into the current hash map
     * @param other - the other hash map
     * @return - the current hash map object
     */
    HashMap& operator=(const HashMap& other);

    /**
     * @brief gets a key and returns the index of the bucket of the key
     * @param key - the key
     * @return - the index of the bucket
     */
    int bucketIndex(const KeyT& key) const;

    /**
     * @brief gets a key and returns the size of the bucket of the key
     * @param key - the key
     * @return - the size of the bucket of the key
     */
    int bucketSize(const KeyT& key) const;

    /**
     * @brief clears the map
     */
    void clear() noexcept;

    /**
     * @brief returns the value in the key int the map (const)
     * @param key - the key
     * @return - the value of the key
     */
    const ValueT operator[](const KeyT& key) const noexcept;

    /**
     * @brief returns the value in the key int the map (const)
     * @param key- the key
     * @return - the value of the key
     */
    ValueT& operator[](const KeyT& key) noexcept;

    /**
     * @brief Checks if the current map equals other map
     * @param other - the other hash map
     * @return true if the maps are equal, false otherwise
     */
    bool operator==(const HashMap& other) const noexcept ;

    /**
     * @brief Checks if the current map not equals other map
     * @param other - the other hash map
     * @return true if the maps are not equal, false otherwise
     */
    bool operator!=(const HashMap& other) const noexcept ;
};

template <class KeyT, class ValueT>
bool HashMap<KeyT, ValueT>::operator!=(const HashMap &other) const noexcept
{
    return (!(this->operator==(other)));
}

template <class KeyT, class ValueT>
bool HashMap<KeyT, ValueT>::operator==(const HashMap &other) const noexcept
{
    // Checks if the size is different
    if (_size != other.size())
    {
        return false;
    }

    if (empty() && other.empty())
    {
        return true;
    }

    for (HashMap::const_iterator it = begin(); it != end(); it++)
    {
        KeyT currKey = it.operator*().first;

        if (this->operator[](currKey) != other.operator[](currKey))
        {
            return false;
        }
    }
    return true;
}


template <class KeyT, class ValueT>
const ValueT HashMap<KeyT, ValueT>::operator[](const KeyT &key) const noexcept
{
    // check if the key exists
    if (containsKey(key))
    {
        for (HashMap::const_iterator it = begin(); it != end(); it++)
        {
            if (it->first == key)
            {
                return it->second;
            }
        }
    }
    return ValueT();
}

template <class KeyT, class ValueT>
ValueT& HashMap<KeyT, ValueT>::operator[](const KeyT &key) noexcept
{
    // check if the key exists
    if (containsKey(key))
    {
        for (HashMap::const_iterator it = begin(); it != end(); it++)
        {
            if (it.operator*().first == key)
            {
                return it.operator*().second;
            }
        }
    }

    insert(key, ValueT());
    return this->at(key);
}


template <class KeyT, class ValueT>
void HashMap<KeyT, ValueT>::clear() noexcept
{
    if (!empty())
    {
        for (int i = 0; i < capacity(); i++)
        {
            _listArr[i].clear();
        }
    }
    _size = 0;
}

template <class KeyT, class ValueT>
int HashMap<KeyT, ValueT>::bucketSize(const KeyT &key) const
{
    // Checks if the key exists in the map
    if (!containsKey(key))
    {
        throw std::out_of_range("Out of range");
    }

    int index = bucketIndex(key); // Gets the index of the key

    int sizeOfList = _listArr[index].size();

    return sizeOfList;
}

template <class KeyT, class ValueT>
int HashMap<KeyT, ValueT>::bucketIndex(const KeyT &key) const
{
    // Checks if the key exists in the map
    if (!containsKey(key))
    {
        throw std::out_of_range("Out of range");
    }

    int hash = _hashCode(key);

    return hash;
}

template <class KeyT, class ValueT>
HashMap<KeyT, ValueT>::~HashMap() noexcept
{
    delete [] _listArr;
}

template <class KeyT, class ValueT>
bool HashMap<KeyT, ValueT>::erase(const KeyT &key)
{
    if (empty())
    {
        return false;
    }

    if (!containsKey(key))
    {
        return false;
    }

    int hash = _hashCode(key);

    for (typename listPair::iterator it = _listArr[hash].begin(); it !=_listArr[hash].end(); it++)
    {
        if (it.operator*().first == key)
        {
            _listArr[hash].erase(it);
            _size--;
            _checkIfDecrease();
            return true;
        }
    }
    return false;
}

template <class KeyT, class ValueT>
HashMap<KeyT, ValueT>::HashMap():_size(DEFAULT_SIZE), _capacity(DEFAULT_CAPACITY)
{
    try
    {
        _listArr = new std::list<std::pair<KeyT, ValueT>>[_capacity];
    }
    catch (std::bad_alloc& e)
    {
        std::cout << "Bad alloc" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Goes over the array and initializes lists with default constructor
    for (int i = 0; i < _capacity; i++)
    {
        _listArr[i] = std::list<std::pair<KeyT, ValueT>>();
    }
}

template <class KeyT, class ValueT>
HashMap<KeyT, ValueT>::HashMap(const std::vector<KeyT> &keys, const std::vector<ValueT>& values)
        :HashMap()
{
    // Checks if the size of the vectors are different, if yes, throws exception
    if (keys.size() != values.size())
    {
        throw std::invalid_argument("Invalid args");
    }

    // Inserts the pairs into the hash map
    for (int i = 0; i < (int)keys.size(); i++)
    {
        // Checks if the key exists, if yes, overrides the value
        if (containsKey(keys[i]))
        {
            // If the key exists, overrides the value of the key
            // the key will not be inserted again
            at(keys[i]) = values[i];
        }
        else
        {
            // If the key doesn't exist, inserts the key and the value into the hash map
            insert(keys[i], values[i]);

        }
    }
}

template <class KeyT, class ValueT>
HashMap<KeyT, ValueT>::HashMap(const HashMap& other):_size(other.size()), _capacity(other.capacity())
{
    try
    {
        _listArr = new std::list<std::pair<KeyT, ValueT>>[_capacity];
    }
    catch (std::bad_alloc& e)
    {
        std::cout << "Bad alloc" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Goes over the array and initializes lists with default constructor
    for (int i = 0; i < _capacity; i++)
    {
        _listArr[i] = other._listArr[i];
    }
}

template <class KeyT, class ValueT>
bool HashMap<KeyT, ValueT>::insert(const KeyT& key, const ValueT& value)
{
    // If the key already exists, returns false and doesn't insert the key
    if (containsKey(key))
    {
        return false;
    }

    if (((double) (size() + 1) / capacity()) > _highLoadFactor)
    {
        _changeSize(_capacity * 2);
    }
    int index = _hashCode(key);

    pair pairToInsert(key, value);

    _listArr[index].push_back(pairToInsert);

    _size++; // Increase the number of pairs in the hash map

    return true;
}

template <class KeyT, class ValueT>
void HashMap<KeyT, ValueT>::_checkIfDecrease()
{
    double loadFactor = getLoadFactor();

    if (_capacity >= MIN_CAPACITY_SIZE)
    {
        if (loadFactor < _lowerLoadFactor)
        {
            int newSize = _capacity / 2;
            _changeSize(newSize);
        }
    }
}

template <class KeyT, class ValueT>
bool HashMap<KeyT, ValueT>::empty() const
{
    return _size == DEFAULT_SIZE;
}

template <class KeyT, class ValueT>
void HashMap<KeyT, ValueT>::_changeSize(int newSize)
{
    auto temp = new listPair[newSize];

    // for each value calculate the new hash value
    // insert each value into temp
    for (int j = 0; j < capacity(); ++j)
    {
        for (const auto &p: _listArr[j])
        {
            int index = std::hash<KeyT>{}(p.first) & (newSize - 1);
            temp[index].push_back(p);
        }
    }
    delete[] _listArr;

   _listArr = new listPair[newSize];

    for (int i = 0; i < newSize; ++i)
    {
        _listArr[i] = temp[i];
    }
    _capacity = newSize;
    delete [] temp;

}

template <class KeyT, class ValueT>
bool HashMap<KeyT, ValueT>::containsKey(const KeyT& key) const
{
    int index = _hashCode(key); // Gets the hash value of the key

    // Checks if the list in the hash index is empty
    if (_listArr[index].size() == 0)
    {
        return false;
    }

    // Search the key in the according list (the list in the index calculated by the hash function)
    for (const auto& it  : _listArr[index])
    {
        // Checks if the key of the current pair equals key
        if (it.first == key)
        {
            return true;
        }
    }
    return false;
}

template <class KeyT, class ValueT>
int HashMap<KeyT, ValueT>::_hashCode(const KeyT& key) const
{
    int result = std::hash<KeyT>{}(key) & (_capacity - 1);
    return result;
}

template <class KeyT, class ValueT>
int HashMap<KeyT, ValueT>::size() const
{
    return _size;
}

template <class KeyT, class ValueT>
int HashMap<KeyT, ValueT>::capacity() const
{
    return _capacity;
}

template <class KeyT, class ValueT>
ValueT & HashMap<KeyT, ValueT>::at(const KeyT& key)
{
    int index = _hashCode(key);

    // Goes over the list in the hash index, searches for the key and returns the value of the key
    // when found
    for (auto it = _listArr[index].begin(); it != _listArr[index].end(); ++it)
    {
        if (it->first == key)
        {
            return it->second;
        }
    }
    throw std::invalid_argument("The key does not exist");
}


template <class KeyT, class ValueT>
const ValueT& HashMap<KeyT, ValueT>::at(const KeyT& key) const
{
    int index = _hashCode(key);

    // Goes over the list in the hash index, searches for the key and returns the value of the key
    // when found
    for (auto it = _listArr[index].begin(); it != _listArr[index].end(); ++it)
    {
        if (it->first == key)
        {
            return it->second;
        }
    }
    throw std::invalid_argument("The key does not exist");
}

template <class KeyT, class ValueT>
double HashMap<KeyT, ValueT>::getLoadFactor() const
{
    double a = (double)_size / _capacity;
    return a;
}

template <class KeyT, class ValueT>
HashMap<KeyT, ValueT>& HashMap<KeyT, ValueT>::operator=(const HashMap& other)
{
    // Check if other isn't this object
    if (this == &other)
    {
        return *this;
    }

    delete [] _listArr;

    try
    {
        _listArr = new std::list<std::pair<KeyT, ValueT>>[_capacity];
    }
    catch (std::bad_alloc& e)
    {
        std::cout << "Bad alloc" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Goes over the items in the other map and copies them into the current map
    for(pair p: other)
    {
        this->insert(p.first, p.second);
    }
    return *this;
}

#endif //CPP_EX3_HASHMAP_HPP