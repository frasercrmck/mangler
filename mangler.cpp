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

  return 0;
}
