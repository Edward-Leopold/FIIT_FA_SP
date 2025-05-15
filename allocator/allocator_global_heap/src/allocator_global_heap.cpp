#include <not_implemented.h>
#include "../include/allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(logger *logger): _logger(logger)
{
    if (_logger) _logger->debug("allocator_global_heap constructor called");
}

[[nodiscard]] void *allocator_global_heap::do_allocate_sm(
    size_t size)
{
    if (size == 0) return nullptr;

    if (_logger) {
        _logger->debug("do_allocate_sm" + std::to_string(size) + " bytes");
        _logger->trace("do_allocate_sm" + std::to_string(size) + " bytes");
    }

    size_t total_size = size + sizeof(size_t);
    void* raw_ptr = ::operator new(total_size);
    if (raw_ptr == nullptr) {
        if (_logger)
            _logger->error("bad alloc error occured");
        throw std::bad_alloc();
    }
    auto meta_data = static_cast<size_t*>(raw_ptr);
    *meta_data = size;
    void* user_ptr = static_cast<void*>(meta_data + 1);

    if (_logger) {
        _logger->debug("do_allocate_sm" + std::to_string(size) + " bytes");
        _logger->trace("do_allocate_sm" + std::to_string(size) + " bytes");
    }

    return user_ptr;
}

void allocator_global_heap::do_deallocate_sm(
    void *at)
{
    if (_logger) {
        _logger->debug("do_deallocate_sm called");
        _logger->trace("do_deallocate_sm called");
    }

    if (at == nullptr) {
        if (_logger) {
            _logger->error("nullptr passed to do_deallocate_sm function");
            _logger->trace("do_deallocate_sm(nullptr)");
            _logger->debug("do_deallocate_sm(nullptr)");
        }
        return;
    }

    char* ptr = static_cast<char*>(at);
    ptr -= sizeof(size_t);
    size_t size_to_dealloc = *reinterpret_cast<size_t*>(ptr);
    void* orig = static_cast<void*>(ptr);
    ::operator delete(orig);

    if (_logger) {
        _logger->debug("do_deallocate_sm deallocated " + std::to_string(size_to_dealloc) + " bytes");
        _logger->trace("do_deallocate_sm deallocated " + std::to_string(size_to_dealloc) + " bytes");
    }
}

allocator_global_heap::~allocator_global_heap()
{
    if (_logger) _logger->debug("allocator_global_heap destructor called");
}

allocator_global_heap::allocator_global_heap(const allocator_global_heap &other) : _logger(other.get_logger())
{
    if (_logger) _logger->debug("allocator_global_heap copy constructor called");
}

allocator_global_heap &allocator_global_heap::operator=(const allocator_global_heap &other)
{
    if (_logger) _logger->debug("allocator_global_heap copy assignment operator called");

    if (&other == this) {
        if (_logger) {
            _logger->error("Same object for copy assignment operator");
            _logger->trace("allocator_global_heap copy assignment called");
            _logger->debug("allocator_global_heap copy assignment called");
        }
        return *this;
    }
    _logger = other._logger;

    if (_logger) _logger->debug("allocator_global_heap copy assignment operator finished successfully");
    return *this;
}

bool allocator_global_heap::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    if (_logger) _logger->debug("allocator_global_heap do_is_equal method called");
    return (typeid(*this) == typeid(other));
}

allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept : _logger(other._logger)
{
    if (_logger) _logger->debug("allocator_global_heap move constructor called");
    other._logger = nullptr;
}

allocator_global_heap &allocator_global_heap::operator=(allocator_global_heap &&other) noexcept
{
    if (_logger) _logger->debug("allocator_global_heap move assignment operator called");

    if (&other == this) {
        if (_logger) {
            _logger->error("Same object for move assignment operator");
            _logger->trace("allocator_global_heap move assignment called");
            _logger->debug("allocator_global_heap move assignment called");
        }
        return *this;
    }
    _logger = other._logger;
    other._logger = nullptr;

    if (_logger) _logger->debug("allocator_global_heap move assignment operator finished successfully");
    return *this;
}