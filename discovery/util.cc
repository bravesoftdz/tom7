
#include <sys/stat.h>
#include <cstring>
#include <string.h>

#include "util.h"
#include "extent.h"

#ifdef WIN32
   /* chdir */
#  include <direct.h>
   /* getpid */
#  include <process.h>
   /* time */
#  include <time.h>
   /* rename */
#  include <io.h>
   /* setclipboard */
#  include <windows.h>
#else /* posix */
   /* chdir, unlink */
#  include <unistd.h>
   /* getpid */
#  include <sys/types.h>
   /* isalnum */
#  include <ctype.h>
#endif

struct linereal : public line {
  int x0, y0, x1, y1;
  int dx, dy;
  int stepx, stepy;
  int frac;

  virtual ~linereal() {}
  
  linereal(int x0_, int y0_, int x1_, int y1_) : 
    x0(x0_), y0(y0_), x1(x1_), y1(y1_) {
    

    dy = y1 - y0;
    dx = x1 - x0;

    if (dy < 0) { 
      dy = -dy;
      stepy = -1; 
    } else {
      stepy = 1; 
    }

    if (dx < 0) {
      dx = -dx;
      stepx = -1;
    } else { 
      stepx = 1; 
    }

    dy <<= 1;
    dx <<= 1;

    if (dx > dy) {
      frac = dy - (dx >> 1);
    } else {
      frac = dx - (dy >> 1);
    }
  }

  bool next(int & cx, int & cy) {
    if (dx > dy) {
      if (x0 == x1) return false;
      else {
	if (frac >= 0) {
	  y0 += stepy;
	  frac -= dx;
	}
	x0 += stepx;
	frac += dy;
	cx = x0;
	cy = y0;
	return true;
      }
    } else {
      if (y0 == y1) return false;
      else {
	if (frac >= 0) {
	  x0 += stepx;
	  frac -= dy;
	}
	y0 += stepy;
	frac += dx;
	cx = x0;
	cy = y0;
	return true;
      }
    }
  }
  
  virtual void destroy() {
    delete this;
  }

};

line * line::create(int a, int b, int c, int d) {
  return new linereal(a, b, c, d);
}

bool util::isdir(string f) {
  struct stat st;
  return (!stat(f.c_str(), &st)) && (st.st_mode & S_IFDIR);
}

bool util::existsfile(string s) {
  struct stat st;

  return !stat(s.c_str(), &st);
}

bool util::existsdir(string d) {
  return isdir(d); /* (existsfile(d) && isdir(d.c_str())); */
}

/* XXX what mode? */
bool util::makedir(string d) {
# ifdef WIN32
  return !mkdir(d.c_str());
# else /* posix */
  return !mkdir(d.c_str(), 0755);
# endif
}

string readfile(string s) {
  if (util::isdir(s)) return "";
  if (s == "") return "";

  FILE * f = fopen(s.c_str(), "rb");

  if (!f) return "";
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char * ss = (char*)malloc(size);
  fread(ss, 1, size, f);

  fclose(f);

  string ret = string(ss, size);
  free(ss);

  return ret;
}

static bool hasmagicf(FILE * f, const string & mag) {
  char * hdr = (char*)malloc(mag.length());
  if (!hdr) return false;

  /* we may not even be able to read sizeof(header) bytes! */
  if (mag.length() != fread(hdr, 1, mag.length(), f)) {
    free(hdr);
    return false;
  }

  for(unsigned int i = 0; i < mag.length(); i++) {
    if (hdr[i] != mag[i]) {
      free(hdr);
      return false;
    }
  }
  
  free(hdr);
  return true;
}

bool util::hasmagic(string s, const string &mag) {
  FILE * f = fopen(s.c_str(), "rb");
  if (!f) return false;

  bool hm = hasmagicf(f, mag);
  
  fclose(f);
  return hm;
}

string util::readfilemagic(string s, const string &mag) {
  if (isdir(s)) return "";
  if (s == "") return "";

  // printf("opened %s\n", s.c_str());

  /* PERF try this: and see! */
  // printf("Readfile '%s'\n", s.c_str());

  FILE * f = fopen(s.c_str(), "rb");

  if (!f) return "";


  if (!hasmagicf(f, mag)) {
    fclose(f);
    return "";
  }

  /* ok, now just read file */

  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char * ss = (char*)malloc(size);
  fread(ss, 1, size, f);

  fclose(f);

  string ret = string(ss, size);
  free(ss);

  return ret;
}

bool writefile(string fn, string s) {

  FILE * f = fopen(fn.c_str(), "wb");
  if (!f) return false;

  /* XXX check failure */
  fwrite(s.c_str(), 1, s.length(), f);

  fclose(f);
  
  return true;

}

string itos(int i) {
  char s[64];
  sprintf(s, "%d", i);
  return (string)s;
}

string util::ptos(void * p) {
  char s[64];
  sprintf(s, "%p", p);
  return (string)s;
}


string sizes(int i) {
  string s = "    ";
  s[0] = 255&(i >> 24);
  s[1] = 255&(i >> 16);
  s[2] = 255&(i >> 8);
  s[3] = 255& i;
  return s;
}

/* XXX these have terrible names */

/* represent int i (as i mod (2^(b/8))) 
   using only b bytes */
string shint(int b, int i) {
  return sizes(i).substr(4-b, b);
}

/* inverse of shint. does not check that
   there is enough room in s to read b bytes
   from idx ... */
int shout(int b, string s, unsigned int & idx) {
  int r = 0;
  while(b--) {
    r = ((unsigned char)s[idx++]) + (r<<8);
  }
  return r;
}

unsigned int util::hash(string s) {
  unsigned int h = 0x714FA5DD;
  for(unsigned int i = 0; i < s.length(); i ++) {
    h = (h << 11) | (h >> (32 - 11));
    h *= 3113;
    h ^= (unsigned char)s[i];
  }
  return h;
}

string util::lcase(string in) {
  string out;
  for(unsigned int i = 0; i < in.length(); i++) {
    if (in[i] >= 'A' &&
	in[i] <= 'Z') out += in[i]|32;
    
    else out += in[i];
  }
  return out;
}

string util::ucase(string in) {
  string out;
  for(unsigned int i = 0; i < in.length(); i++) {
    if (in[i] >= 'a' &&
	in[i] <= 'z') out += (in[i] & (~ 32));
    
    else out += in[i];
  }
  return out;
}

string util::fileof(string s) {
  int i = s.length () - 1;
  for(; i >= 0; i --) {
    if (s[i] == DIRSEPC) {
      return s.substr(i + 1, s.length () - (i + 1));
    }
  }
  return s;
}

string util::pathof(string s) {
  if (s == "") return ".";
  int i = s.length () - 1;
  for(; i >= 0; i --) {
    if (s[i] == DIRSEPC) {
      return s.substr(0, i);
    }
  }
  return ".";
}

/* XX can use endswith below */
string util::ensureext(string f, string ext) {
  if (f.length () < ext.length())
    return f + ext;
  else {
    if (f.substr(f.length () - ext.length(),
		 ext.length()) != ext)
      return f + ext;
    else return f;
  }
}

bool util::endswith (string big, string small_) {
  if (small_.length() > big.length()) return false;
  return big.substr(big.length() - small_.length (),
		    small_.length()) == small_;
}

bool util::startswith (string big, string small_) {
  if (small_.length () > big.length()) return false;
  return big.substr(0, small_.length()) == small_;
}

int util::changedir(string s) {
  return !chdir(s.c_str());
}

int util::getpid() {
  return ::getpid();
}

int stoi(string s) {
  return atoi(s.c_str());
}

/* XXX race. should use creat
   with O_EXCL on unix, at least. */
FILE * util::open_new(string fname) {
  if (!existsfile(fname))
    return fopen(fname.c_str(), "wb+");
  else return 0;
}

string util::getline(string & chunk) {
  string ret;
  for(unsigned int i = 0; i < chunk.length(); i ++) {
    if (chunk[i] == '\r') continue;
    else if (chunk[i] == '\n') {
      chunk = chunk.substr(i + 1, chunk.length() - (i + 1));
      return ret;
    } else ret += chunk[i];
  }
  /* there doesn't need to be a final trailing newline. */
  chunk = "";
  return ret;
}

/* PERF */
string util::fgetline(FILE * f) {
  string out;
  int c;
  while ( (c = fgetc(f)), ((c != EOF) && (c != '\n')) ) {
    /* ignore CR */
    if (c != '\r') {
      out += (char)c;
    }
  }
  return out;
}

/* PERF use substr instead of accumulating: this is used
   frequently in the net stuff */
/* return first token in line, removing it from 'line' */
string util::chop(string & line) {
  for (unsigned int i = 0; i < line.length(); i ++) {
    if (line[i] != ' ') {
      string acc;
      for(unsigned int j = i; j < line.length (); j ++) {
	if (line[j] == ' ') {
	  line = line.substr(j, line.length() - j);
	  return acc;
	} else acc += line[j];
      }
      line = "";
      return acc;
    }
  }
  /* all whitespace */
  line = "";
  return "";
}

/* PERF same */
string util::chopto(char c, string & line) {
  string acc;
  for (unsigned int i = 0; i < line.length(); i ++) {
    if (line[i] != c) {
      acc += line[i];
    } else {
      if (i < (line.length() - 1)) {
	line = line.substr(i + 1, line.length() - (i + 1));
	return acc;
      } else {
	line = "";
	return acc;
      }
    }
  }
  /* character didn't appear; treat as an invisible
     occurrence at the end */
  line = "";
  return acc;
}

string util::losewhitel(const string & s) {
  for(unsigned int i = 0; i < s.length(); i ++) {
    switch(s[i]) {
    case ' ':
    case '\n':
    case '\r':
    case '\t':
      /* keep going ... */
      break;
    default:
      return s.substr(i, s.length () - i);
    }
  }
  /* all whitespace */
  return "";
}

string util::tempfile(string suffix) {
  static int tries = 0;

  char * fname = new char[suffix.length() + 128];
  extentda<char> ef(fname);

  do {
    sprintf(fname, 
	    "%d_%d_%d%s", 
	    tries, getpid(), random(),
	    suffix.c_str());
    tries++;
  } while(existsfile(fname));

  return fname;

}

/* break up the strings into tokens. A token is either
   a single character (non-numeral) or a sequence of
   numerals that are interpreted as a number. We then
   do lexicographic comparison on this stream of tokens.
   assumes ascii. 

   l < r     -1
   l = r      0
   l > r      1

   XXX this treats

   abc 0123 def
   abc 123 def

   as equal strings. perhaps don't allow 0 to start a 
   number?
   
   n.b. it is easy to overflow here, so perhaps comparing
   as we go is better
*/
int util::natural_compare(const string & l, const string & r) {

  for(int caseless = 0; caseless < 2; caseless ++) {
    
    unsigned int il = 0;
    unsigned int ir = 0;
    
    while(il < l.length() || ir < r.length()) {
      /* if out of tokens in either string, it comes first. */
      if (il >= l.length()) return -1;
      if (ir >= r.length()) return 1;
      
      int lc = (unsigned char)l[il];
      int rc = (unsigned char)r[ir];
      
      if (lc >= '0' && lc <= '9') {
	if (rc >= '0' && rc <= '9') {
	  /* compare ints */
	  int ll = 0;
	  int rr = 0;

	  while (il < l.length() && l[il] >= '0' && l[il] <= '9') {
	    ll *= 10;
	    ll += (l[il] - '0');
	    il ++;
	  }

	  while (ir < r.length() && r[ir] >= '0' && r[ir] <= '9') {
	    rr *= 10;
	    rr += (r[ir] - '0');
	    ir ++;
	  }

	  if (ll < rr) return -1;
	  if (ll > rr) return 1;
	  /* otherwise continue... */
	
	  il ++;
	  ir ++;
	} else {
	  /* treat numbers larger than any char. */
	  return 1;
	}
      } else {
	if (rc >= '0' && rc <= '9') {
	  return -1;
	} else {
	  /* compare chars */
	  if ((rc|32) >= 'a' && (rc|32) <= 'z' &&
	      (lc|32) >= 'a' && (rc|32) <= 'z' &&
	      !caseless) {
	  
	    /* letters are case-insensitive */
	    if ((lc|32) < (rc|32)) return -1;
	    if ((lc|32) > (rc|32)) return 1;
	  } else {
	    if (lc < rc) return -1;
	    if (lc > rc) return 1;
	  }
	
	  /* same so far. continue... */
	
	  il ++;
	  ir ++;
	}
      }
    
    }
    /* strings look equal when compared
       as case-insensitive. so try again
       sensitive */
  }

  /* strings are case-sensitive equal! */

  return 0;
}

/* same as above, but ignore "the" at beginning */
/* XXX also ignore symbols ie ... at the beginning */
int util::library_compare(const string & l, const string & r) {

  /* XXX currently IGNOREs symbols, which could give incorrect
     results for strings that are equal other than their
     leading punctuation */
  unsigned int idxl = 0;
  unsigned int idxr = 0;
  while (idxl < l.length() && (!isalnum(l[idxl]))) idxl++;
  while (idxr < r.length() && (!isalnum(r[idxr]))) idxr++;

  bool thel = false;
  bool ther = false;
  if (l.length() >= (5 + idxl) &&
      (l[idxl + 0]|32) == 't' &&
      (l[idxl + 1]|32) == 'h' &&
      (l[idxl + 2]|32) == 'e' &&
      (l[idxl + 3])    == ' ') thel = true;

  if (r.length() >= (5 + idxr) &&
      (r[idxr + 0]|32) == 't' &&
      (r[idxr + 1]|32) == 'h' &&
      (r[idxr + 2]|32) == 'e' &&
      (r[idxr + 3])    == ' ') ther = true;

  if (thel != ther) {
    if (thel) idxl += 4;
    else idxr += 4;
  }

  return natural_compare (l.substr(idxl, l.length () - idxl),
			  r.substr(idxr, r.length () - idxr));
}

/* XXX impossible to specify a spec for just ^ */
bool util::matchspec(string spec, char c) {
  if (!spec.length()) return false;
  else if (spec[0] == '^') 
  return !matchspec(spec.substr(1, spec.length() - 1), c);

  /* now loop looking for c in string, or ranges */
  for(unsigned int i = 0; i < spec.length(); i ++) {
    /* ok if starts range, since they are inclusive */
    if (spec[i] == c) return true;

    /* handle ranges */
    if (spec[i] == '-') {
      /* can't be first or last */
      if (i && i < (spec.length() - 1)) {
	if (spec[i - 1] <= c &&
	    spec[i + 1] >= c) return true;
	/* skip dash and next char */
	i ++;
      }
    }
  }
  return false; /* no match */
}


bool util::library_matches(char k, const string & s) {
  /* skip symbolic */
  unsigned int idx = 0;
  while (idx < s.length() && (!isalnum(s[idx]))) idx++;
  
  /* skip 'the' */
  if (s.length() >= (idx + 5) &&
      (s[idx]|32) == 't' &&
      (s[idx + 1]|32) == 'h' &&
      (s[idx + 2]|32) == 'e' &&
      (s[idx + 3])    == ' ') return (s[idx + 4]|32) == (k|32);
  else return (s.length () > 0 && (s[idx]|32) == (k|32));
}

/* try a few methods to remove a file.
   An executable can't remove itself in
   Windows 98, though.
*/
bool util::remove(string f) {
  if (!existsfile(f.c_str())) return true;
  else {
# ifdef WIN32
    /* We can do this by:
       rename tmp  delme1234.exe
       exec(delme1234.exe "-replace" "escape.exe")
          (now, the program has to have a flag -replace
	   that instructs it to replace escape.exe 
	   with itself, then exit)
       .. hopefully exec will unlock the original
       process's executable!! */

    /* try unlinking. if that fails,
       rename it away. */
    if (0 == unlink(f.c_str())) return true;

    string fname = tempfile(".deleteme");
    if (0 == rename(f.c_str(), fname.c_str())) return true;

# else /* posix */
    if (0 == unlink(f.c_str())) return true;
# endif
  } 
  return false;

}

bool util::move(string src, string dst) {
# ifdef WIN32
  if (0 == rename(src.c_str(), dst.c_str()))
    return true;
  else return false;

# else /* posix */
  /* XXX actually, posix has rename too. */
  if (0 == link(src.c_str(), dst.c_str())) {
    /* succeed regardless of whether we
       can remove the old link or not. */
    unlink(src.c_str());
    return true;
  } else {
    /* try copy and unlink... (link doesn't work on AFS?) */
    if (copy(src, dst)) {
      unlink(src.c_str());
      return true;
    } return false;
  }
# endif
}


bool util::copy(string src, string dst) {
  FILE * s = fopen(src.c_str(), "rb");
  if (!s) return false;
  FILE * d = fopen(dst.c_str(), "wb+");
  if (!d) { fclose(s); return false; }

  char buf[256];
  int x = 0;
  do {
    /* XXX doesn't distinguish error from EOF, but... */
    x = fread(buf, 1, 256, s);
    if (x) {
      if ((signed)fwrite(buf, 1, x, d) < x) {
	fclose(s);
	fclose(d);
	return false;
      }
    }
  } while (x == 256);

  fclose(s);
  fclose(d);
  return true;
}

string util::dirplus(const string &dir_, const string &file) {
  if (dir_.empty()) return file;
  if (!file.empty() && file[0] == DIRSEPC) return file;
  string dir = dir_;
  if (dir[dir.size() - 1] != DIRSEPC)
    dir += DIRSEPC;
  return dir + file;
}

void util::toattic(string f) {
  string nf = f;
  // printf("TOATTIC %s...\n", f.c_str());
  /* in case it doesn't exist... */
  makedir(ATTIC_DIR);
  for(unsigned int i = 0; i < nf.length() ; i ++) {
    if (nf[i] == DIRSEPC) nf[i] = '_';
  }
  /* XXX race */
  int tries = 12;
  while(tries--) {
    string dest = (string)ATTIC_DIR + DIRSEP + 
                  itos(random()) + (string)"_" + nf;
    if (!existsfile(dest)) {
      // printf("move(%s,%s)\n", f.c_str(), dest.c_str());
      move(f, dest);
      return;
    }
  }
}

string util::cdup(const string & dir) {
  /* XXX right second argument to rfind? I want to find the last / */
  int idx = dir.rfind(DIRSEP, dir.length () - 1);
  if (idx != (signed)string::npos) {
    if (idx) return dir.substr(0, idx);
    else return ".";
  } else return ".";
}

void util::createpathfor(string f) {
  string s;
  for(unsigned int i = 0; i < f.length();  i++) {
    if (f[i] == DIRSEPC) {
      /* initial / will cause s == "" for first
	 appearance */
      if (s != "") makedir(s);
    }
    s += f[i];
  }
}

FILE * util::fopenp(string f, string m) {
  createpathfor(f);
  return fopen(f.c_str(), m.c_str());
}

string util::replace(string src, string findme, string rep) {
  int idx = src.length() - 1;

  if (findme.length () < 1) return src;

  /* idx represents the position in src which, for all chars greater
     than it, there begins no match of findme */
  while (idx >= 0 && idx != (signed)string::npos) {
    idx = src.rfind(findme, idx);
    if (idx != (signed)string::npos) {
      /* do replacement */
      src.replace(idx, findme.length(), rep);
      /* want to ensure termination even if rep contains findmes */
      idx -= findme.length();
    } else break;
  }
  return src;
}

/* implementation of imperative union-find is
   beautifully simple */
void onionfind::onion(int a, int b) {
  if (find(a) != find(b)) arr[find(a)] = b;
}

int onionfind::find(int a) {
  if (arr[a] == -1) return a;
  else return arr[a] = find(arr[a]);
}

onionfind::onionfind(int size) {
  arr = new int[size];
  for (int i = 0; i < size; i++) arr[i] = -1;
}

int bitbuffer::ceil(int bits) {
  return (bits >> 3) + !!(bits & 7);
}


bool bitbuffer::nbits(string s, int n, int & idx, unsigned int & out) {
# define NTHBIT(x) !! (s[(x) >> 3] & (1 << (7 - ((x) & 7))))

  out = 0;

  while(n--) {
    out <<= 1;
    /* check bounds */
    if ((unsigned)(idx >> 3) >= s.length()) return false;
    out |= NTHBIT(idx);
    idx ++;
  }
  return true;

# undef NTHBIT
}

string bitbuffer::getstring() {
  int n = ceil(bits);
  if (data) return string((char *)data, n);
  else return "";
}

void bitbuffer::writebits(int n, unsigned int b) {
  /* assumes position already holds 0 */
# define WRITEBIT(x, v) data[(x) >> 3] |= ((!!v) << (7 - ((x) & 7)))

  /* printf("writebits(%d, %d)\n", n, b); */

  for(int i = 0; i < n; i ++) {
    int bytes_needed = ceil(bits + 1);

    /* allocate more */
    if (bytes_needed > size) {
      int nsize = (size + 1) * 2;
      unsigned char * tmp = (unsigned char *) malloc(nsize * sizeof (unsigned char));
      if (!tmp) abort ();
      memset(tmp, 0, nsize);
      memcpy(tmp, data, size);
      free(data);
      data = tmp;
      size = nsize;
    }

    int bit = !!(b & (1 << (n - (i + 1))));
    /* printf("  write %d at %d\n", bit, bits); */
    WRITEBIT(bits, bit);
    bits++;
  }

# undef WRITEBIT
}

#if WIN32
// for ShellExecute
# include <shellapi.h>
# include <shlobj.h>
#endif

/* return true on success */
bool util::launchurl(const string & url) {
  /* XXX ??? */
#if 0
#ifdef OSX
  CFURLRef urlcfurl = CFURLCreateWithBytes(kCFAllocatorDefault, 
					   (const UInt8*)url.c_str(), 
					   (CFIndex)strlen(urlstring), 
					   kCFStringEncodingASCII, NULL);
  if (urlcfurl) {
      OSStatus status = LSOpenCFURLRef(urlcfurl, NULL);
      CFRelease(urlcfurl);
      return (status == noErr);
    }
  return 0;
#endif
#endif

#if WIN32
  return ((int)ShellExecute(NULL, "open", url.c_str(), 
			    NULL, NULL, SW_SHOWNORMAL)) > 32;
#endif

  /* otherwise.. */
  return false;
}


float util::randfrac() {
  return random() / (float)RAND_MAX;
}

/* XXX implement */
/* see http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/dataexchange/clipboard/usingtheclipboard.asp ... ugh. 

   better: http://www.libsdl.org/projects/scrap/
*/
bool util::setclipboard(string as) {
# ifdef WIN32
  /* handle = hdwp. but how to get one of those? */
  // SetClipboardData(CF_TEXT, as.c_str());
  return false;
  // return true;
# else
  /* how on other platforms? */
  return false;
# endif
}


/* XXX, could use better source of randomness (kernel)
   on systems that support it. But we don't have any
   real need for cryptographic randomness.

   web sequence numbers are chosen randomly, now, so we
   actually do. 
*/
int util::random () {
# ifdef WIN32
  return ::rand();
# else
  return ::random();
# endif
}

/* ensure that random is seeded */
struct randomseed {
  randomseed() {
# ifdef WIN32
    srand(time(0) ^ getpid());
# else
    srandom(time(0) ^ getpid());
# endif
    /* run it a bit */
    for(int i = 0; i < 256; i ++)
      (void)util::random();
  }
};

randomseed randomseed__unused;

