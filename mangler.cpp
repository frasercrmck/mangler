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

template <size_t N, typename traits = std::char_traits<char>>
struct string_literal {

  template <typename... chars>
  constexpr string_literal(chars... cs)
      : data_{cs...} {}

  constexpr std::size_t size() const { return N; }

  constexpr char operator[](std::size_t idx) const {
    // TODO: statically check bounds?
    return data_[idx];
  }

  template <size_t U>
  constexpr auto operator+(const string_literal<U> &other) const {
    string_literal<N + U> out;

    for (unsigned i = 0; i < N; ++i) {
      out.data_[i] = data_[i];
    }
    for (unsigned i = 0; i < U; ++i) {
      out.data_[i + N] = other.data_[i];
    }
    out.data_[N + U] = '\0';

    return out;
  }

  constexpr bool operator==(const char *other) const {
    if (N != length(other)) {
      return false;
    }

    for (unsigned i = 0; i <= N; ++i) {
      if (data_[i] != other[i]) {
        return false;
      }
    }
    return true;
  }

  template <size_t U>
  constexpr bool operator==(const string_literal<U> &other) const {
    return *this == other.c_str();
  }

  constexpr const char *c_str() const { return data_; }

  char data_[N + 1];
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

  return 0;
}
