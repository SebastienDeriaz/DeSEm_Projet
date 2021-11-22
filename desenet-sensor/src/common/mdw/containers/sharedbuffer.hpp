#pragma once
#include <cstdint>
#include <memory>
#include <cstring>

namespace hei {

    /**
     * @brief Buffer for shared data allowing multiple instances to use actually the same buffer and releases the
     *        buffer once the last user drops it. The buffer is **not resizeable!**
     *
     * The buffer can be either proxy an existing buffer or a new buffer where the data is actually allocated on the
     * heap can be created. If the buffer is allocated dynamically on the heap, the heap resources will be released
     * once the last holder of the buffer deletes the buffer object automatically. The data is shared, so if someone
     * makes changes on a buffer object, all other objects referring the same buffer do reflect these changes.
     * If you want your own copy of a buffer, you have to call the copy() method and use the SharedBuffer instance
     * returned by that method.
     */
    template <typename T>
    class SharedBuffer {
    public:
        typedef T               valueType;
        typedef T &             reference;
        typedef const T &       constReference;
        typedef std::size_t     sizeType;
        typedef std::ptrdiff_t  differenceType;
        typedef T *             pointer;
        typedef const T *       constPointer;
        typedef pointer         iterator;
        typedef constPointer    constIterator;

        /**
         * @brief Creates an empty and thus unusable shared buffer.
         */
        SharedBuffer(): data_(nullptr), length_(0) {}

        /**
         * @brief Allocates and returns a shared buffer with the given size on the heap.
         *
         * @param length    The number of elements the buffer has to be in size.
         */
        explicit SharedBuffer(sizeType length) {
            data_ = std::shared_ptr<T>(new T[length * sizeof(T)], [](pointer p){delete[] p;});

            if (data_.get() != nullptr)
            {
                length_ = length;
            }
            else
            {
                length_ = 0;
            }
        }

        /**
         * @brief Creates a shared buffer that represents the memory region passed by data and length.
         *
         * Note that it is important to understand that as long as there are instances of this buffer left, the memory at data
         * can be modified, so if you are passing a shared pointer to another method, check that the use count of the buffer is
         * 1 after the method was called in order to avoid dangling shared buffers still pointing to the memory.
         *
         * @param data      Pointer to the start of the memory region to represent by the shared buffer.
         * @param length    The actual number of elements inside the buffer.
         */
        static SharedBuffer proxy(pointer data, sizeType length) {
            SharedBuffer buffer;
            buffer.data_ = std::shared_ptr<T>(data, [](pointer){});
            buffer.length_ = length * sizeof(T);
            return buffer;
        }

        /**
         * @brief Creates a readonly shared buffer that represents the memory region passed by data and length.
         *
         * Note that it is important to understand that as long as there are instances of this buffer left, the memory at data
         * can be read, so if you are passing a shared pointer to another method, check that the use count of the buffer is
         * 1 after the method was called in order to avoid dangling shared buffers still pointing to the memory.
         *
         * @param data      Pointer to the start of the readonly memory region to represent by the shared buffer.
         * @param length    The actual number of elements inside the buffer.
         */
        static const SharedBuffer proxy(constPointer data, sizeType length) {
            SharedBuffer buffer;
            buffer.data_ = std::shared_ptr<T>(const_cast<pointer>(data), [](pointer){});
            buffer.length_ = length * sizeof(T);
            return buffer;
        }

        /**
         * @brief Creates a shared buffer by copying the data from the given memory location.
         *
         * Note that the actual buffer needed to hold the data is allocated on the heap.
         *
         * @param data      Pointer to the start of the readonly memory region to copy into the shared buffer.
         * @param length    The actual size of the memory region to copy into the buffer.
         */
        static SharedBuffer copy(constPointer data, sizeType length) {
            SharedBuffer buffer;
            buffer.data_ = std::shared_ptr<T>(new T[length * sizeof(T)], [](pointer p){delete[] p;});

            if (buffer.data_.get() != nullptr)
            {
                buffer.length_ = length * sizeof(T);
                std::memcpy(buffer.data_.get(), data, length * sizeof(T));
            }
            else
            {
                buffer.length_ = 0;
            }
            return buffer;
        }

        /**
         * @brief Returns a copy of himself by copying the data to the new shared buffer.
         *
         * Note that the actual buffer needed to hold the data is allocated on the heap.
         *
         * @return A new shared buffer as a copy.
         */
        SharedBuffer copy() const {
            SharedBuffer buffer;
            buffer.data_ = std::shared_ptr<T>(new T[length_], [](pointer p){delete[] p;});

            if (buffer.data_.get() != nullptr)
            {
                buffer.length_ = length_;
                std::memcpy(buffer.data_.get(), data_.get(), length_);
            }
            else
            {
                buffer.length_ = 0;
            }
            return buffer;
        }

        /**
         * @brief Returns an iterator to the first element of the container.
         *
         * If the container is empty, the returned iterator will be equal to end().
         *
         * @return Iterator to the first element of the container.
         */
        iterator begin() {
            return data_.get();
        }

        /**
         * @brief Returns an iterator to the element following the last element of the container.
         *
         * This element acts as a placeholder; attempting to access it results in undefined behavior.
         *
         * @return Iterator to the element following the last element of the container.
         */
        iterator end() {
            return data_.get() + length_ / sizeof(T);
        }

        /**
         * @brief Returns an iterator to the first element of the container.
         *
         * If the container is empty, the returned iterator will be equal to end().
         *
         * @return Iterator to the first element of the container.
         */
        constIterator cbegin() const {
            return data_.get();
        }

        /**
         * @brief Returns an iterator to the element following the last element of the container.
         *
         * This element acts as a placeholder; attempting to access it results in undefined behavior.
         *
         * @return Iterator to the element following the last element of the container.
         */
        constIterator cend() const {
            return data_.get() + length_ / sizeof(T);
        }

        /**
         * @brief Exchanges the contents of the container with those of other. Does not cause iterators and references to
         *        associate with the other container.
         *
         * @param other Container to exchange the contents with.
         */
        void swap(SharedBuffer<T> &other) {
            std::swap(data_, other.data_);
            std::swap(length_, other.length_);
        }

        /**
         * @brief Returns the number of elements in the container, i.e. std::distance(begin(), end()).
         *
         * @return The number of elements in the container.
         */
        sizeType size() const {
            return length_ / sizeof(T);
        }

        /**
         * @brief Returns the number of elements in the container, i.e. std::distance(begin(), end()).
         *
         * @return The number of elements in the container.
         */
        sizeType length() const {
            return length_ / sizeof(T);
        }

        /**
         * @brief Checks if the container has no elements, i.e. whether begin() == end().
         *
         * @return True if the container is empty, false otherwise.
         */
        bool empty() const {
            return !length_;
        }

        /**
         * @brief Returns a reference to the element at specified location pos. No bounds checking is performed.
         *
         * @param pos   Position of the element to return.
         * @return      Reference to the requested element.
         */
        reference operator [](sizeType pos) {
            return data_.get()[pos];
        }

        /**
         * @brief Returns a constant reference to the element at specified location pos. No bounds checking is performed.
         *
         * @param pos   Position of the element to return.
         * @return      Reference to the requested element.
         */
        const reference operator [](sizeType pos) const {
            return data_.get()[pos];
        }

        /**
         * @brief Returns pointer to the underlying array serving as element storage. The pointer is such that range
         *        [data(); data() + size()) is always a valid range, even if the container is empty.
         *
         * @return Pointer to the underlying element storage. For non-empty containers, returns &front().
         */
        pointer data() {
            return data_.get();
        }

        /**
         * @brief Returns pointer to the underlying array serving as element storage. The pointer is such that range
         *        [data(); data() + size()) is always a valid range, even if the container is empty.
         *
         * @return Pointer to the underlying element storage. For non-empty containers, returns &front().
         */
        constPointer data() const {
            return data_.get();
        }

        /**
         * @brief Returns the number of shared buffer instances that share ownership of the managed object, or ​0​ if the managed
         *        object has already been deleted, i.e. *this is empty.
         *
         * @return  The number of shared buffer instances sharing the ownership of the managed buffer.
         */
        sizeType use_count() const {
            return data_.use_count();
        }

    private:
        std::shared_ptr<T> data_;
        sizeType length_;
    };

    /**
     * @brief Specializes the std::swap algorithm for std::array. Swaps the contents of lhs and rhs. Calls lhs.swap(rhs).
     *
     * @param lhs   First container whose contents to swap.
     * @param rhs   Second container whose contents to swap.
     */
    template <typename T>
    void swap(SharedBuffer<T> &lhs, SharedBuffer<T> &rhs) {
        lhs.swap(rhs);
    }
}

