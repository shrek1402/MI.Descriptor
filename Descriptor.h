#pragma once

#ifndef STD
  #define STD ::std::
#endif

#ifndef MI_NODISCARD
  #define MI_NODISCARD [[nodiscard]]
#endif

template<class ToType, class FromType>
MI_NODISCARD ToType narrow_cast(FromType value) {
  // Преобразовывает value из FromType в ToType и обратно в FromType,
  // получившееся значение сравнивается с исходным, должно получиться то же самое.
  assert(static_cast<FromType>(static_cast<ToType>(value)) == value && "narrow_cast<> was failed. ");

  return (static_cast<ToType>(STD forward<FromType>(value)));
}

struct default_descriptor_property {
  public:
    using value_type      = size_t;
    using difference_type = size_t;

  public:
    ~default_descriptor_property() = default;

    constexpr default_descriptor_property()                                   = default;
    constexpr default_descriptor_property(const default_descriptor_property&) = default;
    constexpr default_descriptor_property(default_descriptor_property&&)      = default;
    constexpr default_descriptor_property& operator=(const default_descriptor_property&) = default;
    constexpr default_descriptor_property& operator=(default_descriptor_property&&) = default;

  public:
    explicit constexpr default_descriptor_property(const size_t index) : _index(index) {
    }

    constexpr default_descriptor_property& operator=(const size_t index) {
      _index = index;

      return *this;
    }

  public:
    template<class Ty, STD enable_if_t<STD is_convertible_v<Ty, value_type>, int> = 0>
    explicit constexpr default_descriptor_property(const STD initializer_list<Ty> init_list) : _index(/**/) {
      assert(init_list.size() == 1);

      _index = narrow_cast<value_type>(*init_list.begin());
    }

    template<class Ty, STD enable_if_t<STD is_convertible_v<Ty, value_type>, int> = 0>
    constexpr default_descriptor_property& operator=(const STD initializer_list<Ty> init_list) {
      assert(init_list.size() == 1);

      _index = narrow_cast<value_type>(*init_list.begin());

      return *this;
    }

  public:
    // ОПЕРАТОР ПРИВЕДЕНИЯ
    explicit constexpr operator size_t() const {
      return _index;
    }

  public:
    // ПРЕФИКСНЫЙ ИНКРЕМЕНТ
    constexpr default_descriptor_property& operator++() {
      ++_index;

      return *this;
    }

    // ПОСТФИКСНЫЙ ИНКРЕМЕНТ
    MI_NODISCARD constexpr default_descriptor_property operator++(int) {
      const default_descriptor_property old = *this;
      ++*this;

      return old;
    }

    // ПРЕФИКСНЫЙ ДЕКРЕМЕНТ
    constexpr default_descriptor_property& operator--() {
      --_index;

      return *this;
    }

    // ПОСТФИКСНЫЙ ДЕКРЕМЕНТ
    MI_NODISCARD constexpr default_descriptor_property operator--(int) {
      const default_descriptor_property old = *this;
      --*this;

      return old;
    }

    // УНАРНЫЙ ИНКРЕМЕНТ
    constexpr default_descriptor_property& operator+=(const difference_type shift) {
      _index += shift;

      return *this;
    }

    // УНАРНЫЙ ИНКРЕМЕНТ
    MI_NODISCARD constexpr default_descriptor_property operator+(const difference_type shift) const {
      default_descriptor_property it = *this;
      it += shift;

      return it;
    }

    // УНАРНЫЙ ДЕКРЕМЕНТ
    constexpr default_descriptor_property& operator-=(const difference_type shift) {
      _index -= shift;

      return *this;
    }

    // УНАРНЫЙ ДЕКРЕМЕНТ
    MI_NODISCARD constexpr default_descriptor_property operator-(const difference_type shift) const {
      default_descriptor_property it = *this;
      it -= shift;

      return it;
    }

  public:
    MI_NODISCARD constexpr bool operator==(const default_descriptor_property& rhs) const {
      return this->_index == rhs._index;
    }

    MI_NODISCARD constexpr bool operator!=(const default_descriptor_property& rhs) const {
      return !(*this == rhs);
    }

    MI_NODISCARD constexpr bool operator<(const default_descriptor_property& rhs) const {
      return this->_index < rhs._index;
    }

    MI_NODISCARD constexpr bool operator>(const default_descriptor_property& rhs) const {
      return rhs < *this;
    }

    MI_NODISCARD constexpr bool operator<=(const default_descriptor_property& rhs) const {
      return !(*this > rhs);
    }

    MI_NODISCARD constexpr bool operator>=(const default_descriptor_property& rhs) const {
      return !(*this < rhs);
    }

  private:
    value_type _index;
};

template<class SelectorType, class PropertyType = default_descriptor_property>
class descriptor_base {
  public:
    friend struct hash<descriptor_base>;

  public:
    using property_type   = PropertyType;
    using difference_type = typename property_type::difference_type;

  public:
    ~descriptor_base() = default;

  public:
    constexpr descriptor_base() : _property(property_type{}) {
    }

  public:
    constexpr descriptor_base(const descriptor_base& other) = default;
    constexpr descriptor_base(descriptor_base&& other)      = default;
    constexpr descriptor_base& operator=(const descriptor_base& other) = default;
    constexpr descriptor_base& operator=(descriptor_base&& other) = default;

  public:
    constexpr explicit descriptor_base(const property_type& prop) noexcept : _property(prop) {
    }

    constexpr descriptor_base& operator=(const property_type& prop) noexcept {
      _property = prop;

      return *this;
    }

    constexpr explicit descriptor_base(property_type&& prop) noexcept : _property(STD move(prop)) {
    }

    constexpr descriptor_base& operator=(property_type&& prop) noexcept {
      _property = STD move(prop);

      return *this;
    }

  public:
    template<class... Tys, STD enable_if_t<STD is_constructible_v<property_type, Tys...>, int> = 0>
    constexpr explicit descriptor_base(Tys&&... values) noexcept : _property(STD forward<Tys>(values)...) {
    }

    template<class Ty, STD enable_if_t<STD is_constructible_v<property_type, Ty>, int> = 0>
    constexpr descriptor_base& operator=(Ty&& values) {
      _property(STD forward<Ty>(values));

      return *this;
    }

  public:
    template<class Ty, STD enable_if_t<STD is_constructible_v<property_type, STD initializer_list<Ty>>, int> = 0>
    constexpr descriptor_base(const STD initializer_list<Ty> init_list) : _property() {
      _property = init_list;
    }

    template<class Ty, STD enable_if_t<STD is_constructible_v<property_type, STD initializer_list<Ty>>, int> = 0>
    constexpr descriptor_base& operator=(const STD initializer_list<Ty> init_list) {
      _property = init_list;

      return *this;
    }

  public:
    constexpr explicit operator size_t() const {
      return static_cast<size_t>(_property);
    }

  public:
    constexpr descriptor_base& operator++() {
      ++_property;

      return *this;
    }

    MI_NODISCARD constexpr descriptor_base operator++(int) {
      const descriptor_base old = *this;
      ++*this;

      return old;
    }

    constexpr descriptor_base& operator--() {
      --_property;

      return *this;
    }

    MI_NODISCARD constexpr descriptor_base operator--(int) {
      const descriptor_base old = *this;
      --*this;

      return old;
    }

    constexpr descriptor_base& operator+=(const difference_type i) {
      _property += i;

      return *this;
    }

    MI_NODISCARD constexpr descriptor_base operator+(const difference_type i) const {
      descriptor_base it = *this;
      it += i;

      return it;
    }

    constexpr descriptor_base& operator-=(const difference_type i) {
      _property -= i;

      return *this;
    }

    MI_NODISCARD constexpr descriptor_base operator-(const difference_type i) const {
      descriptor_base it = *this;
      it -= i;

      return it;
    }

  public:
    MI_NODISCARD constexpr bool operator==(const descriptor_base& rhs) const {
      return this->_property == rhs._property;
    }

    MI_NODISCARD constexpr bool operator!=(const descriptor_base& rhs) const {
      return !(*this == rhs);
    }

    MI_NODISCARD constexpr bool operator<(const descriptor_base& rhs) const {
      return this->_property < rhs._property;
    }

    MI_NODISCARD constexpr bool operator>(const descriptor_base& rhs) const {
      return rhs < *this;
    }

    MI_NODISCARD constexpr bool operator<=(const descriptor_base& rhs) const {
      return !(*this > rhs);
    }

    MI_NODISCARD constexpr bool operator>=(const descriptor_base& rhs) const {
      return !(*this < rhs);
    }

  private:
    property_type _property;
};