#pragma once

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <any>

namespace mocha
{

template <typename T>
class HandleMap
{
 public:
  /**
   * @brief if the HandleMap has the given id
   * 
   * @param identifier
   * @return true 
   * @return false 
   */
  bool has(int id)
  {
    for (int i : identifier_)
    {
      if (i == id)
        return true;
    }
    return false;
  }

  /**
   * @brief add an id and its data to HandleMap
   * 
   * @param id
   * @param item 
   */
  void add(int id, const T& item)
  {
    if (has(id)) return;

    identifier_.push_back(id);
    data_.push_back(item);

    connection_[id] = data_.size() - 1; 
  }

  /**
   * @brief removes id and its data and swaps its place with the last id and data
   * 
   * @param id 
   */
  void remove(int id)
  {
    if (!has(id)) return;

    // whole operation works as data_ and identifier_ are same size and each are 
    // stored in same position
    int index = connection_[id];    // index of data
    int last = identifier_.size()-1;        // last identifier

    std::swap(identifier_[index], identifier_[last]);
    std::swap(data_[index], data_[last]);

    connection_[identifier_[index]] = index;

    identifier_.pop_back();
    data_.pop_back();
    connection_.erase(id);
  }

  /**
   * @brief get pointer to data of id
   * 
   * @param id 
   * @return T* 
   */
  T* get(int id)
  {
    return &(data_[connection_[id]]);
  }

  /**
   * @brief set ids data to item
   * 
   * @param id 
   * @param item 
   */
  void set(int id, const T& item)
  {
    if (!has(id)) return;

    data_[connection_[id]] = item;
  }

  /**
   * @brief clear all 
   * 
   */
  void clear()
  {
    identifier_.clear();
    data_.clear();
    connection_.clear();
  }

  /**
   * @brief return a vector with all ids
   * 
   * @return std::vector<int> 
   */
  std::vector<int> view()
  {
    return identifier_;
  }

  /**
   * @brief amount of id,data - pairs stored
   * 
   * @return int 
   */
  int size()
  {
    return identifier_.size();
  }

 private:
  std::vector<int>  identifier_;
  std::vector<T>    data_;
  std::unordered_map<int, int> connection_;
};

template<typename ...T>
class View
{
 public:
  /**
   * @brief Construct a new View object
   * 
   * @param args used to store HandleMap types defined with T...
   */
  View(HandleMap<T>&... args) : maps(args...) {}

  /**
   * @brief get vector of all ids that match the given types inside world
   * 
   * @return std::vector<int> 
   */
  std::vector<int> get()
  {
    std::vector<int> out;
    std::vector<int> base = std::get<0>(maps).view();
    ((base = (std::get<HandleMap<T>&>(maps).view().size() < base.size() 
                  ? std::get<HandleMap<T>&>(maps).view() : base)), ...);
    
    for (int i : base)
    {
      if ((std::get<HandleMap<T>&>(maps).has(i) && ...))
        out.push_back(i);
    }
    return out;
  }

 private:
  std::tuple<HandleMap<T>&...> maps;
};

class World
{
 public:
  /**
   * @brief singleton structure
   * 
   * @return World& 
   */
  static World& get()
  {
    static World inst;
    return inst;
  }

  /**
   * @brief increment entity amount
   * 
   * @return int 
   */
  int next() {return next_entity++;}

  /**
   * @brief decrement entity amount
   * 
   */
  void back() {next_entity--;}

  /**
   * @brief get all HandleMaps inside world
   * 
   * @return std::unordered_map<std::type_index, void*>  void* is HandleMap<T>
   */
  std::unordered_map<std::type_index, void*> view()
  {
    return maps_;
  }

  /**
   * @brief get a single HandleMap that has the type T
   * 
   * @tparam T 
   * @return HandleMap<T>& 
   */
  template<typename T>
  HandleMap<T>& view()
  {
    std::type_index t {typeid(T)};
    if (!maps_.contains(t))
      maps_[t] = new HandleMap<T>();
    
    return *static_cast<HandleMap<T>*>(maps_[t]);
  }

  /**
   * @brief delete to keep singleton functionality
   * 
   */
  void operator=(const World&) = delete;

 private:
  World(){};
  std::unordered_map<std::type_index, void*> maps_ {};
  int next_entity {0};
};

class System
{
 public:
  virtual void update(float dt) = 0;
};

/**
 * @brief create an entity
 * 
 * @return int 
 */
int create();

/**
 * @brief remove an entity
 * 
 * @param id 
 */
void remove(int id);

/**
 * @brief add a component to an entity id
 * 
 * @tparam T component type
 * @param id entity id
 * @param data component data
 */
template<typename T>
void emplace(int id, const T& data)
{
  World::get().view<T>().add(id, data);
}

/**
 * @brief get component data pointer from an entity
 * 
 * @tparam T component type
 * @param id entity id
 * @return T* pointer to component
 */
template<typename T>
T* get(int id)
{
  return World::get().view<T>().get(id);
}

/**
 * @brief if the entity has a component of type T
 * 
 * @tparam T 
 * @param id 
 * @return true 
 * @return false 
 */
template<typename T>
bool has(int id)
{
  return World::get().view<T>().has(id);
}

/**
 * @brief get all entities that have the given types as components
 * 
 * @tparam T types of components
 * @return std::vector<int> all entities that matchs
 */
template<typename ...T>
std::vector<int> view()
{
  return View<T...>(World::get().view<T>()...).get();
}

} // mocha