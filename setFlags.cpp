
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

  - Module Name:        setFlags

  - Programmer(s):      Jan C. Depner

  - Date Written:       

  - Purpose:            Highlights points based on the flags set in the data
                        validity field.

  - Arguments:
                        - options         =   OPTIONS structure
                        - misc            =   MISC structure

  - Return Value:
                        - void

****************************************************************************/

void setFlags (MISC *misc, OPTIONS *options)
{
  //  Turn off highlighted points.

  if (misc->highlight_count)
    {
      misc->highlight.clear ();
      misc->highlight_count = 0;
    }


  uint32_t check = 0;

  switch (options->flag_index)
    {
    case 1:
      check = PFM_SUSPECT;
      break;

    case 2:
      check = PFM_SELECTED_SOUNDING;
      break;

    case 3:
      check = PFM_SELECTED_FEATURE;
      break;

    case 4:
      check = PFM_DESIGNATED_SOUNDING;
      break;

    case 5:
      check = PFM_INVAL;
      break;

    case PRE_USER + 0:
      check = PFM_USER_01;
      break;

    case PRE_USER + 1:
      check = PFM_USER_02;
      break;

    case PRE_USER + 2:
      check = PFM_USER_03;
      break;

    case PRE_USER + 3:
      check = PFM_USER_04;
      break;

    case PRE_USER + 4:
      check = PFM_USER_05;
      break;

    case PRE_USER + 5:
      check = PFM_USER_06;
      break;

    case PRE_USER + 6:
      check = PFM_USER_07;
      break;

    case PRE_USER + 7:
      check = PFM_USER_08;
      break;

    case PRE_USER + 8:
      check = PFM_USER_09;
      break;

    case PRE_USER + 9:
      check = PFM_USER_10;
      break;
    }

  if (options->flag_index)
    {
      for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
        {
          //  If we are displaying and editing only a single line, only get those points that are in that line.

          if (!misc->num_lines || check_line (misc, misc->data[i].line))
            {
              if (!check_bounds (options, misc, i, NVTrue, misc->slice))
                {
                  if (misc->data[i].val & check)
                    {
                      try
                        {
                          misc->highlight.resize (misc->highlight_count + 1);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "%s %s %s %d - highlight - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
                          exit (-1);
                        }

                      misc->highlight[misc->highlight_count] = i;
                      misc->highlight_count++;
                    }
                }
            }
        }
    }
}
