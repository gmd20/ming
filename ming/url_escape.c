// from golang net/url 

enum encodng {
  kEncodePath           = 1,
  kEncodePathSegment    = 2,
  kEncodeHost           = 3,
  kEncodeZone           = 4,
  kEncodeUserPassword   = 5,
  kEncodeQueryComponent = 6,
  kEncodeFragment       = 7
};
const char kHexChar[] = "0123456789ABCDEF";

// Return true if the specified character should be escaped when
// appearing in a URL string, according to RFC 3986.
//
// Please be informed that for now shouldEscape does not check all
// reserved characters correctly. See golang.org/issue/5684.
int shouldEscape(char c, int mode ) {
  // §2.3 Unreserved characters (alphanum)
  if ('A' <= c && c <= 'Z' || 'a' <= c && c <= 'z' || '0' <= c && c <= '9') {
    return 0;
  }

  if (mode == kEncodeHost || mode == kEncodeZone) {
    // §3.2.2 Host allows
    //	sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
    // as part of reg-name.
    // We add : because we include :port as part of host.
    // We add [ ] because we include [ipv6]:port as part of host.
    // We add < > because they're the only characters left that
    // we could possibly allow, and Parse will reject them if we
    // escape them (because hosts can't use %-encoding for
    // ASCII bytes).
    switch (c) {
     case '!':
     case '$':
     case '&':
     case '\'':
     case '(':
     case ')':
     case '*':
     case '+':
     case ',':
     case ';':
     case '=':
     case ':':
     case '[':
     case ']':
     case '<':
     case '>':
     case '"':
       return 0;
    }
  }

  switch (c) {
   case '-':
   case '_':
   case '.':
   case '~':
     // §2.3 Unreserved characters (mark)
     return 0;

   case '$':
   case '&':
   case '+':
   case ',':
   case '/':
   case ':':
   case ';':
   case '=':
   case '?':
   case '@':
     // §2.2 Reserved characters (reserved)
     // Different sections of the URL allow a few of
     // the reserved characters to appear unescaped.
     switch (mode) {
      case kEncodePath: // §3.3
        // The RFC allows : @ & = + $ but saves / ; , for assigning
        // meaning to individual path segments. This package
        // only manipulates the path as a whole, so we allow those
        // last three as well. That leaves only ? to escape.
        return (c == '?')?1:0;

      case kEncodePathSegment: // §3.3
        // The RFC allows : @ & = + $ but saves / ; , for assigning
        // meaning to individual path segments.
        return (c == '/' || c == ';' || c == ',' || c == '?')?1:0;

      case kEncodeUserPassword: // §3.2.1
        // The RFC allows ';', ':', '&', '=', '+', '$', and ',' in
        // userinfo, so we must escape only '@', '/', and '?'.
        // The parsing of userinfo treats ':' as special so we must escape
        // that too.
        return (c == '@' || c == '/' || c == '?' || c == ':')?1:0;

      case kEncodeQueryComponent: // §3.4
        // The RFC reserves (so we must escape) everything.
        return 1;

      case kEncodeFragment: // §4.1
        // The RFC text is silent but the grammar allows
        // everything, so escape nothing.
        return 0;
     }
  }

  // Everything else must be escaped.
  return 1;
}

int escape(int mode, char *s, int s_len, char *escapsed_s, int escaped_len) {
  int space_count = 0;
  int hex_count = 0;
  int i;
  char c;
  int j;
  char *t;
  int required_len;
  for (i = 0; i < s_len; i++) {
    c = s[i];
    if (shouldEscape(c, mode)) {
      if (c == ' ' && mode == kEncodeQueryComponent) {
        space_count++;
      } else {
        hex_count++;
      }
    }
  }

  if (space_count == 0 && hex_count == 0) {
    return s_len;
  }

  required_len = s_len + 2*hex_count;
  if (required_len >= escaped_len) {
    return required_len; // no enough space
  }

  j = 0;
  t = escapsed_s;
  for (i = 0; i < s_len; i++) {
    c = s[i];
    if (c == ' ' && mode == kEncodeQueryComponent) {
      t[j] = '+';
      j++;
    } else if (shouldEscape(c, mode)) {
      t[j] = '%';
      t[j+1] = kHexChar[c>>4];
      t[j+2] = kHexChar[c&15];
      j += 3;
    } else {
      t[j] = s[i];
      j++;
    }
  }
  t[j] = '\0';
  return required_len;
}

// PathEscape escapes the string so it can be safely placed
// inside a URL path segment.
int path_escape(char *s, int s_len, char *escaped_s, int escaped_len) {
  return escape(kEncodePathSegment, s, s_len,  escaped_s, escaped_len);
}

int query_escape(char *s, int s_len, char *escaped_s, int escaped_len) {
  return escape(kEncodeQueryComponent, s, s_len, escaped_s, escaped_len);
}
