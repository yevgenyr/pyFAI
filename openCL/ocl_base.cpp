/*
 *   Project: OpenCL Framework and kernels for pyFAI.
 *
 *   List of files: ocl_base.cpp
 *                  ocl_base.hpp
 *                  ocl_xrpd1d_fullsplit.cpp
 *                  ocl_xrpd2d_fullsplit.cpp
 *                  ocl_xrpd1d.hpp
 *                  ocl_xrpd2d.hpp
 *                  ocl_tools.h
 *                  ocl_ckerr.h
 *                  ocl_azim_kernel_2.cl
 *                  ocl_azim_kernel2D_2.cl
 *                  ocl_xrpd1d.i
 *                  ocl_xrpd2d.i
 *                  setup_xrpd1d.py
 *                  setup_xrpd2d.py
 *
 *   Copyright (C) 2011-12 European Synchrotron Radiation Facility
 *                           Grenoble, France
 *
 *   Principal authors: D. Karkoulis (karkouli@esrf.fr)
 *   Last revision: 11/05/2011
 *    
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   and the GNU Lesser General Public License  along with this program.
 *   If not, see <http://www.gnu.org/licenses/>.
 */

/* OpenCL library for acceleration of Azimuthal regroupping */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "ocl_base.hpp"

#define CE CL_CHECK_ERR_PR
#define C  CL_CHECK_PR

#define CEN CL_CHECK_ERR_PRN
#define CN  CL_CHECK_PRN

#define CER CL_CHECK_ERR_PR_RET
#define CR  CL_CHECK_PR_RET

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//#define silent
#ifdef _SILENT
  #define fprintf(stream,...)
#endif


typedef unsigned long lui;

/**
 * \brief Overloaded constructor for base class.
 *
 * Output is set to file "fname"
 *
 */
ocl::ocl(const char *fname){
  ContructorInit();
  setDocstring("OpenCL base functionality for xrpd1d. \nFeel free to play around but you will not be able to perform integrations"\
                "at this level.\nYou may check the OpenCL platforms and devices found in your system at this point. Try print_devices\n"\
                "Try any of the derived classes xrpd1d and xrpd2d for complete functionality. \n","ocl_base.hpp");
  stream=fopen(fname,"w");
  usesStdout=0;

}

/**
 * \brief Default constructor for base class.
 *
 * Output is set to stdout
 *
 */
ocl::ocl(){
  ContructorInit();
  setDocstring("OpenCL base functionality for xrpd1d. \nFeel free to play around but you will not be able to perform integrations"\
                "at this level.\nYou may check the OpenCL platforms and devices found in your system at this point. Try print_devices\n"\
                "Try any of the derived classes xrpd1d and xrpd2d for complete functionality. \n","ocl_base.hpp");  
}

ocl::~ocl(){
  clean();
  delete oclconfig;
  delete sgs;
  if(!usesStdout) fclose(stream);
  delete[] docstr;

}

/**
 * \brief Common initialisations for all the constructors
 */
void ocl::ContructorInit()
{
  stream=stdout;
  usesStdout=1;
  
  hasActiveContext=0;
  isConfigured  = 0;
  hasQueue      = 0;
  hasBuffers    = 0;
  hasProgram    = 0;
  hasKernels    = 0;
  hasTthLoaded  = 0;
  
  useSolidAngle = 0;
  useMask       = 0;
  useDummyVal   = 0;
  useTthRange   = 0;

  reset_time();
  
  sgs = new az_argtype;
  oclconfig = new ocl_config_type;
  oclconfig->Nbuffers=0;
  oclconfig->Nkernels=0;
  oclconfig->oclmemref =NULL;
  oclconfig->oclkernels=NULL;
  sgs->Nx = 0;
  sgs->Nimage = 0;
  sgs->Nbins = 0;
  sgs->Nbinst = 0;
  sgs->Nbinsc = 0;
  sgs->usefp64 = 0;
  docstr = new char[8192];
  
}

/**
 *  \brief Prints a list of OpenCL capable devices, their platforms and their ids to stream
 */  
void ocl::show_devices(){

  //Print all the probed devices to stream
  ocl_check_platforms(stream);
return;
}

/**
 *  \brief Prints a list of OpenCL capable devices, their platforms and their ids to stdout
 */
void ocl::print_devices(){

  //Print all the probed devices to stdout
  ocl_check_platforms();
return;
}

/**
 * \brief Not implemented
 */
void ocl::show_device_details(){

  return;
}

/**
 * \brief Returns a documentation string
 */
void ocl::help(){

  printf("%s\n",docstr);
  return;
}

/**
 * \brief Init a default context.
 *
 * Typically the default is GPU but may vary depending on the prominent libOpenCL.
 * If a context exists, clean() is called to reset and prevent memory leaks before requesting
 * the new context
 *
 * @param useFp64 Optional Boolean parameter to limit search only in FP64 capable devices (default = false).
 *
 */
int ocl::init(const bool useFp64){

  //Pick a device and initiate a context. If a context exists destroy it
  clean();
  if(ocl_init_context(oclconfig,"DEF",(int)useFp64,stream)) return -1;
  else hasActiveContext=1;
return 0;
}

/**
 * \brief Init a context with predefined device type.
 *
 * If a context exists, clean() is called to reset and prevent memory leaks before requesting
 * the new context
 *
 * @param devicetype A string containing the type of the required device. Possible options are
 *                   "GPU","gpu","CPU","cpu","ACC","acc","DEF","def","ALL","all"
 * @param useFp64 Optional Boolean parameter to limit search only in FP64 capable devices (default = false).
 *
 */
int ocl::init(const char *devicetype,const bool useFp64){

  //Pick a device and initiate a context. If a context exists destroy it
  this->clean();
  if(ocl_init_context(oclconfig,devicetype,(int)useFp64,stream)) return -1;
  else hasActiveContext=1;

return 0;
}

/**
 * \brief Init a context with predefined device type and device id.
 *
 * If a context exists, clean() is called to reset and prevent memory leaks before requesting
 * the new context. The platform and device id can be queried by the show_devices() method of
 * the parent class.
 *
 * @param devicetype A string containing the type of the required device. Possible options are
 *                   "GPU","gpu","CPU","cpu","ACC","acc","DEF","def","ALL","all"
 * @param platformid An integer stating the id of the platform (C notation)
 * @param devid An integer stating the id of the device (C notation)
 * @param useFp64 Optional Boolean parameter to limit search only in FP64 capable devices (default = True).
 *
 */
int ocl::init(const char *devicetype,int platformid,int devid,const bool useFp64){

  //Pick a device and initiate a context. If a context exists destroy it
  clean();
  if(ocl_init_context(oclconfig,devicetype,platformid,devid,(int)useFp64,stream)) return -1;
  else hasActiveContext=1;
return 0;
}

/**
 * \brief Free OpenCL related resources allocated by the library.
 *
 * clean() is used to reinitiate the library back in a vanilla state.
 * It may be asked to preserve the context created by init or completely clean up OpenCL.
 * Guard/Status flags that are set will be reset.
 *
 */
int ocl::clean(int preserve_context){

  reset_time();
  if(!preserve_context)
  {
    if(hasActiveContext){
      ocl_destroy_context(oclconfig->oclcontext);
      hasActiveContext=0;
      fprintf(stream,"--released OpenCL context\n");
      return 0;
    }
  }

return -2;
}

/**
 * \brief Forcibly and recklessly release and OpenCL context
 *
 * Calls to kill_context() may result to memory leaks depending on occassion and OpenCL
 * driver. Only available to test such cases and will be removed in the future.
 */
void ocl::kill_context(){
  if(hasActiveContext)
  {
    ocl_destroy_context(this->oclconfig->oclcontext);
    hasActiveContext=0;
    fprintf(stream,"Forced destroy context\n");
  }else fprintf(stream,"Attempted Forced destroy context ignored\n");
  return;
}

/**
 * \brief Resets profiling counters to initial values
 */
void ocl::reset_time()
{
  execTime_ms   = 0.0f;
  memCpyTime_ms = 0.0f;
  execCount = 0;
}

/**
 * \brief Returns the total kernel execution time in ms
 */
float ocl::get_exec_time()
{
  return execTime_ms;
}

/**
 * \brief Returns the total time spent in memory copies in ms
 */
float ocl::get_memCpy_time()
{
  return memCpyTime_ms;
}

/**
 * \brief Returns the count of integrations performed
 */
unsigned int ocl::get_exec_count()
{
  return execCount;
}

/**
 * \brief Progressive OpenCL buffer release
 *
 * Releases OpenCL buffers referenced by the OpenCL toolbox using
 * ocl_relNbuffers_byref
 *
 * @param level How many buffers to "pop" (last to first)
 */
void ocl::clean_clbuffers(int level){
  //Check that level < Nbuffers is done internally
  ocl_relNbuffers_byref(oclconfig,level);
}

/**
 * \brief Progressive OpenCL kernel release
 *
 * Releases OpenCL kernels referenced by the OpenCL toolbox using
 * ocl_relNkernels_byref
 *
 * @param level How many kernels to "pop" (last to first)
 */
void ocl::clean_clkernels(int level){
  ocl_relNkernels_byref(oclconfig,level);
}

/**
 * A default text exists, but the method will try to load its readme file.
 * If the file cannot be found or another problem exists, the default string is returned.
 *
 */
void ocl::setDocstring(const char *default_text, const char *filename)
{
  using namespace std;

  //Create a backup string in case the default string fails to reallocate
  char *bkp = new char[8192];
  strncpy(bkp,default_text,8192);
  if(bkp[8191]!='\0')bkp[8191]='\0';
  strcpy(docstr,bkp);

  ifstream readme;
  int len=0;
  size_t count;

  readme.open(filename,ios::in);

  //If the file exists:
  if(readme){

    //And can find the end get its size and rewind it.
    readme.seekg(0,ios_base::end);
    len = readme.tellg();
    readme.seekg(0,ios_base::beg);

    //If the file is bigger than the default string array we need to reallocate it.
    //If reallocation failed we point the default string to the backup string to avoid segmentation fault if help() is called
    //A successfull reallocation means we need to delete the backup string
    if(len >= 8192)
    {
      if(docstr) delete[] docstr;
      docstr = NULL;
      docstr = new char[len+1];
      if(!docstr)
      {
        docstr = bkp;
        readme.close();
        return;
      }else delete[] bkp;

    //-1 is the fail code of tellg. In that case docstring has not been reallocated.
    //we just need to delete the backup string and return the default one
    }else if (len == -1)
    {
      readme.close();
      delete[] bkp;
      return;
    }else delete[] bkp;

    //Read from file and check we read ALL the data
    if( readme.read(docstr,len).gcount() != len) fprintf(stderr,"setDocstring read size mismatch!\n");
    docstr[len] = '\0';
    readme.close();
  }
  return;
}
