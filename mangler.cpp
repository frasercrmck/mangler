#include <cstdio>
#include <string>
#include <type_traits>

namespace mangler {

constexpr unsigned length(const char *str) {
  unsigned len = 0;
  while ((*str++) != '\0') {
    ++len;
  }

  return len;
}

// Helpers to check predicate over all types in a parameter pack
template <typename... Conds> struct and_ : std::true_type {};

template <typename Cond, typename... Conds>
struct and_<Cond, Conds...>
    : std::conditional<Cond::value, and_<Conds...>, std::false_type>::type {};

template <size_t N, typename traits = std::char_traits<char>>
struct string_literal {

  template <typename... chars>
  constexpr string_literal(chars... cs)
      : data{cs...} {
    static_assert(and_<std::is_same<char, chars>...>::value,
                  "All types must be of type char");
  }

  constexpr std::size_t size() const { return N; }

  constexpr char operator[](std::size_t idx) const {
    // TODO: statically check bounds?
    return data[idx];
  }

  template <size_t U>
  constexpr auto operator+(const string_literal<U> &other) const {
    string_literal<N + U> out;

    for (unsigned i = 0; i < N; ++i) {
      out.data[i] = data[i];
    }
    for (unsigned i = 0; i < U; ++i) {
      out.data[i + N] = other.data[i];
    }
    out.data[N + U] = '\0';

    return out;
  }

  constexpr bool operator==(const char *other) const {
    if (N != length(other)) {
      return false;
    }

    for (unsigned i = 0; i <= N; ++i) {
      if (data[i] != other[i]) {
        return false;
      }
    }
    return true;
  }

  constexpr bool operator!=(const char *other) const {
    return !this->operator==(other);
  }

  template <size_t U>
  constexpr bool operator==(const string_literal<U> &other) const {
    return *this == other.c_str();
  }

  template <size_t U>
  constexpr bool operator!=(const string_literal<U> &other) const {
    return !this->operator==(other.c_str());
  }

  constexpr const char *c_str() const { return data; }

  char data[N + 1];
};

template <typename charT, charT... chars>
constexpr string_literal<sizeof...(chars)> operator"" _S() {
  return string_literal<sizeof...(chars)>(chars...);
}
}

using mangler::operator"" _S;

int main() {
  constexpr auto foo = "hello world!"_S;

  static_assert(foo.size() == 12, "size() failed!");

  static_assert(foo[0] == 'h', "Operator [] failed!");
  static_assert(foo[1] == 'e', "Operator [] failed!");
  static_assert(foo[2] == 'l', "Operator [] failed!");
  static_assert(foo[3] == 'l', "Operator [] failed!");
  static_assert(foo[4] == 'o', "Operator [] failed!");
  static_assert(foo[5] == ' ', "Operator [] failed!");
  static_assert(foo[6] == 'w', "Operator [] failed!");
  static_assert(foo[7] == 'o', "Operator [] failed!");
  static_assert(foo[8] == 'r', "Operator [] failed!");
  static_assert(foo[9] == 'l', "Operator [] failed!");
  static_assert(foo[10] == 'd', "Operator [] failed!");
  static_assert(foo[11] == '!', "Operator [] failed!");
  static_assert(foo[12] == '\0', "Operator [] failed!");

  printf("%s\n", foo.c_str());

  constexpr auto hello = "hello"_S;
  constexpr auto world = " world!"_S;

  constexpr auto hello_world = hello + world;

  static_assert(hello_world[0] == 'h', "Operator + failed!");
  static_assert(hello_world[12] == '\0', "Operator + failed!");

  static_assert(hello_world == foo, "Operator == failed!");
  static_assert(hello_world == "hello world!", "Operator == failed!");

  static_assert(hello_world != hello, "Operator != failed!");
  static_assert(hello_world != "hello world!!!", "Operator != failed!");

  return 0;
}
