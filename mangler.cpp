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
  explicit constexpr string_literal(chars... cs) : data{cs...} {
    static_assert(and_<std::is_same<char, chars>...>::value,
                  "All types must be of type char");
  }

  constexpr std::size_t size() const { return N; }

  constexpr char operator[](std::size_t idx) const {
    // TODO: statically check bounds?
    return data[idx];
  }

  template <std::size_t P, std::size_t L>
  constexpr string_literal<L> substr() const {
    static_assert(L > 0 && L <= N, "Invalid substring length");
    static_assert(P <= N - L, "Invalid substring position");
    string_literal<L> out;

    for (unsigned i = 0; i < L; ++i) {
      out.data[i] = data[P + i];
    }
    out.data[L] = '\0';
    return out;
  }

  constexpr int find_first_of(char C) const {
    for (std::size_t i = 0; i < N; ++i) {
      if (data[i] == C) {
        return static_cast<int>(i);
      }
    }
    return -1;
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

constexpr std::size_t num_digits(uint32_t x) {
  if (x >= 1000000000) {
    return 10;
  }
  if (x >= 100000000) {
    return 9;
  }
  if (x >= 10000000) {
    return 8;
  }
  if (x >= 1000000) {
    return 7;
  }
  if (x >= 100000) {
    return 6;
  }
  if (x >= 10000) {
    return 5;
  }
  if (x >= 1000) {
    return 4;
  }
  if (x >= 100) {
    return 3;
  }
  if (x >= 10) {
    return 2;
  }
  return 1;
}

template <uint32_t U> static constexpr auto string_literal_from_num() {
  constexpr std::size_t digits = num_digits(U);
  static_assert(digits > 0, "Number of digits can't be 0");

  int num = U;
  int pos = digits;

  string_literal<digits> out;

  while (pos > 0) {
    out.data[--pos] = static_cast<char>('0' + (num % 10));
    num /= 10;
  }

  out.data[digits] = '\0';
  return out;
}

template <typename charT, charT... chars> constexpr auto operator"" _F() {
  constexpr std::size_t X = sizeof...(chars);
  constexpr string_literal<X> full_str(chars...);

  constexpr int brace_pos = full_str.find_first_of('(');

  constexpr std::size_t len =
      brace_pos == -1 ? X : static_cast<std::size_t>(brace_pos);

  constexpr auto name = full_str.template substr<0, len>();
  constexpr std::size_t size = name.size();

  constexpr auto size_num = mangler::string_literal_from_num<size>();

  constexpr auto mangled_str = "_Z"_S + size_num + name;

  return mangled_str;
}
}

using mangler::operator"" _S;
using mangler::operator"" _F;

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

  printf("%s%s\n", hello.c_str(), world.c_str());

  constexpr auto hello_world = hello + world;

  static_assert(hello_world[0] == 'h', "Operator + failed!");
  static_assert(hello_world[12] == '\0', "Operator + failed!");

  static_assert(hello_world == foo, "Operator == failed!");
  static_assert(hello_world == "hello world!", "Operator == failed!");

  static_assert(hello_world != hello, "Operator != failed!");
  static_assert(hello_world != "hello world!!!", "Operator != failed!");

  constexpr auto sub = hello_world.substr<0, 5>();
  constexpr auto sub_full = hello_world.substr<0, 12>();
  static_assert(hello == sub, "Substring failed!");
  static_assert(hello_world == sub_full, "Substring failed!");

  static_assert(hello_world.find_first_of('w') == 6, "find failed!");
  static_assert(hello_world.find_first_of('!') == 11, "find failed!");
  static_assert(hello_world.find_first_of('x') == -1, "find failed!");

  printf("%s\n", hello_world.c_str());

  constexpr auto test_num = mangler::string_literal_from_num<100000>();
  static_assert(test_num.size() == 6, "string_literal_from_num failed!");

  constexpr auto mangled_foo = "foo(int)"_F;
  static_assert(mangled_foo == "_Z3foo", "Name mangle failed!");

  constexpr auto mangled_foobarbazfizz = "foobarbazfizz()"_F;
  // Why is this _Z31 on gcc??
  static_assert(mangled_foobarbazfizz == "_Z13foobarbazfizz",
                "Name mangle failed!");

  printf("%s\n", mangled_foo.c_str());
  printf("%s\n", mangled_foobarbazfizz.c_str());

  return 0;
}
