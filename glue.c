#include<Python.h>

typedef struct{
    PyObject *object;
    double val;
    // TODO : Pass Custom error message from C to GO.
    int err;
} PyRes;

void initialize(){
    Py_Initialize();
}

void finalize(){
    Py_Finalize();
}

void py_decref(PyObject *obj){
    Py_DECREF(obj);
}

const char *py_error(){
    PyObject *err = PyErr_Occurred();
    if (err == NULL){
        return NULL;
    }

    PyObject *str = PyObject_Str(err);
    const char *utf8 = PyUnicode_AsUTF8(str);
    Py_DECREF(str);
    return utf8;
}

PyRes call_func(PyObject *func, double x, double y){
    PyObject *args;
    PyObject *kwargs;
    PyObject *result = 0;
    PyRes retval = {NULL,-1,0};

    if(!PyCallable_Check(func)){
        printf("call_func: func is not a callable PyObject \n");
        retval.err = 1;
        goto fail;
    }

    args = Py_BuildValue("(dd)",x,y);
    kwargs = NULL;

    result = PyObject_Call(func, args, kwargs);

    Py_DECREF(args);
    Py_XDECREF(kwargs);

    if(PyErr_Occurred()){
        retval.err = 1;
        goto fail;
    }

    if (!PyFloat_Check(result)){
        printf("call_func: callable didn't return a float\n");
        retval.err = 1;
        goto fail;
    }

    retval.val = PyFloat_AsDouble(result);
    Py_DECREF(result);

    if(PyErr_Occurred()){
        retval.err = 1;
        goto fail;
    }

    return retval;
    fail:
        Py_XDECREF(result);
        return retval;
}


PyRes import_function(const char *modname, const char *symbol){
    PyObject *u_name, *module;
    PyRes res = {NULL,-1,0};
    char buffer[1024];
    u_name = PyUnicode_FromString(modname);
    if(u_name == NULL){
        printf("import_function: Couldnot convert string \"%s\" to unicode ", modname);
        res.err = 1;
        return res;
    }

    module = PyImport_Import(u_name);
    Py_DECREF(u_name);

    if(module == NULL){
        
        printf("import_function: Couldnot import module %s ",modname);
        res.err = 1;
        return res;
    }
    
    res.object = PyObject_GetAttrString(module,symbol);
    if(res.object == NULL){
        printf("import_function: Couldnot Get the %s attribute from %s module",symbol,modname);
        res.err = 1;
        return res;
    }
    Py_DECREF(module);
    return res;
}