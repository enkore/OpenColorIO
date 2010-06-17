/*
Copyright (c) 2003-2010 Sony Pictures Imageworks Inc., et al.
All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of Sony Pictures Imageworks nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <OpenColorSpace/OpenColorSpace.h>

#include "PyColorSpace.h"
#include "PyGroupTransform.h"
#include "PyUtil.h"

OCS_NAMESPACE_ENTER
{
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    bool AddColorSpaceObjectToModule( PyObject* m )
    {
        if ( PyType_Ready(&PyOCS_ColorSpaceType) < 0 ) return false;
        
        Py_INCREF( &PyOCS_ColorSpaceType );
        PyModule_AddObject(m, "ColorSpace",
                (PyObject *)&PyOCS_ColorSpaceType);
        
        return true;
    }
    
    PyObject * BuildConstPyColorSpace(ConstColorSpaceRcPtr colorSpace)
    {
        if (colorSpace.get() == 0x0)
        {
            PyErr_SetString(PyExc_ValueError, "Cannot create PyColorSpace from null object.");
            return NULL;
        }
        
        PyOCS_ColorSpace * pycolorSpace = PyObject_New(
                PyOCS_ColorSpace, (PyTypeObject * ) &PyOCS_ColorSpaceType);
        
        pycolorSpace->constcppobj = new OCS::ConstColorSpaceRcPtr();
        *pycolorSpace->constcppobj = colorSpace;
        
        pycolorSpace->cppobj = new OCS::ColorSpaceRcPtr();
        pycolorSpace->isconst = true;
        
        return ( PyObject * ) pycolorSpace;
    }
    
    PyObject * BuildEditablePyColorSpace(ColorSpaceRcPtr colorSpace)
    {
        if (colorSpace.get() == 0x0)
        {
            PyErr_SetString(PyExc_ValueError, "Cannot create PyColorSpace from null object.");
            return NULL;
        }
        
        PyOCS_ColorSpace * pycolorSpace = PyObject_New(
                PyOCS_ColorSpace, (PyTypeObject * ) &PyOCS_ColorSpaceType);
        
        pycolorSpace->constcppobj = new OCS::ConstColorSpaceRcPtr();
        pycolorSpace->cppobj = new OCS::ColorSpaceRcPtr();
        *pycolorSpace->cppobj = colorSpace;
        
        pycolorSpace->isconst = false;
        
        return ( PyObject * ) pycolorSpace;
    }
    
    bool IsPyColorSpace(PyObject * pyobject)
    {
        if(!pyobject) return false;
        return (PyObject_Type(pyobject) == (PyObject *) (&PyOCS_ColorSpaceType));
    }
    
    bool IsPyColorSpaceEditable(PyObject * pyobject)
    {
        if(!IsPyColorSpace(pyobject))
        {
            throw OCSException("PyObject must be an OCS::ColorSpace.");
        }
        
        PyOCS_ColorSpace * pycolorSpace = reinterpret_cast<PyOCS_ColorSpace *> (pyobject);
        return (!pycolorSpace->isconst);
    }
    
    ConstColorSpaceRcPtr GetConstColorSpace(PyObject * pyobject, bool allowCast)
    {
        if(!IsPyColorSpace(pyobject))
        {
            throw OCSException("PyObject must be an OCS::ColorSpace.");
        }
        
        PyOCS_ColorSpace * pycolorspace = reinterpret_cast<PyOCS_ColorSpace *> (pyobject);
        if(pycolorspace->isconst && pycolorspace->constcppobj)
        {
            return *pycolorspace->constcppobj;
        }
        
        if(allowCast && !pycolorspace->isconst && pycolorspace->cppobj)
        {
            return *pycolorspace->cppobj;
        }
        
        throw OCSException("PyObject must be a valid OCS::ColorSpace.");
    }
    
    ColorSpaceRcPtr GetEditableColorSpace(PyObject * pyobject)
    {
        if(!IsPyColorSpace(pyobject))
        {
            throw OCSException("PyObject must be an OCS::ColorSpace.");
        }
        
        PyOCS_ColorSpace * pycolorspace = reinterpret_cast<PyOCS_ColorSpace *> (pyobject);
        if(!pycolorspace->isconst && pycolorspace->cppobj)
        {
            return *pycolorspace->cppobj;
        }
        
        throw OCSException("PyObject must be an editable OCS::ColorSpace.");
    }
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    
    
    
    
    
    
    
    
    
    namespace
    {
        int PyOCS_ColorSpace_init( PyOCS_ColorSpace * self, PyObject * args, PyObject * kwds );
        void PyOCS_ColorSpace_delete( PyOCS_ColorSpace * self, PyObject * args );
        PyObject * PyOCS_ColorSpace_isEditable( PyObject * self );
        PyObject * PyOCS_ColorSpace_createEditableCopy( PyObject * self );
        
        PyObject * PyOCS_ColorSpace_getName( PyObject * self );
        PyObject * PyOCS_ColorSpace_setName( PyObject * self,  PyObject *args );
        PyObject * PyOCS_ColorSpace_getFamily( PyObject * self );
        PyObject * PyOCS_ColorSpace_setFamily( PyObject * self,  PyObject *args );
        
        PyObject * PyOCS_ColorSpace_getBitDepth( PyObject * self );
        PyObject * PyOCS_ColorSpace_setBitDepth( PyObject * self,  PyObject *args );
        PyObject * PyOCS_ColorSpace_isData( PyObject * self );
        PyObject * PyOCS_ColorSpace_setIsData( PyObject * self,  PyObject *args );
        PyObject * PyOCS_ColorSpace_getHWAllocation( PyObject * self );
        PyObject * PyOCS_ColorSpace_setHWAllocation( PyObject * self,  PyObject *args );
        PyObject * PyOCS_ColorSpace_getHWMin( PyObject * self );
        PyObject * PyOCS_ColorSpace_setHWMin( PyObject * self,  PyObject *args );
        PyObject * PyOCS_ColorSpace_getHWMax( PyObject * self );
        PyObject * PyOCS_ColorSpace_setHWMax( PyObject * self,  PyObject *args );
        
        PyObject * PyOCS_ColorSpace_getTransform( PyObject * self,  PyObject *args );
        PyObject * PyOCS_ColorSpace_getEditableTransform( PyObject * self,  PyObject *args );
        PyObject * PyOCS_ColorSpace_setTransform( PyObject * self,  PyObject *args );
        PyObject * PyOCS_ColorSpace_isTransformSpecified( PyObject * self,  PyObject *args );
        
        ///////////////////////////////////////////////////////////////////////
        ///
        
        PyMethodDef PyOCS_ColorSpace_methods[] = {
            {"isEditable", (PyCFunction) PyOCS_ColorSpace_isEditable, METH_NOARGS, "" },
            {"createEditableCopy", (PyCFunction) PyOCS_ColorSpace_createEditableCopy, METH_NOARGS, "" },
            
            {"getName", (PyCFunction) PyOCS_ColorSpace_getName, METH_NOARGS, "" },
            {"setName", PyOCS_ColorSpace_setName, METH_VARARGS, "" },
            {"getFamily", (PyCFunction) PyOCS_ColorSpace_getFamily, METH_NOARGS, "" },
            {"setFamily", PyOCS_ColorSpace_setFamily, METH_VARARGS, "" },
            
            {"getBitDepth", (PyCFunction) PyOCS_ColorSpace_getBitDepth, METH_NOARGS, "" },
            {"setBitDepth", PyOCS_ColorSpace_setBitDepth, METH_VARARGS, "" },
            {"isData", (PyCFunction) PyOCS_ColorSpace_isData, METH_NOARGS, "" },
            {"setIsData", PyOCS_ColorSpace_setIsData, METH_VARARGS, "" },
            {"getHWAllocation", (PyCFunction) PyOCS_ColorSpace_getHWAllocation, METH_NOARGS, "" },
            {"setHWAllocation", PyOCS_ColorSpace_setHWAllocation, METH_VARARGS, "" },
            {"getHWMin", (PyCFunction) PyOCS_ColorSpace_getHWMin, METH_NOARGS, "" },
            {"setHWMin", PyOCS_ColorSpace_setHWMin, METH_VARARGS, "" },
            {"getHWMax", (PyCFunction) PyOCS_ColorSpace_getHWMax, METH_NOARGS, "" },
            {"setHWMax", PyOCS_ColorSpace_setHWMax, METH_VARARGS, "" },
            
            {"getTransform", PyOCS_ColorSpace_getTransform, METH_VARARGS, "" },
            {"getEditableTransform", PyOCS_ColorSpace_getEditableTransform, METH_VARARGS, "" },
            {"setTransform", PyOCS_ColorSpace_setTransform, METH_VARARGS, "" },
            {"isTransformSpecified", PyOCS_ColorSpace_isTransformSpecified, METH_VARARGS, "" },
            
            {NULL}
        };
    }
    
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    PyTypeObject PyOCS_ColorSpaceType = {
        PyObject_HEAD_INIT(NULL)
        0,                                          //ob_size
        "OCS.ColorSpace",                           //tp_name
        sizeof(PyOCS_ColorSpace),                   //tp_basicsize
        0,                                          //tp_itemsize
        (destructor)PyOCS_ColorSpace_delete,        //tp_dealloc
        0,                                          //tp_print
        0,                                          //tp_getattr
        0,                                          //tp_setattr
        0,                                          //tp_compare
        0,                                          //tp_repr
        0,                                          //tp_as_number
        0,                                          //tp_as_sequence
        0,                                          //tp_as_mapping
        0,                                          //tp_hash 
        0,                                          //tp_call
        0,                                          //tp_str
        0,                                          //tp_getattro
        0,                                          //tp_setattro
        0,                                          //tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   //tp_flags
        "ColorSpace",                               //tp_doc 
        0,                                          //tp_traverse 
        0,                                          //tp_clear 
        0,                                          //tp_richcompare 
        0,                                          //tp_weaklistoffset 
        0,                                          //tp_iter 
        0,                                          //tp_iternext 
        PyOCS_ColorSpace_methods,                   //tp_methods 
        0,                                          //tp_members 
        0,                                          //tp_getset 
        0,                                          //tp_base 
        0,                                          //tp_dict 
        0,                                          //tp_descr_get 
        0,                                          //tp_descr_set 
        0,                                          //tp_dictoffset 
        (initproc) PyOCS_ColorSpace_init,           //tp_init 
        0,                                          //tp_alloc 
        PyType_GenericNew,                          //tp_new 
        0,                                          //tp_free
    };
    
    ///////////////////////////////////////////////////////////////////////////
    ///
    
    namespace
    {
        ///////////////////////////////////////////////////////////////////////
        ///
        int PyOCS_ColorSpace_init( PyOCS_ColorSpace *self, PyObject * /*args*/, PyObject * /*kwds*/ )
        {
            ///////////////////////////////////////////////////////////////////
            /// init pyobject fields
            
            self->constcppobj = new OCS::ConstColorSpaceRcPtr();
            self->cppobj = new OCS::ColorSpaceRcPtr();
            self->isconst = true;
            
            try
            {
                *self->cppobj = OCS::ColorSpace::Create();
                self->isconst = false;
                return 0;
            }
            catch ( const std::exception & e )
            {
                std::string message = "Cannot create colorSpace: ";
                message += e.what();
                PyErr_SetString( PyExc_RuntimeError, message.c_str() );
                return -1;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        void PyOCS_ColorSpace_delete( PyOCS_ColorSpace *self, PyObject * /*args*/ )
        {
            delete self->constcppobj;
            delete self->cppobj;
            
            self->ob_type->tp_free((PyObject*)self);
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_isEditable( PyObject * self )
        {
            return PyBool_FromLong(IsPyColorSpaceEditable(self));
        }
        
        PyObject * PyOCS_ColorSpace_createEditableCopy( PyObject * self )
        {
            try
            {
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                ColorSpaceRcPtr copy = colorSpace->createEditableCopy();
                return BuildEditablePyColorSpace( copy );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_getName( PyObject * self )
        {
            try
            {
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                return PyString_FromString( colorSpace->getName() );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCS_ColorSpace_setName( PyObject * self, PyObject * args )
        {
            try
            {
                char * name = 0;
                if (!PyArg_ParseTuple(args,"s:setName", &name)) return NULL;
                
                ColorSpaceRcPtr colorSpace = GetEditableColorSpace(self);
                colorSpace->setName( name );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_getFamily( PyObject * self )
        {
            try
            {
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                return PyString_FromString( colorSpace->getFamily() );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCS_ColorSpace_setFamily( PyObject * self, PyObject * args )
        {
            try
            {
                char * name = 0;
                if (!PyArg_ParseTuple(args,"s:setFamily", &name)) return NULL;
                
                ColorSpaceRcPtr colorSpace = GetEditableColorSpace(self);
                colorSpace->setFamily( name );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_getBitDepth( PyObject * self )
        {
            try
            {
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                return PyString_FromString( BitDepthToString( colorSpace->getBitDepth()) );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCS_ColorSpace_setBitDepth( PyObject * self, PyObject * args )
        {
            try
            {
                char * name = 0;
                if (!PyArg_ParseTuple(args,"s:setBitDepth", &name)) return NULL;
                
                ColorSpaceRcPtr colorSpace = GetEditableColorSpace(self);
                colorSpace->setBitDepth( BitDepthFromString( name ) );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_isData( PyObject * self )
        {
            try
            {
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                return PyBool_FromLong( colorSpace->isData() );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCS_ColorSpace_setIsData( PyObject * self, PyObject * args )
        {
            try
            {
                bool isData = false;
                if (!PyArg_ParseTuple(args,"O&:setIsData",
                    ConvertPyObjectToBool, &isData)) return NULL;
                
                ColorSpaceRcPtr colorSpace = GetEditableColorSpace(self);
                colorSpace->setIsData( isData );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_getHWAllocation( PyObject * self )
        {
            try
            {
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                return PyString_FromString( HwAllocationToString( colorSpace->getHWAllocation()) );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCS_ColorSpace_setHWAllocation( PyObject * self, PyObject * args )
        {
            try
            {
                HwAllocation hwalloc;
                if (!PyArg_ParseTuple(args,"O&:setHWAllocation",
                    ConvertPyObjectToHwAllocation, &hwalloc)) return NULL;
                
                ColorSpaceRcPtr colorSpace = GetEditableColorSpace(self);
                colorSpace->setHWAllocation( hwalloc );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_getHWMin( PyObject * self )
        {
            try
            {
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                return PyFloat_FromDouble( colorSpace->getHWMin() );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCS_ColorSpace_setHWMin( PyObject * self, PyObject * args )
        {
            try
            {
                float val = 0.0;
                if (!PyArg_ParseTuple(args,"f:setHWMin", &val)) return NULL;
                
                ColorSpaceRcPtr colorSpace = GetEditableColorSpace(self);
                colorSpace->setHWMin( val );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_getHWMax( PyObject * self )
        {
            try
            {
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                return PyFloat_FromDouble( colorSpace->getHWMax() );
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCS_ColorSpace_setHWMax( PyObject * self, PyObject * args )
        {
            try
            {
                float val = 0.0;
                if (!PyArg_ParseTuple(args,"f:setHWMax", &val)) return NULL;
                
                ColorSpaceRcPtr colorSpace = GetEditableColorSpace(self);
                colorSpace->setHWMax( val );
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_getTransform( PyObject * self,  PyObject *args )
        {
            try
            {
                ColorSpaceDirection dir;
                if (!PyArg_ParseTuple(args,"O&:getEditableTransform",
                    ConvertPyObjectToColorSpaceDirection, &dir)) return NULL;
                
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                ConstGroupTransformRcPtr transform = colorSpace->getTransform(dir);
                return BuildConstPyGroupTransform(transform);
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCS_ColorSpace_getEditableTransform( PyObject * self,  PyObject *args )
        {
            try
            {
                ColorSpaceDirection dir;
                if (!PyArg_ParseTuple(args,"O&:getEditableTransform",
                    ConvertPyObjectToColorSpaceDirection, &dir)) return NULL;
                
                ColorSpaceRcPtr colorSpace = GetEditableColorSpace(self);
                GroupTransformRcPtr transform = colorSpace->getEditableTransform(dir);
                return BuildEditablePyGroupTransform(transform);
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        PyObject * PyOCS_ColorSpace_setTransform( PyObject * self,  PyObject *args )
        {
        
            try
            {
                PyObject * pytransform = 0;
                ColorSpaceDirection dir;
                if (!PyArg_ParseTuple(args,"OO&:setTransform", &pytransform,
                    ConvertPyObjectToColorSpaceDirection, &dir)) return NULL;
                
                ConstGroupTransformRcPtr transform = GetConstGroupTransform(pytransform, true);
                ColorSpaceRcPtr colorSpace = GetEditableColorSpace(self);
                colorSpace->setTransform(transform, dir);
                
                Py_RETURN_NONE;
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        PyObject * PyOCS_ColorSpace_isTransformSpecified( PyObject * self,  PyObject *args )
        {
            try
            {
                ColorSpaceDirection dir;
                if (!PyArg_ParseTuple(args,"O&:getEditableTransform",
                    ConvertPyObjectToColorSpaceDirection, &dir)) return NULL;
                
                ConstColorSpaceRcPtr colorSpace = GetConstColorSpace(self, true);
                
                return PyBool_FromLong(colorSpace->isTransformSpecified(dir));
            }
            catch(...)
            {
                Python_Handle_Exception();
                return NULL;
            }
        }
        
        
    }

}
OCS_NAMESPACE_EXIT