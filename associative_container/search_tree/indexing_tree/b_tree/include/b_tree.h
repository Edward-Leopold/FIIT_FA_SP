#ifndef MP_OS_B_TREE_H
#define MP_OS_B_TREE_H

#include <iterator>
#include <utility>
#include <boost/container/static_vector.hpp>
#include <stack>
#include <pp_allocator.h>
#include <search_tree.h>
#include <initializer_list>
#include <logger_guardant.h>

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class B_tree final : private logger_guardant, private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<const tkey, tvalue>;
    using value_type = tree_data_type_const;

private:

    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;

    // region comparators declaration

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;
    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;

    // endregion comparators declaration


    struct btree_node
    {
        boost::container::static_vector<tree_data_type, maximum_keys_in_node + 1> _keys;
        boost::container::static_vector<btree_node*, maximum_keys_in_node + 2> _pointers;
        btree_node() noexcept;
//        std::vector<tree_data_type, pp_allocator<tree_data_type>> _keys;
//        std::vector<btree_node*, pp_allocator<btree_node*>> _pointers;

//        btree_node(pp_allocator<value_type> al);
    };

    pp_allocator<value_type> _allocator;
    logger* _logger;
    btree_node* _root;
    size_t _size;

    logger* get_logger() const noexcept override;
    pp_allocator<value_type> get_allocator() const noexcept;

public:

    // region constructors declaration

    explicit B_tree(const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    explicit B_tree(pp_allocator<value_type> alloc, const compare& comp = compare(), logger *logger = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit B_tree(iterator begin, iterator end, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    // endregion constructors declaration

    // region five declaration

    B_tree(const B_tree& other);

    B_tree(B_tree&& other) noexcept;

    B_tree& operator=(const B_tree& other);

    B_tree& operator=(B_tree&& other) noexcept;

    ~B_tree() noexcept override;

    // endregion five declaration

    // region iterators declaration

    class btree_iterator;
    class btree_reverse_iterator;
    class btree_const_iterator;
    class btree_const_reverse_iterator;

    class btree_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);

    };

    class btree_const_iterator final
    {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_iterator;
        friend class btree_const_reverse_iterator;

        btree_const_iterator(const btree_iterator& it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    class btree_reverse_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_reverse_iterator;

        friend class B_tree;
        friend class btree_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        btree_reverse_iterator(const btree_iterator& it) noexcept;
        operator btree_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_const_reverse_iterator final
    {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_reverse_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_iterator;

        btree_const_reverse_iterator(const btree_reverse_iterator& it) noexcept;
        operator btree_const_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_reverse_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;
    friend class btree_reverse_iterator;
    friend class btree_const_reverse_iterator;

    // endregion iterators declaration

    // region element access declaration

    /*
     * Returns a reference to the mapped value of the element with specified key. If no such element exists, an exception of type std::out_of_range is thrown.
     */
    tvalue& at(const tkey&);
    const tvalue& at(const tkey&) const;

    /*
     * If key not exists, makes default initialization of value
     */
    tvalue& operator[](const tkey& key);
    tvalue& operator[](tkey&& key);

    // endregion element access declaration
    // region iterator begins declaration

    btree_iterator begin();
    btree_iterator end();

    btree_const_iterator begin() const;
    btree_const_iterator end() const;

    btree_const_iterator cbegin() const;
    btree_const_iterator cend() const;

    btree_reverse_iterator rbegin();
    btree_reverse_iterator rend();

    btree_const_reverse_iterator rbegin() const;
    btree_const_reverse_iterator rend() const;

    btree_const_reverse_iterator crbegin() const;
    btree_const_reverse_iterator crend() const;

    // endregion iterator begins declaration

    // region lookup declaration

    size_t size() const noexcept;
    bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */

    btree_iterator find(const tkey& key);
    btree_const_iterator find(const tkey& key) const;

    btree_iterator lower_bound(const tkey& key);
    btree_const_iterator lower_bound(const tkey& key) const;

    btree_iterator upper_bound(const tkey& key);
    btree_const_iterator upper_bound(const tkey& key) const;

    bool contains(const tkey& key) const;

    // endregion lookup declaration

    // region modifiers declaration

    void clear() noexcept;

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<btree_iterator, bool> insert(const tree_data_type& data);
    std::pair<btree_iterator, bool> insert(tree_data_type&& data);

    template <typename ...Args>
    std::pair<btree_iterator, bool> emplace(Args&&... args);

    /*
     * Updates value if key exists, delegates to emplace.
     */
    btree_iterator insert_or_assign(const tree_data_type& data);
    btree_iterator insert_or_assign(tree_data_type&& data);

    template <typename ...Args>
    btree_iterator emplace_or_assign(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    btree_iterator erase(btree_iterator pos);
    btree_iterator erase(btree_const_iterator pos);

    btree_iterator erase(btree_iterator beg, btree_iterator en);
    btree_iterator erase(btree_const_iterator beg, btree_const_iterator en);


    btree_iterator erase(const tkey& key);

    // endregion modifiers declaration
    void print_tree() const {
        print_tree(_root, 0);
    }
private:
    std::pair<size_t, bool> find_index(const tkey& key, btree_node* node) const noexcept {
        if (node == nullptr) return {0, false};

        size_t begin = 0;
        size_t end = node->_keys.size();

        while (begin < end) {
            size_t mid = begin + (end - begin) / 2;

            if (compare_keys(node->_keys[mid].first, key)) {
                begin = mid + 1;
            } else if (compare_keys(key, node->_keys[mid].first)) {
                end = mid;
            } else {
                return {mid, true};
            }
        }

        return {begin, false};
    }

    void split_node(std::stack<std::pair<btree_node**, size_t>>& path, btree_node*& node, size_t& index) {
        btree_node* current = *path.top().first;

        size_t mid_index = current->_keys.size() / 2; // center is right elem
        auto middle_pair = std::move(current->_keys[mid_index]);

        auto* new_node = _allocator.template new_object<btree_node>();

        new_node->_keys.assign(
            std::make_move_iterator(current->_keys.begin() + mid_index + 1),
            std::make_move_iterator(current->_keys.end())
        );
        current->_keys.erase(current->_keys.begin() + mid_index, current->_keys.end());

        // if not a leaf
        if (!is_terma(current)) {
            new_node->_pointers.assign(
                current->_pointers.begin() + mid_index + 1,
                current->_pointers.end()
            );
            current->_pointers.erase(current->_pointers.begin() + mid_index + 1, current->_pointers.end());
        }

        // if split root
        if (path.size() == 1) {
            auto* new_root =  _allocator.template new_object<btree_node>();
            new_root->_keys.push_back(middle_pair);
            new_root->_pointers.push_back(current);
            new_root->_pointers.push_back(new_node);
            _root = new_root;

            path.pop();
            path.push(std::make_pair(&_root, 0));
        } else {
            path.pop();

            btree_node* parent = *path.top().first;
            auto [middle_pair_index_in_parent, found] = find_index(middle_pair.first, parent);

            // print_tree(parent);
            std::cout << "parent: " << parent << std::endl;
            std::cout << "current: " << current << std::endl;
            std::cout << "new_node: " << new_node << std::endl;

            parent->_keys.insert(parent->_keys.begin() + middle_pair_index_in_parent, middle_pair);
            parent->_pointers.insert(parent->_pointers.begin() + middle_pair_index_in_parent + 1, new_node);

            if (node == current) {
                if (index == mid_index) {
                    node = parent;
                    index = middle_pair_index_in_parent;
                } else if (index > mid_index) {
                    node = new_node;
                    index -= (mid_index + 1);
                }
            }
            path.top().second = middle_pair_index_in_parent;

            // print_tree(parent);
        }
    }

    void merge_node_children(std::stack<std::pair<btree_node**, size_t>>& path, btree_node*& parent, size_t& index){
        btree_node* left = parent->_pointers[index];
        btree_node* right = parent->_pointers[index + 1];

        // 1. Переносим ключ из родителя вниз
        left->_keys.push_back(std::move(parent->_keys[index]));

        // 2. Переносим все ключи из правого потомка
        for (auto& key : right->_keys) {
            left->_keys.push_back(std::move(key));
        }

        // 3. Переносим все указатели (если не листовой узел)
        for (auto* child_ptr : right->_pointers) {
            left->_pointers.push_back(child_ptr);
        }

        // 4. Удаляем правый указатель и ключ из родителя
        parent->_keys.erase(parent->_keys.begin() + index);
        parent->_pointers.erase(parent->_pointers.begin() + index + 1);

        // 5. Освобождаем память
        _allocator.template delete_object(right);

        // 6. Если parent стал пустым и это корень — поднимаем merged узел как новый корень
        if (parent == _root && parent->_keys.empty()) {
            _allocator.template delete_object(_root);
            _root = left;
            path = std::stack<std::pair<btree_node**, size_t>>(); // очищаем путь
        }
    }

    std::pair<std::stack<std::pair<btree_node**, size_t>>, size_t>
    insert_inside(tree_data_type&& data, std::stack<std::pair<btree_node**, size_t>>& path) {
        if (_root == nullptr) {
            _root = _allocator.template new_object<btree_node>();
            _root->_keys.push_back(std::move(data));
            _size = 1;
            path.push({ &_root, 0 });
            return { std::move(path), 0 };
        }

        btree_node* current_node = *path.top().first;
        auto [index, found] = find_index(data.first, current_node);
        if (found) {
            current_node->_keys[index].second = std::move(data.second);
            return { std::move(path), index };
        }

        current_node->_keys.insert(current_node->_keys.begin() + index, std::move(data));
        _size++;
        while (!path.empty()) {
            btree_node*& node = *path.top().first;
            // std::cerr << "_keys.size() of node: " << node->_keys.size() << std::endl;
            if (node->_keys.size() <= maximum_keys_in_node) break;
            split_node(path, node, index); // need to check if current_node updates after split
        }

        return { std::move(path), index };
    }

    void borrow_from_left(btree_node* parent, size_t index) {
        btree_node* node = parent->_pointers[index];
        btree_node* left_sibling = parent->_pointers[index - 1];

        node->_keys.insert(node->_keys.begin(), std::move(parent->_keys[index - 1]));

        parent->_keys[index - 1] = std::move(left_sibling->_keys.back());
        left_sibling->_keys.pop_back();

        // if not a leaf
        if (!is_terma(node)) {
            node->_pointers.insert(node->_pointers.begin(), left_sibling->_pointers.back());
            left_sibling->_pointers.pop_back();
        }
    }

    void borrow_from_right(btree_node* parent, size_t index) {
        btree_node* node = parent->_pointers[index];
        btree_node* right_sibling = parent->_pointers[index + 1];

        node->_keys.push_back(std::move(parent->_keys[index]));

        parent->_keys[index] = std::move(right_sibling->_keys.front());
        right_sibling->_keys.erase(right_sibling->_keys.begin());

        if (!is_terma(node)) {
            node->_pointers.push_back(right_sibling->_pointers.front());
            right_sibling->_pointers.erase(right_sibling->_pointers.begin());
        }
    }

    std::pair<std::stack<std::pair<btree_node**, size_t>>, size_t>
    erase_inside(std::stack<std::pair<btree_node**, size_t>>& path, size_t& index) noexcept {
        btree_node* node = *path.top().first;

        node->_keys.erase(node->_keys.begin() + index);
        _size--;

        while (!path.empty()) {
            btree_node* current = *path.top().first;

            if (current->_keys.size() >= minimum_keys_in_node || current == _root) break;

            auto [parent_ptr, parent_index] = path.top();
            if (path.size() < 2)
                break; // root

            path.pop();
            auto [grand_parent_ptr, grand_index] = path.top();
            btree_node* parent = *grand_parent_ptr;

            if (parent_index > 0 && parent->_pointers[parent_index - 1]->_keys.size() > minimum_keys_in_node) {
                borrow_from_left(parent, parent_index);
            } else if (parent_index + 1 < parent->_pointers.size() && parent->_pointers[parent_index + 1]->_keys.size() > minimum_keys_in_node) {
                borrow_from_right(parent, parent_index);
            } else {
                merge_node_children(path, parent, parent_index);
            }

            node = *parent_ptr;
            index = parent_index;
        }

        return { std::move(path), index };
    }

    bool is_terma(const btree_node *node) const noexcept {
        return node == nullptr || node->_pointers.empty();
    }

    std::pair<std::stack<std::pair<btree_node**, size_t>>, std::pair<size_t,bool>> find_path(const tkey& key) const noexcept { // need check
        std::stack<std::pair<btree_node**, size_t>> path;
        if (_root == nullptr) return {std::move(path), {0, false}};

        btree_node** node_ptr = const_cast<btree_node**>(&_root);
        size_t index = 0;
        bool found = false;

        while (!found) {
            btree_node* current_node = *node_ptr;
            auto [i, f] = find_index(key, current_node); // find_index shows at what index passed key might be in current_node
            index = i;
            found = f;

            path.push({node_ptr, index});
            if (is_terma(current_node)) break;

            node_ptr = &current_node->_pointers[index];
        }

        return {std::move(path), {index, found}};
    }

    // new functions
    void print_tree(const btree_node* node, int depth = 0) const {
        if (!node) return;

        // std::cout << node << std::endl;
        // for (auto ind: node->_keys) {
        //     std::cout << ind.first << std::endl;
        // }
        // for (auto n: node->_pointers) {
        //     std::cout << n << std::endl;
        //     for (auto ind: n->_keys) {
        //         std::cout << "    " << ind.first << std::endl;
        //     }
        // }

        std::string indent(depth * 2, ' ');
        std::cout << indent << "Node at depth " << depth << ": ";
        for (const auto& kv : node->_keys) {
            std::cout << "(" << kv.first << ", " << kv.second << ") ";
        }
        std::cout << std::endl;

        if (!is_terma(node)) {
            for (size_t i = 0; i < node->_pointers.size(); ++i) {
                if (node->_pointers[i]) {
                    print_tree(node->_pointers[i], depth + 1);
                }
            }
        }
    }
    // / new functions
};


template<std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
        std::size_t t = 5, typename U>
B_tree(iterator begin, iterator end, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5, typename U>
B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<tkey, tvalue, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_pairs(const B_tree::tree_data_type &lhs,
                                                     const B_tree::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_node::btree_node() noexcept: _keys(), _pointers()
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger* B_tree<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return _logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
pp_allocator<typename B_tree<tkey, tvalue, compare, t>::value_type> B_tree<tkey, tvalue, compare, t>::get_allocator() const noexcept
{
    return _allocator;
}

// region constructors implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger): _root(nullptr), _size(0), _allocator(alloc), _logger(logger), compare(cmp)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        pp_allocator<value_type> alloc,\
        const compare& comp,
        logger* logger): _root(nullptr), _size(0), _allocator(alloc), _logger(logger), compare(comp)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<input_iterator_for_pair<tkey, tvalue> iterator>
B_tree<tkey, tvalue, compare, t>::B_tree(
        iterator begin,
        iterator end,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger): _root(nullptr), _size(0), _allocator(alloc), _logger(logger), compare(cmp)
{
    iterator temp = begin;
    while (temp != end) {
        insert(*temp);
        ++temp;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger): _root(nullptr), _size(0), _allocator(alloc), _logger(logger), compare(cmp)
{
        for (const auto& temp: data) {
            emplace(temp.first, temp.second);
        }
}

// endregion constructors implementation

// region five implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::~B_tree() noexcept
{
    clear();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const B_tree& other):
    _allocator(other._allocator),
    _logger(other._logger),
    _root(nullptr),
    _size(0)
{
    if (other._root) {
        _root = copy_recursive(other._root);
        _size = other._size;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(const B_tree& other)
{
    if (this != &other) {
        clear();
        _allocator = other._allocator;
        _logger = other._logger;
        if (other._root) {
            _root = copy_recursive(other._root);
            _size = other._size;
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(B_tree&& other) noexcept:
    _root(other._root),
    _allocator(std::move(other._allocator)),
    _size(other._size),
    _logger(other._logger)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(B_tree&& other) noexcept
{
    if (this != &other) {
        clear();
        _root = other._root;
        _allocator = std::move(other._allocator);
        _size = other._size;
        _logger = other._logger;
    }
    return *this;
}

// endregion five implementation



// region iterators implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_iterator::btree_iterator(
    const std::stack<std::pair<btree_node**, size_t>>& path,
    size_t index
    ): _path(path), _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept
{
    const auto& top_ref = _path.top();
    return (*top_ref.first)->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept
{
    if (_path.empty()) return nullptr;
    return &((*(_path.top().first))->_keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++()
{
    if (_path.empty()) return *this;

    if (is_terminate_node()) {
        _index++;
        while (!_path.empty() && _index == ((*(_path.top().first))->_keys).size()) {
            _index = _path.top().second;
            _path.pop();
        }
    } else {
        _path.push({&(*(_path.top().first))->_pointers[_index + 1], 0});
        _index = 0;
        while (!is_terminate_node()) {
            _path.push({&(*(_path.top().first))->_pointers[0], 0});
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int)
{
    auto temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--()
{
    if (_path.empty()) return *this;

    if (is_terminate_node()) {
        if (_index == 0) {
            while (!_path.empty() && _index == 0) {
                _index = _path.top().second;
                _path.pop();
            }
        } else {
            --_index;
        }
    } else {
        _path.push({&(*_path.top().first)->_pointers[_index], _index});
        while (!is_terminate_node()) {
            _index = (*(_path.top().first))->_keys.size();
            _path.push({&((*(_path.top().first))->_pointers[_index]), _index - 1});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int)
{
    auto temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator==(const self& other) const noexcept
{
    if (_path.empty() && other._path.empty()) {
        return true;
    }
    return (!_path.empty() && !other._path.empty() && _path.size() == other._path.size() &&
            *(_path.top().first) == *(other._path.top().first) && _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept
{
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    btree_node* current_node = *_path.top().first;
    if (current_node != nullptr) {
        return current_node->_keys.size();
    }
    return 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept
{
    if (_path.empty()) {
        return true;
    }
    btree_node* current_node = *_path.top().first;
    return current_node == nullptr || current_node->_pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const std::stack<std::pair<btree_node* const*,
        size_t>>& path, size_t index):
    _path(path), _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const btree_iterator& it) noexcept:
    _index(it._index)
{
    std::stack<std::pair<btree_node* const*, size_t>> converted;
    std::stack<std::pair<btree_node**, size_t>> temp = it._path;

    std::vector<std::pair<btree_node** , size_t>> reversed;
    while (!temp.empty()) {
        reversed.push_back(temp.top());
        temp.pop();
    }

    for (auto it = reversed.rbegin(); it != reversed.rend(); ++it) {
        btree_node* const* ptr = const_cast<btree_node* const*>((*it).first);
        converted.emplace(ptr, it->second);
    }

    _path = std::move(converted);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept
{
    const auto& top_ref = _path.top();
    return (*top_ref.first)->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept
{
    if (_path.empty()) return nullptr;
    return &static_cast<const value_type&>((*(_path.top().first))->_keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++()
{
    if (_path.empty()) return *this;

    if (is_terminate_node()) {
        _index++;
        while (!_path.empty() && _index == (*(_path.top().first))->_keys.size()) {
            _index = _path.top().second;
            _path.pop();
        }
    } else {
        _path.push({&(*(_path.top().first))->_pointers[_index + 1], _index + 1});
        _index = 0;
        while (!is_terminate_node()) {
            _path.push({&(*(_path.top().first))->_pointers[0], 0});
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++(int)
{
    auto temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--()
{
    if (_path.empty()) return *this;

    if (is_terminate_node()) {
        if (_index == 0) {
            while (!_path.empty() && _index == 0) {
                _index = _path.top().second;
                _path.pop();
            }
        } else {
            --_index;
        }
    } else {
        _path.push({&(*_path.top().first)->_pointers[_index], _index});
        while (!is_terminate_node()) {
            _index = (*(_path.top().first))->_keys.size();
            _path.push({&((*(_path.top().first))->_pointers[_index]), _index - 1});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--(int)
{
    auto temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator==(const self& other) const noexcept
{
    if (_path.empty() && other._path.empty()) {
        return true;
    }
    return (!_path.empty() && !other._path.empty() && _path.size() == other._path.size() &&
            *(_path.top().first) == *(other._path.top().first) && _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    btree_node* current_node = *_path.top().first;
    if (current_node != nullptr) {
        return current_node->_keys.size();
    }
    return 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept
{
    if (_path.empty()) {
        return true;
    }
    const btree_node* current_node = *_path.top().first;
    return current_node == nullptr || current_node->_pointers.size() == 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index): _path(path), _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const btree_iterator& it) noexcept
{
    btree_iterator temp = it;
    --temp;
    _path = temp._path;
    _index = temp._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_iterator() const noexcept
{
    btree_iterator result(_path, _index);
    ++result;
    return result;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept
{
    const auto& top_ref = _path.top();
    return (*top_ref.first)->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept
{
    if (_path.empty()) return nullptr;
    return &(*(_path.top().first))->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++()
{
    if (_path.empty()) return *this;
    btree_iterator fwd = static_cast<btree_iterator>(*this);
    --fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int)
{
    auto temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--()
{
    if (_path.empty()) return *this;
    btree_iterator fwd = static_cast<btree_iterator>(*this);
    ++fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int)
{
    auto temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator==(const self& other) const noexcept
{
    if (_path.empty() && other._path.empty()) {
        return true;
    }
    return (!_path.empty() && !other._path.empty() && _path.size() == other._path.size() &&
            *(_path.top().first) == *(other._path.top().first) && _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    btree_node* current_node = *_path.top().first;
    if (current_node != nullptr) {
        return current_node->_keys.size();
    }
    return 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept
{
    if (_path.empty()) {
        return true;
    }
    const btree_node* current_node = *_path.top().first;
    return current_node == nullptr || current_node->_pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const std::stack<std::pair<btree_node* const*, size_t>>& path, size_t index): _path(path), _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const btree_reverse_iterator& it) noexcept: _index(it._index)
{
    std::stack<std::pair<btree_node* const*, size_t>> converted;
    std::stack<std::pair<btree_node**, size_t>> temp = it._path;

    std::vector<std::pair<btree_node**, size_t>> reversed;
    while (!temp.empty()) {
        reversed.push_back(temp.top());
        temp.pop();
    }

    for (auto iter = reversed.rbegin(); iter != reversed.rend(); ++iter) {
        btree_node* const* ptr = const_cast<btree_node* const*>(iter->first);
        converted.emplace(ptr, iter->second);
    }

    _path = std::move(converted);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_const_iterator() const noexcept
{
    btree_const_iterator result(_path, _index);
    ++result;
    return result;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept
{
    const auto& top_ref = _path.top();
    return (*top_ref.first)->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept
{
    if (_path.empty()) return nullptr;
    return &(*(_path.top().first))->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++()
{
    if (_path.empty()) return *this;
    btree_const_iterator fwd = static_cast<btree_const_iterator>(*this);
    --fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int)
{
    auto temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--()
{
    if (_path.empty()) return *this;
    btree_const_iterator fwd = static_cast<btree_const_iterator>(*this);
    ++fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int)
{
    auto temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator==(const self& other) const noexcept
{
    if (_path.empty() && other._path.empty()) {
        return true;
    }
    return (!_path.empty() && !other._path.empty() && _path.size() == other._path.size() &&
            *(_path.top().first) == *(other._path.top().first) && _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    btree_node* current_node = *_path.top().first;
    if (current_node != nullptr) {
        return current_node->_keys.size();
    }
    return 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept
{
    if (_path.empty()) {
        return true;
    }
    const btree_node* current_node = *_path.top().first;
    return current_node == nullptr || current_node->_pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::index() const noexcept
{
    return _index;
}

// endregion iterators implementation

// region element access implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key)
{
    auto path = find_path(key);
    if (path.second.second == false) {
        throw std::logic_error("this key doesn't exist");
    }
    return ((*(path.first.top().first))->_keys[path.second.first]).second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key) const
{
    auto path = find_path(key);
    if (path.second.second == false) {
        throw std::logic_error("this key doesn't exist");
    }
    return ((*(path.first.top().first))->_keys[path.second.first]).second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](const tkey& key)
{
    auto path = find_path(key);
    if (path.second.second == false) {
        throw std::logic_error("this key doesn't exist");
    }
    return ((*(path.first.top().first))->_keys[path.second.first]).second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](tkey&& key)
{
    auto path = find_path(key);
    if (path.second.second == false) {
        throw std::logic_error("this key doesn't exist");
    }
    return ((*(path.first.top().first))->_keys[path.second.first]).second;
}

// endregion element access implementation

// region iterator begins implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::begin()
{
    if (_root == nullptr) return end();

    std::stack<std::pair<btree_node**, size_t>> result_stack;
    result_stack.push({&_root, 0});

    btree_node* current = _root;
    while (!is_terma(current)) {
        current = current->_pointers[0];
        result_stack.push({&current, 0});
    }

    return btree_iterator(std::move(result_stack), 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::end()
{
    return btree_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::begin() const
{
    if (_root == nullptr) return end();

    std::stack<std::pair<btree_node* const*, size_t>> result_stack;
    result_stack.push({ &_root, 0 });

    btree_node** current = const_cast<btree_node**>(&_root);
    while (!is_terma(*current)) {
        current = &((*current)->_pointers[0]);
        result_stack.push({current, 0});
    }


    return btree_const_iterator(std::move(result_stack), 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const
{
    return btree_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const
{
    return begin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const
{
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()
{
    return btree_reverse_iterator(end());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()
{
    return btree_reverse_iterator(begin());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const
{
    return btree_const_reverse_iterator(btree_reverse_iterator(end()));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const
{
    return btree_const_reverse_iterator(btree_reverse_iterator(begin()));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const
{
    return rbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const
{
    return rend();
}

// endregion iterator begins implementation

// region lookup implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::empty() const noexcept
{
    return _root == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key)
{

    auto [path, index_found] = find_path(key);
    auto [index, found] = index_found;
    if (!found) {
        return end();
    }
    return btree_iterator(path, index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key) const
{
    auto [path, index_found] = find_path(key);
    auto [index, found] = index_found;
    if (!found) {
        return end();
    }
    return btree_iterator(path, index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key)
{
    auto [path, index_found] = find_path(key);
    auto [index, found] = index_found;

    if (found) {
        return btree_iterator(std::move(path), index);
    }

    if (path.empty()) { // need check
        return end();
    }

    btree_node* node = *path.top().first;
    if (index < node->_keys.size()) {
        return btree_iterator(std::move(path), index);
    }

    btree_iterator it(std::move(path), index);
    ++it;
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const
{
    auto [path, index_found] = find_path(key);
    auto [index, found] = index_found;

    if (found) {
        return btree_const_iterator(std::move(path), index);
    }

    if (path.empty()) { // need check
        return end();
    }

    btree_node* node = *path.top().first;
    if (index < node->_keys.size()) {
        return btree_const_iterator(std::move(path), index);
    }

    btree_const_iterator it(std::move(path), index);
    ++it;
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key)
{
    auto [path, index_found] = find_path(key);
    auto [index, found] = index_found;

    if (path.empty()) { // need check
        return end();
    }
    if (found) {
        auto it = btree_iterator(std::move(path), index);
        ++it;
        return it;
    }

    btree_node* node = *path.top().first;
    if (index < node->_keys.size()) {
        return btree_iterator(std::move(path), index);
    }

    btree_iterator it(std::move(path), index);
    ++it;
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const
{
    auto [path, index_found] = find_path(key);
    auto [index, found] = index_found;

    if (path.empty()) { // need check
        return end();
    }
    if (found) {
        auto it = btree_const_iterator(std::move(path), index);
        ++it;
        return it;
    }

    btree_node* node = *path.top().first;
    if (index < node->_keys.size()) {
        return btree_const_iterator(std::move(path), index);
    }

    btree_const_iterator it(std::move(path), index);
    ++it;
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const
{
    auto path = find_path(key);
    return path.second.second;
}

// endregion lookup implementation

// region modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::clear() noexcept
{
    if (_root == nullptr) return;

    std::stack<btree_node*> nodes;
    nodes.push(_root);

    while (!nodes.empty()) {
        btree_node* current = nodes.top();
        nodes.pop();
        if (!is_terma(current)) {
            for (auto* child : current->_pointers) {
                if (child) nodes.push(child);
            }
        }
        _allocator.template delete_object<btree_node>(current);
    }

    _root = nullptr;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(const tree_data_type& data)
{
    return insert(tree_data_type(data));
}
template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(tree_data_type&& data)
{
    auto [path, index_found] = find_path(data.first);
    auto [index, found] = index_found;
    if (found) {
        return {btree_iterator(std::move(path), index), false};
    }
    auto [inserted_path, inserted_index] = insert_inside(std::move(data), path);
    return {btree_iterator(std::move(inserted_path), inserted_index), true};
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::emplace(Args&&... args)
{
    tree_data_type value(std::forward<Args>(args)...);
    return insert(value);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)
{
    auto [iter, is_inserted] = insert(data);
    if (!is_inserted) { // the key existed before
        (*iter).second = data.second;
    }
    return iter;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)
{
    auto [iter, is_inserted] = insert(std::move(data));
    if (!is_inserted) {
        (*iter).second = std::move(data.second);
    }
    return iter;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)
{
    tree_data_type value(std::forward<Args>(args)...);
    return insert_or_assign(std::move(value));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator pos)
{
    return erase(*pos);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator pos)
{
    return erase(*pos);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator beg, btree_iterator en)
{
    btree_iterator temp = beg;
    btree_iterator result = end();
    while (temp != en) {
        result = erase((*temp).first);
        ++temp;
    }
    return result;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator beg, btree_const_iterator en)
{
    btree_const_iterator temp = beg;
    btree_const_iterator result = end();
    while (temp != en) {
        result = erase((*temp).first);
        ++temp;
    }
    return result;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    auto [path, index_found] = find_path(key);
    auto [index, found] = index_found;
    if (!found) return end();

    auto [new_path, new_index] = erase_inside(path, index);
    return btree_iterator(std::move(new_path), new_index);
}

// endregion modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_pairs(const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &lhs,
                   const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &rhs)
{
    return compare::operator()(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_keys(const tkey &lhs, const tkey &rhs)
{
    return compare::operator()(lhs, rhs);
}



#endif