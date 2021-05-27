#ifndef XXqtDefine_h
#define XXqtDefine_h

#define XX_KEY(name) const static char * const name;
#define XX_KEY_C(cls,name,value) const char * const cls::name = value;

#endif
