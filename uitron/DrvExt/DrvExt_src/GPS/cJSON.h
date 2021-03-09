/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

/* cJSON Types: */
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Number 3
#define cJSON_String 4
#define cJSON_Array 5
#define cJSON_Object 6

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct cJSON {
	struct cJSON *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct cJSON *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==cJSON_String */
	int valueint;				/* The item's number, if type==cJSON_Number */
	double valuedouble;			/* The item's number, if type==cJSON_Number */

	char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} cJSON;

typedef struct cJSON_Hooks {
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} cJSON_Hooks;

/* Supply malloc, realloc and free functions to cJSON */
extern void QH_cJSON_InitHooks(cJSON_Hooks* hooks);


/* Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished. */
extern cJSON *QH_cJSON_Parse(const char *value);
/* Render a cJSON entity to text for transfer/storage. Free the char* when finished. */
extern char  *QH_cJSON_Print(cJSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char  *QH_cJSON_PrintUnformatted(cJSON *item);
/* Render a cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
extern char *QH_cJSON_PrintBuffered(cJSON *item,int prebuffer,int fmt);
/* Delete a cJSON entity and all subentities. */
extern void   QH_cJSON_Delete(cJSON *c);

/* Returns the number of items in an array (or object). */
extern int	  QH_cJSON_GetArraySize(cJSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern cJSON *QH_cJSON_GetArrayItem(cJSON *array,int item);
/* Get item "string" from object. Case insensitive. */
extern cJSON *QH_cJSON_GetObjectItem(cJSON *object,const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
extern const char *QH_cJSON_GetErrorPtr(void);

/* These calls create a cJSON item of the appropriate type. */
extern cJSON *QH_cJSON_CreateNull(void);
extern cJSON *QH_cJSON_CreateTrue(void);
extern cJSON *QH_cJSON_CreateFalse(void);
extern cJSON *QH_cJSON_CreateBool(int b);
extern cJSON *QH_cJSON_CreateNumber(double num);
extern cJSON *QH_cJSON_CreateString(const char *string);
extern cJSON *QH_cJSON_CreateArray(void);
extern cJSON *QH_cJSON_CreateObject(void);

/* These utilities create an Array of count items. */
extern cJSON *QH_cJSON_CreateIntArray(const int *numbers,int count);
extern cJSON *QH_cJSON_CreateFloatArray(const float *numbers,int count);
extern cJSON *QH_cJSON_CreateDoubleArray(const double *numbers,int count);
extern cJSON *QH_cJSON_CreateStringArray(const char **strings,int count);

/* Append item to the specified array/object. */
extern void QH_cJSON_AddItemToArray(cJSON *array, cJSON *item);
extern void	QH_cJSON_AddItemToObject(cJSON *object,const char *string,cJSON *item);
extern void	QH_cJSON_AddItemToObjectCS(cJSON *object,const char *string,cJSON *item);	/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the cJSON object */
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
extern void QH_cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
extern void	QH_cJSON_AddItemReferenceToObject(cJSON *object,const char *string,cJSON *item);

/* Remove/Detatch items from Arrays/Objects. */
extern cJSON *QH_cJSON_DetachItemFromArray(cJSON *array,int which);
extern void   QH_cJSON_DeleteItemFromArray(cJSON *array,int which);
extern cJSON *QH_cJSON_DetachItemFromObject(cJSON *object,const char *string);
extern void   QH_cJSON_DeleteItemFromObject(cJSON *object,const char *string);

/* Update array items. */
extern void QH_cJSON_InsertItemInArray(cJSON *array,int which,cJSON *newitem);	/* Shifts pre-existing items to the right. */
extern void QH_cJSON_ReplaceItemInArray(cJSON *array,int which,cJSON *newitem);
extern void QH_cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);

/* Duplicate a cJSON item */
extern cJSON *QH_cJSON_Duplicate(cJSON *item,int recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
need to be released. With recurse!=0, it will duplicate any children connected to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */

/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
extern cJSON *QH_cJSON_ParseWithOpts(const char *value,const char **return_parse_end,int require_null_terminated);

extern void QH_cJSON_Minify(char *json);

/* Macros for creating things quickly. */
#define QH_cJSON_AddNullToObject(object,name)		QH_cJSON_AddItemToObject(object, name, QH_cJSON_CreateNull())
#define QH_cJSON_AddTrueToObject(object,name)		QH_cJSON_AddItemToObject(object, name, QH_cJSON_CreateTrue())
#define QH_cJSON_AddFalseToObject(object,name)		QH_cJSON_AddItemToObject(object, name, QH_cJSON_CreateFalse())
#define QH_cJSON_AddBoolToObject(object,name,b)	QH_cJSON_AddItemToObject(object, name, QH_cJSON_CreateBool(b))
#define QH_cJSON_AddNumberToObject(object,name,n)	QH_cJSON_AddItemToObject(object, name, QH_cJSON_CreateNumber(n))
#define QH_cJSON_AddStringToObject(object,name,s)	QH_cJSON_AddItemToObject(object, name, QH_cJSON_CreateString(s))

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue(object,val)			((object)?(object)->valueint=(object)->valuedouble=(val):(val))
#define cJSON_SetNumberValue(object,val)		((object)?(object)->valueint=(object)->valuedouble=(val):(val))

extern void* cJSON_Malloc(size_t length);
extern void cJSON_Free(void *ptr);
extern void cJSON_List_Blocks(int detail);
extern void cJSON_Init(char *mem_pool, int size);

#ifdef __cplusplus
}
#endif

#endif
