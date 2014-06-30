#include <stdexcept>
#include "CUnqliteWrapper.h"

CUnqliteWrapper::CUnqliteWrapper(std::string db_path) :
    m_Path(db_path), m_Rc(0), m_isOpened(false),
    m_Db(NULL), m_Vm(NULL), m_Scalar(NULL)
{
}

CUnqliteWrapper::~CUnqliteWrapper()
{
    if (m_isOpened)
        close();
}

bool CUnqliteWrapper::storeDocument(std::string collection, rapidjson::Document &doc)
{
    // prepare script
    std::string jx9 = JX9_PROG_STORE_DOCUMENT;
    {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        doc.Accept(writer);
        jx9.replace(jx9.find("%s"), 2, sb.GetString());
    }

    open();
    compileVm(jx9);
    passCollectionToVm(collection);
    executeVm();

    unqlite_value *resVal = unqlite_vm_extract_variable(m_Vm, "rc");
    int res = unqlite_value_to_int(resVal);

    unqlite_vm_release_value(m_Vm, resVal);
    unqlite_vm_release(m_Vm);
    close();
    return res ? true : false;
}

bool CUnqliteWrapper::removeDocumentById(std::string collection, int id)
{
    std::string jx9 = JX9_PROG_REMOVE_DOCUMENT;

    open();
    compileVm(jx9);
    passCollectionToVm(collection);
    passIdToVm(id);
    executeVm();

    unqlite_value *resVal = unqlite_vm_extract_variable(m_Vm, "rc");
    bool res = unqlite_value_to_bool(resVal);

    unqlite_vm_release_value(m_Vm, resVal);
    unqlite_vm_release(m_Vm);
    close();
    return res;
}

void CUnqliteWrapper::fetchAll(std::string collection, rapidjson::Document &doc)
{
    std::string jx9 = JX9_PROG_GET_ALL_DOC_FROM_COLLECTION;

    open();
    compileVm(jx9);
    passCollectionToVm(collection);
    executeVm();

    unqlite_value *resVal = unqlite_vm_extract_variable(m_Vm, "data");
    {
        std::string res = unqlite_value_to_string(resVal, NULL);
        doc.Parse<0>(res.c_str());
    }

    unqlite_vm_release_value(m_Vm, resVal);
    unqlite_vm_release(m_Vm);
    close();
}

void CUnqliteWrapper::fetchById(std::string collection, int id, rapidjson::Document &doc)
{
    std::string jx9 = JX9_PROG_GET_DOC_BY_ID;

    open();
    compileVm(jx9);
    passCollectionToVm(collection);
    passIdToVm(id);
    executeVm();

    unqlite_value *resVal = unqlite_vm_extract_variable(m_Vm, "data");
    {
        std::string res = unqlite_value_to_string(resVal, NULL);
        if (res.empty()) {
            unqlite_vm_release_value(m_Vm, resVal);
            unqlite_vm_release(m_Vm);
            close();
            return;
        }
        doc.Parse<0>(res.c_str());
    }

    unqlite_vm_release_value(m_Vm, resVal);
    unqlite_vm_release(m_Vm);
    close();
}

void CUnqliteWrapper::open()
{
    if (m_isOpened)
        return;

    /* Open our database */
    m_Rc = unqlite_open(&m_Db, m_Path.c_str(), UNQLITE_OPEN_CREATE);
    if (m_Rc != UNQLITE_OK){
        unqlite_lib_shutdown();
        throw std::runtime_error("Out of memory");
    }

    m_isOpened = true;
}

void CUnqliteWrapper::close()
{
    if (!m_isOpened)
        return;

    unqlite_close(m_Db);
    m_isOpened = false;
}

void CUnqliteWrapper::compileVm(std::string &script)
{
    if (!m_isOpened)
        throw std::runtime_error("Database is not avalaible");

    m_Rc = unqlite_compile(m_Db, script.c_str(), script.length(), &m_Vm);
    if (m_Rc != UNQLITE_OK) {
        /* Compile error, extract the compiler error log */
        const char *zBuf;
        int iLen;
        /* Extract error log */
        unqlite_config(m_Db, UNQLITE_CONFIG_JX9_ERR_LOG, &zBuf, &iLen);
        unqlite_lib_shutdown();
        throw std::runtime_error("Jx9 compile error: " + std::string(zBuf));
    }
}

void CUnqliteWrapper::passCollectionToVm(std::string collection)
{
    m_Scalar = unqlite_vm_new_scalar(m_Vm);
    if (m_Scalar == 0) {
        throw std::runtime_error("Cannot create foreign variable");
    }

    unqlite_value_string(m_Scalar, collection.c_str(), -1);
    m_Rc = unqlite_vm_config(
         m_Vm,
         UNQLITE_VM_CONFIG_CREATE_VAR, /* Create variable command */
         "coll", /* Variable name (without the dollar sign) */
         m_Scalar   /* Value */
         );

    if (m_Rc != UNQLITE_OK) {
        unqlite_lib_shutdown();
        throw std::runtime_error("Error while installing $coll");
    }

    unqlite_value_reset_string_cursor(m_Scalar);
    unqlite_vm_release_value(m_Vm, m_Scalar);
}

void CUnqliteWrapper::passIdToVm(int id)
{
    m_Scalar = unqlite_vm_new_scalar(m_Vm);
    if (m_Scalar == 0) {
        throw std::runtime_error("Cannot create foreign variable");
    }

    unqlite_value_int(m_Scalar, id);
    m_Rc = unqlite_vm_config(
         m_Vm,
         UNQLITE_VM_CONFIG_CREATE_VAR, /* Create variable command */
         "id", /* Variable name (without the dollar sign) */
         m_Scalar   /* Value */
         );

    if (m_Rc != UNQLITE_OK) {
        unqlite_lib_shutdown();
        throw std::runtime_error("Error while installing $id");
    }

    unqlite_vm_release_value(m_Vm, m_Scalar);
}

void CUnqliteWrapper::executeVm()
{
    m_Rc = unqlite_vm_exec(m_Vm);
    if (m_Rc != UNQLITE_OK) {
        const char *zErr;
        int iLen = 0;
        unqlite_config(m_Db, UNQLITE_CONFIG_ERR_LOG, &zErr, &iLen);
        unqlite_lib_shutdown();
        throw std::runtime_error(std::string(zErr));
    }
}
