/* $Id: synce_hash.c 3983 2011-03-21 17:23:56Z mark_ellis $ */

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "synce_hash.h"

/*
** public domain code by Jerry Coffin, with improvements by HenkJan Wolthuis.
** Modified 2002/02/09 to manage own creation and deletion of s_hash_table
** structures. The test was modified accordingly.
**
** Tested with Visual C 1.0 and Borland C 3.1.
** Compiles without warnings, and seems like it should be pretty
** portable.
*/


/* Initialize the SHashTable to the size asked for.  Allocates space
** for the correct number of pointers and sets them to NULL.  If it
** can't allocate sufficient memory, signals error by returning NULL.
*/

SHashTable *s_hash_table_new(SHashFunc hash_func, SCompareFunc compare_func, size_t size)
{
	SHashTable *self = calloc(1, sizeof (SHashTable));

  if (self)
  {
    self->size  = size;
    self->table = (bucket**)calloc(size, sizeof(bucket*));

    if ( self->table == NULL )
    {
      free (self);
      return NULL;
    }

    assert(hash_func);
    assert(compare_func);

    self->hash     = hash_func;
    self->equal  = compare_func;
  }

	return self;
}


/*
** Hashes a string to produce an unsigned short, which should be
** sufficient for most purposes.
*/

unsigned s_str_hash(const void* key)
{
      unsigned ret_val = 0;
      int i;
      const unsigned char *string = (const unsigned char*)key;

      while (*string)
      {
            i = tolower(*string); /* Wz 2002/02/10: case-insensitive hash */
            ret_val ^= i;
            ret_val <<= 1;
            string ++;
      }
      return ret_val;
}

int s_str_equal(const void* a, const void* b)
{
  return 0 == strcmp((const char*)a, (const char*)b);
}

int s_str_equal_no_case(const void* a, const void* b)
{
  return 0 == strcasecmp((const char*)a, (const char*)b);
}

/*
** Insert 'key' into hash table.
** Returns pointer to old data associated with the key, if any, or
** NULL if the key wasn't in the table previously.
*/

void *s_hash_table_insert(SHashTable *table, void *key, void *data)
{
      unsigned val = table->hash(key) % table->size;
      bucket *ptr;

      /*
      ** NULL means this bucket hasn't been used yet.  We'll simply
      ** allocate space for our new bucket and put our data there, with
      ** the table pointing at it.
      */

      if (NULL == (table->table)[val])
      {
            (table->table)[val] = (bucket *)malloc(sizeof(bucket));
            if (NULL==(table->table)[val])
                  return NULL;

            (table->table)[val] -> key = key;
            (table->table)[val] -> next = NULL;
            (table->table)[val] -> data = data;
            return (table->table)[val] -> data;
      }

      /*
      ** This spot in the table is already in use.  See if the current string
      ** has already been inserted, and if so, increment its count.
      */

      for (ptr = (table->table)[val];NULL != ptr; ptr = ptr -> next)
            if (table->equal(key, ptr->key))
            {
                  void *old_data;

                  old_data = ptr->data;
                  ptr -> data = data;
                  return old_data;
            }

      /*
      ** This key must not be in the table yet.  We'll add it to the head of
      ** the list at this spot in the hash table.  Speed would be
      ** slightly improved if the list was kept sorted instead.  In this case,
      ** this code would be moved into the loop above, and the insertion would
      ** take place as soon as it was determined that the present key in the
      ** list was larger than this one.
      */

      ptr = (bucket *)malloc(sizeof(bucket));
      if (NULL==ptr)
            return 0;
      ptr -> key = key;
      ptr -> data = data;
      ptr -> next = (table->table)[val];
      (table->table)[val] = ptr;
      return data;
}


/*
** Look up a key and return the associated data.  Returns NULL if
** the key is not in the table.
*/

void *s_hash_table_lookup(SHashTable *table, const void *key)
{
      unsigned val = table->hash(key) % table->size;
      bucket *ptr;

      if (NULL == (table->table)[val])
            return NULL;

      for ( ptr = (table->table)[val];NULL != ptr; ptr = ptr->next )
      {
            if (table->equal(key, ptr -> key ) )
                  return ptr->data;
      }
      return NULL;
}

/*
** Delete a key from the hash table and return associated
** data, or NULL if not present.
*/

void *s_hash_table_remove(SHashTable *table, const void *key)
{
      unsigned val = table->hash(key) % table->size;
      void *data;
      bucket *ptr, *last = NULL;

      if (NULL == (table->table)[val])
            return NULL;

      /*
      ** Traverse the list, keeping track of the previous node in the list.
      ** When we find the node to delete, we set the previous node's next
      ** pointer to point to the node after ourself instead.  We then delete
      ** the key from the present node, and return a pointer to the data it
      ** contains.
      */

      for (last = NULL, ptr = (table->table)[val];
            NULL != ptr;
            last = ptr, ptr = ptr->next)
      {
            if (0 == strcmp(key, ptr -> key))
            {
                  if (last != NULL )
                  {
                        data = ptr -> data;
                        last -> next = ptr -> next;
                        free(ptr);
                        return data;
                  }

                  /*
                  ** If 'last' still equals NULL, it means that we need to
                  ** delete the first node in the list. This simply consists
                  ** of putting our own 'next' pointer in the array holding
                  ** the head of the list.  We then dispose of the current
                  ** node as above.
                  */

                  else
                  {
                        data = ptr->data;
                        (table->table)[val] = ptr->next;
                        free(ptr);
                        return data;
                  }
            }
      }

      /*
      ** If we get here, it means we didn't find the item in the table.
      ** Signal this by returning NULL.
      */

      return NULL;
}


/*
** Frees a complete table by iterating over it and freeing each node.
** the second parameter is the address of a function it will call with a
** pointer to the data associated with each node.  This function is
** responsible for freeing the data, or doing whatever is needed with
** it. Pass "NULL" if you don't need to free anything.
*/

void s_hash_table_destroy(SHashTable *table, SHashTableDataDestroy func)
{
	/* Changed
	* enumerate( table, hashFreeNode);
	* here I expand the enumerate function into this function so I can
	* avoid the dodgy globals which prevent me from freeing nested hash
	* tables.  - Wz 2002/02/10
	*/

	unsigned i;
	bucket *temp;
	void *data;
	
  	if (!table)
		return;
	

	for (i=0;i<table->size; i++)
	{
		if ((table->table)[i] != NULL)
		{
			while ( (temp = (table->table)[i]) )
			{
				data = s_hash_table_remove(table, temp->key);
				if (func && data)
					func(data);
			}
		}
	}

	free(table->table);
	table->table = NULL;
	table->size = 0;

	free (table);
}

/*
** Simply invokes the function given as the second parameter for each
** node in the table, passing it the key and the associated data.
*/

void s_hash_table_foreach(SHashTable *table, SHashTableCallback func, void* cookie)
{
  unsigned i;
  bucket *temp;

  for (i=0;i<table->size; i++)
  {
    if ((table->table)[i] != NULL)
    {
      for (temp = (table->table)[i];
          NULL != temp;
          temp = temp -> next)
      {
        func(temp -> key, temp->data, cookie);
      }
    }
  }
}


#ifdef TEST

#include <stdio.h>

void printer(char *string, void *data)
{
      printf("%s: %s\n", string, (char *)data);
}

int main(void)
{
	SHashTable *table;
	
	char *strings[] = {
		"The first string",
		"The second string",
		"The third string",
		"The fourth string",
		"A much longer string than the rest in this example.",
		"The last string",
		NULL
		};
	
	char *junk[] = {
		"The first data",
		"The second data",
		"The third data",
		"The fourth data",
		"The fifth datum",
		"The sixth piece of data"
		};
	
	int i;
	void *j;
	
	table=construct_table(200);
	
	for (i = 0; NULL != strings[i]; i++ )
		insert(strings[i], junk[i], table);
	
	for (i=0;NULL != strings[i];i++)
	{
		printf("\n");
		enumerate(table, printer);
		del(strings[i],table);
	}
	
	for (i=0;NULL != strings[i];i++)
	{
		j = lookup(strings[i], table);
		if (NULL == j)
			  printf("\n'%s' is not in table",strings[i]);
		else  printf("\nERROR: %s was deleted but is still in table.",
			  strings[i]);
	}
	free_table(table, NULL);
	return 0;
}

#endif /* TEST */
