#ifndef SWF_UTF8_H_INCLUDED
#define SWF_UTF8_H_INCLUDED

int UTF8Length(const char *string);
unsigned short UTF8GetChar(const char** strptr);
int UTF8ExpandString(const char* string, unsigned short** outstr);

#endif /* SWF_UTF8_H_INCLUDED */
