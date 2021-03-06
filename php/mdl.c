#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_mdl.h"

static function_entry mdl_functions[] = {
    PHP_FE(distance_utf, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry mdl_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_MDL_EXTNAME,
    mdl_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_MDL_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MDL
ZEND_GET_MODULE(mdl)
#endif

PHP_FUNCTION(distance_utf)
{
	long block_size;
	long max_distance;
	zval *_s;
	zval *_t;
	zval **data;
	
	HashTable *s_hash, *t_hash;
	HashPosition pointer;

	long min, i, j, i1, j1, k, sl, half_sl, tl, half_tl, cost, *d, distance, del, ins, subs, transp, block, current_distance;
	long stop_execution = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aall", &_s, &_t , &block_size, &max_distance) == FAILURE) {
        RETURN_NULL();
    }
	
	s_hash = Z_ARRVAL_P(_s);
	t_hash = Z_ARRVAL_P(_t);
	sl = zend_hash_num_elements(s_hash);
	tl = zend_hash_num_elements(t_hash);
	
	long s[sl];
	long t[tl];
	
	i = 0;
    for(zend_hash_internal_pointer_reset_ex(s_hash, &pointer); zend_hash_get_current_data_ex(s_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(s_hash, &pointer)) {
		if (Z_TYPE_PP(data) != IS_LONG) {
			RETURN_NULL();
        } else {
			s[i] = Z_LVAL_PP(data);
		}
		i += 1;
    }

	i = 0;
	for(zend_hash_internal_pointer_reset_ex(t_hash, &pointer); zend_hash_get_current_data_ex(t_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(t_hash, &pointer)) {
		if (Z_TYPE_PP(data) != IS_LONG) {
			RETURN_NULL();
		} else {
			t[i] = Z_LVAL_PP(data);
		}
		i += 1;
	}
  sl++;
  tl++;
  
  //one-dimentional representation of 2 dimentional array len(s)+1 * len(t)+1
  d = malloc((sizeof(long))*(sl)*(tl));
  //populate 'horisonal' row
  for(i = 0; i < sl; i++){
    
  }
  //populate 'vertical' row starting from the 2nd position (first one is filled already)
  for(i = 0; i < tl; i++){
    d[i*sl] = i;
  }
  
  //fill up array with scores
  for(i = 1; i<sl; i++){
    d[i] = i;
    if (stop_execution == 1) break;
    current_distance = 10000;
    for(j = 1; j<tl; j++){
      
      cost = 1;
      if(s[i-1] == t[j-1]) cost = 0;
      
      half_sl = (sl - 1)/2;
      half_tl = (tl - 1)/2;
      
      block = block_size < half_sl ? block_size : half_sl;
      block = block < half_tl ? block : half_tl;
      
      while (block >= 1){   
        long swap1 = 1;
        long swap2 = 1;
        i1 = i - (block * 2);
        j1 = j - (block * 2);
        for (k = i1; k < i1 + block; k++) {
          if (s[k] != t[k + block]){
            swap1 = 0;
            break;
          }
        }
        for (k = j1; k < j1 + block; k++) {
          if (t[k] != s[k + block]){
            swap2 = 0;
            break;
          }
        }
        
        del = d[j*sl + i - 1] + 1; 
        ins = d[(j-1)*sl + i] + 1;
        min = del;
        if (ins < min) min = ins;
        //if (i == 2 && j==2) return LONG2NUM(swap2+5); 
        if (i >= block && j >= block && swap1 == 1 && swap2 == 1){
          transp = d[(j - block * 2) * sl + i - block * 2] + cost + block -1; 
          if (transp < min) min = transp;
          block = 0;
        } else if (block == 1) {
          subs = d[(j-1)*sl + i - 1] + cost;
          if (subs < min) min = subs;
        }
        block--;
      } 
      d[j*sl+i]=min;          
      if (current_distance > d[j*sl+i]) current_distance = d[j*sl+i];
    }
    if (current_distance > max_distance) {
      stop_execution = 1;
    }
  }
  distance=d[sl * tl - 1];
  if (stop_execution == 1) distance = current_distance;
  
  free(d);

	RETURN_LONG(distance);
}
