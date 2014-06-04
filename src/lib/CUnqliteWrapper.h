#ifndef CUNQLITEWRAPPER_H
#define CUNQLITEWRAPPER_H

extern "C" {
    #include "unqlite.h"
}

#include "document.h"
#include "stringbuffer.h"
#include "prettywriter.h"

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
 "$rc = db_store($coll, $zRec);"

#define JX9_PROG_GET_ALL_DOC_FROM_COLLECTION "/* Retrieve collection records */"\
 "if (!db_exists($coll)) {"\
 "  return;"\
 "}"\
 "$data = db_fetch_all($coll);"

#define JX9_PROG_GET_DOC_BY_ID "/* Retrieve collection records */"\
 "if (!db_exists($coll)) {"\
 "  return;"\
 "}"\
 "$data = db_fetch_by_id($coll, $id);"

#define JX9_PROG_REMOVE_DOCUMENT "/* Delete records from collection */"\
 "if (!db_exists($coll)) {"\
 "  return;"\
 "}"\
 "$rc = db_drop_record($coll, $id);"

namespace tam {

/**
 * @brief The CUnqliteWrapper class provides a higher level methods for getting and storing data from a unqlite database.
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
    bool storeDocument(std::string collection, rapidjson::Document &doc);

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
     * @return Returns the requested json objects.
     */
    rapidjson::Document fetchAll(std::string collection);

    /**
     * @brief Returns the specified json object from the database.
     * @param collection Collection name.
     * @param id Identifier of the json object.
     * @return Returns the specified json object.
     */
    rapidjson::Document fetchById(std::string collection, int id);

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

} // namespace tam
#endif // CUNQLITEWRAPPER_H
