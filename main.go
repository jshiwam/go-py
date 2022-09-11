package main

/*
#cgo pkg-config: python3
#cgo LDFLAGS: -lpython3.8

#include "glue.c"
*/
import "C"
import (
	"fmt"
	"log"
	"unsafe"
)

func Py_Initialize() {
	C.initialize()
}
func Py_Finalize() {
	C.finalize()
}
func pyError() error {
	pyerr := C.py_error()

	if pyerr == nil {
		return nil
	}

	err := C.GoString(pyerr)

	return fmt.Errorf("%s", err)
}
func main() {
	Py_Initialize()
	defer Py_Finalize()
	fn, err := importPyFunc("Rectangle", "area")
	if err != nil {
		log.Fatal(err)
		return
	}
	x := 7.89
	y := 12.24
	area, err := callPyFunc(fn, x, y)
	if err != nil {
		log.Fatal(err)
		return
	}
	fmt.Printf("Area is %f\n", area)
}

func importPyFunc(moduleName, symbol string) (*C.PyObject, error) {
	cMod := C.CString(moduleName)
	cFunc := C.CString(symbol)

	defer func() {
		C.free(unsafe.Pointer(cMod))
		C.free(unsafe.Pointer(cFunc))
	}()

	fn := C.import_function(cMod, cFunc)
	if fn.err == 1 {
		return nil, pyError()
	}

	return fn.object, nil
}

func close(fn *C.PyObject) {
	if fn == nil {
		return
	}
	C.py_decref(fn)
	fn = nil
}

func callPyFunc(fun *C.PyObject, x float64, y float64) (float64, error) {
	width := C.double(x)
	height := C.double(y)

	area := C.call_func(fun, width, height)

	close(fun)

	if area.err == 1 {
		return -1, pyError()
	}
	result := float64(area.val)
	return result, nil
}
