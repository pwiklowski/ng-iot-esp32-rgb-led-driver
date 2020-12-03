#ifndef __MAIN_H__
#define __MAIN_H__

#define TOKEN_MAX_LEN 1500
#define REFRESH_TOKEN_MAX_LEN 500

typedef struct {
  char access_token[TOKEN_MAX_LEN];
  size_t access_token_length;
  char refresh_token[REFRESH_TOKEN_MAX_LEN];
  size_t refresh_token_length;
  uint64_t valid_to;

} Config_t;


void save_config();

#endif // __MAIN_H__