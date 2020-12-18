
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
    document the API.  Dashes in these comment blocks are used to create bullet lists.  The
    lack of blank lines after a block of dash preceeded comments means that the next block
    of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

*****************************************  IMPORTANT NOTE  **********************************/



#include "pfmEdit3D.hpp"


//!  Tries to mark or unmark a depth record that is near the BFD feature.

void mark_feature (MISC *misc, BFDATA_RECORD *bfd_record)
{
  //  Only flag points if the confidence is greater than 0 and the feature type is Hydrographic...

  uint8_t unset = NVTrue;

  if (bfd_record->confidence_level && bfd_record->feature_type == BFDATA_HYDROGRAPHIC) unset = NVFalse;


  //  No point in trying to find the nearest point for an Informational feature since we don't want to set the flag for those.

  if (!unset && bfd_record->feature_type == BFDATA_INFORMATIONAL) return;


  double min_dist = 999999999999.9;
  int32_t hit = -1;
  for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
    {
      //  Don't use data marked as PFM_DELETED or PFM_REFERENCE.  If we're unsetting a flag, only look at data flagged
      //  as PFM_SELECTED_FEATURE or PFM_DESIGNATED_SOUNDING.

      if (!(misc->data[i].val & (PFM_DELETED | PFM_REFERENCE)) &&
          (!unset || (misc->data[i].val & (PFM_SELECTED_FEATURE | PFM_DESIGNATED_SOUNDING))))
        {
          double dist = sqrt (((bfd_record->latitude - misc->data[i].y) * (bfd_record->latitude - misc->data[i].y)) +
                              ((bfd_record->longitude - misc->data[i].x) * (bfd_record->longitude - misc->data[i].x)));

          if (dist < min_dist)
            {
              min_dist = dist;
              hit = i;
            }
        }
    }


  //  If we found a flagged point...

  if (hit >= 0)
    {
      //  Only flag points if the confidence is greater than 0 and the feature type is Hydrographic...

      if (!unset)
        {
          //  If this is a child record use PFM_DESIGNATED_SOUNDING.

          if (bfd_record->parent_record)
            {
              misc->data[hit].val &= ~PFM_SELECTED_FEATURE;
              misc->data[hit].val |= PFM_DESIGNATED_SOUNDING;
            }
          else
            {
              misc->data[hit].val &= ~PFM_DESIGNATED_SOUNDING;
              misc->data[hit].val |= PFM_SELECTED_FEATURE;
            }
        }


      //  Otherwise, un-flag them...

      else
        {
          misc->data[hit].val &= ~PFM_SELECTED_FEATURE;
          misc->data[hit].val &= ~PFM_DESIGNATED_SOUNDING;
        }
    }
}
