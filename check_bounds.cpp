
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


/***************************************************************************/
/*!

  - Module Name:        check_bounds

  - Programmer(s):      Jan C. Depner

  - Date Written:       July 2000 (On vacation in Disney World, no less)

  - Purpose:            Check all of the conditions for displaying a point.
                        This version simply fills in all of the misc->data values.

  - Arguments:
                        - index    =   Index into the misc->data array for the point
                        - ck       =   NVFalse - no validity check
                        - slice    =   NVFalse - no slice check

  - Return Value:
                        - 2 = don't display
                        - 1 = display translucent (not used)
                        - 0 = display in full color

****************************************************************************/

int32_t check_bounds (OPTIONS *options, MISC *misc, int32_t index, uint8_t ck, uint8_t slice)
{
  double           x = misc->data[index].x;
  double           y = misc->data[index].y;
  uint32_t         v = misc->data[index].val;
  float            z = misc->data[index].z;
  int16_t          pfm = misc->data[index].pfm;
  uint8_t          m = misc->data[index].mask;

  return (check_bounds (options, misc, x, y, z, v, m, pfm, ck, slice));
}
  


/***************************************************************************/
/*!

  - Module Name:        check_bounds

  - Programmer(s):      Jan C. Depner

  - Date Written:       July 2000 (On vacation in Disney World, no less)

  - Purpose:            Check all of the conditions for displaying a point.
                        This version fills in all of the misc->data values with
                        the exception of the mask value.

  - Arguments:
                        - index    =   Index into the misc->data array for the point
                        - mask     =   NVFalse - no mask check
                        - ck       =   NVFalse - no validity check
                        - slice    =   NVFalse - no slice check

  - Return Value:
                        - 2 = don't display
                        - 1 = display translucent (not used)
                        - 0 = display in full color

****************************************************************************/

int32_t check_bounds (OPTIONS *options, MISC *misc, int32_t index, uint8_t mask, uint8_t ck, uint8_t slice)
{
  double           x = misc->data[index].x;
  double           y = misc->data[index].y;
  uint32_t         v = misc->data[index].val;
  float            z = misc->data[index].z;
  int16_t          pfm = misc->data[index].pfm;

  return (check_bounds (options, misc, x, y, z, v, mask, pfm, ck, slice));
}
  


/***************************************************************************/
/*!

  - Module Name:        check_bounds

  - Programmer(s):      Jan C. Depner

  - Date Written:       July 2000 (On vacation in Disney World, no less)

  - Purpose:            Check all of the conditions for displaying a point.

  - Arguments:
                        - x        =   X position of the point
                        - y        =   Y position of the point
                        - z        =   Z value of the point
                        - v        =   Validity of the point
                        - mask     =   Masked point bit flags
                        - pfm      =   PFM layer number
                        - ck       =   NVFalse - no validity check
                        - slice    =   NVFalse - no slice check

  - Return Value:
                        - 2 = don't display
                        - 1 = display translucent
                        - 0 = display in full color

****************************************************************************/

int32_t check_bounds (OPTIONS *options, MISC *misc, double x, double y, float z, uint32_t v, uint8_t mask, int16_t pfm, uint8_t ck, uint8_t slice)
{
  int32_t          iz;


  //  Easy checks first, that way, if we can eliminate this point we don't have to do 
  //  the edge or inside check.  Check for deleted, reference, displaying invalid, masked, 
  //  then displaying class data.

  if (v & PFM_DELETED) return (2);


  //  If this point is masked (either by shape or attribute)...

  if (mask) return (2);


  //  If we're not displaying the PFM that this point is in.

  if (!misc->abe_share->display_pfm[pfm]) return (2);


  //  If we're not displaying nulls and this is a null.

  if (!options->display_null && z >= misc->null_val[pfm]) return (2);


  //  If we're not displaying reference and this is reference.

  if (!options->display_reference && (v & PFM_REFERENCE)) return (2);


  //  If we're checking invalid...

  if (ck)
    {
      //  If the point is manually invalid (only if we're not displaying manually invalid).

      if (!options->display_man_invalid && ((v & PFM_MANUALLY_INVAL))) return (2);


      //  If the point is filter invalid (only if we're not displaying filter invalid).

      if (!options->display_flt_invalid && ((v & PFM_FILTER_INVAL))) return (2);
    }


  //  Now check the bounds.  Simple MBR first, then polygon if needed.

  if (x < misc->displayed_area.min_x || x > misc->displayed_area.max_x || 
      y < misc->displayed_area.min_y || y > misc->displayed_area.max_y)
    return (2);


  if (misc->poly_count)
    {
      if (!inside_polygon2 (misc->polygon_x, misc->polygon_y, misc->poly_count, x, y)) return (2);
    }


  //  Check for slicing if it is enabled.

  if (slice)
    {
      iz = pseudo_dist_from_viewer (misc, x, y);
      if (iz < misc->slice_min || iz > misc->slice_max) return (1);
    }


  return (0);
}



/***************************************************************************/
/*!

  - Module Name:        check_line

  - Programmer(s):      Jan C. Depner

  - Date Written:       October 2001

  - Purpose:            Check a line number to see if it is supposed to be
                        displayed.

  - Arguments:
                        - line     =   line number

  - Return Value:
                        - NVTrue   =   display
                        - NVFalse  =   don't display

****************************************************************************/

uint8_t check_line (MISC *misc, int32_t line)
{
  for (int32_t i = 0 ; i < misc->num_lines ; i++)
    {
      if (line == misc->line_num[i]) return (NVTrue);
    }

  return (NVFalse);
}
