#ifndef CUNQLITEWRAPPER_H
#define CUNQLITEWRAPPER_H

extern "C" {
    #include "unqlite/unqlite.h"
}

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <iostream>

#define JX9_PROG_STORE_DOCUMENT "/* Choosen collection passed with $coll variable. */"\
 "if (!db_exists($coll)) {"\
 "  $rc = db_create($coll);"\
 "  if (!$rc){"\
 "    /*Handle error*/"\
 "    /*print db_errlog();*/"\
 "    return;"\
 "  }"\
 "}"\
 "/* Record to store the string is replaced with the proper json structure */ "\
 "$zRec = %s;"\
 "if ($coll == 'score' || $coll == 'coverage-statistics' || $coll == 'results-statistics')"\
 "  $zRec = json_encode($zRec);"\
 "$rc = db_store($coll, $zRec);"\
 "$recordId = db_last_record_id($coll);"

#define JX9_PROG_GET_ALL_DOC_FROM_COLLECTION "/* Retrieve collection records */"\
 "if (!db_exists($coll)) {"\
 "  $data = \"\";"\
 "  return;"\
 "}"\
 "$data = db_fetch_all($coll);"

#define JX9_PROG_GET_DOC_BY_ID "/* Retrieve collection records */"\
 "if (!db_exists($coll)) {"\
 "  $data = \"\";"\
 "  return;"\
 "}"\
 "$data = db_fetch_by_id($coll, $id);"

#define JX9_PROG_REMOVE_DOCUMENT "/* Delete records from collection */"\
 "if (!db_exists($coll)) {"\
 "  $rc = false;"\
 "  return;"\
 "}"\
 "$rc = db_drop_record($coll, $id);"

#define JX9_PROG_DROP_COLLECTION "/* Choosen collection passed with $coll variable. */"\
 "$rc = db_drop_collection($coll);"

/**
 * @brief The CUnqliteWrapper class provides methods for getting and storing data from a unqlite database.
 */
class CUnqliteWrapper
{
public:

    /**
     * @brief CUnqliteWrapper, constructor.
     * @param db_path Path to the unqlite database file
     */
    CUnqliteWrapper(std::string db_path);
    ~CUnqliteWrapper();

    /**
     * @brief Stores a json object in the specified collection of the database file.
     * @param collection Collection name.
     * @param doc Json object.
     * @return True if the operation was successful.
     */
    bool storeDocument(std::string collection, rapidjson::Document &doc, int *recordId = NULL);

    /**
     * @brief Removes a json object from the specified collection.
     * @param collection Collection name.
     * @param id Id of the json object.
     * @return True if the operation was successful.
     */
    bool removeDocumentById(std::string collection, int id);

    /**
     * @brief Returns all json objects from the specified collection.
     * @param collection Collection name.
     * @param Returns the requested json objects.
     */
    void fetchAll(std::string collection, rapidjson::Document &doc);

    /**
     * @brief Returns the specified json object from the database.
     * @param collection Collection name.
     * @param id Identifier of the json object.
     * @param doc Returns the specified json object.
     */
    void fetchById(std::string collection, int id, rapidjson::Document &doc);

    /**
     * @brief Drops the specified collection from the database.
     * @param collection Collection name.
     */
    bool dropCollection(std::string collection);

private:

    /**
     * @brief Opens the specified database file and compiles the virtual machine.
     * @throw Throws runtimer_error when can't open the database file.
     */
    void open();

    /**
     * @brief Closes database file.
     */
    void close();

    /**
     * @brief Compiles the given script into a byte code.
     * @param script Script to compile.
     * @throw Throws runtimer_error at library error.
     */
    void compileVm(std::string &script);

    /**
     * @brief Passes a collection name to the virtual machine.
     * @param collection Name of the collection
     * @throw Throws runtimer_error at library error.
     */
    void passCollectionToVm(std::string collection);

    /**
     * @brief Passes an document id to the virtual machine.
     * @param id Document identifier.
     * @throw Throws runtimer_error at library error.
     */
    void passIdToVm(int id);

    /**
     * @brief Runs the compiled script.
     * @throw Throws runtimer_error at library error.
     */
    void executeVm();

private:

    /**
     * @brief Path to the database file.
     */
    std::string m_Path;

    /**
     * @brief Global return code.
     */
    int m_Rc;

    /**
     * @brief True if the database is opened.
     */
    bool m_isOpened;

    /**
     * @brief Resource to the database file.
     */
    unqlite *m_Db;

    /**
     * @brief Virtual machine which processes Jx9 scripts.
     */
    unqlite_vm *m_Vm;

    /**
     * @brief Used for passing foreign variables to script.
     */
    unqlite_value *m_Scalar;

};

#endif // CUNQLITEWRAPPER_H
