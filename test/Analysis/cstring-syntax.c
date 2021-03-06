// RUN: %clang_analyze_cc1 -analyzer-checker=unix.cstring.BadSizeArg -analyzer-store=region -Wno-strncat-size -Wno-strlcpy-strlcat-size -Wno-sizeof-array-argument -Wno-sizeof-pointer-memaccess -verify %s
// RUN: %clang_analyze_cc1 -triple armv7-a15-linux -analyzer-checker=unix.cstring.BadSizeArg -analyzer-store=region -Wno-strncat-size -Wno-strlcpy-strlcat-size -Wno-sizeof-array-argument -Wno-sizeof-pointer-memaccess -verify %s
// RUN: %clang_analyze_cc1 -triple aarch64_be-none-linux-gnu -analyzer-checker=unix.cstring.BadSizeArg -analyzer-store=region -Wno-strncat-size -Wno-strlcpy-strlcat-size -Wno-sizeof-array-argument -Wno-sizeof-pointer-memaccess -verify %s
// RUN: %clang_analyze_cc1 -triple i386-apple-darwin10 -analyzer-checker=unix.cstring.BadSizeArg -analyzer-store=region -Wno-strncat-size -Wno-strlcpy-strlcat-size -Wno-sizeof-array-argument -Wno-sizeof-pointer-memaccess -verify %s

typedef __SIZE_TYPE__ size_t;
char  *strncat(char *, const char *, size_t);
size_t strlen (const char *s);
size_t strlcpy(char *, const char *, size_t);

void testStrncat(const char *src) {
  char dest[10];
  strncat(dest, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAA", sizeof(dest) - 1); // expected-warning {{Potential buffer overflow. Replace with 'sizeof(dest) - strlen(dest) - 1' or use a safer 'strlcat' API}}
  strncat(dest, "AAAAAAAAAAAAAAAAAAAAAAAAAAA", sizeof(dest)); // expected-warning {{Potential buffer overflow. Replace with}}
  strncat(dest, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", sizeof(dest) - strlen(dest)); // expected-warning {{Potential buffer overflow. Replace with}}
  strncat(dest, src, sizeof(src)); // expected-warning {{Potential buffer overflow. Replace with}}
  // Should not crash when sizeof has a type argument.
  strncat(dest, "AAAAAAAAAAAAAAAAAAAAAAAAAAA", sizeof(char));
}

void testStrlcpy(const char *src) {
  char dest[10];
  size_t destlen = sizeof(dest);
  size_t srclen = sizeof(src);
  size_t badlen = 20;
  size_t ulen;
  strlcpy(dest, src, sizeof(dest));
  strlcpy(dest, src, destlen);
  strlcpy(dest, src, 10);
  strlcpy(dest, src, 20); // expected-warning {{The third argument is larger than the size of the input buffer. Replace with the value 'sizeof(dest)` or lower}}
  strlcpy(dest, src, badlen); // expected-warning {{The third argument is larger than the size of the input buffer. Replace with the value 'sizeof(dest)` or lower}}
  strlcpy(dest, src, ulen);
  strlcpy(dest + 5, src, 5);
}
