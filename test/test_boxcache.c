#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "../boxcache.h"
#include "../boxutils.h"

#define RM_BINARY "/bin/rm"
#define TOUCH_BINARY "/bin/touch"

char * cache_dir;

void global_setup(void)
{
	char * buf = strdup("/tmp/boxfs-"__FILE__"-XXXXXX");
	cache_dir = mkdtemp(buf);
	printf("Cache directory: %s\n", cache_dir);
	if(!cache_dir) exit(1);
	if(cache_dir != buf) exit(2);
	// Set cache expiration to 1 hour
	cache_init(cache_dir, 60);
}

void global_teardown(void)
{
	// Remove the cache directory (and its contents)
	const char* cmd = RM_BINARY" -rf ";
	char *buf = malloc(strlen(cmd)+strlen(cache_dir)+1);
	strcpy(buf, cmd);
	strcat(buf, cache_dir);
	system(buf);
}

// Set modified time of cache key to the UNIX Epoch
// TODO dynamically set this to (now - expiration time - 1)
// to better test the expiration transition point
void expire_cache_key(const char * key)
{
	char * path = pathappend(cache_dir, key);
	const char* cmd = TOUCH_BINARY" -m -t '197001010000.00' ";
	char *buf = malloc(strlen(cmd)+strlen(path)+1);
	strcpy(buf, cmd);
	strcat(buf, path);
	system(buf);
}

void test_setup(void)
{
	return;
}

void test_teardown(void)
{
	return;
}

MU_TEST(get_missing_key)
#undef __TEST__
#define __TEST__ "get_missing_key"
{
	char * ret;

	ret = cache_get(__TEST__);
	mu_assert(ret == NULL, "_get should return NULL when key is missing");
}

MU_TEST(put_get_missing_key)
#undef __TEST__
#define __TEST__ "put_get_missing_key"
{
	char * ret;
	const char * key = __TEST__;
	const char * value = __TEST__"-value";

	cache_put(key, value);
	ret = cache_get(key);
	mu_assert(ret != NULL, "_get should not return NULL");
	mu_assert(strcmp(ret,value) == 0, "_get should return the value set by _put");
}

MU_TEST(put_get_existing_key)
#undef __TEST__
#define __TEST__ "put_get_existing_key"
{
	char * ret;
	const char * key = __TEST__;
	const char * value1 = __TEST__"-value1";
	const char * value2 = __TEST__"-value2";

	// Put when the key is missing
	cache_put(key, value1);
	ret = cache_get(key);
	mu_assert(ret != NULL, "_get should not return NULL");
	mu_assert(strcmp(ret,value1) == 0, "_get should return the value set by _put");

	// Put when the key is existing
	cache_put(key, value2);
	ret = cache_get(key);
	mu_assert(ret != NULL, "_get should not return NULL");
	mu_assert(strcmp(ret,value2) == 0, "_get should return the value set by _put");
}


MU_TEST(rm_get_missing_key)
#undef __TEST__
#define __TEST__ "rm_get_missing_key"
{
	char * ret;
	const char * key = __TEST__;

	cache_rm(key);
	ret = cache_get(key);
	mu_assert(ret == NULL, "_get should return NULL when key is missing");
}

MU_TEST(rm_get_existing_key)
#undef __TEST__
#define __TEST__ "rm_get_existing_key"
{
	char * ret;
	const char * key = __TEST__;
	const char * value = __TEST__"-value";

	// Put the key
	cache_put(key, value);
	ret = cache_get(key);
	mu_assert(ret != NULL, "_get should not return NULL");
	mu_assert(strcmp(ret,value) == 0, "_get should return the value set by _put");

	// Remove the now-existing key
	cache_rm(key);
	ret = cache_get(key);
	mu_assert(ret == NULL, "_get should return NULL after key removed");
}

MU_TEST(get_expired_key)
#undef __TEST__
#define __TEST__ "get_expired_key"
{
	char * ret;
	const char * key = __TEST__;
	const char * value = __TEST__"-value";

	// Put the key
	cache_put(key, value);
	ret = cache_get(key);
	mu_assert(ret != NULL, "_get should not return NULL");
	mu_assert(strcmp(ret,value) == 0, "_get should return the value set by _put");

	// Force key to be expired
	expire_cache_key(key);

	// Read the now-expired key
	ret = cache_get(key);
	mu_assert(ret == NULL, "_get should return NULL for expired key");
}

MU_TEST(put_get_expired_key)
#undef __TEST__
#define __TEST__ "put_get_expired_key"
{
	char * ret;
	const char * key = __TEST__;
	const char * value = __TEST__"-value";
	const char * value2 = __TEST__"-value2";

	// Put the key
	cache_put(key, value);
	ret = cache_get(key);
	mu_assert(ret != NULL, "_get should not return NULL");
	mu_assert(strcmp(ret,value) == 0, "_get should return the value set by _put");

	// Force key to be expired
	expire_cache_key(key);

	// Update the expired key
	cache_put(key, value2);
	// Read the updated key
	ret = cache_get(key);
	mu_assert(ret != NULL, "_get should not return NULL");
	mu_assert(strcmp(ret,value2) == 0, "_get should return the value set by _put");
}

MU_TEST(rm_expired_key)
#undef __TEST__
#define __TEST__ "rm_expired_key"
{
	char * ret;
	const char * key = __TEST__;
	const char * value = __TEST__"-value";

	// Put the key
	cache_put(key, value);
	ret = cache_get(key);
	mu_assert(ret != NULL, "_get should not return NULL");
	mu_assert(strcmp(ret,value) == 0, "_get should return the value set by _put");

	// Force key to be expired
	expire_cache_key(key);

	// Remove the expired key
	cache_rm(key);
	// Read the removed key
	ret = cache_get(key);
	mu_assert(ret == NULL, "_get should return NULL when key is missing");
}

MU_TEST_SUITE(basic_ops)
{
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(get_missing_key);
	MU_RUN_TEST(put_get_missing_key);
	MU_RUN_TEST(put_get_existing_key);
	MU_RUN_TEST(rm_get_missing_key);
	MU_RUN_TEST(rm_get_existing_key);
}

MU_TEST_SUITE(auto_expire)
{
	MU_RUN_TEST(get_expired_key);
	MU_RUN_TEST(put_get_expired_key);
	MU_RUN_TEST(rm_expired_key);
}

int main(void)
{
	global_setup();
	MU_RUN_SUITE(basic_ops);
	MU_RUN_SUITE(auto_expire);
	MU_REPORT();
	global_teardown();
	return 0;
}
