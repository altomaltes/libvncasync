#ifndef _BASE64_H
#define _BASE64_H

extern int __b64_ntop(unsigned char const *src, size_t srclength, char *target, size_t targsize);
extern int __b64_pton(char const *src, unsigned char *target, size_t targsize);

#define rfbBase64NtoP __b64_ntop
#define rfbBase64PtoN __b64_pton

#endif /* _BASE64_H */
