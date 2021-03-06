
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



#include "pfmEdit3D.hpp"

//!  Check to see if a feature is in the current view, whether it is valid, and whether it contains the (optional) search string.

uint8_t checkFeature (MISC *misc, QString feature_search_string, int32_t ftr, uint8_t *highlight, QString *feature_info)
{
  uint8_t pass = NVTrue;


  //  Check for slicing.

  if (misc->slice)
    {
      int32_t iz = pseudo_dist_from_viewer (misc, misc->feature[ftr].longitude, misc->feature[ftr].latitude);
      if (iz < misc->slice_min || iz > misc->slice_max) return (NVFalse);
    }


  *highlight = NVFalse;
  *feature_info = "";


  QString string0 (misc->feature[ftr].description);

  QString string1 (misc->feature[ftr].remarks);

  if (string1.isEmpty ())
    {
      if (!string0.isEmpty ()) *feature_info = string0;
    }
  else
    {
      if (string0.isEmpty ())
        {
          *feature_info = string1;
        }
      else
        {
          *feature_info = string0 + " :: " + string1;
        }
    }

  QString low_string = feature_info->toLower ();


  //  If we're doing a feature search we have a few things to check to see if we're going to display the feature.

  if (!feature_search_string.isEmpty ())
    {
      //  Check for the search string.

      if (misc->abe_share->feature_search_invert)
        {
          if (low_string.contains (feature_search_string.toLower ()))
            {
              if (misc->abe_share->feature_search_type) pass = NVFalse;
            }
          else
            {
              *highlight = NVTrue;
            }
        }
      else
        {
          if (!low_string.contains (feature_search_string.toLower ()))
            {
              if (misc->abe_share->feature_search_type) pass = NVFalse;
            }
          else
            {
              *highlight = NVTrue;
            }
        }
    }

  return (pass);
}
