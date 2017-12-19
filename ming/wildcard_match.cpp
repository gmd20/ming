
// benchmark: for simple domain name matching, it takes less than 1 millisecond
//            to match 10 million items. it takes roughly the same time as
//            strcmp,  and is faster than "strlen(s) != strlen(pattern)"
//
// origin: Martin Richter https://www.codeproject.com/articles/188256/a-simple-wildcard-matching-function
// wildcard_match
// @s: string to match
// @pattern: matching pattern that contain ? and *
//
// The wildcard pattern can include the characters ‘?’ and ‘*’
// '?' – matches any single character
// '*' – Matches any sequence of characters (including the empty sequence)
int wildcard_match(const char *s, const char *pattern)
{
  while (*pattern) {
    if (*pattern == '*') {
      // 1. '*' matches 0 character
      if (wildcard_match(s, pattern+1))
        return 1;

      // 2. '*'  matches ore or more character
      if (*s && wildcard_match(s+1, pattern))
        return 1;

      // '*' mismatch
      return 0;

    } else if (*pattern == '?') {
      // Matches any character except empty string
      if (!*s) {
        return 0;
      }

      ++s;
      ++pattern;
    } else {
      if (*s++ != *pattern++)
        return 0;
    }
  }

  // Have a match? Only if both are at the end...
  return !*s && !*pattern;
}

