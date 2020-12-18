
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.
*********************************************************************************************/


/****************************************  IMPORTANT NOTE  **********************************

    Comments in this file that start with / * ! or / / ! are being used by Doxygen to
    document the software.  Dashes in these comment blocks are used to create bullet lists.
    The lack of blank lines after a block of dash preceeded comments means that the next
    block of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

*****************************************  IMPORTANT NOTE  **********************************/



#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H



/************************************** VERY IMPORTANT NOTE *********************************

    DO NOT add this file to the list of include files in nvutility.hpp or nvutility.h.
    If you do then every program in ABE will have to have its mk script modified to
    suppress the hundreds of "dereferencing type-punned pointer" warnings issued due to
    the las_definitions.hpp include file.  This file must be included separately by the
    programs using it.  For now those programs are pfmLoad, pfmLoader, and pfmEdit3D.

*************************************** VERY IMPORTANT NOTE *********************************/



#include <stdio.h>
#include <stdlib.h>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QtCore>

#include "nvutility.h"
#include "pfm.h"


/*  CHARTS header files.  */

#include "FileHydroOutput.h"
#include "FileTopoOutput.h"


/*  GSF header files.  */

#include "gsf.h"
#include "gsf_indx.h"


/*  CZMIL header files.  */

#include "czmil.h"


/* LASlib header files.  */

#include <lasreader.hpp>


/*  BAG header files.  */

#include "bag.h"
#include "bag_legacy.h"



typedef struct
{
  char          name[30];           //!<  Name of the attribute.
  float         min;                //!<  Minimum possible value for attribute.
  float         max;                //!<  Maximum possible value for attribute.
  float         scale;              //!<  Scale used to convert float to integer.
} ATTR_BOUNDS;


typedef struct
{
  float              elev;
  float              uncert;
} BAG_RECORD;




#define GSF_ATTRIBUTES        32
#define HOF_ATTRIBUTES        48
#define TOF_ATTRIBUTES        9
#define CZMIL_ATTRIBUTES      24
#define LAS_ATTRIBUTES        16
#define BAG_ATTRIBUTES        1

#define CZMIL_FLAGS           6

#define CZMIL_CSF             18    //!<  Last CZMIL attribute prior to the start of CSF attributes.


void setTimeAttributes (ATTR_BOUNDS *time_attribute);
void setGSFAttributes (ATTR_BOUNDS *gsf_attribute);
void setHOFAttributes (ATTR_BOUNDS *hof_attribute);
void setTOFAttributes (ATTR_BOUNDS *tof_attribute);
void setCZMILAttributes (ATTR_BOUNDS *czmil_attribute, QString *czmil_flag_name);
void setLASAttributes (ATTR_BOUNDS *las_attribute);
void setBAGAttributes (ATTR_BOUNDS *bas_attribute);
void countTimeAttributes (int16_t time_attribute_num, int16_t *attribute_count);
void countGSFAttributes (int16_t *gsf_attribute_num, int16_t *attribute_count);
void countHOFAttributes (int16_t *hof_attribute_num, int16_t *attribute_count);
void countTOFAttributes (int16_t *tof_attribute_num, int16_t *attribute_count);
void countCZMILAttributes (int16_t *czmil_attribute_num, int16_t *attribute_count);
void countLASAttributes (int16_t *las_attribute_num, int16_t *attribute_count);
void countBAGAttributes (int16_t *bag_attribute_num, int16_t *attribute_count);
int32_t gsfAttributesSet (int16_t *gsf_attribute_num);
int32_t hofAttributesSet (int16_t *hof_attribute_num);
int32_t tofAttributesSet (int16_t *tof_attribute_num);
int32_t czmilAttributesSet (int16_t *czmil_attribute_num);
int32_t lasAttributesSet (int16_t *las_attribute_num);
int32_t bagAttributesSet (int16_t *bag_attribute_num);


#endif
