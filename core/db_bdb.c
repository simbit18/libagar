/*
 * Copyright (c) 2012 Hypertriton, Inc. <http://hypertriton.com/>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Berkeley DB database access.
 */

#include <config/have_db4.h>
#ifdef HAVE_DB4

#include <core/core.h>
#include <db.h>

typedef struct ag_db_hash_bt {
	struct ag_db _inherit;
	DB *pDB;
} AG_DbHashBT;

static const struct {
	const char *name;
	Uint32 mask;
} bdbOptions[] = {
	{ "db-create",			DB_CREATE },
	{ "db-create-excl",		DB_EXCL },
	{ "db-auto-commit",		DB_AUTO_COMMIT },
	{ "db-threaded",		DB_THREAD },
	{ "db-truncate",		DB_TRUNCATE },
#ifdef DB_MULTIVERSION
	{ "db-multiversion",		DB_MULTIVERSION },
#endif
#ifdef DB_NOMMAP
	{ "db-no-mmap",			DB_NOMMAP },
#endif
#ifdef DB_READ_UNCOMMITTED
	{ "db-read-uncommitted",	DB_READ_UNCOMMITTED },
#endif
};
static const int bdbOptionCount = sizeof(bdbOptions)/sizeof(bdbOptions[0]);

static void
Init(void *obj)
{
	AG_DbHashBT *db = obj;
	int i;

	for (i = 0; i < bdbOptionCount; i++)
		AG_SetInt(db, bdbOptions[i].name, 0);
}

static int
Open(void *obj, const char *path, Uint flags)
{
	AG_DbHashBT *db = obj;
	Uint32 dbFlags = 0;
	int i, rv;
	DBTYPE dbtype;

	if (strcmp(AGDB_CLASS(db)->name, "hash") == 0) {
		dbtype = DB_HASH;
	} else {
		dbtype = DB_BTREE;
	}
	if ((rv = db_create(&db->pDB, NULL, 0)) != 0) {
		AG_SetError("db_create: %s", db_strerror(rv));
		return (-1);
	}
	for (i = 0; i < bdbOptionCount; i++) {
		if (AG_GetInt(db, bdbOptions[i].name))
			dbFlags |= bdbOptions[i].mask;
	}
	if (flags & AG_DB_READONLY) {
		dbFlags |= DB_RDONLY;
	}
	rv = db->pDB->open(db->pDB, NULL, path, NULL, dbtype, dbFlags, 0);
	if (rv != 0) {
		AG_SetError("db_open %s: %s", path, db_strerror(rv));
		return (-1);
	}
	return (0);
}

static void
Close(void *obj)
{
	AG_DbHashBT *db = obj;
	int rv;
	
	if ((rv = db->pDB->close(db->pDB, 0)) != 0)
		AG_Verbose("db_close: %s; ignoring", db_strerror(rv));
}

static int
Sync(void *obj)
{
	AG_DbHashBT *db = obj;
	int rv;
	
	if ((rv = db->pDB->sync(db->pDB, 0)) != 0) {
		AG_SetError("db_sync: %s", db_strerror(rv));
		return (-1);
	}
	return (0);
}

static int
Exists(void *obj, AG_DbEntry *dbe)
{
	AG_DbHashBT *db = obj;
	DBT key;
	int rv;
	
	memset(&key, 0, sizeof(DBT));
	key.data = dbe->key;
	key.size = dbe->keySize;
	if ((rv = db->pDB->exists(db->pDB, NULL, &key, 0)) != DB_NOTFOUND) {
		return (1);
	}
	return (0);
}

static int
Get(void *obj, AG_DbEntry *dbe)
{
	AG_DbHashBT *db = obj;
	DBT key, data;
	int rv;

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = dbe->key;
	key.size = dbe->keySize;
	if ((rv = db->pDB->get(db->pDB, NULL, &key, &data, 0)) != 0) {
		AG_SetError("db_get: %s", db_strerror(rv));
		return (-1);
	}
	dbe->key = (void *)key.data;
	dbe->keySize = (size_t)key.size;
	dbe->data = (void *)data.data;
	dbe->dataSize = (size_t)data.size;
	return (0);
}
		
static int
Put(void *obj, AG_DbEntry *dbe)
{
	AG_DbHashBT *db = obj;
	DBT key, data;
	int rv;

	memset(&key, 0, sizeof(DBT));
	key.data = dbe->key;
	key.size = dbe->keySize;
	memset(&data, 0, sizeof(DBT));
	data.data = dbe->data;
	data.size = dbe->dataSize;
	if ((rv = db->pDB->put(db->pDB, NULL, &key, &data, 0)) != 0) {
		AG_SetError("db_put: %s", db_strerror(rv));
		return (-1);
	}
	return (0);
}

static int
Del(void *obj, AG_DbEntry *dbe)
{
	AG_DbHashBT *db = obj;
	DBT key;
	int rv;

#if 0
	if ((rv = db->pDB->get(db->pDB, NULL, &key, &data, 0)) == DB_NOTFOUND) {
		AG_SetError("db_del: No such entry");
		return (-1);
	}
#endif
	memset(&key, 0, sizeof(DBT));
	key.data = dbe->key;
	key.size = dbe->keySize;
	if ((rv = db->pDB->del(db->pDB, NULL, &key, 0)) != 0) {
		AG_SetError("DB Delete: %s", db_strerror(rv));
		return (-1);
	}
	return (0);
}

static int
Iterate(void *obj, AG_DbIterateFn fn, void *arg)
{
	AG_DbHashBT *db = obj;
	DBC *c;
	DBT key, data;
	int rv;

	db->pDB->cursor(db->pDB, NULL, &c, 0);
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	while ((rv = c->c_get(c, &key, &data, DB_NEXT)) == 0) {
		AG_DbEntry dbe;

		dbe.db = (AG_Db *)db;
		dbe.key = key.data;
		dbe.keySize = key.size;
		dbe.data = data.data;
		dbe.dataSize = data.size;

		if (fn(&dbe, arg) == -1) {
			c->c_close(c);
			return (-1);
		}
	}
	c->c_close(c);

	if (rv != DB_NOTFOUND) {
		AG_SetError("c_get: %s", db_strerror(rv));
		return (-1);
	} else {
		return (0);
	}
}

AG_DbClass agDbHashClass = {
	{
		"Agar(Db:DbHash)",
		sizeof(AG_DbHashBT),
		{ 0,0 },
		Init,
		NULL,		/* free */
		NULL,		/* destroy */
		NULL,		/* load */
		NULL,		/* save */
		NULL		/* edit */
	},
	"hash",
	N_("Extended Linear Hashing"),
	AG_DB_KEY_DATA,		/* Key is variable data */
	AG_DB_REC_VARIABLE,	/* Variable-sized records */
	Open,
	Close,
	Sync,
	Exists,
	Get,
	Put,
	Del,
	Iterate
};
AG_DbClass agDbBtreeClass = {
	{
		"Agar(Db:DbHash)",
		sizeof(AG_DbHashBT),
		{ 0,0 },
		Init,
		NULL,		/* free */
		NULL,		/* destroy */
		NULL,		/* load */
		NULL,		/* save */
		NULL		/* edit */
	},
	"btree",
	N_("Sorted, Balanced Tree Structure"),
	AG_DB_KEY_DATA,		/* Key is variable data */
	AG_DB_REC_VARIABLE,	/* Variable-sized records */
	Open,
	Close,
	Sync,
	Exists,
	Get,
	Put,
	Del,
	Iterate
};

#endif /* HAVE_DB4 */